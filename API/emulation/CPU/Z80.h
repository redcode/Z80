/* Zilog Z80 CPU Emulator API
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright (C) 1999-2018 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef _emulation_CPU_Z80_H_
#define _emulation_CPU_Z80_H_

#ifdef CPU_Z80_DEPENDENCIES_H
#	include CPU_Z80_DEPENDENCIES_H
#else
#	include <Z/hardware/CPU/architecture/Z80.h>
#endif

typedef struct {
	zusize	  cycles;
	ZZ80State state;
	Z16Bit	  xy;
	zuint8	  r7;
	Z32Bit	  data;
	void*	  context;

	zuint8	(* read    )(void *context, zuint16 address);
	void	(* write   )(void *context, zuint16 address, zuint8 value);
	zuint8	(* in	   )(void *context, zuint16 port);
	void	(* out	   )(void *context, zuint16 port, zuint8 value);
	zuint32 (* int_data)(void *context);
	void	(* halt    )(void *context, zboolean state);
} Z80;

Z_C_SYMBOLS_BEGIN

#ifndef CPU_Z80_API
#	ifdef CPU_Z80_STATIC
#		define CPU_Z80_API
#	else
#		define CPU_Z80_API Z_API
#	endif
#endif

CPU_Z80_API void   z80_power(Z80 *object, zboolean state);
CPU_Z80_API void   z80_reset(Z80 *object);
CPU_Z80_API zusize z80_run  (Z80 *object, zusize cycles);
CPU_Z80_API void   z80_nmi  (Z80 *object);
CPU_Z80_API void   z80_int  (Z80 *object, zboolean state);

Z_C_SYMBOLS_END

#ifndef CPU_Z80_OMIT_ABI_PROTOTYPE

#	include <Z/ABIs/generic/emulation.h>

	Z_C_SYMBOLS_BEGIN

#	ifndef CPU_Z80_ABI
#		ifdef CPU_Z80_STATIC
#			define CPU_Z80_ABI
#		else
#			define CPU_Z80_ABI Z_API
#		endif
#	endif

	CPU_Z80_ABI extern ZCPUEmulatorABI const abi_emulation_cpu_z80;

	Z_C_SYMBOLS_END

#endif

#endif /* _emulation_CPU_Z80_H_ */
