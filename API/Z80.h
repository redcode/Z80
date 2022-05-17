/* Z80 API
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator                                   |
|  Copyright (C) 1999-2022 Manuel Sainz de Baranda y Goñi.                     |
|                                                                              |
|  This emulator is free software: you can redistribute it and/or modify it    |
|  under the terms of the GNU Lesser General Public License as published by    |
|  the Free Software Foundation, either version 3 of the License, or (at your  |
|  option) any later version.                                                  |
|                                                                              |
|  This emulator is distributed in the hope that it will be useful, but        |
|  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY  |
|  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public      |
|  License for more details.                                                   |
|                                                                              |
|  You should have received a copy of the GNU Lesser General Public License    |
|  along with this emulator. If not, see <http://www.gnu.org/licenses/>.       |
|                                                                              |
'=============================================================================*/

#ifndef Z80_H

/** @file Z80.h
  * @brief Zilog Z80 CPU emulator.
  *
  * @details The Z80 library implements a fast, small and accurate emulator
  * of the Zilog Z80. It emulates all that is known to date about this CPU,
  * including the undocumented flags and instructions, MEMPTR, Q, and the
  * special RESET.
  *
  * @version 0.2
  * @date 2022-05-29
  * @author Manuel Sainz de Baranda y Goñi. */

#ifdef Z80_DEPENDENCIES_HEADER
#	define  Z80_H
#	include Z80_DEPENDENCIES_HEADER
#	undef   Z80_H
#else
#	include <Z/macros/language.h>
#	include <Z/types/bitwise.h>
#endif

#ifndef Z80_API
#	if defined(Z80_STATIC) || defined(__DOXYGEN__)
#		define Z80_API
#	else
#		define Z80_API Z_API_IMPORT
#	endif
#endif

/** @brief The major version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MAJOR 0

/** @brief The minor version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MINOR 2

/** @brief The micro version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MICRO 0

/** @brief A string literal with the version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_STRING "0.2"

/** @brief The maximum number of clock cycles that the @ref z80_run and @ref
  * z80_execute functions can emulate per call. */

#define Z80_CYCLE_LIMIT (Z_USIZE_MAXIMUM - Z_USIZE(30))
#define Z80_CYCLES_PER_RESET 5

/** @brief The 8-bit value interpreted as a hook by the Z80 emulator, which
  * corresponds to the <tt>ld h,h</tt> opcode in the Z80 ISA. */

#define Z80_HOOK Z_UINT8(0x64)

/** @brief Defines a pointer to a callback function used by the Z80 emulator
  * to read a byte.
  *
  * @param context The value of the @ref Z80.context member variable of the
  * object invoking the callback.
  * @param address The memory address or I/O port to read from.
  * @return The byte read. */

typedef zuint8 (* Z80Read)(void *context, zuint16 address);

/** @brief Defines a pointer to a callback function used by the Z80 emulator
  * to write a byte.
  *
  * @param context The value of the @ref Z80.context member variable of the
  * object invoking the callback.
  * @param address The memory address or I/O port to write to.
  * @param value The byte to write. */

typedef void (* Z80Write)(void *context, zuint16 address, zuint8 value);

/** @brief Defines a pointer to a callback function used by the Z80 emulator
  * to notify the change of state of the HALT line.
  *
  * @param context The value of the @ref Z80.context member variable of the
  * object invoking the callback.
  * @param state
  *     @c TRUE  if the HALT line goes low  (the CPU enters the HALT state);
  *     @c FALSE if the HALT line goes high (the CPU exits the HALT state). */

typedef void (* Z80HALT)(void *context, zboolean state);

/** @brief Defines a pointer to a callback function used by the Z80 emulator
  * to notify an event.
  *
  * @param context The value of the @ref Z80.context member variable of the
  * object invoking the callback. */

typedef void (* Z80Notify)(void *context);


typedef zusize (* Z80Reset)(void *context, zuint16 address);

/** @struct Z80 Z80.h
  *
  * @brief A Z80 CPU emulator.
  *
  * @details @c Z80 contains the state of an emulated Z80 CPU
  * and the callback pointers needed to interconnect it with the external logic.
  * Before using an object of this type, some of its members
  * must be initialized. In particular the following (in alphabetical order):
  * @ref Z80.context, @ref Z80.fetch, @ref Z80.fetch_opcode, @ref Z80.halt_nop,
  * @ref Z80.in, @ref Z80.out, @ref Z80.read, @ref Z80.write, @ref Z80.read_bus,
  * @ref Z80.halt, @ref Z80.inta, @ref Z80.reti @ref Z80.hook and @ref
  * Z80.model.
  *
  * Callback              | Mandatory
  * --------------------- | ---------
  * @ref Z80.fetch_opcode | yes
  * @ref Z80.fetch        | yes
  * @ref Z80.read         | yes
  * @ref Z80.write        | yes
  * @ref Z80.in           | yes
  * @ref Z80.out          | yes
  * @ref Z80.halt         | no
  * @ref Z80.halt_nop     | no
  * @ref Z80.nmia         | yes
  * @ref Z80.inta         | yes
  * @ref Z80.int_fetch    | no
  * @ref Z80.ld_i_a       | no
  * @ref Z80.ld_r_a       | no
  * @ref Z80.reti         | no
  * @ref Z80.retn         | no
  * @ref Z80.hook         | no
  */

typedef struct {

	/** @brief Clock cycle counter.
	  *
	  * This clock cycle counter is updated as the emulator executes
	  * instructions and responds to different signals. */

	zusize cycles;

	/** @brief Maximum number of clock cycles to be executed in the current
	  * invokation of @ref z80_run. */

	zusize cycle_limit;

	/** @brief Pointer passed as the first argument to the callbacks.
	  *
	  * @details This member variable can be used to maintain a reference to
	  * the object or context to which the Z80 emulator object belongs. */

	void *context;

	/** @brief Callback used to perform opcode fetch operations.
	  *
	  * @details This type of operation is used by the CPU to read the first
	  * byte of an instruction or an instruction prefix. A succession of
	  * @c DDh and/or @c FDh prefixes will cause multiple consecutive such
	  * operations until an opcode is fetched. In the instructions with
	  * double prefix (@c CBDDh or @c CBFDh), only the prefixes are fetched
	  * with this operation, the remaining 2 bytes are accessed by using
	  * normal memory read operations.
	  *
	  * The emulator always incrementents the R register @b before calling
	  * this callback, but the value of its most-significant bit (R7) is not
	  * preserved for speed optimization reasons. In the rare case that the
	  * callback function needs to access that register, it must take R7
	  * from the 7th bit of the @c r7 member variable.
	  *
	  * Implementations that do not distinguish between opcode fetch and
	  * memory read operations should use the same callback function for
	  * both.
	  *
	  * @attention This callback is mandatory, initializing it to @c Z_NULL
	  * will cause the emulator to crash. */

	Z80Read fetch_opcode;

	/** @brief Callback used to perform memory read operations on instruction data.
	  *
	  * @attention This callback is mandatory, initializing it to @c Z_NULL
	  * will cause the emulator to crash. */

	Z80Read fetch;

	/** @brief Callback used to perform a memory read operation.
	  *
	  * @details This type of operation is used by the CPU to read a byte
	  * from a memory address.
	  *
	  * @attention This callback is mandatory, initializing it to @c Z_NULL
	  * will cause the emulator to crash. */

	Z80Read read;

	/** @brief Callback used to perform a memory write operation.
	  *
	  * @details This type of operation is used by the CPU to write a byte
	  * to a memory address.
	  *
	  * @attention This callback is mandatory, initializing it to @c Z_NULL
	  * will cause the emulator to crash. */

	Z80Write write;

	/** @brief Callback used to perform an I/O read operation.
	  *
	  * @details This type of operation is used by the CPU to read a byte
	  * from an I/O port.
	  *
	  * @attention This callback is mandatory, initializing it to @c Z_NULL
	  * will cause the emulator to crash. */

	Z80Read in;

	/** @brief Callback used to perform an I/O write operation.
	  *
	  * @details This type of operation is used by the CPU to write a byte
	  * to an I/O port.
	  *
	  * @attention This callback is mandatory, initializing it to @c Z_NULL
	  * will cause the emulator to crash. */

	Z80Write out;

	/** @brief Callback used to notify that the state of the HALT output
	  * line has changed.
	  *
	  * @details The @c HALT instruction halts the CPU by not incrementing
	  * the PC register, so the instruction is re-executed until an
	  * interrupt is accepted. Only then the PC register is incremented
	  * again and the next instruction is executed. The HALT output line is
	  * active (low) during the HALT state.
	  *
	  * The emulator invokes this callback after changing the value of the
	  * @c halt_line member variable and, when exiting the HALT state,
	  * immediately before executing the interrupt response.
	  *
	  * @attention This callback is optional and @b must be initialized to
	  * @c Z_NULL if not used. */

	Z80HALT halt;

	/** @brief Callback invoked... */

	Z80Read nop;

	/** @brief Callback invoked... */

	Z80Read nmia;

	/** @brief Callback used to notify a maskable interrupt acknowledge
	  * (INTA).
	  *
	  * @details When a maskable interrupt (INT) is accepted, the CPU
	  * generates a special M1 cycle to indicate that the interrupting I/O
	  * device can write to the data bus. Two wait clock cycles are added
	  * to this M1 cycle, allowing sufficient time to identify which device
	  * must insert the data.
	  *
	  * The emulator invokes this callback during the execution of the INT
	  * response. It should be used to identify and prepare the context of
	  * the interrupting device, so that subsequent invocations of the @c
	  * read_bus callback can read the interrupt response vector or the
	  * instruction to be executed.
	  *
	  * @note This callback is optional and @b must be initialized to @c
	  * Z_NULL if not used.

	  * @details When a maskable interrupt (INT) is accepted, the CPU reads
	  * the interrupt response data provided by the interrupting device via
	  * the data bus. Usually only one data bus read operation is performed
	  * during the INT acknowledge cycle, but when the CPU responds to the
	  * interrupt in mode 0, it will perform as many such operations as
	  * needed to read the byte sequence of the instruction to be executed.
	  *
	  * The emulator invokes this callback during the execution of the INT
	  * response. The return value is ignored in interrupt mode 1. In mode
	  * 0, the emulator invokes this callback as many times as needed to
	  * read a complete instruction. Illegal instructions and instruction
	  * prefix sequences are fully supported.
	  *
	  * @attention This callback is optional and @b must be initialized to
	  * @c Z_NULL if not used, in which case the emulator will assume that
	  * the byte read from the data bus is always @c FFh. */

	Z80Read inta;

	/** @brief Callback used to perform a data bus read operation during a
	  * maskable interrupt (INT) response.
	  **/

	Z80Read int_fetch;

	Z80Reset reset;

	/** @brief Callback invoked before executing the <tt>ld i,a</tt>
	  * instruction. */

	Z80Notify ld_i_a;

	/** @brief Callback invoked before executing the <tt>ld r,a</tt>
	  * instruction.
	  *
 	  * @attention This callback is optional and @b must be initialized to
	  * @c Z_NULL if not used. */

	Z80Notify ld_r_a;

	/** @brief Callback invoked before executing the @c reti instruction.
	  *
	  * The Z80 Counter/Timer Circuit (CTC) detects the two-byte @c reti
	  * opcode when it is fetched by the CPU. This instruction is used to
	  * return from an ISR and signal that the computer should initialize
	  * the daisy-chain enable lines for control of nested priority
	  * interrupt handling.
	  *
	  * Although the Z80 CTC is not part of the CPU, the emulator can signal
	  * the execution of a @c reti instruction by invoking this callback in
	  * order to simplify the emulation of machines that use daisy-chain
	  * interrupt servicing, thus avoiding having to implement the detection
	  * of this instruction externally, which is not optimal and would cause
	  * a speed penalty.
	  *
 	  * @attention This callback is optional and @b must be initialized to
	  * @c Z_NULL if not used. */

	Z80Notify reti;

	/** @brief Callback invoked before executing the @c retn instruction.

 	  * @attention This callback is optional and @b must be initialized to
	  * @c Z_NULL if not used. */

	Z80Notify retn;

	/** @brief Callback invoked...
	  *
 	  * @attention This callback is optional and @b must be initialized to
	  * @c Z_NULL if not used. */

	Z80Read hook;

	/** @brief Temporary storage used for instruction fetch. */

	ZInt32 data;

	/** @brief Temporay IX/IY register. */

	ZInt16 xy;

	ZInt16 memptr; 	/**< @brief MEMPTR register. */
	ZInt16 pc;      /**< @brief PC register.     */
	ZInt16 sp;      /**< @brief SP register.     */
	ZInt16 ix;      /**< @brief IX register.     */
	ZInt16 iy;      /**< @brief IY register.     */
	ZInt16 af;      /**< @brief AF register.     */
	ZInt16 bc;      /**< @brief BC register.     */
	ZInt16 de;      /**< @brief DE register.     */
	ZInt16 hl;      /**< @brief HL register.     */
	ZInt16 af_;     /**< @brief AF' register.    */
	ZInt16 bc_;     /**< @brief BC' register.    */
	ZInt16 de_;     /**< @brief DE' register.    */
	ZInt16 hl_;     /**< @brief HL' register.    */
	zuint8 r;       /**< @brief R register.      */
	zuint8 i;       /**< @brief I register.      */

	/** @brief The most significant bit of the R register (R7).
	  *
	  * @details The R register is incremented during each M1 cycle, but its
	  * most significant bit (R7) is not affected. For optimization reasons,
	  * this behavior is not emulated in every M1 cycle, so successive
	  * increments of R corrupt R7. @c z80_run keeps the value of R7 in this
	  * temporary member variable while executing operations and copies it
	  * back to R before returning. Since this variable is a snapshot of the
	  * R register at a given time, the value of the 7 least significant
	  * bits must be considered garbage. */

	zuint8 r7;

	/** @brief Maskable interrup mode.
	  *
	  * @details Contains the number of the maskable interrupt mode in use:
	  * `0`, `1` or `2`. */

	zuint8 im;

	/** @brief Number of signals pending to be processed. */

	zuint8 request;

	/** @brief TODO */

	zuint8 resume;

	zuint8 iff1;  /**< @brief Interrupt flip-flop 1 (IFF1). */
	zuint8 iff2;  /**< @brief Interrupt flip-flop 2 (IFF2). */
	zuint8 q;     /**< @brief Q register.                   */

	/** @brief CPU model.
	  *
	  * @details todo... */

	zuint8 options;

	/** @brief State of the INT line.
	  *
	  * @details Contains @c TRUE if the INT line is active, or @c FALSE
	  * otherwise. */

	zuint8 int_line;

	/** @brief State of the HALT line.
	  *
	  * @details Contains @c TRUE if the HALT line is active, or @c FALSE
	  * otherwise. The emulator always modifies this variable before
	  * invoking the @ref Z80.halt callback. */

	zuint8 halt_line;
} Z80;

#define Z80_MEMPTR(self)  (self).memptr.uint16_value
#define Z80_MEMPTRH(self) (self).memptr.uint8_values.at_1
#define Z80_MEMPTRL(self) (self).memptr.uint8_values.at_0
#define Z80_PC(self)	  (self).pc.uint16_value
#define Z80_SP(self)	  (self).sp.uint16_value
#define Z80_XY(self)	  (self).xy.uint16_value
#define Z80_IX(self)	  (self).ix.uint16_value
#define Z80_IY(self)	  (self).iy.uint16_value
#define Z80_AF(self)	  (self).af.uint16_value
#define Z80_BC(self)	  (self).bc.uint16_value
#define Z80_DE(self)	  (self).de.uint16_value
#define Z80_HL(self)	  (self).hl.uint16_value
#define Z80_AF_(self)	  (self).af_.uint16_value
#define Z80_BC_(self)	  (self).bc_.uint16_value
#define Z80_DE_(self)	  (self).de_.uint16_value
#define Z80_HL_(self)	  (self).hl_.uint16_value
#define Z80_PCH(self)	  (self).pc.uint8_values.at_1
#define Z80_PCL(self)	  (self).pc.uint8_values.at_0
#define Z80_SPH(self)	  (self).sp.uint8_values.at_1
#define Z80_SPL(self)	  (self).sp.uint8_values.at_0
#define Z80_XYH(self)	  (self).xy.uint8_values.at_1
#define Z80_XYL(self)	  (self).xy.uint8_values.at_0
#define Z80_IXH(self)	  (self).ix.uint8_values.at_1
#define Z80_IXL(self)	  (self).ix.uint8_values.at_0
#define Z80_IYH(self)	  (self).iy.uint8_values.at_1
#define Z80_IYL(self)	  (self).iy.uint8_values.at_0
#define Z80_A(self)	  (self).af.uint8_values.at_1
#define Z80_F(self)	  (self).af.uint8_values.at_0
#define Z80_B(self)	  (self).bc.uint8_values.at_1
#define Z80_C(self)	  (self).bc.uint8_values.at_0
#define Z80_D(self)	  (self).de.uint8_values.at_1
#define Z80_E(self)	  (self).de.uint8_values.at_0
#define Z80_H(self)	  (self).hl.uint8_values.at_1
#define Z80_L(self)	  (self).hl.uint8_values.at_0
#define Z80_A_(self)	  (self).af_.uint8_values.at_1
#define Z80_F_(self)	  (self).af_.uint8_values.at_0
#define Z80_B_(self)	  (self).bc_.uint8_values.at_1
#define Z80_C_(self)	  (self).bc_.uint8_values.at_0
#define Z80_D_(self)	  (self).de_.uint8_values.at_1
#define Z80_E_(self)	  (self).de_.uint8_values.at_0
#define Z80_H_(self)	  (self).hl_.uint8_values.at_1
#define Z80_L_(self)	  (self).hl_.uint8_values.at_0

#define Z80_SF 128
#define Z80_ZF  64
#define Z80_YF  32
#define Z80_HF  16
#define Z80_XF   8
#define Z80_PF   4
#define Z80_NF   2
#define Z80_CF   1

#define Z80_OPTION_LD_A_IR_BUG   1
#define Z80_OPTION_OUT_VC_255    2
#define Z80_OPTION_XQ		 8
#define Z80_OPTION_HALT_SKIP    16
#define Z80_OPTION_YQ		32

/** @brief Zilog Z80 NMOS version. */
#define Z80_MODEL_ZILOG_NMOS \
	(Z80_OPTION_LD_A_IR_BUG | Z80_OPTION_XQ | Z80_OPTION_YQ)

/** @brief Zilog Z80 CMOS version. */
#define Z80_MODEL_ZILOG_CMOS \
	(Z80_OPTION_OUT_VC_255 | Z80_OPTION_XQ | Z80_OPTION_YQ)

#define Z80_MODEL_NEC_NMOS \
	Z80_OPTION_LD_A_IR_BUG

#define Z80_MODEL_ST_CMOS \
	(Z80_OPTION_LD_A_IR_BUG | Z80_OPTION_YQ)


#define Z80_REQUEST_RESET	     3
#define Z80_REQUEST_REJECT_NMI	     4
#define Z80_REQUEST_NMI		     8 /**< NMI pending. */
#define Z80_REQUEST_CLEAR_PC	    16
#define Z80_REQUEST_SPECIAL_RESET   32
#define Z80_REQUEST_INT		    64 /**< INT pending. */
#define Z80_REQUEST_ANY_RESET       35


#define Z80_REQUEST_RESET	  3
#define Z80_REQUEST_INTERRUPT	  5

#define Z80_RESUME_HALT		     1 /**< Resume the execution of the HALT state. */
#define Z80_RESUME_XY		     2 /**< Resume at opcode decode stage in INT mode 0. */
#define Z80_RESUME_IM0_XY	     3 /**< Resume at opcode decode stage. */
#define Z80_RESUME_NORMAL_RESET	     4
#define Z80_RESUME_SPECIAL_RESET_XY  5
#define Z80_RESUME_SPECIAL_RESET_NOP 6

Z_EXTERN_C_BEGIN

/** @brief Sets the power state of a Z80 emulator.
  *
  * @param self Pointer to the object on which the function is called.
  * @param state
  *   @c TRUE  = power ON;
  *   @c FALSE = power OFF. */

Z80_API void z80_power(Z80 *self, zboolean state);

/** @brief Performs a normal RESET on a Z80 emulator.
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_instant_reset(Z80 *self);

/** @brief Sends a normal RESET signal to a Z80 emulator.
  *
  * @details todo
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_reset(Z80 *self);

/** @brief Sends a special RESET signal to a Z80 emulator.
  *
  * @details todo
  *
  * @sa
  * - http://www.primrosebank.net/computers/z80/z80_special_reset.htm
  * - US Patent 4486827
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_special_reset(Z80 *self);

/** @brief Sets the state of the INT line of a Z80 emulator.
  *
  * @param self Pointer to the object on which the function is called.
  * @param state
  *   @c TRUE  = ON  (line low);
  *   @c FALSE = OFF (line high). */

Z80_API void z80_int(Z80 *self, zboolean state);

/** @brief Sends a NMI signal to a Z80 emulator.
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_nmi(Z80 *self);

Z80_API void z80_busreq(Z80 *self, zboolean state);

/** @brief Runs a Z80 emulator for a given number of clock @p cycles, executing
  * only instructions without responding to signals.
  *
  * @details Given the fact that one Z80 instruction takes between 4 and 23
  * cycles to be executed, it is not always possible to run the CPU the exact
  * number of @p cycles specfified.
  *
  * @param self Pointer to the object on which the function is called.
  * @param cycles Number of clock cycles to be emulated.
  * @return The actual number of clock cycles emulated. */

Z80_API zusize z80_execute(Z80 *self, zusize cycles);

/** @brief Runs a Z80 emulator for a given number of clock @p cycles.
  *
  * @details Given the fact that one Z80 instruction takes between 4 and 23
  * cycles to be executed, it is not always possible to run the CPU the exact
  * number of @p cycles specfified.
  *
  * @param self Pointer to the object on which the function is called.
  * @param cycles Number of clock cycles to be emulated.
  * @return The actual number of clock cycles emulated. */

Z80_API zusize z80_run(Z80 *self, zusize cycles);


/** @brief Obtains the refresh address of the M1 cycle being executed by a Z80
  * emulator.
  *
  * @details todo
  *
  * @param self Pointer to the object on which the function is called.
  * @return todo */

static Z_INLINE zuint16 z80_refresh_address(Z80 *self)
	{return ((zuint16)self->i << 8) | ((self->r - 1) & 127);}


/** @brief Computes the clock cycle, relative to the start of the instruction,
  * at which the I/O read M-cycle being executed by a Z80 emulator begins.
  *
  * @details todo
  *
  * @param self Pointer to the object on which the function is called.
  * @return todo */

static Z_INLINE zuint8 z80_in_cycle(Z80 *self)
	{
	return self->data.uint8_array[0] == 0xDB
		? /* in a,(BYTE) : 4+3 */
		7
		: /* in J,(c) / in (c) : 4+4 */
		8
		+ /* ini / ind / inir / indr : 4+5 */
		(self->data.uint8_array[1] >> 7);
	}


/** @brief Computes the clock cycle, relative to the start of the instruction,
  * at which the I/O write M-cycle being executed by a Z80 emulator begins.
  *
  * @details todo
  *
  * @param self Pointer to the object on which the function is called.
  * @return todo */

static Z_INLINE zuint8 z80_out_cycle(Z80 *self)
	{
	return self->data.uint8_array[0] == 0xD3
		? /* out (BYTE),a : 4+3 */
		7
		: /* out (c),J / out (c),0 : 4+4 */
		8
		+ /* outi / outd / otir / otdr : 4+5+3 */
		((self->data.uint8_array[1] >> 7) << 2);
	}


Z_EXTERN_C_END

#endif /* Z80_H */
