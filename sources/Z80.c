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

#ifndef CPU_Z80_DEPENDENCIES_H
#	include <Z/macros/value.h>
#	include <Z/macros/pointer.h>
#endif

#if defined(CPU_Z80_HIDE_API)
#	define CPU_Z80_API static
#elif defined(CPU_Z80_STATIC)
#	define CPU_Z80_API
#else
#	define CPU_Z80_API Z_API_EXPORT
#endif

#if defined(CPU_Z80_WITH_MODULE_ABI) && !defined(CPU_Z80_WITH_ABI)
#	define CPU_Z80_WITH_ABI
#endif

#ifdef CPU_Z80_WITH_ABI
#	if defined(CPU_Z80_HIDE_ABI)
#		define CPU_Z80_ABI static
#	elif defined(CPU_Z80_STATIC)
#		define CPU_Z80_ABI
#	else
#		define CPU_Z80_ABI Z_API_EXPORT
#	endif
#endif

#ifdef CPU_Z80_USE_LOCAL_HEADER
#	include "Z80.h"
#else
#	include <emulation/CPU/Z80.h>
#endif


/* MARK: - Types */

typedef zuint8 (* Instruction)(Z80 *object);


/* MARK: - Macros: External */

#define O(member)  Z_OFFSET_OF(Z80, member)
#define ROL(value) value = (zuint8)Z_8BIT_ROTATE_LEFT( value, 1)
#define ROR(value) value = (zuint8)Z_8BIT_ROTATE_RIGHT(value, 1)


/* MARK: - Macros & Functions: Callback */

#define READ_8(address)		object->read	(object->context, (zuint16)(address))
#define WRITE_8(address, value) object->write	(object->context, (zuint16)(address), (zuint8)(value))
#define IN(port)		object->in	(object->context, (zuint16)(port   ))
#define OUT(port, value)	object->out	(object->context, (zuint16)(port   ), (zuint8)(value))
#define INT_DATA		object->int_data(object->context)
#define READ_OFFSET(address)	((zsint8)READ_8(address))
#define SET_HALT		if (object->halt != NULL) object->halt(object->context, TRUE )
#define CLEAR_HALT		if (object->halt != NULL) object->halt(object->context, FALSE)


static Z_INLINE zuint16 read_16bit(Z80 *object, zuint16 address)
	{return (zuint16)(READ_8(address) | (zuint16)READ_8(address + 1) << 8);}


static Z_INLINE void write_16bit(Z80 *object, zuint16 address, zuint16 value)
	{
	WRITE_8(address, (zuint8)value);
	WRITE_8(address + 1, value >> 8);
	}


#define READ_16(address)	 read_16bit (object, (zuint16)(address))
#define WRITE_16(address, value) write_16bit(object, (zuint16)(address), (zuint16)(value))


/* MARK: - Macros: Registers */

#define AF    object->state.Z_Z80_STATE_MEMBER_AF
#define BC    object->state.Z_Z80_STATE_MEMBER_BC
#define DE    object->state.Z_Z80_STATE_MEMBER_DE
#define HL    object->state.Z_Z80_STATE_MEMBER_HL
#define IX    object->state.Z_Z80_STATE_MEMBER_IX
#define IY    object->state.Z_Z80_STATE_MEMBER_IY
#define PC    object->state.Z_Z80_STATE_MEMBER_PC
#define SP    object->state.Z_Z80_STATE_MEMBER_SP
#define AF_   object->state.Z_Z80_STATE_MEMBER_AF_
#define BC_   object->state.Z_Z80_STATE_MEMBER_BC_
#define DE_   object->state.Z_Z80_STATE_MEMBER_DE_
#define HL_   object->state.Z_Z80_STATE_MEMBER_HL_
#define A     object->state.Z_Z80_STATE_MEMBER_A
#define F     object->state.Z_Z80_STATE_MEMBER_F
#define B     object->state.Z_Z80_STATE_MEMBER_B
#define C     object->state.Z_Z80_STATE_MEMBER_C
#define L     object->state.Z_Z80_STATE_MEMBER_L
#define I     object->state.Z_Z80_STATE_MEMBER_I
#define R     object->state.Z_Z80_STATE_MEMBER_R
#define R_ALL ((R & 127) | (R7 & 128))


/* MARK: - Macros: Internal Bits */

#define HALT object->state.Z_Z80_STATE_MEMBER_HALT
#define IFF1 object->state.Z_Z80_STATE_MEMBER_IFF1
#define IFF2 object->state.Z_Z80_STATE_MEMBER_IFF2
#define EI   object->state.Z_Z80_STATE_MEMBER_EI
#define IM   object->state.Z_Z80_STATE_MEMBER_IM
#define NMI  object->state.Z_Z80_STATE_MEMBER_NMI
#define INT  object->state.Z_Z80_STATE_MEMBER_IRQ


/* MARK: - Macros: Temporal Data */

#define CYCLES	    object->cycles
#define R7	    object->r7
#define BYTE(index) object->data.array_uint8[index]
#define BYTE0	    BYTE(0)
#define BYTE1	    BYTE(1)
#define BYTE2	    BYTE(2)
#define BYTE3	    BYTE(3)
#define XY	    object->xy.value_uint16
#define XY_ADDRESS  ((zuint16)(XY + object->data.array_sint8[2]))


/* MARK: - Macros: Flags */

#define SF 128
#define ZF  64
#define YF  32
#define HF  16
#define XF   8
#define PF   4
#define NF   2
#define CF   1

#define SZPF (SF | ZF | PF)
#define SYXF (SF | YF | XF)
#define ZPF  (ZF | PF	  )
#define YXCF (YF | XF | CF)
#define YXF  (YF | XF	  )
#define PNF  (PF | NF	  )
#define HCF  (HF | CF	  )

#define F_H   (F &   HF)
#define F_N   (F &   NF)
#define F_C   (F &   CF)
#define F_SZP (F & SZPF)
#define A_SYX (A & SYXF)
#define A_YX  (A &  YXF)

#define ZF_ZERO(value) (!(value) << 6)


/* MARK: - P/V Flag Computation */

static zuint8 const pf_parity_table[256] = {
/*	0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/* 0 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
/* 1 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* 2 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* 3 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
/* 4 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* 5 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
/* 6 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
/* 7 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* 8 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* 9 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
/* A */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
/* B */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* C */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
/* D */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* E */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
/* F */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4
};

#define PF_PARITY(value) pf_parity_table[value]

#define VF(function, operand)						 \
static Z_INLINE zuint8 pf_overflow_##function##8(zuint8 a, zuint8 b)	 \
	{								 \
	zsint total = ((zsint)((zsint8)a)) operand ((zsint)((zsint8)b)); \
									 \
	return total < -128 || total > 127 ? PF : 0;			 \
	}

VF(add, +)
VF(sub, -)

#undef	VF
#define VF(function, bits, type, operand, minimum, maximum)					\
static Z_INLINE zuint8 pf_overflow_##function##bits(zuint##bits a, zuint##bits b, zuint8 carry) \
	{											\
	type total = ((type)((zsint##bits)a)) operand ((type)((zsint##bits)b)) operand carry;	\
												\
	return total < minimum || total > maximum ? PF : 0;					\
	}

VF(adc,  8, zsint,   +,   -128,   127)
VF(sbc,  8, zsint,   -,   -128,   127)
VF(adc, 16, zsint32, +, -32768, 32767)
VF(sbc, 16, zsint32, -, -32768, 32767)


/* MARK: - 8-Bit Register Resolution

   .----------.   .---------.   .-----------.	.-----------.
   | 76543210 |   |  X / Y  |   |   J / K   |	|   P / Q   |
   |----------|   |---------|   |-----------|	|-----------|
   | __xxx___ |   | 000 = b |   | 000 = b   |	| 000 = b   |
   | _____yyy |   | 001 = c |   | 001 = c   |	| 001 = c   |
   | __jjj___ |   | 010 = d |   | 010 = d   |	| 010 = d   |
   | _____kkk |   | 011 = e |   | 011 = e   |	| 011 = e   |
   | __ppp___ |   | 100 = h |   | 100 = ixh |	| 100 = iyh |
   | _____qqq |   | 101 = l |   | 101 = ixl |	| 101 = iyl |
   '----------'   | 111 = a |   | 111 = a   |	| 111 = a   |
		  '---------'   '-----------'	'----------*/

static zuint8 const x_y_table[8] = {
	O(state.Z_Z80_STATE_MEMBER_B),
	O(state.Z_Z80_STATE_MEMBER_C),
	O(state.Z_Z80_STATE_MEMBER_D),
	O(state.Z_Z80_STATE_MEMBER_E),
	O(state.Z_Z80_STATE_MEMBER_H),
	O(state.Z_Z80_STATE_MEMBER_L),
	0,
	O(state.Z_Z80_STATE_MEMBER_A)
};

static zuint8 const j_k_p_q_table[8] = {
	O(state.Z_Z80_STATE_MEMBER_B),
	O(state.Z_Z80_STATE_MEMBER_C),
	O(state.Z_Z80_STATE_MEMBER_D),
	O(state.Z_Z80_STATE_MEMBER_E),
	O(xy.values_uint8.index1    ),
	O(xy.values_uint8.index0    ),
	0,
	O(state.Z_Z80_STATE_MEMBER_A)
};

#define R_8(name, table, offset, mask, shift) \
static Z_INLINE zuint8 *name(Z80 *object)     \
	{return ((zuint8 *)object) + table[(BYTE(offset) & mask) shift];}

R_8(__xxx___0,	   x_y_table, 0, 56, >> 3   )
R_8(__xxx___1,	   x_y_table, 1, 56, >> 3   )
R_8(_____yyy0,	   x_y_table, 0,  7, Z_EMPTY)
R_8(_____yyy1,	   x_y_table, 1,  7, Z_EMPTY)
R_8(_____yyy3,	   x_y_table, 3,  7, Z_EMPTY)
R_8(__jjj___ , j_k_p_q_table, 1, 56, >> 3   )
R_8(_____kkk , j_k_p_q_table, 1,  7, Z_EMPTY)


/* MARK: - 16-Bit Register Resolution

   .----------.   .---------.	.---------.   .---------.
   | 76543210 |   |    S    |	|    T    |   |    W	|
   |----------|   |---------|	|---------|   |---------|
   | __ss____ |   | 00 = bc |	| 00 = bc |   | 00 = bc |
   | __tt____ |   | 01 = de |	| 01 = de |   | 01 = de |
   '----------'   | 10 = hl |	| 10 = hl |   | 10 = XY |
		  | 11 = sp |	| 11 = af |   | 11 = sp |
		  '---------'	'---------'   '--------*/

static zuint8 const s_table[4] = {
	O(state.Z_Z80_STATE_MEMBER_BC),
	O(state.Z_Z80_STATE_MEMBER_DE),
	O(state.Z_Z80_STATE_MEMBER_HL),
	O(state.Z_Z80_STATE_MEMBER_SP)
};

static zuint8 const t_table[4] = {
	O(state.Z_Z80_STATE_MEMBER_BC),
	O(state.Z_Z80_STATE_MEMBER_DE),
	O(state.Z_Z80_STATE_MEMBER_HL),
	O(state.Z_Z80_STATE_MEMBER_AF)
};

static zuint8 const w_table[4] = {
	O(state.Z_Z80_STATE_MEMBER_BC),
	O(state.Z_Z80_STATE_MEMBER_DE),
	O(xy			     ),
	O(state.Z_Z80_STATE_MEMBER_SP)
};

#define R_16(name, table, offset)	   \
static Z_INLINE zuint16 *name(Z80 *object) \
	{return Z_BOP(zuint16 *, object, table[(BYTE(offset) & 48) >> 4]);}

R_16(__ss____0, s_table, 0)
R_16(__ss____1, s_table, 1)
R_16(__tt____ , t_table, 0)


/* MARK: - Condition Resolution

   .----------.   .----------.
   | 76543210 |   |	Z    |
   |----------|   |----------|
   | __zzz___ |   | 000 = nz |
   | ___zz___ |   | 001 = z  |
   '----------'   | 010 = nc |
		  | 011 = c  |
		  | 100 = po |
		  | 101 = pe |
		  | 110 = p  |
		  | 111 = m  |
		  '---------*/

static zuint8 const z_table[8] = {ZF, ZF, CF, CF, PF, PF, SF, SF};

static Z_INLINE zboolean __zzz___(Z80 *object)
	{
	zuint8 z = (BYTE0 & 56) >> 3;

	return (F & (z_table[z]))
		?  (z & 1)  /* Flag is 1 */
		: !(z & 1); /* Flag is 0 */
	}


/* MARK: - 8-Bit Arithmetic and Logical Operation Resolution and Execution

   .----------.   .-----------.		.-------------------------------.
   | 76543210 |   |	U     |		| S | Z | Y | H | X | P | N | C |
   |----------|   |-----------|   .-----+---+---+---+---+---+---+---+---|
   | __uuu___ |   | 000 = add |   | add | S | Z | 5 | H | 3 | V | 0 | C |
   | _____vvv |   | 001 = adc |   |-----+---+---+---+---+---+---+---+---|
   '----------'   | 010 = sub |   | adc | S | Z | 5 | H | 3 | V | 0 | C |
		  | 011 = sbc |   |-----+---+---+---+---+---+---+---+---|
		  | 100 = and |   | sub | S | Z | 5 | H | 3 | V | 1 | C |
		  | 101 = xor |   |-----+---+---+---+---+---+---+---+---|
		  | 110 = or  |   | sbc | S | Z | 5 | H | 3 | V | 1 | C |
		  | 111 = cp  |   |-----+---+---+---+---+---+---+---+---|
		  |-----------|   | and | S | Z | 5 | 1 | 3 | P | 0 | 0 |
		  |	V     |   |-----+---+---+---+---+---+---+---+---|
		  |-----------|   | xor | S | Z | 5 | 0 | 3 | P | 0 | 0 |
		  | 100 = inc |   |-----+---+---+---+---+---+---+---+---|
		  | 101 = dec |   | or  | S | Z | 5 | 0 | 3 | P | 0 | 0 |
		  '-----------'   |-----+---+---+---+---+---+---+---+---|
				  | cp  | S | Z |v.5| H |v.3| V | 1 | C |
				  |-----+---+---+---+---+---+---+---+---|
				  | inc | S | Z |v.5| H |v.3| V | 0 | . |
				  |-----+---+---+---+---+---+---+---+---|
				  | dec | S | Z |v.5| H |v.3| V | 1 | . |
				  '------------------------------------*/

static void __uuu___(Z80 *object, zuint8 offset, zuint8 value)
	{
	zuint8 t;

	switch ((object->data.array_uint8[offset] >> 3) & 7)
		{
		case 0:	/* ADD */
		t = A + value;

		F =	((zuint)A + value > 255)     /* CF = Carry	*/
			| pf_overflow_add8(A, value) /* PF = Overflow	*/
			| ((A ^ value ^ t) & HF);    /* HF = Half-carry */
		A = t;				     /* NF = 0		*/
		break;

		case 1:	/* ADC */
		t = F_C;

		F =	((zuint)A + value + t > 255)		  /* CF = Carry	     */
			| pf_overflow_adc8(A, value, t)		  /* PF = Overflow   */
			| (((A & 0xF) + (value & 0xF) + t) & HF); /* HF = Half-carry */
								  /* NF = 0	     */
		A += value + t;
		break;

		case 2:	/* SUB */
		t = A - value;

		F =	(A < value)		     /* CF = Borrow	 */
			| NF			     /* NF = 1		 */
			| pf_overflow_sub8(A, value) /* PF = Overflow	 */
			| ((A ^ value ^ t) & HF);    /* HF = Half-Borrow */
		A = t;
		break;

		case 3: /* SBC */
		t = F_C;

		F =	((zsint)A - (zsint)value - (zsint)t < 0)  /* CF = Borrow      */
			| NF					  /* NF = 1	      */
			| pf_overflow_sbc8(A, value, t)		  /* PF = Overflow    */
			| (((A & 0xF) - (value & 0xF) - t) & HF); /* HF = Half-Borrow */

		A -= value + t;
		break;

		case 4: /* AND */
		A &= value;
		F = HF | PF_PARITY(A); /* HF = 1; PF = Parity */
		break;		       /* NF, CF = 0	      */

		case 5: /* XOR */
		A ^= value;
		F = PF_PARITY(A); /* PF = Parity    */
		break;		  /* HF, NF, CF = 0 */

		case 6: /* OR */
		A |= value;
		F = PF_PARITY(A); /* PF = Parity    */
		break;		  /* HF, NF, CF = 0 */

		case 7: /* CP */
		t = A - value;

		F = (zuint8)
			((A < value)		      /* CF = Borrow	    */
			 | NF			      /* NF = 1		    */
			 | pf_overflow_sub8(A, value) /* PF = Overflow	    */
			 | ((A ^ value ^ t) & HF)     /* HF = Half-Borrow   */
			 | (value & YXF)	      /* YF = v.5, XF = v.3 */
			 | ZF_ZERO(t)		      /* ZF = !(A - v)	    */
			 | (t & SF));		      /* SF = (A - v).7	    */
		return;
		}

	F = (zuint8)
		((F & (HF | PF | NF | CF)) /* CF, NF, PF and HF already changed */
		 | A_SYX		   /* SF = A.7; YF = A.5; XF = A.3	*/
		 | ZF_ZERO(A));		   /* ZF = !A				*/
	}


static zuint8 _____vvv(Z80 *object, zuint8 offset, zuint8 value)
	{
	zuint8 t, pn;

	/* DEC */
	if (object->data.array_uint8[offset] & 1)
		{			      /* PF = Overflow */
		pn = value == 128 ? PNF : NF; /* NF = 1        */
		t = value - 1;
		}

	/* INC */
	else	{			    /* PF = Overflow */
		pn = value == 127 ? PF : 0; /* NF = 0	     */
		t = value + 1;
		}

	F = (zuint8)
		((F & CF)		  /* CF unchanged		  */
		 | pn			  /* PF and NF already calculated */
		 | (t & SYXF)		  /* SF = v.7; YF = v.5; XF = v.3 */
		 | ((value ^ 1 ^ t) & HF) /* HF = Half-Borrow		  */
		 | ZF_ZERO(t));		  /* ZF = !v			  */

	return t;
	}


/* MARK: - Rotation and Shift Operation Resolution and Execution

   .----------.   .-----------.
   | 76543210 |   |	G     |
   |----------|   |-----------|
   | __ggg___ |   | 000 = rlc |
   '----------'   | 001 = rrc |
		  | 010 = rl  |
		  | 011 = rr  |
		  | 100 = sla |
		  | 101 = sra |
		  | 110 = sll |
		  | 111 = srl |
		  '----------*/

static zuint8 __ggg___(Z80 *object, zuint8 offset, zuint8 value)
	{
	zuint8 c;

	switch ((object->data.array_uint8[offset] >> 3) & 7)
		{
		/* RLC		 .----------------.
			.----.   |  .---------.   |
			| CF |<-----| 7 <-- 0 |<--'
			'----'	    '--------*/
		case 0:
		ROL(value);
		c = value & CF;
		break;

		/* RRC	.----------------.
			|   .---------.  |   .----.
			'-->| 7 --> 0 |----->| CF |
			    '---------'      '---*/
		case 1:
		c = value & CF;
		ROR(value);
		break;

		/* RL	.-------------------------.
			|  .----.   .---------.   |
			'--| CF |<--| 7 <-- 0 |<--'
			   '----'   '--------*/
		case 2:
		c = value >> 7;
		value = (zuint8)((value << 1) | F_C);
		break;

		/* RR	.-------------------------.
			|   .---------.   .----.  |
			'-->| 7 --> 0 |-->| CF |--'
			    '---------'   '---*/
		case 3:
		c = value & CF;
		value = (zuint8)((value >> 1) | (F_C << 7));
		break;

		/* SLA	.----.	 .---------.
			| CF |<--| 7 <-- 0 |<-- 0
			'----'	 '--------*/
		case 4:
		c = value >> 7;
		value <<= 1;
		break;

		/* SRA	    .---------.   .----.
			.-->| 7 --> 0 |-->| CF |
			|   '---------'   '----'
			|     |
			'----*/
		case 5:
		c = value & CF;
		value = (value & 128) | (value >> 1);
		break;

		/* SLL	.----.	 .---------.
			| CF |<--| 7 <-- 0 |<-- 1
			'----'	 '--------*/
		case 6:
		c = value >> 7;
		value = (value << 1) & 1;
		break;

		/* SRL	     .---------.   .----.
			0 -->| 7 --> 0 |-->| CF |
			     '---------'   '---*/
		case 7:
		c = value & CF;
		value >>= 1;
		break;

		/* Uncoment to Avoid a compiler warning */
		/*default: c = 0; break;*/
		}

	F = (zuint8)((value & SYXF) | ZF_ZERO(value) | PF_PARITY(value) | c);
	return value;
	}


/* MARK: - Bit Set and Reset Operation Resolution and Execution

   .----------.   .---------.
   | 76543210 |   |    M    |
   |----------|   |---------|
   | _m______ |   | 0 = res |
   '----------'   | 1 = set |
		  '--------*/

static Z_INLINE zuint8 _m______(Z80 *object, zuint8 offset, zuint8 value)
	{
	zuint8 t = object->data.array_uint8[offset];

	return (zuint8)((t & 64)
		? value |  (1 << ((t & 56) >> 3))   /* SET */
		: value & ~(1 << ((t & 56) >> 3))); /* RES */
	}


/* MARK: - Macros: Shortening */

#define N(x)	  ((BYTE##x & 56) >> 3)
#define X0	  (*__xxx___0(object))
#define X1	  (*__xxx___1(object))
#define Y0	  (*_____yyy0(object))
#define Y1	  (*_____yyy1(object))
#define Y3	  (*_____yyy3(object))
#define JP	  (*__jjj___ (object))
#define KQ	  (*_____kkk (object))
#define SS0	  (*__ss____0(object))
#define SS1	  (*__ss____1(object))
#define TT	  (*__tt____ (object))
#define Z	    __zzz___ (object)
#define U0(value)   __uuu___ (object, 0, value)
#define U1(value)   __uuu___ (object, 1, value)
#define V0(value)   _____vvv (object, 0, value)
#define V1(value)   _____vvv (object, 1, value)
#define G1(value)   __ggg___ (object, 1, value)
#define G3(value)   __ggg___ (object, 3, value)
#define M1(value)   _m______ (object, 1, value)
#define M3(value)   _m______ (object, 3, value)
#define WW	  (*Z_BOP(zuint16 *, object, w_table[(BYTE1 >> 4) & 3]))


/* MARK: - Macros & Functions: Reusable Code */

#define INSTRUCTION(name) static zuint8 name(Z80 *object)
#define EXIT_HALT	  if (HALT) {PC++; HALT = FALSE; CLEAR_HALT;}
#define PUSH(value)	  WRITE_16(SP -= 2, value)


#define LD_A_I_LD_A_R						  \
	F = (zuint8)	       /* HF = 0 / NF = 0	       */ \
		(A_SYX	       /* SF = A.7; YF = A.5; XF = A.3 */ \
		 | ZF_ZERO(A)  /* ZF = !A		       */ \
		 | (IFF2 << 2) /* PF = IFF2		       */ \
		 | F_C);       /* CF unchanged		       */


#define EX(a, b) t = a; a = b; b = t;


#define EX_VSP_X(register)	\
	t = READ_16(SP);	\
	WRITE_16(SP, register);	\
	register = t;


#define LDX(operator)							   \
	zuint8 n;							   \
									   \
	PC += 2;							   \
	WRITE_8(DE operator, n = READ_8(HL operator));			   \
	n += A;								   \
									   \
	F = (zuint8)		      /* HF = 0, NF = 0			*/ \
		((F & (SF | ZF | CF)) /* SF, ZF, CF unchanged		*/ \
		 | ((n & 2) << 4)     /* YF = ([HL] + A).1		*/ \
		 | (n & XF)	      /* XF = ([HL] + A).3		*/ \
		 | (!!(--BC) << 2));  /* PF = 1 if BC != 0, else PF = 0 */


#define LDXR(operator)	    \
	LDX(operator)	    \
	if (!BC) return 16; \
	PC -= 2; return 21;


#define CPX(operator)						    \
	zuint8 v, n0, n1;					    \
								    \
	PC += 2;						    \
	n1 = (n0 = A - (v = READ_8(HL operator))) - !!F_H;	    \
								    \
	F = (zuint8)						    \
		((n0 & SF)	       /* SF = (A - [HL]).7	 */ \
		 | ZF_ZERO(n0)	       /* ZF = !(A - [HL])	 */ \
		 | ((A ^ v ^ n0) & HF) /* HF = borrow from bit 5 */ \
		 | ((n1 & 2) << 4)     /* YF = (A - [HL] - HF).1 */ \
		 | (n1 & XF)	       /* XF = (A - [HL] - HF).3 */ \
		 | (!!(--BC) << 2)     /* PF = !!BC		 */ \
		 | NF		       /* NF = 1		 */ \
		 | F_C);	       /* CF unchanged		 */


#define CPXR(operator)		   \
	CPX(operator)		   \
	if (!BC || !n0) return 16; \
	PC -= 2;	return 21;


static Z_INLINE void add_RR_NN(Z80 *object, zuint16 *r, zuint16 v)
	{
	zuint16 t = *r + v;

	F =	F_SZP			     /* SF, ZF, PF unchanged   */
		| ((t >> 8) & YXF)	     /* YF = RR.13; XF = RR.11 */
		| (((*r ^ v ^ t) >> 8) & HF) /* HF = RRh half-carry    */
		| ((zuint32)*r + v > 65535); /* CF = Carry	       */
	*r = t;				     /* NF = 0		       */
	}


#define ADD_RR_NN(register, value) \
	add_RR_NN(object, (zuint16 *)&register, value);


#define ADC_SBC_HL_SS(function, sign, cf_test, set_nf)								 \
	zuint8 c = F_C;												 \
	zuint16 v = SS1, t = HL sign v sign c;									 \
														 \
	F = (zuint8)												 \
		(((t >> 8) & SYXF)					/* SF = HL.15; YF = HL.13; XF = HL.11 */ \
		 | ZF_ZERO(t)						/* ZF = !HL			      */ \
		 | ((((HL & 0xFFF) sign (v & 0xFFF) sign c) >> 8) & HF) /* HF = Half-carry of H		      */ \
		 | pf_overflow_##function##16(HL, v, c)			/* PF = Overflow		      */ \
		 | !!(cf_test)						/* CF = Carry			      */ \
		 set_nf);						/* ADC: NF = 0; SBC: NF = 1	      */ \
														 \
	HL = t;													 \
	PC += 2;												 \
	return 15;


#define RXA						  \
	F =	F_SZP  /* SF, ZF, PF unchanged	       */ \
		| A_YX /* YF = A.5; XF = A.3	       */ \
		| c;   /* CF = Ai.7 (rla) / Ai.0 (rra) */


#define RXD(a, b, c)						   \
	zuint8 t;						   \
								   \
	PC += 2;						   \
	WRITE_8(HL, ((t = READ_8(HL)) a 4) | (A b));		   \
	A = (A & 0xF0) | (t c);					   \
								   \
	F = (zuint8)						   \
		(A_SYX		/* SF = A.7; YF = A.5; XF = A.3 */ \
		 | ZF_ZERO(A)	/* ZF = !A			*/ \
		 | PF_PARITY(A) /* PF = Parity of A		*/ \
		 | F_C);	/* CF unchanged			*/ \
				/* HF = 0, NF = 0;		*/


#define BIT_N_VALUE(value)						   \
	zuint8 n = value & (1 << N(1));	/* SF = value.N && N == 7	*/ \
					/* ZF, PF = !value.N		*/ \
	F =	(n ? n & SYXF : ZPF)	/* YF = value.N && N == 5	*/ \
		| HF			/* HF = 1; NF = 0; CF unchanged	*/ \
		| F_C;			/* XF = value.N && N == 3	*/


#define BIT_N_VADDRESS(address)					   \
	Z16Bit a;						   \
	zuint8 n = READ_8(a.value_uint16 = address) & (1 << N(3)); \
								   \
	F =	(n ? (n & SF) : ZPF)				   \
		| (a.values_uint8.index1 & YXF)			   \
		| HF						   \
		| F_C;


#define IN_VC			\
	zuint8 t;		\
				\
	PC += 2;		\
	t = IN(BC);		\
				\
	F = (zuint8)		\
		((t & SYXF)	\
		 | ZF_ZERO(t)	\
		 | PF_PARITY(t) \
		 | F_C);


#define INX(hl_operator, c_operator)									  \
	zuint8 v;											  \
	zuint t;											  \
													  \
	PC += 2;											  \
	t = (zuint)(v = IN(BC)) + ((C c_operator 1) & 255);						  \
	WRITE_8(HL, v);											  \
	HL hl_operator;											  \
	B--;												  \
													  \
	F = (zuint8)											  \
		((B & SYXF)		  /* SF = (B - 1).7; YF = (B - 1).5; XF = (B - 1).3	       */ \
		 | ZF_ZERO(B)		  /* ZF = !(B - 1)					       */ \
		 | PF_PARITY((t & 7) ^ B) /* PF = Parity of (([HL] + ((C +/- 1) & 255)) and 7) xor B   */ \
		 | (v & 128));		  /* NF = IN(BC).7					       */ \
					  /* if (([HL] + ((C +/- 1) & 255)) > 255) HF = 1; else HF = 0 */ \
	if (t > 255) F |= HCF;		  /* if (([HL] + ((C +/- 1) & 255)) > 255) HF = 1; else HF = 0 */


#define INXR(hl_operator, c_operator) \
	INX(hl_operator, c_operator); \
	if (!B)	 return 16;	      \
	PC -= 2; return 21;


#define OUTX(operator)										     \
	zuint8 t;										     \
												     \
	PC += 2;										     \
	OUT(BC, t = READ_8(HL));								     \
	HL operator;										     \
	B--;											     \
												     \
	F = (zuint8)										     \
		((B & SYXF)			/* SF = (B - 1).7; YF = (B - 1).5; XF = (B - 1).3 */ \
		 | ZF_ZERO(B)			/* ZF = !(B - 1)				  */ \
		 | PF_PARITY(((L + t) & 7) ^ B) /* PF = Parity of ((L + [HL]) and 7) xor B	  */ \
		 | (t & 128));			/* NF = IN(BC).7				  */ \
						/* if (L + [HL] > 255) HF = 1; else HF = 0	  */ \
	if ((zuint)t + L > 255) F |= HCF;	/* if (L + [HL] > 255) CF = 1; else CF = 0	  */


#define OTXR(operator)	    \
	OUTX(operator);	    \
	if (!B)	 return 16; \
	PC -= 2; return 21;


#define RET PC = READ_16(SP); SP += 2;


/* MARK: - Instructions: 8-Bit Load Group
.---------------------------------------------------------------------------.
|			0	1	2	3	  Flags		    |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles  |
|  -----------------------------------------------------------------------  |
|  ld X,Y		01xxxyyy			  ........  1 / 4   |
|  ld J,K		<  DD  >01jjjkkk		  ........  2 / 8   |
|  ld P,Q		<  FD  >01pppqqq		  ........  2 / 8   |
|  ld X,BYTE		00xxx110< BYTE >		  ........  2 / 7   |
|  ld J,BYTE		<  DD  >00jjj110< BYTE >	  ........  3 / 11  |
|  ld P,BYTE		<  FD  >00ppp110< BYTE >	  ........  3 / 11  |
|  ld X,(hl)		01xxx110			  ........  2 / 7   |
|  ld X,(ix+OFFSET)	<  DD  >01xxx110<OFFSET>	  ........  5 / 19  |
|  ld X,(iy+OFFSET)	<  FD  >01xxx110<OFFSET>	  ........  5 / 19  |
|  ld (hl),Y		01110yyy			  ........  2 / 7   |
|  ld (ix+OFFSET),Y	<  DD  >01110yyy<OFFSET>	  ........  5 / 19  |
|  ld (iy+OFFSET),Y	<  FD  >01110yyy<OFFSET>	  ........  5 / 19  |
|  ld (hl),BYTE		<  36  >< BYTE >		  ........  3 / 10  |
|  ld (ix+OFFSET),BYTE	<  DD  ><  36  ><OFFSET>< BYTE >  ........  5 / 19  |
|  ld (iy+OFFSET),BYTE	<  FD  ><  36  ><OFFSET>< BYTE >  ........  5 / 19  |
|  ld a,(bc)		<  0A  >			  ........  2 / 7   |
|  ld a,(de)		<  1A  >			  ........  2 / 7   |
|  ld a,(WORD)		<  3A  ><     WORD     >	  ........  4 / 13  |
|  ld (bc),a		<  02  >			  ........  2 / 7   |
|  ld (de),a		<  12  >			  ........  2 / 7   |
|  ld (WORD),a		<  32  ><     WORD     >	  ........  4 / 13  |
|  ld a,i		<  ED  ><  57  >		  szy0xi0.  2 / 9   |
|  ld a,r		<  ED  ><  5F  >		  szy0xi0.  2 / 9   |
|  ld i,a		<  ED  ><  47  >		  ........  2 / 9   |
|  ld r,a		<  ED  ><  4F  >		  ........  2 / 9   |
'--------------------------------------------------------------------------*/

INSTRUCTION(ld_X_Y)	       {PC++; X0 = Y0;						    return  4;}
INSTRUCTION(ld_JP_KQ)	       {PC += 2; JP = KQ;					    return  8;}
INSTRUCTION(ld_X_BYTE)	       {X0 = READ_8((PC += 2) - 1);				    return  7;}
INSTRUCTION(ld_JP_BYTE)	       {JP = READ_8((PC += 3) - 1);				    return 11;}
INSTRUCTION(ld_X_vhl)	       {PC++; X0 = READ_8(HL);					    return  7;}
INSTRUCTION(ld_X_vXYOFFSET)    {X1 = READ_8(XY + READ_OFFSET((PC += 3) - 1));		    return 19;}
INSTRUCTION(ld_vhl_Y)	       {PC++; WRITE_8(HL, Y0);					    return  7;}
INSTRUCTION(ld_vXYOFFSET_Y)    {WRITE_8(XY + READ_OFFSET((PC += 3) - 1), Y1);		    return 19;}
INSTRUCTION(ld_vhl_BYTE)       {WRITE_8(HL, READ_8((PC += 2) - 1));			    return 10;}
INSTRUCTION(ld_vXYOFFSET_BYTE) {PC += 4; WRITE_8(XY + READ_OFFSET(PC - 2), READ_8(PC - 1)); return 19;}
INSTRUCTION(ld_a_vbc)	       {PC++; A = READ_8(BC);					    return  7;}
INSTRUCTION(ld_a_vde)	       {PC++; A = READ_8(DE);					    return  7;}
INSTRUCTION(ld_a_vWORD)	       {A = READ_8(READ_16((PC += 3) - 2));			    return 13;}
INSTRUCTION(ld_vbc_a)	       {PC++; WRITE_8(BC, A);					    return  7;}
INSTRUCTION(ld_vde_a)	       {PC++; WRITE_8(DE, A);					    return  7;}
INSTRUCTION(ld_vWORD_a)	       {WRITE_8(READ_16((PC += 3) - 2), A);			    return 13;}
INSTRUCTION(ld_a_i)	       {PC += 2; A = I; LD_A_I_LD_A_R;				    return  9;}
INSTRUCTION(ld_a_r)	       {PC += 2; A = R_ALL; LD_A_I_LD_A_R;			    return  9;}
INSTRUCTION(ld_i_a)	       {PC += 2; I = A;						    return  9;}
INSTRUCTION(ld_r_a)	       {PC += 2; R = R7 = A;					    return  9;}


/* MARK: - Instructions: 16-Bit Load Group
.---------------------------------------------------------------------------.
|			0	1	2	3	  Flags		    |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles  |
|  -----------------------------------------------------------------------  |
|  ld SS,WORD		00ss0001<     WORD     >	  ........  3 / 10  |
|  ld ix,WORD		<  DD  ><  21  ><     WORD     >  ........  4 / 14  |
|  ld iy,WORD		<  FD  ><  21  ><     WORD     >  ........  4 / 14  |
|  ld hl,(WORD)		<  2A  ><     WORD     >	  ........  5 / 16  |
|  ld SS,(WORD)		<  ED  >01ss1011<     WORD     >  ........  6 / 20  |
|  ld ix,(WORD)		<  DD  ><  2A  ><     WORD     >  ........  6 / 20  |
|  ld iy,(WORD)		<  FD  ><  2A  ><     WORD     >  ........  6 / 20  |
|  ld (WORD),hl		<  22  ><     WORD     >	  ........  5 / 16  |
|  ld (WORD),SS		<  ED  >01ss0011<     WORD     >  ........  6 / 20  |
|  ld (WORD),ix		<  DD  ><  22  ><     WORD     >  ........  6 / 20  |
|  ld (WORD),iy		<  FD  ><  22  ><     WORD     >  ........  6 / 20  |
|  ld sp,hl		<  F9  >			  ........  1 / 6   |
|  ld sp,ix		<  DD  ><  F9  >		  ........  2 / 10  |
|  ld sp,iy		<  FD  ><  F9  >		  ........  2 / 10  |
|  push TT		11tt0101			  ........  3 / 11  |
|  push ix		<  DD  ><  E5  >		  ........  4 / 15  |
|  push iy		<  FD  ><  E5  >		  ........  4 / 15  |
|  pop TT		11tt0001			  ........  3 / 10  |
|  pop ix		<  DD  ><  E1  >		  ........  4 / 14  |
|  pop iy		<  FD  ><  E1  >		  ........  4 / 14  |
'--------------------------------------------------------------------------*/

INSTRUCTION(ld_SS_WORD)	 {SS0 = READ_16((PC += 3) - 2);		 return 10;}
INSTRUCTION(ld_XY_WORD)	 {XY  = READ_16((PC += 4) - 2);		 return 14;}
INSTRUCTION(ld_hl_vWORD) {HL  = READ_16(READ_16((PC += 3) - 2)); return 16;}
INSTRUCTION(ld_SS_vWORD) {SS1 = READ_16(READ_16((PC += 4) - 2)); return 20;}
INSTRUCTION(ld_XY_vWORD) {XY  = READ_16(READ_16((PC += 4) - 2)); return 20;}
INSTRUCTION(ld_vWORD_hl) {WRITE_16(READ_16((PC += 3) - 2), HL);	 return 16;}
INSTRUCTION(ld_vWORD_SS) {WRITE_16(READ_16((PC += 4) - 2), SS1); return 20;}
INSTRUCTION(ld_vWORD_XY) {WRITE_16(READ_16((PC += 4) - 2), XY);	 return 20;}
INSTRUCTION(ld_sp_hl)	 {PC++; SP = HL;			 return  6;}
INSTRUCTION(ld_sp_XY)	 {PC += 2; SP = XY;			 return 10;}
INSTRUCTION(push_TT)	 {PC++; WRITE_16(SP -= 2, TT);		 return 11;}
INSTRUCTION(push_XY)	 {PC += 2; WRITE_16(SP -= 2, XY);	 return 15;}
INSTRUCTION(pop_TT)	 {PC++; TT = READ_16(SP); SP += 2;	 return 10;}
INSTRUCTION(pop_XY)	 {PC += 2; XY = READ_16(SP); SP += 2;	 return 14;}


/* MARK: - Instructions: Exchange, Block Transfer and Search Groups
.--------------------------------------------------------------------------------.
|			0	1	2	3	  Flags			 |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles	 |
|  ----------------------------------------------------------------------------	 |
|  ex de,hl		<  EB  >			  ........  1 / 4	 |
|  ex af,af'		<  08  >			  ........  1 / 4	 |
|  exx			<  D9  >			  ........  1 / 4	 |
|  ex (sp),hl		<  E3  >			  ........  5 / 19	 |
|  ex (sp),ix		<  DD  ><  E3  >		  ........  6 / 23	 |
|  ex (sp),iy		<  FD  ><  E3  >		  ........  6 / 23	 |
|  ldi			<  ED  ><  A0  >		  ..*0**0.  4 / 16	 |
|  ldir			<  ED  ><  B0  >		  ..*0*00.  5,4 / 21,16	 |
|  ldd			<  ED  ><  A8  >		  ..*0**0.  4 / 16	 |
|  lddr			<  ED  ><  B8  >		  ..*0*00.  5,4 / 21,16	 |
|  cpi			<  ED  ><  A1  >		  ******1.  4 / 16	 |
|  cpir			<  ED  ><  B1  >		  ******1.  5,4 / 21,16	 |
|  cpd			<  ED  ><  A9  >		  ******1.  4 / 16	 |
|  cpdr			<  ED  ><  B9  >		  ******1.  5,4 / 21,16  |
'-------------------------------------------------------------------------------*/

INSTRUCTION(ex_de_hl)  {zuint16 t; PC++; EX(DE, HL)			     return  4;}
INSTRUCTION(ex_af_af_) {zuint16 t; PC++; EX(AF, AF_)			     return  4;}
INSTRUCTION(exx)       {zuint16 t; PC++; EX(BC, BC_) EX(DE, DE_) EX(HL, HL_) return  4;}
INSTRUCTION(ex_vsp_hl) {zuint16 t; PC++; EX_VSP_X(HL)			     return 19;}
INSTRUCTION(ex_vsp_XY) {zuint16 t; PC += 2; EX_VSP_X(XY)		     return 23;}
INSTRUCTION(ldi)       {LDX (++)					     return 16;}
INSTRUCTION(ldir)      {LDXR(++)						       }
INSTRUCTION(ldd)       {LDX (--)					     return 16;}
INSTRUCTION(lddr)      {LDXR(--)						       }
INSTRUCTION(cpi)       {CPX (++)					     return 16;}
INSTRUCTION(cpir)      {CPXR(++)						       }
INSTRUCTION(cpd)       {CPX (--)					     return 16;}
INSTRUCTION(cpdr)      {CPXR(--)						       }


/* MARK: - Instructions: 8-Bit Arithmetic and Logical Group
.---------------------------------------------------------------------------.
|			0	1	2	3	  Flags		    |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles  |
|  -----------------------------------------------------------------------  |
|  U [a,]Y		10uuuyyy			  sz5h3***  1 / 4   |
|  U [a,]K		<  DD  >10uuukkk		  sz5h3***  2 / 8   |
|  U [a,]Q		<  FD  >10uuuqqq		  sz5h3***  2 / 8   |
|  U [a,]BYTE		11uuu110< BYTE >		  sz5h3***  2 / 7   |
|  U [a,](hl)		10uuu110			  sz5h3***  2 / 7   |
|  U [a,](ix+OFFSET)	<  DD  >10uuu110<OFFSET>	  sz5h3***  5 / 19  |
|  U [a,](iy+OFFSET)	<  FD  >10uuu110<OFFSET>	  sz5h3***  5 / 19  |
|  V X			00xxxvvv			  sz5h3v0.  1 / 4   |
|  V J			<  DD  >00jjjvvv		  sz5h3v0.  2 / 8   |
|  V P			<  FD  >00pppvvv		  sz5h3v0.  2 / 8   |
|  V (hl)		00110vvv			  sz5h3v*.  3 / 11  |
|  V (ix+OFFSET)	<  DD  >00110vvv<OFFSET>	  sz5h3v*.  6 / 23  |
|  V (iy+OFFSET)	<  FD  >00110vvv<OFFSET>	  sz5h3v*.  6 / 23  |
'--------------------------------------------------------------------------*/

INSTRUCTION(U_a_Y)	   {PC++; U0(Y0);					    return  4;}
INSTRUCTION(U_a_KQ)	   {PC += 2; U1(KQ);					    return  8;}
INSTRUCTION(U_a_BYTE)	   {U0(READ_8((PC += 2) - 1));				    return  7;}
INSTRUCTION(U_a_vhl)	   {PC++; U0(READ_8(HL));				    return  7;}
INSTRUCTION(U_a_vXYOFFSET) {U1(READ_8(XY + READ_OFFSET((PC += 3) - 1)));	    return 19;}
INSTRUCTION(V_X)	   {zuint8 *r; PC++;    r = __xxx___0(object); *r = V0(*r); return  4;}
INSTRUCTION(V_JP)	   {zuint8 *r; PC += 2; r = __jjj___ (object); *r = V1(*r); return  8;}
INSTRUCTION(V_vhl)	   {PC++; WRITE_8(HL, V0(READ_8(HL)));			    return 11;}
INSTRUCTION(V_vXYOFFSET)   {zuint16 a = (zuint16)(XY + READ_OFFSET((PC += 3) - 1));
			    WRITE_8(a, V1(READ_8(a)));				    return 23;}


/* MARK: - Instructions: General-Purpose Arithmetic and CPU Control Group
.---------------------------------------------------------------------------.
|			0	1	2	3	  Flags		    |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles  |
|  -----------------------------------------------------------------------  |
|  nop			<  00  >			  ........  1 / 4   |
|  halt			<  76  >			  ........  1 / 4   |
|  di			<  F3  >			  ........  1 / 4   |
|  ei			<  FB  >			  ........  1 / 4   |
|  im 0			<  ED  ><  46  >		  ........  2 / 8   |
|			<  ED  ><  4E  >				    |
|			<  ED  ><  66  >				    |
|			<  ED  ><  6E  >				    |
|  im 1			<  ED  ><  56  >		  ........  2 / 8   |
|			<  ED  ><  76  >				    |
|  im 2			<  ED  ><  5E  >		  ........  2 / 8   |
|			<  ED  ><  7E  >				    |
|  daa			<  27  >			  szyhxp.c  1 / 4   |
|  cpl			<  2F  >			  ..y1x.1.  1 / 4   |
|  neg			<  ED  ><  44  >		  szyhxv1c  2 / 8   |
|			<  ED  ><  4C  >				    |
|			<  ED  ><  54  >				    |
|			<  ED  ><  5C  >				    |
|			<  ED  ><  64  >				    |
|			<  ED  ><  6C  >				    |
|			<  ED  ><  74  >				    |
|			<  ED  ><  7C  >				    |
|  ccf			<  3F  >			  ..***.0c  1 / 4   |
|  scf			<  37  >			  ..*0*.01  1 / 4   |
'--------------------------------------------------------------------------*/

INSTRUCTION(nop)  {PC++;			     return 4;}
INSTRUCTION(halt) {HALT = 1; SET_HALT;		     return 4;}
INSTRUCTION(di)	  {PC++; IFF1 = IFF2 = 0; EI = TRUE; return 4;}
INSTRUCTION(ei)	  {PC++; IFF1 = IFF2 = 1; EI = TRUE; return 4;}
INSTRUCTION(im_0) {PC += 2; IM = 0;		     return 8;}
INSTRUCTION(im_1) {PC += 2; IM = 1;		     return 8;}
INSTRUCTION(im_2) {PC += 2; IM = 2;		     return 8;}


INSTRUCTION(daa)
	{
	zuint8 t = (F_H || (A & 0xF ) > 9) ? 6 : 0;

	if (F_C || A > 0x99) t |= 0x60;
	t = (F_N) ? A - t : A + t;
	PC++;

	F = (zuint8)
		((F_N)			 /* NF unchanged		 */
		 | (t & SYXF)		 /* SF = A.7; YF = A.5; XF = A.3 */
		 | ZF_ZERO(t)		 /* ZF = !Af			 */
		 | ((A & HF) ^ (t & HF)) /* HF = Ai.4 xor Af.4		 */
		 | PF_PARITY(t)		 /* PF = Parity of Af		 */
		 | (F_C | (A > 0x99)));	 /* CF = CF | (Ai > 0x99)	 */

	A = t;
	return 4;
	}


INSTRUCTION(cpl)
	{
	PC++; A = ~A;

	F =	(F & (SF | ZF | PF | CF)) /* SF, ZF, PF, CF unchanged */
		| HF			  /* HF = 1		      */
		| NF			  /* NF = 1		      */
		| A_YX;			  /* YF = A.5; XF = A.3	      */

	return 4;
	}


INSTRUCTION(neg)
	{
	zuint8 t = -A; PC += 2;

	F = (zuint8)
		((t & SYXF)	      /* SF = -A.7; YF = -A.5; XF = -A.3 */
		 | ZF_ZERO(t)	      /* ZF = !-A			 */
		 | ((0 ^ A ^ t) & HF) /* HF = Half-borrow		 */
		 | ((t == 128) << 2)  /* PF = Overflow			 */
		 | NF		      /* NF = 1				 */
		 | !!A);	      /* CF = !!A			 */

	A = t;
	return 8;
	}


INSTRUCTION(ccf)
	{
	PC++;

	F = (zuint8)
		(F_SZP	       /* SF, ZF, PF unchanged */
		 | A_YX	       /* YF = A.5; XF = A.3   */
		 | (F_C << 4)  /* HF = CF	       */
		 | (~F & CF)); /* CF = ~CF	       */
			       /* NF = 0	       */
	return 4;
	}


INSTRUCTION(scf)
	{
	PC++;

	F =	F_SZP  /* SF, ZF, PF unchanged */
		| A_YX /* YF = A.5; XF = A.3   */
		| CF;  /* CF = 1	       */
		       /* HF = 0; NF = 0       */
	return 4;
	}


/* MARK: - Instructions: 16-Bit Arithmetic Group
.---------------------------------------------------------------------------.
|			0	1	2	3	  Flags		    |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles  |
|  -----------------------------------------------------------------------  |
|  add hl,SS		00ss1001			  ..***.0*  3 / 11  |
|  adc hl,SS		<  ED  >01ss1010		  *****v0*  4 / 15  |
|  sbc hl,SS		<  ED  >01ss0010		  *****v0*  4 / 15  |
|  add ix,WW		<  DD  >00ww1001		  ..***.0*  4 / 15  |
|  add iy,WW		<  FD  >00ww1001		  ..***.0*  4 / 15  |
|  inc SS		00ss0011			  ........  1 / 6   |
|  inc ix		<  DD  ><  23  >		  ........  2 / 10  |
|  inc iy		<  FD  ><  23  >		  ........  2 / 10  |
|  dec SS		00ss1011			  ........  1 / 6   |
|  dec ix		<  DD  ><  2B  >		  ........  2 / 10  |
|  dec iy		<  FD  ><  2B  >		  ........  2 / 10  |
'--------------------------------------------------------------------------*/

INSTRUCTION(add_hl_SS) {PC++;	 ADD_RR_NN(HL, SS0)			 return 11;}
INSTRUCTION(adc_hl_SS) {ADC_SBC_HL_SS(adc, +, (zuint32)v + c + HL > 65535, Z_EMPTY)}
INSTRUCTION(sbc_hl_SS) {ADC_SBC_HL_SS(sbc, -, (zuint32)v + c > HL, | NF)	   }
INSTRUCTION(add_XY_WW) {PC += 2; ADD_RR_NN(XY, WW)			 return 15;}
INSTRUCTION(inc_SS)    {PC++;	 SS0++;					 return  6;}
INSTRUCTION(inc_XY)    {PC += 2; XY++;					 return 10;}
INSTRUCTION(dec_SS)    {PC++;	 SS0--;					 return  6;}
INSTRUCTION(dec_XY)    {PC += 2; XY--;					 return 15;}


/* MARK: - Instructions: Rotate and Shift Group
.---------------------------------------------------------------------------.
|			0	1	2	3	  Flags		    |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles  |
|  -----------------------------------------------------------------------  |
|  rlca			<  07  >			  ..y0x.0c  1 / 4   |
|  rla			<  17  >			  ..y0x.0c  1 / 4   |
|  rrca			<  0F  >			  ..y0x.0c  1 / 4   |
|  rra			<  1F  >			  ..y0x.0c  1 / 4   |
|  G Y			<  CB  >00gggyyy		  szy0xp0c  2 / 8   |
|  G (hl)		<  CB  >00ggg110		  szy0xp0c  4 / 15  |
|  G (ix+OFFSET)	<  DD  ><  CB  ><OFFSET>00ggg110  szy0xp0c  6 / 23  |
|  G (iy+OFFSET)	<  FD  ><  CB  ><OFFSET>00ggg110  szy0xp0c  6 / 23  |
|  G (ix+OFFSET),Y	<  DD  ><  CB  ><OFFSET>00gggyyy  szy0xp0c  6 / 23  |
|  G (iy+OFFSET),Y	<  FD  ><  CB  ><OFFSET>00gggyyy  szy0xp0c  6 / 23  |
|  rld			<  ED  ><  6F  >		  szy0xp0.  5 / 18  |
|  rrd			<  ED  ><  67  >		  szy0xp0.  5 / 18  |
'--------------------------------------------------------------------------*/

INSTRUCTION(rlca)	   {PC++; ROL(A); F = F_SZP | (A & YXCF);			      return  4;}
INSTRUCTION(rla)	   {zuint8 c; PC++; c = A >> 7; A = (zuint8)((A << 1) | F_C); RXA     return  4;}
INSTRUCTION(rrca)	   {PC++; ROR(A); F = F_SZP | A_YX | (A >> 7);			      return  4;}
INSTRUCTION(rra)	   {zuint8 c; PC++; c = A & 1; A = (zuint8)((A >> 1) | (F << 7)); RXA return  4;}
INSTRUCTION(G_Y)	   {zuint8 *r = _____yyy1(object); *r = G1(*r);			      return  8;}
INSTRUCTION(G_vhl)	   {WRITE_8(HL, G1(READ_8(HL)));				      return 15;}
INSTRUCTION(G_vXYOFFSET)   {zuint16 a = XY_ADDRESS; WRITE_8(a,	    G3(READ_8(a)));	      return 23;}
INSTRUCTION(G_vXYOFFSET_Y) {zuint16 a = XY_ADDRESS; WRITE_8(a, Y3 = G3(READ_8(a)));	      return 23;}
INSTRUCTION(rld)	   {RXD(<<, & 0xF, >> 4)					      return 18;}
INSTRUCTION(rrd)	   {RXD(>>, << 4, & 0xF)					      return 18;}


/* MARK: - Instructions: Bit Set, Reset and Test Group
.---------------------------------------------------------------------------.
|			0	1	2	3	  Flags		    |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles  |
|  -----------------------------------------------------------------------  |
|  bit N,Y		<  CB  >01nnnyyy		  szy1xz0.  2 / 8   |
|  bit N,(hl)		<  CB  >01nnn110		  sz?1?z0.  3 / 12  |
|  bit N,(ix+OFFSET)	<  DD  ><  CB  ><OFFSET>01nnn***  sz*1*z0.  5 / 20  |
|  bit N,(iy+OFFSET)	<  FD  ><  CB  ><OFFSET>01nnn***  sz*1*z0.  5 / 20  |
|  M N,Y		<  CB  >1mnnnyyy		  ........  2 / 8   |
|  M N,(hl)		<  CB  >1mnnn110		  ........  4 / 15  |
|  M N,(ix+OFFSET)	<  DD  ><  CB  ><OFFSET>1mnnn110  ........  6 / 23  |
|  M N,(iy+OFFSET)	<  FD  ><  CB  ><OFFSET>1mnnn110  ........  6 / 23  |
|  M N,(ix+OFFSET),Y	<  DD  ><  CB  ><OFFSET>1mnnnyyy  ........  6 / 23  |
|  M N,(iy+OFFSET),Y	<  FD  ><  CB  ><OFFSET>1mnnnyyy  ........  6 / 23  |
'--------------------------------------------------------------------------*/

INSTRUCTION(bit_N_Y)	     {BIT_N_VALUE(Y1)					      return  8;}
INSTRUCTION(bit_N_vhl)	     {BIT_N_VALUE(READ_8(HL))				      return 12;}
INSTRUCTION(bit_N_vXYOFFSET) {BIT_N_VADDRESS(XY_ADDRESS)			      return 20;}
INSTRUCTION(M_N_Y)	     {zuint8 *t = _____yyy1(object); *t = M1(*t);	      return  8;}
INSTRUCTION(M_N_vhl)	     {WRITE_8(HL, M1(READ_8(HL)));			      return 15;}
INSTRUCTION(M_N_vXYOFFSET)   {zuint16 a = XY_ADDRESS; WRITE_8(a,      M3(READ_8(a))); return 23;}
INSTRUCTION(M_N_vXYOFFSET_Y) {zuint16 a = XY_ADDRESS; WRITE_8(a, Y3 = M3(READ_8(a))); return 23;}


/* MARK: - Instructions: Jump Group
.-------------------------------------------------------------------------------.
|			0	1	2	3	  Flags			|
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles	|
|  ---------------------------------------------------------------------------	|
|  jp WORD		<  C3  ><     WORD     >	  ........  3 / 10	|
|  jp Z,WORD		11zzz010<     WORD     >	  ........  3 / 10	|
|  jr OFFSET		<  18  ><OFFSET>		  ........  3 / 12	|
|  jr Z,OFFSET		001zz000<OFFSET>		  ........  3,2 / 12,7	|
|  jp (hl)		<  E9  >			  ........  1 / 4	|
|  jp (ix)		<  DD  ><  E9  >		  ........  2 / 8	|
|  jp (iy)		<  FD  ><  E9  >		  ........  2 / 8	|
|  djnz OFFSET		<  10  ><OFFSET>		  ........  3,2 / 13,8	|
'------------------------------------------------------------------------------*/

INSTRUCTION(jp_WORD)	 {PC = READ_16(PC + 1);							return 10;}
INSTRUCTION(jp_Z_WORD)	 {PC = Z ? READ_16(PC + 1) : PC + 3;					return 10;}
INSTRUCTION(jr_OFFSET)	 {PC += (2 + READ_OFFSET(PC + 1));					return 12;}
INSTRUCTION(jr_Z_OFFSET) {BYTE0 &= 223; PC += 2; if (Z) {PC += READ_OFFSET(PC - 1); return 12;} return	7;}
INSTRUCTION(jp_hl)	 {PC = HL;								return	4;}
INSTRUCTION(jp_XY)	 {PC = XY;								return	8;}
INSTRUCTION(djnz_OFFSET) {PC += 2; if (--B) {PC += READ_OFFSET(PC - 1); return 13;}		return	8;}


/* MARK: - Instructions: Call and Return Group
.--------------------------------------------------------------------------------.
|			0	1	2	3	  Flags			 |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles	 |
|  ----------------------------------------------------------------------------	 |
|  call WORD		<  CD  ><     WORD     >	  ........  5 / 17	 |
|  call Z,WORD		11zzz100<     WORD     >	  ........  3,5 / 10,17	 |
|  ret			<  C9  >			  ........  3 / 10	 |
|  ret Z		11zzz000			  ........  1,3 / 5,11	 |
|  reti			<  ED  ><  4D  >		  ........  4 / 14	 |
|  retn			<  ED  ><  45  >		  ........  4 / 14	 |
|			<  ED  ><  55  >					 |
|			<  ED  ><  5D  >					 |
|			<  ED  ><  65  >					 |
|			<  ED  ><  6D  >					 |
|			<  ED  ><  75  >					 |
|			<  ED  ><  7D  >					 |
|  rst N		11nnn111			  ........  3 / 11	 |
'-------------------------------------------------------------------------------*/

INSTRUCTION(call_WORD)	 {PUSH(PC + 3); PC = READ_16(PC + 1);	    return 17;}
INSTRUCTION(call_Z_WORD) {if (Z) return call_WORD(object); PC += 3; return 10;}
INSTRUCTION(ret)	 {RET;					    return 10;}
INSTRUCTION(ret_Z)	 {if (Z) {RET; return 11;} PC++;	    return  5;}
INSTRUCTION(reti)	 {IFF1 = IFF2; RET;			    return 14;}
INSTRUCTION(retn)	 {IFF1 = IFF2; RET;			    return 14;}
INSTRUCTION(rst_N)	 {PUSH(PC + 1); PC = BYTE0 & 56;	    return 11;}


/* MARK: - Instructions: Input and Output Group
.--------------------------------------------------------------------------------.
|			0	1	2	3	  Flags			 |
|  Assembly		76543210765432107654321076543210  szyhxpnc  Cycles	 |
|  ----------------------------------------------------------------------------	 |
|  in a,(BYTE)		<  DB  >< BYTE >		  ........  3 / 11	 |
|  in X,(c)		<  ED  >01xxx000		  szy0xp0.  3 / 12	 |
|  in 0,(c)		<  ED  ><  70  >		  szy0xp0.  3 / 12	 |
|  ini			<  ED  ><  A2  >		  ********  4 / 16	 |
|  inir			<  ED  ><  B2  >		  010*0***  5,4 / 21,16	 |
|  ind			<  ED  ><  AA  >		  ********  4 / 16	 |
|  indr			<  ED  ><  BA  >		  010*0***  5,4 / 21,16	 |
|  out (BYTE),a		<  D3  >< BYTE >		  ........  3 / 11	 |
|  out (c),X		<  ED  >01xxx001		  ........  3 / 12	 |
|  out (c),0		<  ED  ><  71  >		  ........  3 / 12	 |
|  outi			<  ED  ><  A3  >		  ********  4 / 16	 |
|  otir			<  ED  ><  B3  >		  010*0_**  5,4 / 21,16	 |
|  outd			<  ED  ><  AB  >		  ********  4 / 16	 |
|  otdr			<  ED  ><  BB  >		  010*0***  5,4 / 21,16	 |
'-------------------------------------------------------------------------------*/

INSTRUCTION(in_a_BYTE)	 {A = IN((A << 8) | READ_8((PC += 2) - 1)); return 11;}
INSTRUCTION(in_X_vc)	 {IN_VC; X1 = t;			    return 12;}
INSTRUCTION(in_0_vc)	 {IN_VC;				    return 16;}
INSTRUCTION(ini)	 {INX (++, +)				    return 16;}
INSTRUCTION(inir)	 {INXR(++, +)					      }
INSTRUCTION(ind)	 {INX (--, -)				    return 16;}
INSTRUCTION(indr)	 {INXR(--, -)					      }
INSTRUCTION(out_vBYTE_a) {OUT((A << 8) | READ_8((PC += 2) - 1), A); return 11;}
INSTRUCTION(out_vc_X)	 {PC += 2; OUT(BC, X1);			    return 12;}
INSTRUCTION(out_vc_0)	 {PC += 2; OUT(BC, 0);			    return 12;}
INSTRUCTION(outi)	 {OUTX(++)				    return 16;}
INSTRUCTION(otir)	 {OTXR(++)					      }
INSTRUCTION(outd)	 {OUTX(--)				    return 16;}
INSTRUCTION(otdr)	 {OTXR(--)					      }


/* MARK: - Opcode Selector Prototypes */

INSTRUCTION(CB);
INSTRUCTION(DD);
INSTRUCTION(ED);
INSTRUCTION(FD);
INSTRUCTION(XY_CB);
INSTRUCTION(ED_illegal);
INSTRUCTION(XY_illegal);


/* MARK: - Instruction Function Tables */

static Instruction const instruction_table[256] = {
/*	0	     1		 2	      3		   4		5	  6	       7	 8	      9		 A	      B		 C	      D		 E	    F */
/* 0 */ nop,	     ld_SS_WORD, ld_vbc_a,    inc_SS,	   V_X,		V_X,	  ld_X_BYTE,   rlca,	 ex_af_af_,   add_hl_SS, ld_a_vbc,    dec_SS,	 V_X,	      V_X,	 ld_X_BYTE, rrca,
/* 1 */ djnz_OFFSET, ld_SS_WORD, ld_vde_a,    inc_SS,	   V_X,		V_X,	  ld_X_BYTE,   rla,	 jr_OFFSET,   add_hl_SS, ld_a_vde,    dec_SS,	 V_X,	      V_X,	 ld_X_BYTE, rra,
/* 2 */ jr_Z_OFFSET, ld_SS_WORD, ld_vWORD_hl, inc_SS,	   V_X,		V_X,	  ld_X_BYTE,   daa,	 jr_Z_OFFSET, add_hl_SS, ld_hl_vWORD, dec_SS,	 V_X,	      V_X,	 ld_X_BYTE, cpl,
/* 3 */ jr_Z_OFFSET, ld_SS_WORD, ld_vWORD_a,  inc_SS,	   V_vhl,	V_vhl,	  ld_vhl_BYTE, scf,	 jr_Z_OFFSET, add_hl_SS, ld_a_vWORD,  dec_SS,	 V_X,	      V_X,	 ld_X_BYTE, ccf,
/* 4 */ ld_X_Y,	     ld_X_Y,	 ld_X_Y,      ld_X_Y,	   ld_X_Y,	ld_X_Y,	  ld_X_vhl,    ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_vhl,  ld_X_Y,
/* 5 */ ld_X_Y,	     ld_X_Y,	 ld_X_Y,      ld_X_Y,	   ld_X_Y,	ld_X_Y,	  ld_X_vhl,    ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_vhl,  ld_X_Y,
/* 6 */ ld_X_Y,	     ld_X_Y,	 ld_X_Y,      ld_X_Y,	   ld_X_Y,	ld_X_Y,	  ld_X_vhl,    ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_vhl,  ld_X_Y,
/* 7 */ ld_vhl_Y,    ld_vhl_Y,	 ld_vhl_Y,    ld_vhl_Y,	   ld_vhl_Y,	ld_vhl_Y, halt,	       ld_vhl_Y, ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_Y,      ld_X_Y,	 ld_X_vhl,  ld_X_Y,
/* 8 */ U_a_Y,	     U_a_Y,	 U_a_Y,	      U_a_Y,	   U_a_Y,	U_a_Y,	  U_a_vhl,     U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_vhl,   U_a_Y,
/* 9 */ U_a_Y,	     U_a_Y,	 U_a_Y,	      U_a_Y,	   U_a_Y,	U_a_Y,	  U_a_vhl,     U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_vhl,   U_a_Y,
/* A */ U_a_Y,	     U_a_Y,	 U_a_Y,	      U_a_Y,	   U_a_Y,	U_a_Y,	  U_a_vhl,     U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_vhl,   U_a_Y,
/* B */ U_a_Y,	     U_a_Y,	 U_a_Y,	      U_a_Y,	   U_a_Y,	U_a_Y,	  U_a_vhl,     U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_Y,	      U_a_Y,	 U_a_vhl,   U_a_Y,
/* C */ ret_Z,	     pop_TT,	 jp_Z_WORD,   jp_WORD,	   call_Z_WORD,	push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      ret,	 jp_Z_WORD,   CB,	 call_Z_WORD, call_WORD, U_a_BYTE,  rst_N,
/* D */ ret_Z,	     pop_TT,	 jp_Z_WORD,   out_vBYTE_a, call_Z_WORD,	push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      exx,	 jp_Z_WORD,   in_a_BYTE, call_Z_WORD, DD,	 U_a_BYTE,  rst_N,
/* E */ ret_Z,	     pop_TT,	 jp_Z_WORD,   ex_vsp_hl,   call_Z_WORD,	push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      jp_hl,	 jp_Z_WORD,   ex_de_hl,	 call_Z_WORD, ED,	 U_a_BYTE,  rst_N,
/* F */ ret_Z,	     pop_TT,	 jp_Z_WORD,   di,	   call_Z_WORD,	push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      ld_sp_hl,	 jp_Z_WORD,   ei,	 call_Z_WORD, FD,	 U_a_BYTE,  rst_N
};

static Instruction const instruction_table_CB[256] = {
/*	0	 1	  2	   3	    4	     5	      6		 7	  8	   9	    A	     B	      C	       D	E	   F */
/* 0 */ G_Y,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_vhl,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_Y,     G_Y,	G_vhl,	   G_Y,
/* 1 */ G_Y,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_vhl,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_Y,     G_Y,	G_vhl,	   G_Y,
/* 2 */ G_Y,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_vhl,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_Y,     G_Y,	G_vhl,	   G_Y,
/* 3 */ G_Y,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_vhl,	 G_Y,	  G_Y,	   G_Y,	    G_Y,     G_Y,     G_Y,     G_Y,	G_vhl,	   G_Y,
/* 4 */ bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y,
/* 5 */ bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y,
/* 6 */ bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y,
/* 7 */ bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_Y, bit_N_vhl, bit_N_Y,
/* 8 */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y,
/* 9 */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y,
/* A */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y,
/* B */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y,
/* C */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y,
/* D */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y,
/* E */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y,
/* F */ M_N_Y,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_vhl,	 M_N_Y,	  M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,   M_N_Y,	M_N_vhl,   M_N_Y
};

static Instruction const instruction_table_XY_CB[256] = {
/*	0		 1		  2		   3		    4		     5		      6		       7		8		 9		  A		   B		    C		     D		      E		       F */
/* 0 */ G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,	G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,
/* 1 */ G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,	G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,
/* 2 */ G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,	G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,
/* 3 */ G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,	G_vXYOFFSET_Y,	 G_vXYOFFSET_Y,	  G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET_Y,   G_vXYOFFSET,     G_vXYOFFSET_Y,
/* 4 */ bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET,
/* 5 */ bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET,
/* 6 */ bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET,
/* 7 */ bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET, bit_N_vXYOFFSET,
/* 8 */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y,
/* 9 */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y,
/* A */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y,
/* B */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y,
/* C */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y,
/* D */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y,
/* E */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y,
/* F */ M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET_Y, M_N_vXYOFFSET,   M_N_vXYOFFSET_Y
};

static Instruction const instruction_table_XY[256] = {
/*	0		1		2		3		4		5		6		   7		   8	       9	   A		B	    C		D	    E		    F */
/* 0 */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	   XY_illegal,	   XY_illegal, add_XY_WW,  XY_illegal,	XY_illegal, XY_illegal, XY_illegal, XY_illegal,	    XY_illegal,
/* 1 */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	   XY_illegal,	   XY_illegal, add_XY_WW,  XY_illegal,	XY_illegal, XY_illegal, XY_illegal, XY_illegal,	    XY_illegal,
/* 2 */ XY_illegal,	ld_XY_WORD,	ld_vWORD_XY,	inc_XY,		V_JP,		V_JP,		ld_JP_BYTE,	   XY_illegal,	   XY_illegal, add_XY_WW,  ld_XY_vWORD, dec_XY,	    V_JP,	V_JP,	    ld_JP_BYTE,	    XY_illegal,
/* 3 */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	V_vXYOFFSET,	V_vXYOFFSET,	ld_vXYOFFSET_BYTE, XY_illegal,	   XY_illegal, add_XY_WW,  XY_illegal,	XY_illegal, XY_illegal, XY_illegal, XY_illegal,	    XY_illegal,
/* 4 */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	ld_JP_KQ,	ld_JP_KQ,	ld_X_vXYOFFSET,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_illegal, ld_JP_KQ,	ld_JP_KQ,   ld_X_vXYOFFSET, XY_illegal,
/* 5 */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	ld_JP_KQ,	ld_JP_KQ,	ld_X_vXYOFFSET,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_illegal, ld_JP_KQ,	ld_JP_KQ,   ld_X_vXYOFFSET, XY_illegal,
/* 6 */ ld_JP_KQ,	ld_JP_KQ,	ld_JP_KQ,	ld_JP_KQ,	ld_JP_KQ,	ld_JP_KQ,	ld_X_vXYOFFSET,	   ld_JP_KQ,	   ld_JP_KQ,   ld_JP_KQ,   ld_JP_KQ,	ld_JP_KQ,   ld_JP_KQ,	ld_JP_KQ,   ld_X_vXYOFFSET, ld_JP_KQ,
/* 7 */ ld_vXYOFFSET_Y, ld_vXYOFFSET_Y, ld_vXYOFFSET_Y, ld_vXYOFFSET_Y, ld_vXYOFFSET_Y, ld_vXYOFFSET_Y, XY_illegal,	   ld_vXYOFFSET_Y, XY_illegal, XY_illegal, XY_illegal,	XY_illegal, ld_JP_KQ,	ld_JP_KQ,   ld_X_vXYOFFSET, XY_illegal,
/* 8 */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	U_a_KQ,		U_a_KQ,		U_a_vXYOFFSET,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_illegal, U_a_KQ,	U_a_KQ,	    U_a_vXYOFFSET,  XY_illegal,
/* 9 */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	U_a_KQ,		U_a_KQ,		U_a_vXYOFFSET,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_illegal, U_a_KQ,	U_a_KQ,	    U_a_vXYOFFSET,  XY_illegal,
/* A */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	U_a_KQ,		U_a_KQ,		U_a_vXYOFFSET,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_illegal, U_a_KQ,	U_a_KQ,	    U_a_vXYOFFSET,  XY_illegal,
/* B */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	U_a_KQ,		U_a_KQ,		U_a_vXYOFFSET,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_illegal, U_a_KQ,	U_a_KQ,	    U_a_vXYOFFSET,  XY_illegal,
/* C */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_CB,	    XY_illegal, XY_illegal, XY_illegal,	    XY_illegal,
/* D */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	   XY_illegal,	   XY_illegal, XY_illegal, XY_illegal,	XY_illegal, XY_illegal, XY_illegal, XY_illegal,	    XY_illegal,
/* E */ XY_illegal,	pop_XY,		XY_illegal,	ex_vsp_XY,	XY_illegal,	push_XY,	XY_illegal,	   XY_illegal,	   XY_illegal, jp_XY,	   XY_illegal,	XY_illegal, XY_illegal, XY_illegal, XY_illegal,	    XY_illegal,
/* F */ XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	XY_illegal,	   XY_illegal,	   XY_illegal, ld_sp_XY,   XY_illegal,	XY_illegal, XY_illegal, XY_illegal, XY_illegal,	    XY_illegal
};

static Instruction const instruction_table_ED[256] = {
/*	0	    1		2	    3		 4	     5		 6	     7		 8	     9		 A	     B		  C	      D		  E	      F */
/* 0 */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* 1 */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* 2 */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* 3 */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* 4 */ in_X_vc,    out_vc_X,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_0,	     ld_i_a,	 in_X_vc,    out_vc_X,	 adc_hl_SS,  ld_SS_vWORD, neg,	      reti,	  im_0,	      ld_r_a,
/* 5 */ in_X_vc,    out_vc_X,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_1,	     ld_a_i,	 in_X_vc,    out_vc_X,	 adc_hl_SS,  ld_SS_vWORD, neg,	      retn,	  im_2,	      ld_a_r,
/* 6 */ in_X_vc,    out_vc_X,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_0,	     rrd,	 in_X_vc,    out_vc_X,	 adc_hl_SS,  ld_SS_vWORD, neg,	      retn,	  im_0,	      rld,
/* 7 */ in_0_vc,    out_vc_0,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_1,	     ED_illegal, in_X_vc,    out_vc_X,	 adc_hl_SS,  ld_SS_vWORD, neg,	      retn,	  im_2,	      ED_illegal,
/* 8 */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* 9 */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* A */ ldi,	    cpi,	ini,	    outi,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ldd,	     cpd,	 ind,	     outd,	  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* B */ ldir,	    cpir,	inir,	    otir,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, lddr,	     cpdr,	 indr,	     otdr,	  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* C */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* D */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* E */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal,
/* F */ ED_illegal, ED_illegal, ED_illegal, ED_illegal,	 ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal, ED_illegal,  ED_illegal, ED_illegal, ED_illegal, ED_illegal
};


/* MARK: - Prefixed Instruction Set Selection and Execution */

#define DD_FD(register)						       \
	zuint8 cycles;						       \
								       \
	XY = register;						       \
	R++;							       \
	cycles = instruction_table_XY[BYTE1 = READ_8(PC + 1)](object); \
	register = XY;						       \
	return cycles;


INSTRUCTION(DD) {DD_FD(IX)}
INSTRUCTION(FD) {DD_FD(IY)}
INSTRUCTION(CB) {R++; return instruction_table_CB[BYTE1 = READ_8((PC += 2) - 1)](object);}
INSTRUCTION(ED) {R++; return instruction_table_ED[BYTE1 = READ_8( PC	   + 1)](object);}


INSTRUCTION(XY_CB)
	{
	PC += 4;
	BYTE2 = READ_8(PC - 2);
	return instruction_table_XY_CB[BYTE3 = READ_8(PC - 1)](object);
	}


/* MARK: - Illegal Instruction Handling */

INSTRUCTION(XY_illegal) {PC += 1; return instruction_table[BYTE0 = BYTE1](object) + 4;}
INSTRUCTION(ED_illegal) {PC += 2; return 8;}


/* MARK: - Main Functions */

CPU_Z80_API void z80_power(Z80 *object, zboolean state)
	{
	if (state)
		{
#		ifdef Z_Z80_RESET_IS_EQUAL_TO_POWER_ON
			z80_reset(object);
#		else
			PC   = Z_Z80_VALUE_AFTER_POWER_ON_PC;
			SP   = Z_Z80_VALUE_AFTER_POWER_ON_SP;
			IX   = Z_Z80_VALUE_AFTER_POWER_ON_IX;
			IY   = Z_Z80_VALUE_AFTER_POWER_ON_IY;
			AF   = Z_Z80_VALUE_AFTER_POWER_ON_AF;
			BC   = Z_Z80_VALUE_AFTER_POWER_ON_BC;
			DE   = Z_Z80_VALUE_AFTER_POWER_ON_DE;
			HL   = Z_Z80_VALUE_AFTER_POWER_ON_HL;
			AF_  = Z_Z80_VALUE_AFTER_POWER_ON_AF_;
			BC_  = Z_Z80_VALUE_AFTER_POWER_ON_BC_;
			DE_  = Z_Z80_VALUE_AFTER_POWER_ON_DE_;
			HL_  = Z_Z80_VALUE_AFTER_POWER_ON_HL_;
			R    = Z_Z80_VALUE_AFTER_POWER_ON_R;
			I    = Z_Z80_VALUE_AFTER_POWER_ON_I;
			IFF1 = Z_Z80_VALUE_AFTER_POWER_ON_IFF1;
			IFF2 = Z_Z80_VALUE_AFTER_POWER_ON_IFF2;
			IM   = Z_Z80_VALUE_AFTER_POWER_ON_IM;

			EI = HALT = INT = NMI = 0;
#		endif
		}

	else	PC = SP = IX = IY = AF = BC = DE = HL = AF_ = BC_ = DE_ = HL_ = I = R =
		IFF1 = IFF2 = IM = EI = HALT = INT = NMI = 0;
	}


CPU_Z80_API void z80_reset(Z80 *object)
	{
	PC   = Z_Z80_VALUE_AFTER_RESET_PC;
	SP   = Z_Z80_VALUE_AFTER_RESET_SP;
	IX   = Z_Z80_VALUE_AFTER_RESET_IX;
	IY   = Z_Z80_VALUE_AFTER_RESET_IY;
	AF   = Z_Z80_VALUE_AFTER_RESET_AF;
	BC   = Z_Z80_VALUE_AFTER_RESET_BC;
	DE   = Z_Z80_VALUE_AFTER_RESET_DE;
	HL   = Z_Z80_VALUE_AFTER_RESET_HL;
	AF_  = Z_Z80_VALUE_AFTER_RESET_AF_;
	BC_  = Z_Z80_VALUE_AFTER_RESET_BC_;
	DE_  = Z_Z80_VALUE_AFTER_RESET_DE_;
	HL_  = Z_Z80_VALUE_AFTER_RESET_HL_;
	R    = Z_Z80_VALUE_AFTER_RESET_R;
	I    = Z_Z80_VALUE_AFTER_RESET_I;
	IFF1 = Z_Z80_VALUE_AFTER_RESET_IFF1;
	IFF2 = Z_Z80_VALUE_AFTER_RESET_IFF2;
	IM   = Z_Z80_VALUE_AFTER_RESET_IM;

	EI = HALT = INT = NMI = 0;
	}


CPU_Z80_API zusize z80_run(Z80 *object, zusize cycles)
	{
	zuint32 data;

	/*-------------.
	| Clear cycles |
	'-------------*/
	CYCLES = 0;

	/*--------------.
	| Backup R7 bit |
	'--------------*/
	R7 = R;

	/*------------------------------.
	| Execute until cycles consumed |
	'------------------------------*/
	while (CYCLES < cycles)
		{
		/*--------------------------------------.
		| Jump to NMI handler if NMI pending... |
		'--------------------------------------*/
		if (NMI)
			{
			EXIT_HALT;			/* Resume CPU if halted.				   */
			R++;				/* Consume memory refresh.				   */
			NMI = FALSE;			/* Clear the NMI pulse.					   */
			/*IFF2 = IFF1;*/		/* Backup IFF1 (it doesn't occur, acording to Sean Young). */
			IFF1 = 0;			/* Reset IFF1 to don't bother the NMI routine.		   */
			PUSH(PC);			/* Save return addres in the stack.			   */
			PC = Z_Z80_ADDRESS_NMI_POINTER;	/* Make PC point to the NMI routine.			   */
			CYCLES += 11;			/* Accepting a NMI consumes 11 cycles.			   */
			continue;
			}

		/*--------------------------.
		| Execute INT if pending... |
		'--------------------------*/
		if (INT && IFF1 && !EI)
			{
			EXIT_HALT;	 /* Resume CPU on halt.		*/
			R++;		 /* Consume memory refresh.	*/
			IFF1 = IFF2 = 0; /* Clear interrupt flip-flops.	*/

			switch (IM)
				{
				/*------------------------------.
				| IM 0: Execute bus instruction |
				'------------------------------*/
				case 0:

				if ((data = INT_DATA)) switch (data & Z_UINT32(0xFF000000))
					{
					case Z_UINT32(0xC3000000): /* JP */
					PC = (zuint16)(data >> 8);
					CYCLES += 10;
					break;

					case Z_UINT32(0xCD000000): /* CALL */
					PUSH(PC);
					PC = (zuint16)(data >> 8);
					CYCLES += 17;
					break;

					default: /* RST (and possibly others) */
					PUSH(PC);
					PC = (zuint16)((data >> 8) & 0x38);
					CYCLES += 11;
					}

				CYCLES += 2;
				break;

				/*----------------------.
				| IM 1: Execute rst 38h |
				'----------------------*/
				case 1:
				PUSH(PC);
				PC = 0x38;
				CYCLES += (11 + 2);
				break;

				/*---------------------------.
				| IM 2: Execute rst [i:byte] |
				'---------------------------*/
				case 2:
				PUSH(PC);
				PC = READ_16(((zuint16)(I << 8)) | (INT_DATA & 0xFF));
				CYCLES += (17 + 2);
				break;
				}

			continue;
			}

		/*---------------------------------------.
		| Consume memory refresh and update bits |
		'---------------------------------------*/
		R++;
		EI = FALSE;

		/*-----------------------------------------------.
		| Execute instruction and update consumed cycles |
		'-----------------------------------------------*/
		CYCLES += instruction_table[BYTE0 = READ_8(PC)](object);
		}

	/*---------------.
	| Restore R7 bit |
	'---------------*/
	R = R_ALL;

	/*-----------------------.
	| Return consumed cycles |
	'-----------------------*/
	return CYCLES;
	}


CPU_Z80_API void z80_nmi(Z80 *object)		      {NMI = TRUE ;}
CPU_Z80_API void z80_int(Z80 *object, zboolean state) {INT = state;}


/* MARK: - ABI */

#ifdef CPU_Z80_WITH_ABI

	static void will_read_state(Z80 *object) {R  = R_ALL;}
	static void did_write_state(Z80 *object) {R7 = R;    }

	static ZCPUEmulatorExport const exports[7] = {
		{Z_EMULATOR_FUNCTION_POWER,	      {(void (*)(void))z80_power      }},
		{Z_EMULATOR_FUNCTION_RESET,	      {(void (*)(void))z80_reset      }},
		{Z_EMULATOR_FUNCTION_RUN,	      {(void (*)(void))z80_run	      }},
		{Z_EMULATOR_FUNCTION_WILL_READ_STATE, {(void (*)(void))will_read_state}},
		{Z_EMULATOR_FUNCTION_DID_WRITE_STATE, {(void (*)(void))did_write_state}},
		{Z_EMULATOR_FUNCTION_NMI,	      {(void (*)(void))z80_nmi	      }},
		{Z_EMULATOR_FUNCTION_IRQ,	      {(void (*)(void))z80_int	      }}
	};

	static ZCPUEmulatorInstanceImport const instance_imports[6] = {
		{Z_EMULATOR_FUNCTION_READ_8BIT,	 O(read	   )},
		{Z_EMULATOR_FUNCTION_WRITE_8BIT, O(write   )},
		{Z_EMULATOR_FUNCTION_IN_8BIT,	 O(in	   )},
		{Z_EMULATOR_FUNCTION_OUT_8BIT,	 O(out	   )},
		{Z_EMULATOR_FUNCTION_IRQ_DATA,	 O(int_data)},
		{Z_EMULATOR_FUNCTION_HALT,	 O(halt	   )}
	};

	CPU_Z80_ABI ZCPUEmulatorABI const abi_emulation_cpu_z80 = {
		/* dependency_count	 */ 0,
		/* dependencies		 */ NULL,
		/* export_count		 */ 7,
		/* exports		 */ exports,
		/* instance_size	 */ sizeof(Z80),
		/* instance_state_offset */ O(state),
		/* instance_state_size	 */ sizeof(ZZ80State),
		/* instance_import_count */ 6,
		/* instance_imports	 */ instance_imports
	};

#endif

#ifdef CPU_Z80_WITH_MODULE_ABI

#	ifndef CPU_Z80_DEPENDENCIES_H
#		include <Z/ABIs/generic/module.h>
#	endif

	static ZModuleUnit const unit = {"Z80", "Z80", Z_VERSION(0, 1, 0), &abi_emulation_cpu_z80};
	static ZModuleDomain const domain = {"Emulation.CPU", Z_VERSION(1, 0, 0), 1, &unit};
	Z_API_WEAK_EXPORT ZModuleABI const __module_abi__ = {1, &domain};

#endif


/* Z80.c EOF */
