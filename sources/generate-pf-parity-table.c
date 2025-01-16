/* generate-pf-parity-table
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator - PF Parity Table Generator       |
|  Copyright (C) 2024-2025 Manuel Sainz de Baranda y Goñi.                     |
|                                                                              |
|  This program is free software: you can redistribute it and/or modify it     |
|  under the terms of the GNU General Public License as published by the Free  |
|  Software Foundation, either version 3 of the License, or (at your option)   |
|  any later version.                                                          |
|                                                                              |
|  This program is distributed in the hope that it will be useful, but         |
|  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY  |
|  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License     |
|  for more details.                                                           |
|                                                                              |
|  You should have received a copy of the GNU General Public License along     |
|  with this program. If not, see <http://www.gnu.org/licenses/>.              |
|                                                                              |
'=============================================================================*/

#include <Z/macros/language.h>
#include <Z/types/integral.h>
#include <stdio.h>


static Z_ALWAYS_INLINE zuint8 pf_parity(zuint8 value)
	{return (zuint8)(((0x9669U >> ((value ^ (value >> 4)) & 0xF)) & 1) << 2);}


int main(int argc, char **argv)
	{
	zuint8 x, y;

	Z_UNUSED(argc) Z_UNUSED(argv)

	puts(	"static zuint8 const pf_parity_table[256] = {\n"
		"/*\t0  1  2  3  4  5  6  7\t8  9  A  B  C  D  E  F */");

	for (y = 0; y < 16; y++)
		{
		printf("/* %X */", y);

		for (x = 0; x < 16; x++)
			printf(" %u%s",
				pf_parity(y * 16 + x),
				x == 15 && y == 15 ? "};" : ",");

		putchar('\n');
		}

	return 0;
	}


/* generate-pf-parity-table.c EOF */
