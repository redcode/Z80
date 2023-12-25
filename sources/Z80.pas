(*     ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ______________________________________
|        /\_____\\_____\\_____\                                      |
|  Zilog \/_____//_____//_____/ CPU Emulator - Pascal Binding        |
|  Copyright (C) 2022-2023 Zoran Vučenović.                          |
|                                                                    |
|  Permission to use, copy, modify, and/or distribute this software  |
|  for any purpose with or without fee is hereby granted.            |
|                                                                    |
|  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL     |
|  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED     |
|  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL      |
|  THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR        |
|  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM    |
|  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,   |
|  NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN         |
|  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.          |
|                                                                    |
'===================================================================*)

unit Z80;

{$mode ObjFPC}{$H+}
{$J-}
{$inline on}
{$PackRecords C}

interface

uses
	SysUtils,
	dynlibs;

const
	DefaultZ80LibPath =
		{$if defined(mswindows)}
			'Z80.dll';
		{$elseif defined(darwin)}
			'libZ80.dylib';
		{$elseif defined(unix)}
			'libZ80.so';
		{$else}
			'';
		{$endif}

	Z80_MAXIMUM_CYCLES = SizeUInt(SizeUInt(SizeUInt.MaxValue) - 30);
	Z80_MAXIMUM_CYCLES_PER_STEP = 23;

	Z80_HOOK = $64;

	Z80_SF = 128;
	Z80_ZF =  64;
	Z80_YF =  32;
	Z80_HF =  16;
	Z80_XF =   8;
	Z80_PF =   4;
	Z80_NF =   2;
	Z80_CF =   1;

type
	PZ80 = ^TZ80;

	TZ80RegisterPair = packed record
	case Integer of
		0: (word: UInt16);
		{$ifdef endian_little}
			1: (bytes: packed record low: UInt8; high: UInt8 end);
		{$else}
			1: (bytes: packed record high: UInt8; low: UInt8 end);
		{$endif}
	end;

	Z80Read    = function(context: Pointer; address: UInt16): UInt8; cdecl;
	Z80Write   = procedure(context: Pointer; address: UInt16; value: UInt8); cdecl;
	Z80Halt    = procedure(context: Pointer; state: UInt8); cdecl;
	Z80Notify  = procedure(context: Pointer); cdecl;
	Z80Illegal = function(cpu: PZ80; opcode: UInt8): UInt8; cdecl;

	TZ80 = record
		cycles:       SizeUInt;
		cycle_limit:  SizeUInt;
		context:      Pointer;
		fetch_opcode: Z80Read;
		fetch:        Z80Read;
		read:         Z80Read;
		write:        Z80Write;
		input:        Z80Read;
		output:       Z80Write;
		halt:         Z80Halt;
		nop:          Z80Read;
		nmia:         Z80Read;
		inta:         Z80Read;
		int_fetch:    Z80Read;
		ld_i_a:       Z80Notify;
		ld_r_a:       Z80Notify;
		reti:         Z80Notify;
		retn:         Z80Notify;
		hook:         Z80Read;
		illegal:      Z80Illegal;
		data:         array [0..3] of UInt8;
		ix_iy:        array [0..1] of TZ80RegisterPair;
		pc:           TZ80RegisterPair;
		sp:           TZ80RegisterPair;
		xy:           TZ80RegisterPair;
		memptr:       TZ80RegisterPair;
		af:           TZ80RegisterPair;
		bc:           TZ80RegisterPair;
		de:           TZ80RegisterPair;
		hl:           TZ80RegisterPair;
		af_:          TZ80RegisterPair;
		bc_:          TZ80RegisterPair;
		de_:          TZ80RegisterPair;
		hl_:          TZ80RegisterPair;
		r:            UInt8;
		i:            UInt8;
		r7:           UInt8;
		im:           UInt8;
		request:      UInt8;
		resume:       UInt8;
		iff1:         UInt8;
		iff2:         UInt8;
		q:            UInt8;
		options:      UInt8;
		int_line:     UInt8;
		halt_line:    UInt8;
	end;

const
	Z80_OPTION_OUT_VC_255             =  1;
	Z80_OPTION_LD_A_IR_BUG            =  2;
	Z80_OPTION_HALT_SKIP              =  4;
	Z80_OPTION_XQ                     =  8;
	Z80_OPTION_IM0_RETX_NOTIFICATIONS = 16;
	Z80_OPTION_YQ                     = 32;

	Z80_MODEL_ZILOG_NMOS = (Z80_OPTION_LD_A_IR_BUG or Z80_OPTION_XQ or Z80_OPTION_YQ);
	Z80_MODEL_ZILOG_CMOS = (Z80_OPTION_OUT_VC_255 or Z80_OPTION_XQ or Z80_OPTION_YQ);
	Z80_MODEL_NEC_NMOS   = Z80_OPTION_LD_A_IR_BUG;
	Z80_MODEL_ST_CMOS    = (Z80_OPTION_OUT_VC_255 or Z80_OPTION_LD_A_IR_BUG or Z80_OPTION_YQ);

	Z80_REQUEST_REJECT_NMI    =  2;
	Z80_REQUEST_NMI           =  4;
	Z80_REQUEST_INT           =  8;
	Z80_REQUEST_SPECIAL_RESET = 16;

	Z80_RESUME_HALT     = 1;
	Z80_RESUME_XY       = 2;
	Z80_RESUME_IM0_XY   = 3;

	Z80_HALT_EXIT       = 0;
	Z80_HALT_ENTER      = 1;
	Z80_HALT_EXIT_EARLY = 2;
	Z80_HALT_CANCEL     = 3;

var
	z80_power:         procedure(self: PZ80; state: ByteBool); cdecl;
	z80_instant_reset: procedure(self: PZ80); cdecl;
	z80_special_reset: procedure(self: PZ80); cdecl;
	z80_int:           procedure(self: PZ80; state: ByteBool); cdecl;
	z80_nmi:           procedure(self: PZ80); cdecl;
	z80_execute:       function(self: PZ80; cycles: SizeUInt): SizeUInt; cdecl;
	z80_run:           function(self: PZ80; cycles: SizeUInt): SizeUInt; cdecl;

function z80_r(const self: PZ80): UInt8; inline;
function z80_refresh_address(const self: PZ80): UInt16; inline;
function z80_in_cycle(const self: PZ80): UInt8; inline;
function z80_out_cycle(const self: PZ80): UInt8; inline;

function IsLibraryLoaded: Boolean;
function LoadLibrary(const LibPath: String = ''): Boolean;
function UnloadLibrary: Boolean;


implementation

procedure z80_break(self: PZ80);
begin
	self^.cycle_limit := 0;
end;


function z80_r(const self: PZ80): UInt8;
begin
	Result := (self^.r and 127) or (Self^.r7 and 128);
end;


function z80_refresh_address(const self: PZ80): UInt16;
begin
	Result := UInt16(self^.i) shl 8;
	Result := Result or ((self^.r - 1) and 127) or (self^.r7 and 128);
end;


function z80_in_cycle(const self: PZ80): UInt8;
begin
	if self^.data[0] = $DB then begin
		Result := 7;
	end else begin
		Result := 8 + (self^.data[1] shr 7);
	end;
end;


function z80_out_cycle(const self: PZ80): UInt8;
begin
	if self^.data[0] = $D3 then begin
		Result := 7;
	end else begin
		Result := 8 + ((self^.data[1] shr 7) shl 2);
	end;
end;


var
	LibHandle: TLibHandle;


function IsLibraryLoaded: Boolean;
begin
	Result := LibHandle <> NilHandle;
end;


function LoadLibrary(const LibPath: String): Boolean;
var
	 LibName: String;
begin
	if LibHandle = NilHandle then begin
		Result := False;

		if LibPath = '' then begin
			LibName := DefaultZ80LibPath;
		end else begin
			LibName := LibPath;
		end;

		LibHandle := SafeLoadLibrary(LibName);
		if LibHandle = NilHandle then Exit;

		Pointer(z80_power)         := GetProcAddress(LibHandle, 'z80_power');
		Pointer(z80_instant_reset) := GetProcAddress(LibHandle, 'z80_instant_reset');
		Pointer(z80_special_reset) := GetProcAddress(LibHandle, 'z80_special_reset');
		Pointer(z80_int)           := GetProcAddress(LibHandle, 'z80_int');
		Pointer(z80_nmi)           := GetProcAddress(LibHandle, 'z80_nmi');
		Pointer(z80_execute)       := GetProcAddress(LibHandle, 'z80_execute');
		Pointer(z80_run)           := GetProcAddress(LibHandle, 'z80_run');
	end;

	Result := True;
end;


function UnloadLibrary: Boolean;
begin
	if IsLibraryLoaded then begin
		if not dynlibs.UnloadLibrary(LibHandle) then Exit(False);
		LibHandle := NilHandle;
	end;

	z80_power         := nil;
	z80_instant_reset := nil;
	z80_special_reset := nil;
	z80_int           := nil;
	z80_nmi           := nil;
	z80_execute       := nil;
	z80_run           := nil;

	Result := True;
end;


procedure Init;
begin
	LibHandle := NilHandle;
	UnloadLibrary;
end;


initialization
	Init;

finalization
	UnloadLibrary;

end.
