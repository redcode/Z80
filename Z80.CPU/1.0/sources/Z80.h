/* Z80.h
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2011 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include <Q/hardware/CPU/architecture/Z80.h>
#include <Q/types/generic functions.h>

#ifndef __Z80_H__
#define __Z80_H__

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

qsize	z80_run		(Z80*	  object,
			 qsize	  cycles);

void	z80_power	(Z80*	  object,
			 qboolean state);

void	z80_reset	(Z80*	  object);

qsize	z80_nmi		(Z80*	  object);

qsize	z80_irq		(Z80*	  object,
			 qboolean state);

#endif /* __Z80_H__ */

