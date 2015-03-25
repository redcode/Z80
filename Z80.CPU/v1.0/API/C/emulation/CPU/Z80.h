/* Zilog Z80 CPU Emulator v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 1999-2015 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __emulation_CPU_Z80_H__
#define __emulation_CPU_Z80_H__

#include <Q/hardware/CPU/architecture/Z80.h>
#include <Q/types/generic functions.h>

#ifndef EMULATION_CPU_Z80_NO_SLOTS
#	include <Q/macros/slot.h>
#endif

#if defined(BUILDING_DYNAMIC_EMULATION_CPU_Z80)
#	define CPU_Z80_API Q_API_EXPORT
#elif defined(BUILDING_STATIC_EMULATION_CPU_Z80)
#	define CPU_Z80_API Q_PUBLIC
#elif defined(USE_STATIC_EMULATION_CPU_Z80)
#	define CPU_Z80_API
#else
#	define CPU_Z80_API Q_API
#endif

typedef struct {
	qsize	  cycles;
	QZ80State state;
	Q16Bit	  xy;
	quint8	  r7;
	Q32Bit	  data;

#	ifdef EMULATION_CPU_Z80_NO_SLOTS
		void* cb_context;

		struct {Q16BitAddressRead8Bit  read;
			Q16BitAddressWrite8Bit write;
			Q16BitAddressRead8Bit  in;
			Q16BitAddressWrite8Bit out;
			QRead32Bit	       int_data;
			QSwitch		       halt;
		} cb;
#	else
		struct {QSlot(Q16BitAddressRead8Bit ) read;
			QSlot(Q16BitAddressWrite8Bit) write;
			QSlot(Q16BitAddressRead8Bit ) in;
			QSlot(Q16BitAddressWrite8Bit) out;
			QSlot(QRead32Bit	    ) int_data;
			QSlot(QSwitch		    ) halt;
		} cb;
#	endif
} Z80;

Q_C_SYMBOLS_BEGIN

CPU_Z80_API qsize z80_run   (Z80*     object,
			     qsize    cycles);

CPU_Z80_API void  z80_power (Z80*     object,
			     qboolean state);

CPU_Z80_API void  z80_reset (Z80*     object);

CPU_Z80_API void  z80_nmi   (Z80*     object);

CPU_Z80_API void  z80_irq   (Z80*     object,
			     qboolean state);

Q_C_SYMBOLS_END

#endif /* __emulation_CPU_Z80_H__ */
