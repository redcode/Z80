/* Zilog Z80 CPU Emulator v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 1999 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include <Q/hardware/CPU/architecture/Z80.h>
#include <Q/types/generic functions.h>

#ifndef __modules_emulation_CPU_Z80_H__
#define __modules_emulation_CPU_Z80_H__

typedef struct {
	QZ80State state;
	Q16Bit	  xy;
	quint8	  r7;
	Q32Bit	  data;
	qsize	  ticks;
	void*	  cb_context;

	struct {Q16BitAddressRead8Bit  read;
		Q16BitAddressWrite8Bit write;
		Q16BitAddressRead8Bit  in;
		Q16BitAddressWrite8Bit out;
		QRead32Bit	       int_data;
		QSwitch		       halt;
	} cb;
} Z80;


#ifndef BUILDING_Z80_CPU

#	ifdef __cplusplus
	extern "C" {
#	endif

#	ifdef USING_STATIC_LIBRARY_Z80_CPU
#		define Z80_CPU_API
#	else
#		define Z80_CPU_API Q_API
#	endif

	Z80_CPU_API qsize z80_run   (Z80*     object,
				     qsize    cycles);

	Z80_CPU_API void  z80_power (Z80*     object,
				     qboolean state);

	Z80_CPU_API void  z80_reset (Z80*     object);

	Z80_CPU_API void  z80_nmi   (Z80*     object);

	Z80_CPU_API void  z80_irq   (Z80*     object,
				     qboolean state);

#	ifdef __cplusplus
	}
#	endif

#endif

#endif /* __modules_emulation_CPU_Z80_H__ */
