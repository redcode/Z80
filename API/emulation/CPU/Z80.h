/* Zilog Z80 CPU Emulator API
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright (C) 1999-2018 Manuel Sainz de Baranda y Goñi.

This library  is free software: you  can redistribute it and/or  modify it under
the terms of  the GNU General Public  License as published by  the Free Software
Foundation,  either version  3 of  the License,  or (at  your option)  any later
version.

This library is distributed in the hope  that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty  of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should  have received a  copy of the GNU  General Public License  along with
this library. If not, see <http://www.gnu.org/licenses/>. */

#ifndef _emulation_CPU_Z80_H_
#define _emulation_CPU_Z80_H_

#ifdef CPU_Z80_DEPENDENCIES_H
#	include CPU_Z80_DEPENDENCIES_H
#else
#	include <Z/hardware/CPU/architecture/Z80.h>
#endif

/** Z80 emulator instance object. */

typedef struct {

	/** Number of cycles executed in the current call to @c z80_run.
	  * @details @c z80run sets this variable to 0 before starting to
	  * execute instructions and its value persists after returning.
	  * A callback can use this variable to know in which cycle it is
	  * being called. */

	zusize cycles;

	/** The value of the first argument used when calling a callback.
	  * @details This variable should be initialized before using the
	  * emulator and can be used to reference the context or instance
	  * of the machine being emulated. */

	void *context;

	/** CPU registers and internal bits.
	  * @details It contains the values of the registers, as well as the
	  * interruption flip-flops, variables related to interruptions and
	  * other necessary flags. This is what a debugger should use as its
	  * data source. */

	ZZ80State state;

	/** Temporay IX/IY register for instructions with DDh/FDh prefix.
	  * @details Since instructions with prefix DD and FD behave similarly,
	  * differing only in the use of register IX or IY, for reasons of size
	  * optimization, a single register is used that acts as both. During
	  * opcode analysis, the IX or IY register is copied to this variable
	  * and, once the instruction emulation is complete, its contents are
	  * copied back to the appropriate register. */

	Z16Bit xy;

	/** Backup of the 7th bit of the R register.
	  * @details The value of the R register is incremented as instructions
	  * are executed, but its most significant bit remains unchanged. For
	  * optimization reasons, this bit is saved at the beginning of the
	  * execution of @c z80_run and restored before returning. If an
	  * instruction directly affects the R register, this variable is also
	  * updated accordingly. */

	zuint8 r7;

	/** Temporary storage for opcode fetching.
	  * @details It is used during the opcode fetching in an opportunistic
	  * way to store the bytes of the instruction. It is an internal private
	  * variable.*/

	Z32Bit data;

	/** Callback: Called when the CPU needs to read 8 bits from memory.
	  * @param context The value of the member @c context.
	  * @param address The memory address to read.
	  * @return The 8 bits read from memory. */

	zuint8 (* read)(void *context, zuint16 address);

	/** Callback: Called when the CPU needs to write 8 bits to memory.
	  * @param context The value of the member @c context.
	  * @param address The memory address to write.
	  * @param value The value to write in address. */

	void (* write)(void *context, zuint16 address, zuint8 value);

	/** Callback: Called when the CPU needs to read 8 bits from an I/O port.
	  * @param context The value of the member @c context.
	  * @param port The number of the I/O port.
	  * @return The 8 bits read from the I/O port. */

	zuint8 (* in)(void *context, zuint16 port);

	/** Callback: Called when the CPU needs to write 8 bits to an I/O port.
	  * @param context The value of the member @c context.
	  * @param port The number of the I/O port.
	  * @param value The value to write. */

	void (* out)(void *context, zuint16 port, zuint8 value);

	/** Callback: Called when the CPU starts executing a maskable interrupt
	  * and the interruption mode is 0. This callback must return the
	  * instruction that the CPU would read from the data bus in this case.
	  * @param context The value of the member @c context.
	  * @return A 32-bit value containing the bytes of an instruction. The
	  * instruction must begin at the most significant byte (big endian). */

	zuint32 (* int_data)(void *context);

	/** Callback: Called when the CPU enters or exits the halt state.
	  * @param context The value of the member @c context.
	  * @param state @c TRUE if halted; @c FALSE otherwise. */

	void (* halt)(void *context, zboolean state);
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
  * @param object A pointer to a Z80 emulator instance object.
  * @param state @c TRUE = power ON; @c FALSE = power OFF. */

CPU_Z80_API void z80_power(Z80 *object, zboolean state);

/** Resets the CPU by reinitializing its variables and sets its registers to
  * the state they would be in a real Z80 CPU after a pulse in the RESET line.
  * @param object A pointer to a Z80 emulator instance object. */

CPU_Z80_API void z80_reset(Z80 *object);

/** Runs the CPU for a given number of @p cycles.
  * @param object A pointer to a Z80 emulator instance object.
  * @param cycles The number of cycles to be executed.
  * @return The number of cycles executed.
  * @note Given the fact that one Z80 instruction needs between 4 and 23
  * cycles to be executed, it's not always possible to run the CPU the exact
  * number of @p cycles specfified. */

CPU_Z80_API zusize z80_run(Z80 *object, zusize cycles);

/** Performs a non-maskable interrupt.
  * @details This is equivalent to a pulse in the NMI line of a real Z80 CPU.
  * @param object A pointer to a Z80 emulator instance object. */

CPU_Z80_API void z80_nmi(Z80 *object);

/** Changes the state of the maskable interrupt.
  * @details This is equivalent to a change in the INT line of a real Z80 CPU.
  * @param object A pointer to a Z80 emulator instance object.
  * @param state @c TRUE = line high; @c FALSE = line low. */

CPU_Z80_API void z80_int(Z80 *object, zboolean state);

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
