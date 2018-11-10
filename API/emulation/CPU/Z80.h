/*  ______   ______  ______
   /\___  \ /\  __ \/\  __ \
   \/__/  /_\ \  __ \ \ \/\ \
      /\_____\ \_____\ \_____\
Zilog \/_____/\/_____/\/_____/ CPU Emulator ----------------------------------
Copyright (C) 1999-2018 Manuel Sainz de Baranda y Goñi.

This emulator is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published  by the Free Software
Foundation, either  version 3 of  the License, or  (at your option)  any later
version.

This emulator is distributed  in the hope that it will  be useful, but WITHOUT
ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received  a copy of the GNU General Public License  along with
this emulator. If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------- */

#ifndef _emulation_CPU_Z80_H_
#define _emulation_CPU_Z80_H_

#ifdef CPU_Z80_DEPENDENCIES_H
#	include CPU_Z80_DEPENDENCIES_H
#else
#	include <Z/hardware/CPU/architecture/Z80.h>
#endif

/** Z80 emulator instance.
  * @details This structure contains the state of the emulated CPU and callback
  * pointers necessary to interconnect the emulator with external logic. There
  * is no constructor function, so, before using an object of this type, some
  * of its members must be initialized, in particular the following:
  * @c context, @c read, @c write, @c in, @c out, @c int_data and @c halt. */

typedef struct {

	/** Number of cycles executed in the current call to @c z80_run.
	  * @details @c z80run sets this variable to @c 0 before starting
	  * to execute instructions and its value persists after returning.
	  * The callbacks can use this variable to know during what cycle
	  * they are being called. */

	zusize cycles;

	/** The value used as the first argument when calling a callback.
	  * @details This variable should be initialized before using the
	  * emulator and can be used to reference the context/instance of
	  * the machine being emulated. */

	void *context;

	/** Callback: Called when the CPU needs to read 8 bits from memory.
	  * @param context The value of the member @c context.
	  * @param address The memory address to read from.
	  * @return The 8 bits read from memory. */

	zuint8 (* read)(void *context, zuint16 address);

	/** Callback: Called when the CPU needs to write 8 bits to memory.
	  * @param context The value of the member @c context.
	  * @param address The memory address to write to.
	  * @param value The value to write. */

	void (* write)(void *context, zuint16 address, zuint8 value);

	/** Callback: Called when the CPU needs to read 8 bits from an I/O port.
	  * @param context The value of the member @c context.
	  * @param port The number of the I/O port to read from.
	  * @return The 8 bits read from the I/O port. */

	zuint8 (* in)(void *context, zuint16 port);

	/** Callback: Called when the CPU needs to write 8 bits to an I/O port.
	  * @param context The value of the member @c context.
	  * @param port The number of the I/O port to write to.
	  * @param value The value to write. */

	void (* out)(void *context, zuint16 port, zuint8 value);

	/** Callback: Called when the CPU needs to read one instruction from
	  * the data bus to service a maskable interrupt (INT) in mode 0.
	  * @param context The value of the member @c context.
	  * @return A 32-bit value containing the bytes of one instruction. The
	  * instruction must begin at the most significant byte (big endian). */

	zuint32 (* int_data)(void *context);

	/** Callback: Called when the CPU enters or exits the halt state.
	  * @param context The value of the member @c context.
	  * @param state @c TRUE if halted; @c FALSE otherwise.
	  * @note This callback is optional and must be set to @c NULL if not
	  * used. */

	void (* halt)(void *context, zboolean state);

	/** CPU registers and internal bits.
	  * @details It contains the state of the registers, as well as the
	  * interrupt flip-flops, variables related to interrupts and other
	  * necessary flags. This is what a debugger should use as its data
	  * source. */

	ZZ80State state;

	/** Backup of the 7th bit of the R register.
	  * @details The value of the R register is incremented as instructions
	  * are executed, but its most significant bit remains unchanged. For
	  * optimization reasons, this bit is saved at the beginning of the
	  * execution of @c z80_run and restored before returning. If an
	  * instruction directly affects the R register, this variable is also
	  * updated accordingly. */

	zuint8 r7;

	/** Temporay IX/IY register for instructions with @c DDh/FDh prefix.
	  * @details Since instructions with prefix @c DDh and @c FDh behave
	  * similarly, differing only in the use of register IX or IY, for
	  * reasons of size optimization, a single register is used that acts
	  * as both. During opcode analysis, the IX or IY register is copied
	  * to this variable and, once the instruction emulation is complete,
	  * its contents are copied back to the appropriate register. */

	Z16Bit xy;

	/** Temporary storage for opcode fetching.
	  * @details This is an internal private variable. */

	Z32Bit data;
} Z80;

Z_C_SYMBOLS_BEGIN

#ifndef CPU_Z80_API
#	ifdef CPU_Z80_STATIC
#		define CPU_Z80_API
#	else
#		define CPU_Z80_API Z_API
#	endif
#endif

/** Changes the CPU power status.
  * @param object A pointer to a Z80 emulator instance.
  * @param state @c TRUE = power ON; @c FALSE = power OFF. */

CPU_Z80_API void z80_power(Z80 *object, zboolean state);

/** Resets the CPU.
  * @details This is equivalent to a pulse on the RESET line of a real Z80.
  * @param object A pointer to a Z80 emulator instance. */

CPU_Z80_API void z80_reset(Z80 *object);

/** Runs the CPU for a given number of @p cycles.
  * @param object A pointer to a Z80 emulator instance.
  * @param cycles The number of cycles to be executed.
  * @return The number of cycles executed.
  * @note Given the fact that one Z80 instruction needs between 4 and 23 cycles
  * to be executed, it's not always possible to run the CPU the exact number of
  * @p cycles specfified. */

CPU_Z80_API zusize z80_run(Z80 *object, zusize cycles);

/** Performs a non-maskable interrupt (NMI).
  * @details This is equivalent to a pulse on the NMI line of a real Z80.
  * @param object A pointer to a Z80 emulator instance. */

CPU_Z80_API void z80_nmi(Z80 *object);

/** Changes the state of the maskable interrupt (INT).
  * @details This is equivalent to a change on the INT line of a real Z80.
  * @param object A pointer to a Z80 emulator instance.
  * @param state @c TRUE = line high; @c FALSE = line low. */

CPU_Z80_API void z80_int(Z80 *object, zboolean state);

Z_C_SYMBOLS_END

#ifdef CPU_Z80_WITH_ABI

#	ifndef CPU_Z80_DEPENDENCIES_H
#		include <Z/ABIs/generic/emulation.h>
#	endif

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
