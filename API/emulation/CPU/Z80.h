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
	zusize	  cycles;  /**< Cycles executed in the last call to z80_run. */
	void*	  context; /**< Callback functions' "context" argument.	     */
	ZZ80State state;   /**< CPU registers and internal bits.	     */
	Z16Bit	  xy;	   /**< Temporaty IX/IY register for DD/FD prefixes. */
	zuint8	  r7;	   /**< Backup of the 7th bit of the R register.     */
	Z32Bit	  data;	   /**< Temporary cache of the current instruction.  */

	/** Called when the CPU needs to read 8 bits from memory.
	  * @param context A pointer to the calling emulator instance.
	  * @param address The memory address to read.
	  * @return The 8 bits read from memory. */

	zuint8 (* read)(void *context, zuint16 address);

	/** Called when the CPU needs to write 8 bits to memory.
	  * @param context The value of the member `context`.
	  * @param address The memory address to write.
	  * @param value The value to write in address. */

	void (* write)(void *context, zuint16 address, zuint8 value);

	/** Called when the CPU needs to read 8 bits from an I/O port.
	  * @param context The value of the member `context`.
	  * @param port The number of the I/O port.
	  * @return The 8 bits read from the I/O port. */

	zuint8 (* in)(void *context, zuint16 port);

	/** Called when the CPU needs to write 8 bits to an I/O port.
	  * @param context The value of the member `context`.
	  * @param port The number of the I/O port.
	  * @param value The value to write. */

	void (* out)(void *context, zuint16 port, zuint8 value);

	/** Called when the CPU starts executing a maskable interrupt and the
	  * interruption mode is 0. This callback must return the instruction
	  * that the CPU would read from the data bus in this case.
	  * @param context The value of the member `context`.
	  * @return A 32-bit value containing the bytes of an instruction. The
	  * instruction must begin at the most significant byte of the value. */

	zuint32 (* int_data)(void *context);

	/** Called when the CPU enters or exits the halt state.
	  * @param context The value of the member `context`.
	  * @param state TRUE if halted, FALSE otherwise. */

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
  * @param state TRUE for power ON, FALSE otherwise. */

CPU_Z80_API void z80_power(Z80 *object, zboolean state);

/** Resets the CPU by reinitializing its variables and sets its registers to
  * the state they would be in a real Z80 CPU after a pulse in the RESET line
  * @param object A pointer to a Z80 emulator instance object. */

CPU_Z80_API void z80_reset(Z80 *object);

/** Runs the CPU for a given number of cycles.
  * @details Given the fact that one Z80 instruction needs between 4 and 23
  * cycles to be executed, it's not always possible to run the CPU the exact
  * number of cycles specfified.
  * @param object A pointer to a Z80 emulator instance object.
  * @param cycles The number of cycles to be executed.
  * @return The number of cycles executed. */

CPU_Z80_API zusize z80_run(Z80 *object, zusize cycles);

/** Performs a non-maskable interrupt.
  * @details This is equivalent to a pulse in the NMI line of a real Z80 CPU.
  * @param object A pointer to a Z80 emulator instance object. */

CPU_Z80_API void z80_nmi(Z80 *object);

/** Changes the state of the maskable interrupt.
  * @details This is equivalent to a change in the INT line of a real Z80 CPU.
  * @param object A pointer to a Z80 emulator instance object.
  * @param state TRUE = set line high, FALSE = set line low */

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
