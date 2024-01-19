/* Z80 API
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator                                   |
|  Copyright (C) 1999-2024 Manuel Sainz de Baranda y Goñi.                     |
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
  * of the Zilog Z80 that emulates all that is known to date about this CPU,
  * including the undocumented behaviors, MEMPTR, Q and the special RESET.
  *
  * @version 0.2
  * @date 2024
  * @author Manuel Sainz de Baranda y Goñi */

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

/** @brief Major version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MAJOR 0

/** @brief Minor version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MINOR 2

/** @brief Micro version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MICRO 0

/** @brief String literal with the version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_STRING "0.2"

/** @brief Maximum number of clock cycles that <tt>@ref z80_run</tt> and
  * <tt>@ref z80_execute</tt> can emulate. */

#define Z80_MAXIMUM_CYCLES (Z_USIZE_MAXIMUM - Z_USIZE(30))

/** @brief Maximum number of clock cycles that <tt>@ref z80_run</tt> will
  * emulate if instructed to execute 1 clock cycle.
  *
  * This is the number of clock cycles it takes to execute the longest
  * instruction through interrupt mode 0, not counting the M-cycle used to fetch
  * a @c 0xDD or @c 0xFD prefix. For <tt>@ref z80_execute</tt>, subtract 4 clock
  * cycles from this value. */

#define Z80_MAXIMUM_CYCLES_PER_STEP 25

/** @brief Minimum number of clock cycles that <tt>@ref z80_run</tt> or
  * <tt>@ref z80_execute</tt> will emulate if instructed to execute 1 clock
  * cycle. */

#define Z80_MINIMUM_CYCLES_PER_STEP 4

/** @brief Opcode interpreted as a trap by the Z80 library. It corresponds to
  * the <tt>ld h,h</tt> instruction in the Z80 ISA. */

#define Z80_HOOK 0x64

#define Z80_SF 128 /**< @brief Bitmask of the Z80 S flag.   */
#define Z80_ZF  64 /**< @brief Bitmask of the Z80 Z flag.   */
#define Z80_YF  32 /**< @brief Bitmask of the Z80 Y flag.   */
#define Z80_HF  16 /**< @brief Bitmask of the Z80 H flag.   */
#define Z80_XF   8 /**< @brief Bitmask of the Z80 X flag.   */
#define Z80_PF   4 /**< @brief Bitmask of the Z80 P/V flag. */
#define Z80_NF   2 /**< @brief Bitmask of the Z80 N flag.   */
#define Z80_CF   1 /**< @brief Bitmask of the Z80 C flag.   */

typedef struct Z80 Z80;

/** @brief Defines a pointer to a <tt>@ref Z80</tt> callback function invoked to
  * perform a read operation.
  *
  * @param context The <tt>@ref Z80::context</tt> of the calling object.
  * @param address The memory address or I/O port to read from.
  * @return The byte read. */

typedef zuint8 (* Z80Read)(void *context, zuint16 address);

/** @brief Defines a pointer to a <tt>@ref Z80</tt> callback function invoked to
  * perform a write operation.
  *
  * @param context The <tt>@ref Z80::context</tt> of the calling object.
  * @param address The memory address or I/O port to write to.
  * @param value The byte to write. */

typedef void (* Z80Write)(void *context, zuint16 address, zuint8 value);

/** @brief Defines a pointer to a <tt>@ref Z80</tt> callback function invoked to
  * notify a signal change on the HALT line.
  *
  * @param context The <tt>@ref Z80::context</tt> of the calling object.
  * @param signal A code specifying the type of signal change. */

typedef void (* Z80Halt)(void *context, zuint8 signal);

/** @brief Defines a pointer to a <tt>@ref Z80</tt> callback function invoked to
  * notify an event.
  *
  * @param context The <tt>@ref Z80::context</tt> of the calling object. */

typedef void (* Z80Notify)(void *context);

/** @brief Defines a pointer to a <tt>@ref Z80</tt> callback function invoked to
  * delegate the emulation of an illegal instruction.
  *
  * @param cpu The calling object.
  * @param opcode The illegal opcode.
  * @return The number of clock cycles consumed by the instruction. */

typedef zuint8 (* Z80Illegal)(Z80 *cpu, zuint8 opcode);

/** @struct Z80 Z80.h
  *
  * @brief A Z80 CPU emulator.
  *
  * A @c Z80 object contains the state of an emulated Z80 CPU, pointers to
  * callback functions that interconnect the emulator with the external logic
  * and a context that is passed to these functions.
  *
  * Because no constructor function is provided, it is mandatory to directly
  * initialize all callback pointers and <tt>@ref Z80::options</tt> before using
  * an object of this type. Optional callbacks must be set to @c Z_NULL when not
  * in use. */

struct Z80 {

	/** @brief Number of clock cycles already executed. */

	zusize cycles;

	/** @brief Maximum number of clock cycles to be executed. */

	zusize cycle_limit;

	/** @brief Pointer to pass as the first argument to all callback
	  * functions.
	  *
	  * This member is intended to hold a reference to the context to which
	  * the object belongs. It is safe not to initialize it when this is not
	  * necessary. */

	void *context;

	/** @brief Invoked to perform an opcode fetch.
	  *
	  * This callback indicates the beginning of an opcode fetch M-cycle.
	  * The function must return the byte located at the memory address
	  * specified by the second argument. */

	Z80Read fetch_opcode;

	/** @brief Invoked to perform a memory read on instruction data.
	  *
	  * This callback indicates the beginning of a memory read M-cycle
	  * during which the CPU fetches one byte of instruction data (i.e., one
	  * byte of the instruction that is neither a prefix nor an opcode). The
	  * function must return the byte located at the memory address
	  * specified by the second argument. */

	Z80Read fetch;

	/** @brief Invoked to perform a memory read.
	  *
	  * This callback indicates the beginning of a memory read M-cycle. The
	  * function must return the byte located at the memory address
	  * specified by the second argument. */

	Z80Read read;

	/** @brief Invoked to perform a memory write.
	  *
	  * This callback indicates the beginning of a memory write M-cycle. The
	  * function must write the third argument into the memory location
	  * specified by the second argument. */

	Z80Write write;

	/** @brief Invoked to perform an I/O port read.
	  *
	  * This callback indicates the beginning of an I/O read M-cycle. The
	  * function must return the byte read from the I/O port specified by
	  * the second argument. */

	Z80Read in;

	/** @brief Invoked to perform an I/O port write.
	  *
	  * This callback indicates the beginning of an I/O write M-cycle. The
	  * function must write the third argument to the I/O port specified by
	  * the second argument. */

	Z80Write out;

	/** @brief Invoked to notify a signal change on the HALT line.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. Its invocation is always deferred until the next emulation step
	  * so that the emulator can abort the signal change if any invalidating
	  * condition occurs, such as the acceptance of an interrupt during the
	  * execution of a @c halt instruction.
	  *
	  * The second parameter of the function specifies the type of signal
	  * change and can only contain a boolean value if the Z80 library has
	  * not been built with special RESET support:
	  *
	  * - @c 1 indicates that the HALT line is going low during the last
	  *   clock cycle of a @c halt instruction, which means that the CPU
	  *   is entering the HALT state.
	  *
	  * - @c 0 indicates that the HALT line is going high during the last
	  *   clock cycle of an internal NOP executed during the HALT state,
	  *   i.e., the CPU is exiting the HALT state due to an interrupt or
	  *   normal RESET.
	  *
	  * If the library has been built with special RESET support, the values
	  * <tt>@ref Z80_HALT_EXIT_EARLY</tt> and <tt>@ref Z80_HALT_CANCEL</tt>
	  * are also possible for the second parameter. */

	Z80Halt halt;

	/** @brief Invoked to perform an opcode fetch that corresponds to an
	  * internal NOP.
	  *
	  * This callback indicates the beginning of an opcode fetch M-cycle of
	  * 4 clock cycles that is generated in the following two cases:
	  *
	  * - During the HALT state, the CPU repeatedly executes an internal NOP
	  *   that fetches the next opcode after the @c halt instruction without
	  *   incrementing the PC register. This opcode is read again and again
	  *   until an exit condition occurs (i.e., NMI, INT or RESET).
	  *
	  * - After detecting a special RESET signal, the CPU completes the
	  *   ongoing instruction or interrupt response and then zeroes the PC
	  *   register during the first clock cycle of the next M1 cycle. If no
	  *   interrupt has been accepted at the end of the instruction or
	  *   interrupt response, the CPU produces an internal NOP to allow for
	  *   the fetch-execute overlap to take place, during which it fetches
	  *   the next opcode and zeroes PC.
	  *
	  * This callback is optional but note that setting it to @c Z_NULL is
	  * equivalent to enabling <tt>@ref Z80_OPTION_HALT_SKIP</tt>. */

	Z80Read nop;

	/** @brief Invoked to perform an opcode fetch that corresponds to a
	  * non-maskable interrupt acknowledge M-cycle.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. It indicates the beginning of an NMI acknowledge M-cycle. The
	  * value returned by the function is ignored. */

	Z80Read nmia;

	/** @brief Invoked to perform a data bus read that corresponds to a
	  * maskable interrupt acknowledge M-cycle.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. It indicates the beginning of an INT acknowledge M-cycle. The
	  * function must return the byte that the interrupting I/O device
	  * supplies to the CPU via the data bus during this M-cycle.
	  *
	  * When this callback is @c Z_NULL, the emulator assumes that the value
	  * read from the data bus is @c 0xFF. */

	Z80Read inta;

	/** @brief Invoked to perform a memory read on instruction data during a
	  * maskable interrupt response in mode 0.
	  *
	  * The role of this callback is analogous to that of
	  * <tt>@ref Z80::fetch</tt>, but it is specific to the INT response in
	  * mode 0. Ideally, the function should return a byte of instruction
	  * data that the interrupting I/O device supplies to the CPU via the
	  * data bus, but depending on the emulated hardware, the device may not
	  * be able to do this during a memory read M-cycle because the memory
	  * is addressed instead, in which case the function must return the
	  * byte located at the memory address specified by the second
	  * parameter.
	  *
	  * This callback will only be invoked if <tt>@ref Z80::inta</tt> is not
	  * @c Z_NULL and returns an opcode that implies subsequent memory read
	  * M-cycles to fetch the non-opcode bytes of the instruction, so it is
	  * safe not to initialize it or set it to @c Z_NULL if such a scenario
	  * is not possible. */

	Z80Read int_fetch;

	/** @brief Invoked to notify that an <tt>ld i,a</tt> instruction has
	  * been fetched.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. It is invoked before executing the instruction. */

	Z80Notify ld_i_a;

	/** @brief Invoked to notify that an <tt>ld r,a</tt> instruction has
	  * been fetched.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. It is invoked before executing the instruction. */

	Z80Notify ld_r_a;

	/** @brief Invoked to notify that a @c reti instruction has been
	  * fetched.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. It is invoked before executing the instruction. */

	Z80Notify reti;

	/** @brief Invoked to notify that a @c retn instruction has been
	  * fetched.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. It is invoked before executing the instruction. */

	Z80Notify retn;

	/** @brief Invoked when a trap is fetched.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use, in which case the opcode of the trap will be executed normally.
	  * The function receives the memory address of the trap as the second
	  * parameter and must return the opcode to be executed instead of the
	  * trap. If the function returns a trap (i.e., <tt>@ref Z80_HOOK</tt>),
	  * the emulator will do nothing, so the trap will be fetched again
	  * unless the function has modified <tt>@ref Z80::pc</tt> or replaced
	  * the trap in memory with another opcode. Also note that returning a
	  * trap does not revert the increment of <tt>@ref Z80::r</tt> performed
	  * before each opcode fetch. */

	Z80Read hook;

	/** @brief Invoked to delegate the execution of an illegal instruction.
	  *
	  * This callback is optional and must be set to @c Z_NULL when not in
	  * use. Only those instructions with the @c 0xED prefix that behave the
	  * same as two consecutive @c nop instructions are considered illegal.
	  * The function receives the illegal opcode as the second parameter and
	  * must return the number of clock cycles taken by the instruction.
	  *
	  * At the time of invoking this callback, and relative to the start of
	  * the instruction, only <tt>@ref Z80::r</tt> has been incremented
	  * (twice), so <tt>@ref Z80::pc</tt> still contains the memory address
	  * of the @c 0xED prefix. */

	Z80Illegal illegal;

	/** @brief Temporary storage used for instruction fetch. */

	ZInt32 data;

	ZInt16 ix_iy[2]; /**< @brief Index registers, IX and IY.    */
	ZInt16 pc;       /**< @brief Register PC (program counter). */
	ZInt16 sp;       /**< @brief Register SP (stack pointer).   */

	/** @brief Temporary index register.
	  *
	  * All instructions with the @c 0xDD prefix behave exactly the same as
	  * their counterparts with the @c 0xFD prefix, differing only in the
	  * index register: the former use IX, whereas the latter use IY. When
	  * one of these prefixes is fetched, the corresponding index register
	  * is copied into this member; the instruction logic is then executed
	  * and finally this member is copied back into the index register. */

	ZInt16 xy;

	ZInt16 memptr; /**< @brief Register MEMPTR, also known as WZ.        */
	ZInt16 af;     /**< @brief Register pair AF (accumulator and flags). */
	ZInt16 bc;     /**< @brief Register pair BC.                         */
	ZInt16 de;     /**< @brief Register pair DE.                         */
	ZInt16 hl;     /**< @brief Register pair HL.                         */
	ZInt16 af_;    /**< @brief Register pair AF'.                        */
	ZInt16 bc_;    /**< @brief Register pair BC'.                        */
	ZInt16 de_;    /**< @brief Register pair DE'.                        */
	ZInt16 hl_;    /**< @brief Register pair HL'.                        */
	zuint8 r;      /**< @brief Register R (memory refresh).              */
	zuint8 i;      /**< @brief Register I (interrupt vector base).       */

	/** @brief Backup of bit 7 of the R register.
	  *
	  * The Z80 CPU increments the R register during each M1 cycle without
	  * altering its most significant bit, commonly known as R7. However,
	  * the emulator only performs normal full-byte increments for speed
	  * reasons, which eventually corrupts R7.
	  *
	  * Before entering the execution loop, both <tt>@ref z80_execute</tt>
	  * and <tt>@ref z80_run</tt> copy <tt>@ref Z80::r</tt> into this member
	  * to preserve the value of R7, so that they can restore it before
	  * returning. The emulation of the <tt>ld r, a</tt> instruction also
	  * updates the value of this member. */

	zuint8 r7;

	/** @brief Maskable interrupt mode.
	  *
	  * Contains the number of the maskable interrupt mode in use: @c 0,
	  * @c 1 or @c 2. */

	zuint8 im;

	/** @brief Requests pending to be responded. */

	zuint8 request;

	/** @brief Type of unfinished operation to be resumed. */

	zuint8 resume;

	zuint8 iff1; /**< @brief Interrupt enable flip-flop #1 (IFF1). */
	zuint8 iff2; /**< @brief Interrupt enable flip-flop #2 (IFF2). */
	zuint8 q;    /**< @brief Pseudo-register Q. */

	/** @brief Emulation options.
	  *
	  * This member specifies the different emulation options that are
	  * enabled. It is mandatory to initialize it before using the emulator.
	  * Setting it to @c 0 disables all options. */

	zuint8 options;

	/** @brief State of the INT line.
	  *
	  * The value of this member is @c 1 if the INT line is low; otherwise,
	  * @c 0. */

	zuint8 int_line;

	/** @brief State of the HALT line.
	  *
	  * The value of this member is @c 1 if the HALT line is low; otherwise,
	  * @c 0. The emulator updates this member before invoking
	  * <tt>@ref Z80::halt</tt>, not after. */

	zuint8 halt_line;
};

/** @brief <tt>@ref Z80::options</tt> bitmask that enables emulation of the
  * <tt>out (c),255</tt> instruction, specific to the Zilog Z80 CMOS. */

#define Z80_OPTION_OUT_VC_255 1

/** @brief <tt>@ref Z80::options</tt> bitmask that enables emulation of the bug
  * affecting the Zilog Z80 NMOS, which causes the P/V flag to be reset when a
  * maskable interrupt is accepted during the execution of the
  * <tt>ld a,{i|r}</tt> instructions. */

#define Z80_OPTION_LD_A_IR_BUG 2

/** @brief <tt>@ref Z80::options</tt> bitmask that enables the HALTskip
  * optimization. */

#define Z80_OPTION_HALT_SKIP 4

/** @brief <tt>@ref Z80::options</tt> bitmask that enables the XQ factor in the
  * emulation of the @c ccf and @c scf instructions. */

#define Z80_OPTION_XQ 8

/** @brief <tt>@ref Z80::options</tt> bitmask that enables notifications for any
  * @c reti or @c retn instruction executed during the interrupt mode 0
  * response. */

#define Z80_OPTION_IM0_RETX_NOTIFICATIONS 16

/** @brief <tt>@ref Z80::options</tt> bitmask that enables the YQ factor in the
  * emulation of the @c ccf and @c scf instructions. */

#define Z80_OPTION_YQ 32

/** @brief <tt>@ref Z80::options</tt> bitmask that enables full emulation of the
  * Zilog NMOS models. */

#define Z80_MODEL_ZILOG_NMOS \
	(Z80_OPTION_LD_A_IR_BUG | Z80_OPTION_XQ | Z80_OPTION_YQ)

/** @brief <tt>@ref Z80::options</tt> bitmask that enables full emulation of the
  * Zilog CMOS models. */

#define Z80_MODEL_ZILOG_CMOS \
	(Z80_OPTION_OUT_VC_255 | Z80_OPTION_XQ | Z80_OPTION_YQ)

/** @brief <tt>@ref Z80::options</tt> bitmask that enables full emulation of the
  * NEC NMOS models. */

#define Z80_MODEL_NEC_NMOS \
	Z80_OPTION_LD_A_IR_BUG

/** @brief <tt>@ref Z80::options</tt> bitmask that enables full emulation of the
  * ST CMOS models. */

#define Z80_MODEL_ST_CMOS \
	(Z80_OPTION_OUT_VC_255 | Z80_OPTION_LD_A_IR_BUG | Z80_OPTION_YQ)

/** @brief <tt>@ref Z80::request</tt> bitmask that prevents the NMI signal from
  * being accepted. */

#define Z80_REQUEST_REJECT_NMI 2

/** @brief <tt>@ref Z80::request</tt> bitmask indicating that an NMI signal has
  * been received. */

#define Z80_REQUEST_NMI 4

/** @brief <tt>@ref Z80::request</tt> bitmask indicating that the INT line is
  * low and interrupts are enabled. */

#define Z80_REQUEST_INT 8

/** @brief <tt>@ref Z80::request</tt> bitmask indicating that a special RESET
  * signal has been received. */

#define Z80_REQUEST_SPECIAL_RESET 16

/** @brief <tt>@ref Z80::resume</tt> value indicating that the emulator ran out
  * of clock cycles during the HALT state. */

#define Z80_RESUME_HALT 1

/** @brief <tt>@ref Z80::resume</tt> value indicating that the emulator ran out
  * of clock cycles by fetching a prefix @c 0xDD or @c 0xFD. */

#define Z80_RESUME_XY 2

/** @brief <tt>@ref Z80::resume</tt> value indicating that the emulator ran out
  * of clock cycles by fetching a prefix @c 0xDD or @c 0xFD, during a maskable
  * interrupt response in mode 0. */

#define Z80_RESUME_IM0_XY 3

/** @brief Value of the second parameter of <tt>@ref Z80::halt</tt> when the
  * HALT line goes high due to a special RESET signal. */

#define Z80_HALT_EXIT_EARLY 2

/** @brief Value of the second paratemer of <tt>@ref Z80::halt</tt> when the
  * HALT line goes low and then high due to a special RESET signal during the
  * execution of a @c halt instruction. */

#define Z80_HALT_CANCEL 3

/** @brief Accesses the MEMPTR register of a <tt>@ref Z80</tt> @p object. */

#define Z80_MEMPTR(object) (object).memptr.uint16_value

/** @brief Accesses the PC register of a <tt>@ref Z80</tt> @p object. */

#define Z80_PC(object) (object).pc.uint16_value

/** @brief Accesses the SP register of a <tt>@ref Z80</tt> @p object. */

#define Z80_SP(object) (object).sp.uint16_value

/** @brief Accesses the temporary index register of a <tt>@ref Z80</tt> @p
  * object */

#define Z80_XY(object) (object).xy.uint16_value

/** @brief Accesses the IX register of a <tt>@ref Z80</tt> @p object. */

#define Z80_IX(object) (object).ix_iy[0].uint16_value

/** @brief Accesses the IY register of a <tt>@ref Z80</tt> @p object. */

#define Z80_IY(object) (object).ix_iy[1].uint16_value

/** @brief Accesses the AF register of a <tt>@ref Z80</tt> @p object. */

#define Z80_AF(object) (object).af.uint16_value

/** @brief Accesses the BC register of a <tt>@ref Z80</tt> @p object. */

#define Z80_BC(object) (object).bc.uint16_value

/** @brief Accesses the DE register of a <tt>@ref Z80</tt> @p object. */

#define Z80_DE(object) (object).de.uint16_value

/** @brief Accesses the HL register of a <tt>@ref Z80</tt> @p object. */

#define Z80_HL(object) (object).hl.uint16_value

/** @brief Accesses the AF' register of a <tt>@ref Z80</tt> @p object. */

#define Z80_AF_(object) (object).af_.uint16_value

/** @brief Accesses the BC' register of a <tt>@ref Z80</tt> @p object. */

#define Z80_BC_(object) (object).bc_.uint16_value

/** @brief Accesses the DE' register of a <tt>@ref Z80</tt> @p object. */

#define Z80_DE_(object) (object).de_.uint16_value

/** @brief Accesses the HL' register of a <tt>@ref Z80</tt> @p object. */

#define Z80_HL_(object) (object).hl_.uint16_value

/** @brief Accesses the most significant byte of the MEMPTR register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_MEMPTRH(object) (object).memptr.uint8_values.at_1

/** @brief Accesses the least significant byte of the MEMPTR register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_MEMPTRL(object) (object).memptr.uint8_values.at_0

/** @brief Accesses the most significant byte of the PC register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_PCH(object) (object).pc.uint8_values.at_1

/** @brief Accesses the least significant byte of the PC register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_PCL(object) (object).pc.uint8_values.at_0

/** @brief Accesses the most significant byte of the SP register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_SPH(object) (object).sp.uint8_values.at_1

/** @brief Accesses the least significant byte of the SP register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_SPL(object) (object).sp.uint8_values.at_0

/** @brief Accesses the most significant byte of the temporary index register
  * of a <tt>@ref Z80</tt> @p object. */

#define Z80_XYH(object) (object).xy.uint8_values.at_1

/** @brief Accesses the least significant byte of the temporary index register
  * of a <tt>@ref Z80</tt> @p object. */

#define Z80_XYL(object) (object).xy.uint8_values.at_0

/** @brief Accesses the IXH register of a <tt>@ref Z80</tt> @p object. */

#define Z80_IXH(object) (object).ix_iy[0].uint8_values.at_1

/** @brief Accesses the IXL register of a <tt>@ref Z80</tt> @p object. */

#define Z80_IXL(object) (object).ix_iy[0].uint8_values.at_0

/** @brief Accesses the IYH register of a <tt>@ref Z80</tt> @p object. */

#define Z80_IYH(object) (object).ix_iy[1].uint8_values.at_1

/** @brief Accesses the IYL register of a <tt>@ref Z80</tt> @p object. */

#define Z80_IYL(object) (object).ix_iy[1].uint8_values.at_0

/** @brief Accesses the A register of a <tt>@ref Z80</tt> @p object. */

#define Z80_A(object) (object).af.uint8_values.at_1

/** @brief Accesses the F register of a <tt>@ref Z80</tt> @p object. */

#define Z80_F(object) (object).af.uint8_values.at_0

/** @brief Accesses the B register of a <tt>@ref Z80</tt> @p object. */

#define Z80_B(object) (object).bc.uint8_values.at_1

/** @brief Accesses the C register of a <tt>@ref Z80</tt> @p object. */

#define Z80_C(object) (object).bc.uint8_values.at_0

/** @brief Accesses the D register of a <tt>@ref Z80</tt> @p object. */

#define Z80_D(object) (object).de.uint8_values.at_1

/** @brief Accesses the E register of a <tt>@ref Z80</tt> @p object. */

#define Z80_E(object) (object).de.uint8_values.at_0

/** @brief Accesses the H register of a <tt>@ref Z80</tt> @p object. */

#define Z80_H(object) (object).hl.uint8_values.at_1

/** @brief Accesses the L register of a <tt>@ref Z80</tt> @p object. */

#define Z80_L(object) (object).hl.uint8_values.at_0

/** @brief Accesses the most significant byte of the AF' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_A_(object) (object).af_.uint8_values.at_1

/** @brief Accesses the least significant byte of the AF' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_F_(object) (object).af_.uint8_values.at_0

/** @brief Accesses the most significant byte of the BC' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_B_(object) (object).bc_.uint8_values.at_1

/** @brief Accesses the least significant byte of the BC' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_C_(object) (object).bc_.uint8_values.at_0

/** @brief Accesses the most significant byte of the DE' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_D_(object) (object).de_.uint8_values.at_1

/** @brief Accesses the least significant byte of the DE' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_E_(object) (object).de_.uint8_values.at_0

/** @brief Accesses the most significant byte of the HL' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_H_(object) (object).hl_.uint8_values.at_1

/** @brief Accesses the least significant byte of the HL' register of a
  * <tt>@ref Z80</tt> @p object. */

#define Z80_L_(object) (object).hl_.uint8_values.at_0

#define Z80_WZ  Z80_MEMPTR  /**< @brief Same as <tt>@ref Z80_MEMPTR</tt>.  */
#define Z80_WZH Z80_MEMPTRH /**< @brief Same as <tt>@ref Z80_MEMPTRH</tt>. */
#define Z80_WZL Z80_MEMPTRL /**< @brief Same as <tt>@ref Z80_MEMPTRL</tt>. */

Z_EXTERN_C_BEGIN

/** @brief Sets the power state of a <tt>@ref Z80</tt>.
  *
  * @param self Pointer to the object on which the function is called.
  * @param state
  *   @c Z_TRUE  = power on;
  *   @c Z_FALSE = power off. */

Z80_API void z80_power(Z80 *self, zboolean state);

/** @brief Performs an instantaneous normal RESET on a <tt>@ref Z80</tt>.
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_instant_reset(Z80 *self);

/** @brief Sends a special RESET signal to a <tt>@ref Z80</tt>.
  *
  * @sa
  * - http://www.primrosebank.net/computers/z80/z80_special_reset.htm
  * - US Patent 4486827
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_special_reset(Z80 *self);

/** @brief Sets the state of the INT line of a <tt>@ref Z80</tt>.
  *
  * @param self Pointer to the object on which the function is called.
  * @param state
  *   @c Z_TRUE  = set line low;
  *   @c Z_FALSE = set line high. */

Z80_API void z80_int(Z80 *self, zboolean state);

/** @brief Triggers the NMI line of a <tt>@ref Z80</tt>.
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_nmi(Z80 *self);

/** @brief Runs a <tt>@ref Z80</tt> for a given number of clock @p cycles,
  * executing only instructions without responding to signals.
  *
  * @param self Pointer to the object on which the function is called.
  * @param cycles Number of clock cycles to be emulated.
  * @return The actual number of clock cycles emulated. */

Z80_API zusize z80_execute(Z80 *self, zusize cycles);

/** @brief Runs a <tt>@ref Z80</tt> for a given number of clock @p cycles.
  *
  * @param self Pointer to the object on which the function is called.
  * @param cycles Number of clock cycles to be emulated.
  * @return The actual number of clock cycles emulated. */

Z80_API zusize z80_run(Z80 *self, zusize cycles);


/** @brief Ends the emulation loop of <tt>@ref z80_execute</tt> or
  * <tt>@ref z80_run</tt>.
  *
  * This function should only be used inside callback functions. It zeroes
  * <tt>@ref Z80::cycle_limit</tt>, thus breaking the emulation loop after the
  * ongoing emulation step has finished executing.
  *
  * @param self Pointer to the object on which the function is called. */

static Z_ALWAYS_INLINE void z80_break(Z80 *self)
	{self->cycle_limit = 0;}


/** @brief Gets the full value of the R register of a <tt>@ref Z80</tt>.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The value of the R register. */

static Z_ALWAYS_INLINE zuint8 z80_r(Z80 const *self)
	{return (self->r & 127) | (self->r7 & 128);}


/** @brief Obtains the refresh address of the M1 cycle being executed by a
  * <tt>@ref Z80</tt>.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The refresh address. */

static Z_ALWAYS_INLINE zuint16 z80_refresh_address(Z80 const *self)
	{
	return Z_CAST(zuint16)(
		((zuint16)self->i << 8) |
		((self->r - 1) & 127)   |
		(self->r7 & 128));
	}


/** @brief Obtains the clock cycle, relative to the start of the instruction, at
  * which the I/O read M-cycle being executed by a <tt>@ref Z80</tt> begins.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The clock cycle at which the I/O read M-cycle begins. */

static Z_ALWAYS_INLINE zuint8 z80_in_cycle(Z80 const *self)
	{
	return Z_CAST(zuint8)(self->data.uint8_array[0] == 0xDB
		? /* in a,(BYTE) : 4+3 */
		7
		: /* in J,(c) / in (c) : 4+4 */
		8
		+ /* ini / ind / inir / indr : 4+5 */
		(self->data.uint8_array[1] >> 7));
	}


/** @brief Obtains the clock cycle, relative to the start of the instruction, at
  * which the I/O write M-cycle being executed by a <tt>@ref Z80</tt> begins.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The clock cycle at which the I/O write M-cycle begins. */

static Z_ALWAYS_INLINE zuint8 z80_out_cycle(Z80 const *self)
	{
	return Z_CAST(zuint8)(self->data.uint8_array[0] == 0xD3
		? /* out (BYTE),a : 4+3 */
		7
		: /* out (c),J / out (c),0 : 4+4 */
		8
		+ /* outi / outd / otir / otdr : 4+5+3 */
		((self->data.uint8_array[1] >> 7) << 2));
	}


Z_EXTERN_C_END

#endif /* Z80_H */
