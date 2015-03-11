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

#ifdef EMULATION_CPU_Z80_NO_SLOTS
#	define Z80_CB(Type) Type
#else
#	include <Q/macros/slot.h>
#	define Z80_CB QSlot
#endif

#if defined(BUILDING_DYNAMIC_EMULATION_CPU_Z80)
#	define Z80_API Q_API_EXPORT
#elif defined(BUILDING_STATIC_EMULATION_CPU_Z80)
#	define Z80_API Q_PUBLIC
#elif defined(USE_STATIC_EMULATION_CPU_Z80)
#	define Z80_API
#else
#	define Z80_API Q_API
#endif

typedef struct {
	qsize	  cycles;
	QZ80State state;
	Q16Bit	  xy;
	quint8	  r7;
	Q32Bit	  data;

#	ifdef EMULATION_CPU_Z80_NO_SLOTS
		void* cb_context;
#	endif

	struct {Z80_CB(Q16BitAddressRead8Bit ) read;
		Z80_CB(Q16BitAddressWrite8Bit) write;
		Z80_CB(Q16BitAddressRead8Bit ) in;
		Z80_CB(Q16BitAddressWrite8Bit) out;
		Z80_CB(QRead32Bit	     ) int_data;
		Z80_CB(QSwitch		     ) halt;
	} cb;
} Z80;

Q_C_SYMBOLS_BEGIN

Z80_API qsize z80_run	(Z80*	  object,
			 qsize	  cycles);

Z80_API void  z80_power (Z80*	  object,
			 qboolean state);

Z80_API void  z80_reset (Z80*	  object);

Z80_API void  z80_nmi	(Z80*	  object);

Z80_API void  z80_irq	(Z80*	  object,
			 qboolean state);

Q_C_SYMBOLS_END

#endif /* __emulation_CPU_Z80_H__ */
