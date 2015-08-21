/* Zilog Z80 CPU Emulator C API v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 1999-2015 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __emulation_CPU_Z80_H__
#define __emulation_CPU_Z80_H__

#include <Z/hardware/CPU/architecture/Z80.h>
#include <Z/types/generic functions.h>

#ifndef EMULATION_CPU_Z80_NO_SLOTS
#	include <Z/macros/slot.h>
#endif

#if defined(BUILDING_DYNAMIC_EMULATION_CPU_Z80)
#	define CPU_Z80_API Z_API_EXPORT
#elif defined(BUILDING_STATIC_EMULATION_CPU_Z80)
#	define CPU_Z80_API Z_PUBLIC
#elif defined(USE_STATIC_EMULATION_CPU_Z80)
#	define CPU_Z80_API
#else
#	define CPU_Z80_API Z_API
#endif

typedef struct {
	zsize	  cycles;
	ZZ80State state;
	Z16Bit	  xy;
	zuint8	  r7;
	Z32Bit	  data;

#	ifdef EMULATION_CPU_Z80_NO_SLOTS
		void* cb_context;

		struct {Z16BitAddressRead8Bit  read;
			Z16BitAddressWrite8Bit write;
			Z16BitAddressRead8Bit  in;
			Z16BitAddressWrite8Bit out;
			ZRead32Bit	       int_data;
			ZSwitch		       halt;
		} cb;
#	else
		struct {ZSlot(Z16BitAddressRead8Bit ) read;
			ZSlot(Z16BitAddressWrite8Bit) write;
			ZSlot(Z16BitAddressRead8Bit ) in;
			ZSlot(Z16BitAddressWrite8Bit) out;
			ZSlot(ZRead32Bit	    ) int_data;
			ZSlot(ZSwitch		    ) halt;
		} cb;
#	endif
} Z80;

Z_C_SYMBOLS_BEGIN

CPU_Z80_API zsize z80_run   (Z80*     object,
			     zsize    cycles);

CPU_Z80_API void  z80_power (Z80*     object,
			     zboolean state);

CPU_Z80_API void  z80_reset (Z80*     object);

CPU_Z80_API void  z80_nmi   (Z80*     object);

CPU_Z80_API void  z80_irq   (Z80*     object,
			     zboolean state);

Z_C_SYMBOLS_END

#endif /* __emulation_CPU_Z80_H__ */
