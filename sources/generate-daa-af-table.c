/* generate-daa-af-table
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator - `daa` AF Table Generator        |
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

#include <Z80.h>
#include <stdio.h>


static zuint8 cpu_fetch_opcode(void *context, zuint16 address)
	{
	Z_UNUSED(context) Z_UNUSED(address)
	return 0x27; /* `daa` */
	}


int main(int argc, char **argv)
	{
	Z80 cpu;
	zuint8 hnc, f, x, y;

	Z_UNUSED(argc) Z_UNUSED(argv)
	cpu.fetch_opcode = cpu_fetch_opcode;
	puts("static zuint16 const daa_af_table[2048] = {");

	for (hnc = 0; hnc < 8; hnc++)
		{
		printf(	"/* HNC */\n/* %u%u%u\t"
			"0\t 1\t  2\t   3\t    4\t     5\t      6\t       7\t"
			"8\t 9\t  A\t   B\t    C\t     D\t      E\t       F"
			" */\n",
			(zuint)( hnc >> 2     ),
			(zuint)((hnc >> 1) & 1),
			(zuint)( hnc	   & 1));

		f = ((hnc << 2) | hnc) & (Z80_HF | Z80_NF | Z80_CF);

		for (y = 0; y < 16; y++)
			{
			printf("/* %X */", (zuint)y);

			for (x = 0; x < 16; x++)
				{
				z80_power(&cpu, Z_TRUE);
				Z80_F(cpu) = f;
				Z80_A(cpu) = y * 16 + x;
				z80_execute(&cpu, 1);

				printf(	" H(%04X)%s",
					(zuint)Z80_AF(cpu),
					x == 15 && y == 15 && hnc == 7
						? "};" : ",");
				}

			putchar('\n');
			}
		}

	return 0;
	}


/* generate-daa-af-table.c EOF */
