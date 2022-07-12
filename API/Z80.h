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
  * including the undocumented behaviour, MEMPTR, Q and the special RESET.
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

/** @brief Major version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MAJOR 0

/** @brief Minor version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MINOR 2

/** @brief Micro version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_MICRO 0

/** @brief String literal with the version number of the Z80 library. */

#define Z80_LIBRARY_VERSION_STRING "0.2"

/** @brief Maximum number of clock cycles that the @ref z80_run and @ref
  * z80_execute functions can emulate per call. */

#define Z80_CYCLE_LIMIT (Z_USIZE_MAXIMUM - Z_USIZE(30))

#define Z80_CYCLES_PER_RESET 5

/** @brief Opcode interpreted as a hook by the Z80 library, which
  * corresponds to the <tt>ld h,h</tt> instruction of the Z80 ISA. */

#define Z80_HOOK Z_UINT8(0x64)

/** @brief Defines a pointer to a @ref Z80 callback function invoked to perform
  * a read operation.
  *
  * @param context The value of the @ref Z80::context member of the calling
  * object.
  * @param address The memory address or I/O port to read from.
  * @return The byte read. */

typedef zuint8 (* Z80Read)(void *context, zuint16 address);

/** @brief Defines a pointer to a @ref Z80 callback function invoked to perform
  * a write operation.
  *
  * @param context The value of the @ref Z80::context member of the calling
  * object.
  * @param address The memory address or I/O port to write to.
  * @param value The byte to write. */

typedef void (* Z80Write)(void *context, zuint16 address, zuint8 value);

/** @brief Defines a pointer to a @ref Z80 callback function invoked to notify
  * a signal change on the HALT line.
  *
  * @param context The value of the @ref Z80::context member of the calling
  * object.
  * @param state
  *     @c TRUE  if the HALT line goes low  (the CPU enters the HALT state);
  *     @c FALSE if the HALT line goes high (the CPU exits the HALT state). */

typedef void (* Z80HALT)(void *context, zboolean state);

/** @brief Defines a pointer to a @ref Z80 callback function invoked to notify
  * an event.
  *
  * @param context The value of the @ref Z80::context member of the calling
  * object. */

typedef void (* Z80Notify)(void *context);

/** @brief Defines a pointer to a @ref Z80 callback function invoked to delegate
  * the emulation of an illegal instruction.
  *
  * @param context The value of the @ref Z80::context member variable of the
  * calling object.
  * @param opcode The illegal opcode.
  * @return The number of clock cycles consumed by the instruction. */

typedef zuint8 (* Z80Illegal)(void *context, zuint8 opcode);

/** @brief Defines a pointer to a @ref Z80 callback function invoked to obtain
  * the duration of a RESET signal.
  *
  * @param context The value of the @ref Z80::context member of the calling
  * object.
  * @param address The value in the address bus when the RESET signal begins.
  * @return The number of clock cycles that the RESET signal lasts. */

typedef zusize (* Z80Reset)(void *context, zuint16 address);

/** @struct Z80 Z80.h
  *
  * @brief A Z80 CPU emulator.
  *
  * @details @c Z80 contains the state of an emulated Z80 CPU and the callback
  * pointers needed to interconnect it with the external logic. */

typedef struct {

	/** @brief Number of clock cycles already executed. */

	zusize cycles;

	/** @brief Maximum number of clock cycles to be executed. */

	zusize cycle_limit;

	/** @brief Pointer to pass as first argument to the callback functions.
	  *
	  * This member is intended to hold a reference to the context to which
	  * the object belongs. It is safe not to initialize it when this is not
	  * necessary. */

	void *context;

	/** @brief Invoked to perform an opcode fetch.
	  *
	  * This callback indicates the beginning of an opcode fetch M-cycle.
	  * The function must return the byte located at the memory address
	  * specified by the second parameter. */

	Z80Read fetch_opcode;

	/** @brief Invoked to perform a memory read on instruction data.
	  *
	  * This callback indicates the beginning of a memory read M-cycle
	  * during which the CPU fetches one byte of instruction data (i.e., one
	  * byte of the instruction that is neither a prefix nor an opcode). The
	  * function must return the byte located at the memory address
	  * specified by the second parameter. */

	Z80Read fetch;

	/** @brief Invoked to perform a memory read.
	  *
	  * This callback indicates the beginning of a memory read M-cycle. The
	  * function must return the byte located at the memory address
	  * specified by the second parameter. */

	Z80Read read;

	/** @brief Invoked to peform a memory write.
	  *
	  * This callback indicates the beginning of a memory write M-cycle. The
	  * function must write the value of the third parameter into the memory
	  * location specified by the second parameter. */

	Z80Write write;

	/** @brief Invoked to perform an I/O port read.
	  *
	  * This callback indicates the beginning of an I/O read M-cycle. The
	  * function must return the byte read from the I/O port specified by
	  * the second parameter. */

	Z80Read in;

	/** @brief Invoked to perform an I/O port write.
	  *
	  * This callback indicates the beginning of an I/O write M-cycle. The
	  * function must write the value of the third parameter to the I/O port
	  * specified by the second parameter. */

	Z80Write out;

	/** @brief Invoked when the state of the HALT line changes.
	  *
	  * This callback indicates that the CPU is entering or exiting the HALT
	  * state. It is invoked after updating the value of @ref Z80::halt_line,
	  * which is passed as the second parameter to the function.
	  *
	  * When exiting the HALT state, this callback is invoked before @ref
	  * Z80::inta, @ref Z80::reti or @ref Z80::reset. */

	Z80HALT halt;

	/** @brief Invoked to perform an opcode fetch that corresponds to an
	  * internal NOP.
	  *
	  * This callback indicates the beginning of an internal NOP, which is
	  * an opcode fetch M-cycle that is generated in two situations:
	  *
	  * - During the HALT state, the CPU repeatedly executes an internal NOP
	  *   that fetches the next opcode after @c halt without incrementing
	  *   the PC register. This opcode is read again and again until an exit
	  *   condition occurs.
	  *
	  * - After detecting a special reset signal, the CPU completes the
	  *   ongoing instruction and then executes an internal NOP during which
	  *   it zeroes the PC register.
	  *
	  * The role of this callback is analogous to the role of @ref
	  * Z80::fetch_opcode but the function is free to return any value
	  * (since the opcode is disregarded).
	  *
	  * This callback is optional. Setting it to @c Z_NULL is equivalent to
	  * setting the @ref Z80_OPTION_HALT_SKIP option. */

	Z80Read nop;

	/** @brief Invoked to perform an opcode fetch that correspond to a
	  * non-maskable interrupt acknowledge M-cycle.
	  *
 	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Read nmia;

	/** @brief Invoked to perform the data bus read of a maskable interrupt
	  * acknowledge M-cycle.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Read inta;

	/** @brief Callback invoked to perform a memory read on instruction data
	  * during a maskable interrupt response in mode 0.
	  *
	  * @attention This callback becomes mandatory when the @c Z80::inta
	  * callback is used. Setting it to @c Z_NULL will cause the program to
	  * crash. */

	Z80Read int_fetch;

	/** @brief Callback invoked to query the duration of a RESET signal.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Reset reset;

	/** @brief Invoked when an <tt>ld i,a</tt> instruction is fetched.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Notify ld_i_a;

	/** @brief Invoked when an <tt>ld r,a</tt> instruction is fetched.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Notify ld_r_a;

	/** @brief Invoked when a @c reti instruction is fetched.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Notify reti;

	/** @brief Callback invoked when a @c retn instruction is fetched.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Notify retn;

	/** @brief Invoked when a trap is fecthed.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Read hook;

	/** @brief Invoked to delegate the emulation of an illegal opcode.
	  *
	  * @attention This callback is optional and must be set to @c Z_NULL
	  * when not used. */

	Z80Illegal illegal;

	/** @brief Temporary storage used for instruction fetch. */

	ZInt32 data;

	/** @brief Temporary IX/IY register.
	  *
	  * All instructions with @c DDh prefix behave in exactly the same way
	  * as their counterparts with @c FDh prefix, differing only in which
	  * index register is used. This allows optimizing the size of the Z80
	  * library by using a temporary index register, which avoids duplicate
	  * code.
	  *
	  * When a @c DDh or @c FDh prefix is fetched, the corresponding index
	  * register is copied into this member. The instruction logic is then
	  * executed and finally this member is copied back into the index
	  * register. */

	ZInt16 xy;

	ZInt16 memptr; /**< @brief MEMPTR register. */
	ZInt16 pc;     /**< @brief PC register.     */
	ZInt16 sp;     /**< @brief SP register.     */
	ZInt16 ix;     /**< @brief IX register.     */
	ZInt16 iy;     /**< @brief IY register.     */
	ZInt16 af;     /**< @brief AF register.     */
	ZInt16 bc;     /**< @brief BC register.     */
	ZInt16 de;     /**< @brief DE register.     */
	ZInt16 hl;     /**< @brief HL register.     */
	ZInt16 af_;    /**< @brief AF' register.    */
	ZInt16 bc_;    /**< @brief BC' register.    */
	ZInt16 de_;    /**< @brief DE' register.    */
	ZInt16 hl_;    /**< @brief HL' register.    */
	zuint8 r;      /**< @brief R register.      */
	zuint8 i;      /**< @brief I register.      */

	/** @brief The most significant bit of the R register.
	  *
	  * The Z80 CPU increments the R register during each M1 cycle without
	  * altering its most significant bit, commonly known as R7. However, 
	  * the Z80 library performs normal increments for speed reasons, which
	  * eventually corrupts R7.
	  *
	  * Before entering the execution loop, the @ref z80_execute and @ref
	  * z80_run functions copy @ref Z80::r into this member to preserve the
	  * value of R7, so that it can be restored before returning. The
	  * emulation of the <tt>ld r, a</tt> instruction also updates the value
	  * of this member. */

	zuint8 r7;

	/** @brief Maskable interrup mode.
	  *
	  * Contains the number of the maskable interrupt mode in use: @c 0,
	  * @c 1 or @c 2. */

	zuint8 im;

	/** @brief Requests pending to be responded. */

	zuint8 request;

	/** @brief Type of unfinished operation to be resumed. */

	zuint8 resume;

	zuint8 iff1;  /**< @brief Interrupt enable flip-flop 1 (IFF1). */
	zuint8 iff2;  /**< @brief Interrupt enable flip-flop 2 (IFF2). */
	zuint8 q;     /**< @brief Q "register". */

	/** @brief Emulation options. */

	zuint8 options;

	/** @brief State of the INT line.
	  *
	  * The value of this member is @c TRUE if the INT line is low;
	  * otherwise, @c FALSE. */

	zuint8 int_line;

	/** @brief State of the HALT line.
	  *
	  * The value of this member is @c TRUE if the HALT line is low;
	  * otherwise, @c FALSE. The emulator always updates this member before
	  * invoking the @ref Z80::halt callback. */

	zuint8 halt_line;
} Z80;

#define Z80_SF 128 /**< @brief Bitmask of the @ref Z80 S   flag. */
#define Z80_ZF  64 /**< @brief Bitmask of the @ref Z80 Z   flag. */
#define Z80_YF  32 /**< @brief Bitmask of the @ref Z80 Y   flag. */
#define Z80_HF  16 /**< @brief Bitmask of the @ref Z80 H   flag. */
#define Z80_XF   8 /**< @brief Bitmask of the @ref Z80 X   flag. */
#define Z80_PF   4 /**< @brief Bitmask of the @ref Z80 P/V flag. */
#define Z80_NF   2 /**< @brief Bitmask of the @ref Z80 N   flag. */
#define Z80_CF   1 /**< @brief Bitmask of the @ref Z80 C   flag. */

/** @brief @ref Z80 option that enables the HALTskip optimization. */

#define Z80_OPTION_HALT_SKIP 16

/** @brief @ref Z80 option that enables emulation of the bug affecting the Zilog
  * Z80 NMOS, which causes the P/V flag to be reset when a maskable interrupt is
  * accepted during the execution of the <tt>ld a,{i|r}</tt> instructions. */

#define Z80_OPTION_LD_A_IR_BUG 1

/** @brief @ref Z80 option that enables emulation of the <tt>out (c),255</tt>
  * instruction, specific to the Zilog Z80 CMOS. */

#define Z80_OPTION_OUT_VC_255 2

/** @brief @ref Z80 option that enables the XQ factor in the emulation of the
  * @c ccf and @c scf instructions. */

#define Z80_OPTION_XQ 8

/** @brief @ref Z80 option that enables the YQ factor in the emulation of the
  * @c ccf and @c scf instructions. */

#define Z80_OPTION_YQ 32

/** @brief @ref Z80 meta-option that enables full emulation of the Zilog NMOS
  * models. */

#define Z80_MODEL_ZILOG_NMOS \
	(Z80_OPTION_LD_A_IR_BUG | Z80_OPTION_XQ | Z80_OPTION_YQ)

/** @brief @ref Z80 meta-option that enables full emulation of the Zilog CMOS
  * models. */

#define Z80_MODEL_ZILOG_CMOS \
	(Z80_OPTION_OUT_VC_255 | Z80_OPTION_XQ | Z80_OPTION_YQ)

/** @brief @ref Z80 meta-option that enables full emulation of the NEC NMOS
  * models. */

#define Z80_MODEL_NEC_NMOS \
	Z80_OPTION_LD_A_IR_BUG

/** @brief @ref Z80 meta-option that enables full emulation of the ST CMOS
  * models. */

#define Z80_MODEL_ST_CMOS \
	(Z80_OPTION_LD_A_IR_BUG | Z80_OPTION_YQ)

/** @brief @ref Z80 request flag anouncing an incoming RESET signal. */

#define Z80_REQUEST_RESET 3

/** @brief @ref Z80 request flag that prevents the NMI signal from being
  * accepted. */

#define Z80_REQUEST_REJECT_NMI 4

/** @brief @ref Z80 request flag indicating that an NMI signal has been
  * detected. */

#define Z80_REQUEST_NMI 8

#define Z80_REQUEST_CLEAR_PC 16

/** @brief @ref Z80 request flag anouncing an incoming special RESET signal. */

#define Z80_REQUEST_SPECIAL_RESET 32

#define Z80_REQUEST_INT 64
#define Z80_REQUEST_ANY_RESET 35
#define Z80_REQUEST_RESET 3
#define Z80_REQUEST_INTERRUPT 5

/** @brief @ref Z80 resume code that is set when the emulator runs out of clock
  * cycles during the HALT state. */

#define Z80_RESUME_HALT 1

/** @brief @ref Z80 resume code that is set when the emulator runs out of clock
  * cycles by fetching a prefix sequence or an illegal instruction with @c DDh
  * or @c FDh prefix. */

#define Z80_RESUME_XY 2

/** @brief @ref Z80 resume code that is set when the emulator runs out of clock
  * cycles during a maskable interrupt response in mode 0, by fetching a prefix
  * sequence or an illegal instruction with @c DDh or @c FDh prefix. */

#define Z80_RESUME_IM0_XY 3

#define Z80_RESUME_SPECIAL_RESET_XY  5
#define Z80_RESUME_SPECIAL_RESET_NOP 6

/** @brief Accesses the MEMPTR register of a @ref Z80 @p object. */

#define Z80_MEMPTR(object) (object).memptr.uint16_value

/** @brief Accesses the PC register of a @ref Z80 @p object. */

#define Z80_PC(object) (object).pc.uint16_value

/** @brief Accesses the SP register of a @ref Z80 @p object. */

#define Z80_SP(object) (object).sp.uint16_value

/** @brief Accesses the temporary IX/IY register of a @ref Z80 @p object */

#define Z80_XY(object) (object).xy.uint16_value

/** @brief Accesses the IX register of a @ref Z80 @p object. */

#define Z80_IX(object) (object).ix.uint16_value

/** @brief Accesses the IY register of a @ref Z80 @p object. */

#define Z80_IY(object) (object).iy.uint16_value

/** @brief Accesses the AF register of a @ref Z80 @p object. */

#define Z80_AF(object) (object).af.uint16_value

/** @brief Accesses the BC register of a @ref Z80 @p object. */

#define Z80_BC(object) (object).bc.uint16_value

/** @brief Accesses the DE register of a @ref Z80 @p object. */

#define Z80_DE(object) (object).de.uint16_value

/** @brief Accesses the HL register of a @ref Z80 @p object. */

#define Z80_HL(object) (object).hl.uint16_value

/** @brief Accesses the AF' register of a @ref Z80 @p object. */

#define Z80_AF_(object) (object).af_.uint16_value

/** @brief Accesses the BC' register of a @ref Z80 @p object. */

#define Z80_BC_(object) (object).bc_.uint16_value

/** @brief Accesses the DE' register of a @ref Z80 @p object. */

#define Z80_DE_(object) (object).de_.uint16_value

/** @brief Accesses the HL' register of a @ref Z80 @p object. */

#define Z80_HL_(object) (object).hl_.uint16_value

/** @brief Accesses the MEMPTRH register of a @ref Z80 @p object. */

#define Z80_MEMPTRH(object) (object).memptr.uint8_values.at_1

/** @brief Accesses the MEMPTRL register of a @ref Z80 @p object. */

#define Z80_MEMPTRL(object) (object).memptr.uint8_values.at_0

/** @brief Accesses the PCH register of a @ref Z80 @p object. */

#define Z80_PCH(object) (object).pc.uint8_values.at_1

/** @brief Accesses the PCL register of a @ref Z80 @p object. */

#define Z80_PCL(object) (object).pc.uint8_values.at_0

/** @brief Accesses the SPH register of a @ref Z80 @p object. */

#define Z80_SPH(object) (object).sp.uint8_values.at_1

/** @brief Accesses the SPL register of a @ref Z80 @p object. */

#define Z80_SPL(object) (object).sp.uint8_values.at_0

/** @brief Accesses the temporary IXH/IYH register of a @ref Z80 @p object. */

#define Z80_XYH(object) (object).xy.uint8_values.at_1

/** @brief Accesses the temporary IXL/IYL register of a @ref Z80 @p object. */

#define Z80_XYL(object) (object).xy.uint8_values.at_0

/** @brief Accesses the IXH register of a @ref Z80 @p object. */

#define Z80_IXH(object) (object).ix.uint8_values.at_1

/** @brief Accesses the IXL register of a @ref Z80 @p object. */

#define Z80_IXL(object) (object).ix.uint8_values.at_0

/** @brief Accesses the IYH register of a @ref Z80 @p object. */

#define Z80_IYH(object) (object).iy.uint8_values.at_1

/** @brief Accesses the IYL register of a @ref Z80 @p object. */

#define Z80_IYL(object) (object).iy.uint8_values.at_0

/** @brief Accesses the A register of a @ref Z80 @p object. */

#define Z80_A(object) (object).af.uint8_values.at_1

/** @brief Accesses the F register of a @ref Z80 @p object. */

#define Z80_F(object) (object).af.uint8_values.at_0

/** @brief Accesses the B register of a @ref Z80 @p object. */

#define Z80_B(object) (object).bc.uint8_values.at_1

/** @brief Accesses the C register of a @ref Z80 @p object. */

#define Z80_C(object) (object).bc.uint8_values.at_0

/** @brief Accesses the D register of a @ref Z80 @p object. */

#define Z80_D(object) (object).de.uint8_values.at_1

/** @brief Accesses the E register of a @ref Z80 @p object. */

#define Z80_E(object) (object).de.uint8_values.at_0

/** @brief Accesses the H register of a @ref Z80 @p object. */

#define Z80_H(object) (object).hl.uint8_values.at_1

/** @brief Accesses the L register of a @ref Z80 @p object. */

#define Z80_L(object) (object).hl.uint8_values.at_0

/** @brief Accesses the A' register of a @ref Z80 @p object. */

#define Z80_A_(object) (object).af_.uint8_values.at_1

/** @brief Accesses the F' register of a @ref Z80 @p object. */

#define Z80_F_(object) (object).af_.uint8_values.at_0

/** @brief Accesses the B' register of a @ref Z80 @p object. */

#define Z80_B_(object) (object).bc_.uint8_values.at_1

/** @brief Accesses the C' register of a @ref Z80 @p object. */

#define Z80_C_(object) (object).bc_.uint8_values.at_0

/** @brief Accesses the D' register of a @ref Z80 @p object. */

#define Z80_D_(object) (object).de_.uint8_values.at_1

/** @brief Accesses the E' register of a @ref Z80 @p object. */

#define Z80_E_(object) (object).de_.uint8_values.at_0

/** @brief Accesses the H' register of a @ref Z80 @p object. */

#define Z80_H_(object) (object).hl_.uint8_values.at_1

/** @brief Accesses the L' register of a @ref Z80 @p object. */

#define Z80_L_(object) (object).hl_.uint8_values.at_0

Z_EXTERN_C_BEGIN

/** @brief Sets the power state of a @ref Z80 object.
  *
  * @param self Pointer to the object on which the function is called.
  * @param state
  *   @c TRUE  = power on;
  *   @c FALSE = power off. */

Z80_API void z80_power(Z80 *self, zboolean state);

/** @brief Performs an instantaneous normal RESET on a @ref Z80 object.
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_instant_reset(Z80 *self);

/** @brief Sends a normal RESET signal to a @ref Z80 object.
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_reset(Z80 *self);

/** @brief Sends a special RESET signal to a @ref Z80 object.
  *
  * @sa
  * - http://www.primrosebank.net/computers/z80/z80_special_reset.htm
  * - US Patent 4486827
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_special_reset(Z80 *self);

/** @brief Sets the state of the INT line of a @ref Z80 object.
  *
  * @param self Pointer to the object on which the function is called.
  * @param state
  *   @c TRUE  = set line low;
  *   @c FALSE = set line high. */

Z80_API void z80_int(Z80 *self, zboolean state);

/** @brief Triggers the NMI line of a @ref Z80 object.
  *
  * @param self Pointer to the object on which the function is called. */

Z80_API void z80_nmi(Z80 *self);

Z80_API void z80_busreq(Z80 *self, zboolean state);

/** @brief Runs a @ref Z80 object for a given number of clock @p cycles,
  * executing only instructions without responding to signals.
  *
  * Given the fact that one Z80 instruction takes between 4 and 23 cycles to be
  * executed, it is not always possible to run the CPU the exact number of @p
  * cycles specfified.
  *
  * @param self Pointer to the object on which the function is called.
  * @param cycles Number of clock cycles to be emulated.
  * @return The actual number of clock cycles emulated. */

Z80_API zusize z80_execute(Z80 *self, zusize cycles);

/** @brief Runs a @ref Z80 object for a given number of clock @p cycles.
  *
  * Given the fact that one Z80 instruction takes between 4 and 23 cycles to be
  * executed, it is not always possible to run the CPU the exact number of @p
  * cycles specfified.
  *
  * @param self Pointer to the object on which the function is called.
  * @param cycles Number of clock cycles to be emulated.
  * @return The actual number of clock cycles emulated. */

Z80_API zusize z80_run(Z80 *self, zusize cycles);


/** @brief Gets the value of the R register of a @ref Z80 object.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The value of the R register. */

static Z_INLINE zuint8 z80_r(Z80 *self)
	{return (self->r & 127) | (self->r7 & 128);}


/** @brief Obtains the refresh address of the M1 cycle being executed by a @ref
  * Z80 object.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The refresh address. */

static Z_INLINE zuint16 z80_refresh_address(Z80 *self)
	{return ((zuint16)self->i << 8) | ((self->r - 1) & 127);}


/** @brief Obtains the clock cycle, relative to the start of the instruction, at
  * which the I/O read M-cycle being executed by a @ref Z80 object begins.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The clock cyle at which the I/O read M-cycle begins. */

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


/** @brief Obtains the clock cycle, relative to the start of the instruction, at
  * which the I/O write M-cycle being executed by a @ref Z80 object begins.
  *
  * @param self Pointer to the object on which the function is called.
  * @return The clock cyle at which the I/O write M-cycle begins. */

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
