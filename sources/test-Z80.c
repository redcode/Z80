/* test-Z80
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator - Test Tool                       |
|  Copyright (C) 2021-2023 Manuel Sainz de Baranda y Goñi.                     |
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

#ifdef TEST_Z80_WITH_ARCHIVE_EXTRACTION
#	include <zip.h>
#	include <zlib.h>
#	include <Z/formats/archive/TAR.h>
#endif

#include <Z/constants/pointer.h>
#include <Z/macros/array.h>
#include <Z80.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* MARK: - Macros */

#ifdef TEST_Z80_WITH_EXECUTE
#	define RUN z80_execute
#else
#	define RUN z80_run
#endif

#define OPCODE_NOP	 0x00
#define OPCODE_RET	 0xC9
#define OPCODE_HALT	 0x76
#define OPCODE_CALL_WORD 0xCD
#define OPCODE_JP_WORD	 0xC3

#define TEST_FORMAT_CPM	     0 /* CP/M program in COM format.		      */
#define TEST_FORMAT_HARSTON  1 /* Z80 Instruction Set Exerciser for Spectrum. */
#define TEST_FORMAT_RAK	     2 /* Patrik Rak's Zilog Z80 CPU Test Suite.      */
#define TEST_FORMAT_WOODMASS 3 /* Mark Woodmass' Z80 Test Suite.	      */


/* MARK: - Types */

typedef struct {
	/* Name of the archive if the file is compressed; `Z_NULL` otherwise. */
	char const* archive_name;

	/* Name of the file, or path to the file inside the archive if the file
	   is compressed. */
	char const* file_path;

	/* Total number of clock cycles executed when the test is passed. */
	zusize cycles_expected[1 + (Z_USIZE_BITS < 64)];

	/* Size of the file. */
	zuint16 file_size;

	/* Offset of the executable code inside the file. */
	zuint16 code_offset;

	/* Size of the executable code. */
	zuint16 code_size;

	/* Memory address where to jump to start executing the program. */
	zuint16 start_address;

	/* Value of the PC register once the test completes. */
	zuint16 exit_address;

	/* Format of the program. */
	zuint8 format;

	/* Number of lines printed when the test is passed. */
	zuint8 lines_expected;

	/* Rightmost position reached by the cursor when the test is passed. */
	zuint8 columns_expected;
} Test;


/* MARK: - Global Variables */

#if Z_USIZE_BITS < 64
#	define C(high, low) {Z_USIZE(0x##low), Z_USIZE(0x##high)}
#else
#	define C(high, low) {Z_USIZE(0x##high##low)}
#endif

static Test const tests[22] = {
	{"Yaze v1.10 (1998-01-28)(Cringle, Frank D.)(Sources)[!].tar.gz", "yaze-1.10/test/zexdoc.com",				C(A, E19F287A) /* 46,734,977,146 */,  8704,   0,  8704, 0x0100, 0,	TEST_FORMAT_CPM,       68, 34},
	{Z_NULL, "Z80 Documented Instruction Set Exerciser for Spectrum (2018)(Harston, Jonathan Graham)[!].tap",		C(A, E4E22836) /* 46,789,699,638 */,  8716,  91,  8624, 0x8000, 0x803D, TEST_FORMAT_HARSTON,   69, 32},
	{"Yaze v1.10 (1998-01-28)(Cringle, Frank D.)(Sources)[!].tar.gz", "yaze-1.10/test/zexall.com",				C(A, E19F287A) /* 46,734,977,146 */,  8704,   0,  8704, 0x0100, 0,	TEST_FORMAT_CPM,       68, 34},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2009)(Bobrowski, Jan)[!].tap",				C(A, E4E1B837) /* 46,789,670,967 */,  8656, 108,  8547, 0x8000, 0x803D, TEST_FORMAT_HARSTON,   69, 31},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2011)(Bobrowski, Jan)(Narrowed to BIT Instructions)[!].tap", C(0, 4F67AEDF) /*  1,332,195,039 */,  8656, 108,  8547, 0x8000, 0x803D, TEST_FORMAT_HARSTON,    4, 31},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2017-0x)(Harston, Jonathan Graham)[!].tap",			C(A, E4E20746) /* 46,789,691,206 */,  8704,  91,  8612, 0x8000, 0x803D, TEST_FORMAT_HARSTON,   69, 32},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2018)(Harston, Jonathan Graham)[!].tap",			C(A, E4E22836) /* 46,789,699,638 */,  8716,  91,  8624, 0x8000, 0x803D, TEST_FORMAT_HARSTON,   69, 32},
	{"Z80 Instruction Set Exerciser for Spectrum 2 v0.1 (2012-11-27)(Rak, Patrik)[!].zip", "zexall2-0.1/zexall2.tap",	C(C, 18A43876) /* 51,953,023,094 */,  9316,  87,  9228, 0x8000, 0x8040, TEST_FORMAT_HARSTON,   76, 31},
	{Z_NULL, "Z80 Test Suite (2008)(Woodmass, Mark)[!].tap",								C(0, 9C3040EF) /*  2,620,408,047 */,  5573, 120,  5452, 0x8057, 0x80E6, TEST_FORMAT_WOODMASS,  50, 32},
	{Z_NULL, "Z80 Test Suite (2008)(Woodmass, Mark)[!].tap",								C(0, 0308BF63) /*     50,904,931 */,  5573, 120,  5452, 0x8049, 0x80E6, TEST_FORMAT_WOODMASS,  61, 32},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80full.tap",				C(0, 4303ABF1) /*  1,124,314,097 */, 13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,      156, 32},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80doc.tap",				C(0, 436E8265) /*  1,131,315,813 */, 13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,      156, 32},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80flags.tap",				C(0, 20ED11DC) /*    552,407,516 */, 13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,      156, 32},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80docflags.tap",			C(0, 2110B9B1) /*    554,744,241 */, 13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,      156, 32},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80ccf.tap",				C(0, 23AB74CA) /*    598,439,114 */, 14219,  91, 14127, 0x8000, 0x7003, TEST_FORMAT_RAK,      156, 32},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80memptr.tap",				C(0, 215CF3BD) /*    559,739,837 */, 13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,      156, 32},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80full.tap",				C(0, 4382DC6A) /*  1,132,649,578 */, 14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,      164, 32},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80doc.tap",				C(0, 43EE72CE) /*  1,139,700,430 */, 14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,      164, 32},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80flags.tap",				C(0, 212F17D5) /*    556,734,421 */, 14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,      164, 32},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80docflags.tap",			C(0, 2152FFDA) /*    559,087,578 */, 14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,      164, 32},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80ccf.tap",				C(0, 23F34E43) /*    603,147,843 */, 14875,  91, 14783, 0x8000, 0x7003, TEST_FORMAT_RAK,      164, 32},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80memptr.tap",				C(0, 219FC276) /*    564,118,134 */, 14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,      164, 32}
};

#undef C

static struct {char const *key; zuint8 options;} const cpu_models[4] = {
	{"zilog-nmos", Z80_MODEL_ZILOG_NMOS},
	{"zilog-cmos", Z80_MODEL_ZILOG_CMOS},
	{"nec-nmos",   Z80_MODEL_NEC_NMOS  },
	{"st-cmos",    Z80_MODEL_ST_CMOS   }
};

static char const new_line[2] = "\n";

/*--------------------------------------------------------------------------.
| The search paths specified by using the `-p` option are collected in the  |
| `search_paths` array of size `search_path_count`. `path_buffer` is used   |
| to compose a complete file path consisting of "<search-path>/<filename>". |
'==========================================================================*/
static char*  path_buffer	= Z_NULL;
static char** search_paths	= Z_NULL;
static zuint  search_path_count = 0;

/*-----------------------------------------------------------------------------.
| `verbosity` contains the verbosity level specified by using the `-V` option. |
| `show_test_output` indicates whether to print the text output of the tests.  |
'=============================================================================*/
static zuint8   verbosity = 4;
static zboolean show_test_output;

/*-----------------------------------------------------------------------------.
| [0] = the byte read from even I/O ports; specified by using the `-0` option. |
| [0] = the byte read from odd I/O ports; specified by using the `-1` option.  |
| The default values are those from a Sinclair ZX Spectrum 48K with no devices |
| attached.								       |
'=============================================================================*/
static zuint8 in_values[2] = {191, 255};

/*---------------------------------------------------.
| Instance of the Z80 emulator and 64 KiB of memory. |
'===================================================*/
static Z80    cpu;
static zuint8 memory[65536];

/*------------------------------------------------------------------------.
| `completed` indicates whether the test has reached its exit address, in |
| which case `cycles` contains the number of clock cycles executed during |
| the last `RUN` of the emulation. `lines` is incremented every time the  |
| test prints a new line. `cursor_x` holds the X position of the cursor,  |
| i.e., the size of the current line. `columns` contains the rightmost	  |
| position reached by the cursor throughout the test.			  |
'========================================================================*/
static zboolean completed;
static zusize   cursor_x, columns, cycles, lines;

/*------------------------------------------------------------------------.
| `zx_spectrum_print_hook_address` contains the address of the hook that  |
| intercepts the routine called by the test to print characters. When a	  |
| TAB character (17h) is printed, the `zx_spectrum_tab` counter is set to |
| 2 to indicate that it is necessary to process the incoming <TAB n> and  |
| <TAB STOP> bytes before continuing to print characters. `bad_character` |
| is set to `TRUE` if the test prints any unsupported control characters. |
|									  |
| To learn more about the TAB control sequence of the ZX Spectrum, read:  |
| * Sinclair Research (1983). "Sinclair ZX Spectrum BASIC Programming"	  |
|   2nd edition, pp. 103, 196.						  |
| * Ardley, Neil (1984). "ZX Spectrum + User Guide" (Dorling Kindersley;  |
|   Sinclair Research. ISBN 0863180809), pp. 67-68.			  |
|									  |
| These three variables are only used for ZX Spectrum tests.		  |
'========================================================================*/
static zuint16	zx_spectrum_print_hook_address;
static zuint	zx_spectrum_tab;
static zboolean zx_spectrum_bad_character;


/* MARK: - CPU Callbacks: Common */

static zuint8 cpu_read(void *context, zuint16 address)
	{
	Z_UNUSED(context)
	return memory[address];
	}


static zuint8 cpu_in(void *context, zuint16 port)
	{
	Z_UNUSED(context)
	return in_values[port & 1];
	}


static void cpu_out(void *context, zuint16 port, zuint8 value)
	{Z_UNUSED(context) Z_UNUSED(port) Z_UNUSED(value)}


static void cpu_halt(void *context, zuint8 state)
	{
	Z_UNUSED(context) Z_UNUSED(state)
	cycles	   = cpu.cycles;
	cpu.cycles = Z80_MAXIMUM_CYCLES;
	completed  = TRUE;
	}


/* MARK: - CPU Callbacks: Auxiliary Functions */

static void cr(void)
	{
	if (show_test_output) putchar('\n');
	if (cursor_x > columns) columns = cursor_x;
	cursor_x = 0;
	lines++;
	}


/* MARK: - CPU Callbacks: CP/M */

static void cpm_cpu_write(void *context, zuint16 address, zuint8 value)
	{
	Z_UNUSED(context)
	memory[address] = value;
	}


static zuint8 cpm_cpu_hook(void *context, zuint16 address)
	{
	zuint8 character;

	Z_UNUSED(context)
	if (address != 5) return OPCODE_NOP;

	/* BDOS function 2 (C_WRITE) - Console output */
	if (Z80_C(cpu) == 2) switch ((character = Z80_E(cpu)))
		{
		case 0x0A: /* LF */ cr();
		case 0x0D: /* CR */ break;

		default:
		if (show_test_output) putchar(character);
		cursor_x++;
		}

	/* BDOS function 9 (C_WRITESTR) - Output string */
	else if (Z80_C(cpu) == 9)
		{
		zuint16 i = Z80_DE(cpu);
		zuint   c = 255;

		while (c--) switch ((character = memory[i++]))
			{
			case 0x24: /* $  */ return OPCODE_RET;
			case 0x0A: /* LF */ cr();
			case 0x0D: /* CR */ break;

			default:
			if (show_test_output) putchar(character);
			cursor_x++;
			}

		if (show_test_output) puts(" [TRUNCATED]");
		}

	return OPCODE_RET;
	}


/* MARK: - CPU Callbacks: ZX Spectrum */

static void zx_spectrum_cpu_write(void *context, zuint16 address, zuint8 value)
	{
	Z_UNUSED(context)
	if (address > 0x3FFF) memory[address] = value;
	}


static zuint8 zx_spectrum_cpu_hook(void *context, zuint16 address)
	{
	Z_UNUSED(context)
	if (address != zx_spectrum_print_hook_address) return OPCODE_NOP;

	if (!zx_spectrum_tab) switch (Z80_A(cpu))
		{
		case 0x0D: /* CR */
		cr();
		break;

		case 0x17: /* TAB */
		zx_spectrum_tab = 2;
		break;

		case 0x7F: /* © */
		if (show_test_output) printf("©");
		cursor_x++;
		break;

		default:
		if (Z80_A(cpu) >= 32 && Z80_A(cpu) < 127)
			{
			if (show_test_output) putchar(Z80_A(cpu));
			cursor_x++;
			}

		else zx_spectrum_bad_character = TRUE;
		}

	else if (--zx_spectrum_tab)
		{
		zuint c = Z80_A(cpu) & (32 - 1), x = cursor_x & (32 - 1);

		if (c < x) cr();
		else cursor_x += (c -= x);
		if (show_test_output) while (c--) putchar(' ');
		}

	return OPCODE_RET;
	}


/* Only needed for Woody's Z80 Test Suite. */
static zuint8 zx_spectrum_cpu_fetch_opcode(void *context, zuint16 address)
	{
	Z_UNUSED(context)

	return (address == 0x0D6B /* 0D6B: THE 'CLS' COMMAND ROUTINE  */ ||
		address == 0x1601 /* 1601: THE 'CHAN_OPEN' SUBROUTINE */
	)
		? OPCODE_RET : memory[address];
	}


/* MARK: - Main Section */

static char const *compose_path(char const *base_path, char const *file_path)
	{
	zusize base_path_size;

	if (base_path == Z_NULL) return file_path;
	base_path_size = strlen(base_path);
	memcpy(path_buffer, base_path, base_path_size);
	path_buffer[base_path_size] = '/';
	strcpy(path_buffer + base_path_size + 1, file_path);
	return path_buffer;
	}


static zboolean load_file(
	char const* search_path,
	char const* file_path,
	zuint32	    file_size,
	zuint16	    offset,
	zuint16	    size,
	void*	    buffer
)
	{
	zboolean ok = FALSE;
	FILE *file = fopen(compose_path(search_path, file_path), "rb");

	if (file != Z_NULL)
		{
		fseek(file, 0, SEEK_END);

		if (ftell(file) == file_size)
			{
			fseek(file, offset, SEEK_SET);
			ok = fread(buffer, size, 1, file) == 1;
			}

		fclose(file);
		}

	return ok;
	}


static zboolean load_test(char const *search_path, Test const *test, void *buffer)
	{
#	ifdef TEST_Z80_WITH_ARCHIVE_EXTRACTION
		zboolean ok = load_file(
			search_path, test->file_path, test->file_size,
			test->code_offset, test->code_size, buffer);

		if (!ok && test->archive_name != Z_NULL)
			{
			search_path = compose_path(search_path, test->archive_name);

			/* .tar.gz */
			if (strrchr(test->archive_name, '.')[1] == 'g')
				{
				gzFile gz = gzopen(search_path, "rb");

				if (gz != Z_NULL)
					{
					union {zuint8 data[Z_TAR_BLOCK_SIZE]; Z_TARPOSIXHeader fields;} header;

					while (!gzeof(gz))
						{
						char *end;
						zulong file_size, block_tail_size;

						if (gzread(gz, header.data, Z_TAR_BLOCK_SIZE) != Z_TAR_BLOCK_SIZE) break;
						header.fields.size[Z_ARRAY_SIZE(header.fields.size) - 1] = 0;
						file_size = strtoul((char const *)header.fields.size, &end, 8);
						if ((zuint8 *)end == header.fields.size || *end) break;

						if (!strcmp(test->file_path, (char const *)header.fields.name))
							{
							ok =	file_size				== test->file_size &&
								gzseek(gz, test->code_offset, SEEK_CUR) != -1		   &&
								gzread(gz, buffer, test->code_size)	== test->code_size;

							break;
							}

						if (-1 == gzseek(gz, (block_tail_size = (file_size % Z_TAR_BLOCK_SIZE))
							? file_size + (Z_TAR_BLOCK_SIZE - block_tail_size)
							: file_size, SEEK_CUR)
						)
							break;
						}

					gzclose(gz);
					}
				}

			/* .zip */
			else	{
				int error;
				zip_t *zip = zip_open(search_path, ZIP_RDONLY | ZIP_CHECKCONS, &error);

				if (zip != Z_NULL)
					{
					zip_file_t *file;
					zip_stat_t stat;

					if (	!zip_stat(zip, test->file_path, ZIP_FL_ENC_STRICT, &stat)    &&
						(stat.valid & ZIP_STAT_SIZE) && stat.size == test->file_size &&
						(file = zip_fopen(zip, test->file_path, 0)) != Z_NULL
					)
						{
						if (	zip_fread(file, buffer, test->code_offset) == test->code_offset &&
							zip_fread(file, buffer, test->code_size)   == test->code_size
						)
							ok = TRUE;

						zip_fclose(file);
						}

					zip_close(zip);
					}
				}
			}

		return ok;

#	else
		return load_file(
			search_path, test->file_path, test->file_size,
			test->code_offset, test->code_size, buffer);
#	endif
	}


static zuint8 run_test(int test_index)
	{
	Test const *test = &tests[test_index];
	zuint16 start_address = test->start_address;
	zboolean passed;
	zuint i = 0;

#	if Z_USIZE_BITS < 64
		zuint32 j = 0;
#	endif

	if (verbosity)
		{
		printf(verbosity == 1 ? "%02d" : "[%02d] ", test_index);

		if (verbosity >= 2)
			{
			if (test->archive_name == Z_NULL) printf("%s", test->file_path);
			else printf("%s/%s", test->archive_name, test->file_path);
			if (verbosity >= 3) printf("\n* Loading program");
			}

		printf("... ");
		}

	memset(memory, 0, 65536);

	for (;	i < search_path_count &&
		!load_test(search_paths[i], test, memory + (start_address & 0xFF00));
		i++
	);

	if (	i == search_path_count &&
		!load_test(Z_NULL, test, memory + (start_address & 0xFF00))
	)
		{
		error_loading_file:
		if (verbosity) puts(show_test_output
			? "Error, test skipped\n"
			: "Error, test skipped");

		return FALSE;
		}

	if (verbosity >= 3) puts("OK");
	z80_power(&cpu, TRUE);

	if (test->format == TEST_FORMAT_CPM)
		{
		cpu.fetch_opcode = cpu_read;
		cpu.write	 = cpm_cpu_write;
		cpu.hook	 = cpm_cpu_hook;
		memory[0]	 = OPCODE_HALT;
		memory[5]	 = Z80_HOOK; /* PRINT */
		}

	else	{
		cpu.write = zx_spectrum_cpu_write;
		cpu.hook  = zx_spectrum_cpu_hook;
		cpu.im	  = 1;
		cpu.i	  = 0x3F;

		if (test->format == TEST_FORMAT_WOODMASS)
			{
			if (verbosity >= 3) printf("* Loading firmware... ");

			for (	i = 0;
				i < search_path_count &&
				!load_file(search_paths[i], "ZX Spectrum.rom", 16384, 0, 16384, memory);
				i++
			);

			if (	i == search_path_count &&
				!load_file(Z_NULL, "ZX Spectrum.rom", 16384, 0, 16384, memory)
			)
				goto error_loading_file;

			if (verbosity >= 3) puts("OK");
			Z80_SP(cpu) = 0x7FE8;
			Z80_AF(cpu) = 0x3222;

			cpu.fetch_opcode = zx_spectrum_cpu_fetch_opcode;

			/* 0010: THE 'PRINT A CHARACTER' RESTART */
			memory[0x0010] = OPCODE_JP_WORD; /* jp PRINT */
			memory[0x0011] = 0xF2;
			memory[0x0012] = 0x70;

			/* 70F2: PRINT */
			memory[zx_spectrum_print_hook_address = 0x70F2] = Z80_HOOK;
			}

		else	{
			cpu.fetch_opcode = cpu_read;

			/* 0010: THE 'PRINT A CHARACTER' RESTART */
			memory[zx_spectrum_print_hook_address = 0x0010] = Z80_HOOK;

			/* 0D6B: THE 'CLS' COMMAND ROUTINE */
			memory[0x0D6B] = OPCODE_RET;

			/* 1601: THE 'CHAN_OPEN' SUBROUTINE */
			memory[0x1601] = OPCODE_RET;

			if (test->format == TEST_FORMAT_RAK)
				{
				/* 7000: START */
				memory[0x7000] = OPCODE_CALL_WORD;
				memory[0x7001] = (zuint8)start_address;
				memory[0x7002] = (zuint8)(start_address >> 8);
				/*memory[0x7003] = HALT;*/
				start_address = 0x7000;
				}
			}
		}

	memory[test->exit_address] = OPCODE_HALT;
	Z80_PC(cpu)		   = start_address;
	cycles			   =
	lines			   =
	columns			   =
	cursor_x		   = 0;
	zx_spectrum_tab		   = 0;
	zx_spectrum_bad_character  =
	completed		   = FALSE;

	if (verbosity >= 3) printf("* Running program%s", show_test_output ? ":\n\n" : "... ");

#	if Z_USIZE_BITS < 64
		for (i = 0; i < test->cycles_expected[1];)
			{
			RUN(&cpu, Z_UINT32_MAXIMUM / 2);

			if (completed)
				{
				if ((j += (zuint32)cycles) < cycles) i++;
				cycles = j;
				goto check_results;
				}

			if ((j += (zuint32)cpu.cycles) < cpu.cycles) i++;
			}

		RUN(&cpu, test->cycles_expected[0] + Z_UINT32(0x10000000) - j);
		check_results:
#	else
		RUN(&cpu, test->cycles_expected[0] + Z_USIZE(0x10000000));
#	endif

	if (cursor_x > columns) columns = cursor_x;

	/*---------------------------------------------------------------------.
	| The test is passed if it has reached its exit address at the correct |
	| clock cycle, has printed the correct number of lines and columns,    |
	| and has not printed any unsupported control characters.	       |
	'=====================================================================*/

	passed = completed
		&& !zx_spectrum_bad_character
		&& lines   == test->lines_expected
		&& columns == test->columns_expected
		&& cycles  == test->cycles_expected[0]
#		if Z_USIZE_BITS < 64
			&& i == test->cycles_expected[1]
#		endif
		;

	if (verbosity)
		{
		char const *failure_reason;

		if (!passed)
			{
			if (!completed)
				failure_reason = "clock cycle limit exceeded; program aborted";

			else if (
				zx_spectrum_bad_character	||
				lines   != test->lines_expected ||
				columns != test->columns_expected
			)
				failure_reason = "incorrect behavior detected";

			else failure_reason = "incorrect number of clock cycles";
			}

		if (show_test_output)
			{
			zuint has_final_new_line = test->format == TEST_FORMAT_RAK;

			if (passed) puts(&new_line[has_final_new_line]);

			else printf(
				"%s>>> Test failed: %s.\n\n",
				&new_line[!lines || (completed && has_final_new_line)],
				failure_reason);
			}

		else	{
			if (passed) puts ("Passed");
			else printf("Failed: %s\n", failure_reason);
			}
		}

	return passed;
	}


static zboolean is_option(char const* string, char const* short_option, char const* long_option)
	{return !strcmp(string, short_option) || !strcmp(string, long_option);}


static zboolean to_uint8(char const* string, zuint8 maximum_value, zuint8 *byte)
	{
	char *end;
	zulong value = strtoul(string, &end, 0);

	if (end == string || *end || value > maximum_value) return FALSE;
	if (byte != Z_NULL) *byte = (zuint8)value;
	return TRUE;
	}


int main(int argc, char **argv)
	{
	zboolean all = FALSE;
	zuint32 tests_run = 0;
	zusize maximum_search_path_size = 0;
	int j, i = 0;

	/*--------------------------------------------.
	| String specifying what has been detected as |
	| invalid when parsing the command line.      |
	'============================================*/
	char const *invalid;

	/*------------------------------.
	| [0] = Number of tests failed. |
	| [1] = Number of tests passed. |
	'==============================*/
	zuint results[2] = {0, 0};

	/*--------------------------------------------.
	| The emulator will behave as a Zilog NMOS if |
	| the user does not specify a CPU model.      |
	'============================================*/
	cpu.options = Z80_MODEL_ZILOG_NMOS;

	while (++i < argc && *argv[i] == '-')
		{
		if (is_option(argv[i], "-h", "--help"))
			{
			puts(	"Usage: test-Z80 [options] (--all|<test>...)\n"
				"\n"
				"Options:\n"
				"  -V, --verbosity (0..4)  Set the verbosity level [default: 4].\n"
				"  -0, --even-in (0..255)  Set the byte read from even I/O ports [default: 191].\n"
				"  -1, --odd-in (0..255)   Set the byte read from odd I/O ports [default: 255].\n"
				"  -a, --all               Run all tests.\n"
				"  -h, --help              Print this help message and exit.\n"
				"  -m, --model <model>     Specify the CPU model to emulate.\n"
				"  -p, --path <path>       Add a path where to look for the required files.\n"
				"  -v, --version           Print version information and exit.\n"
				"\n"
				"CPU models:\n"
				"  zilog-nmos  Zilog NMOS [default]\n"
				"  zilog-cmos  Zilog CMOS\n"
				"  nec-nmos    NEC NMOS\n"
				"  st-cmos     SGS-Thomson CMOS\n"
				"\n"
				"Tests:\n"
				"  Versions of the Z80 Documented Instruction Set Exerciser:\n"
				"    00  CP/M ~ Cringle, Frank D. (1998-01-28).\n"
				"    01  ZX Spectrum ~ Harston, Jonathan Graham (2018).\n"
				"  Versions of the Z80 Full Instruction Set Exerciser:\n"
				"    02  CP/M ~ Cringle, Frank D. (1998-01-28).\n"
				"    03  ZX Spectrum ~ Bobrowski, Jan (2009).\n"
				"    04  ZX Spectrum ~ Bobrowski, Jan (2011). Narrowed to `bit` instructions.\n"
				"    05  ZX Spectrum ~ Harston, Jonathan Graham (2017).\n"
				"    06  ZX Spectrum ~ Harston, Jonathan Graham (2018).\n"
				"    07  ZX Spectrum ~ Rak, Patrik (2012-11-27).\n"
				"  Z80 Test Suite ~ ZX Spectrum ~ Woodmass, Mark (2008):\n"
				"    08  Tests flags.\n"
				"    09  Tests MEMPTR.\n"
				"  Zilog Z80 CPU Test Suite v1.0 ~ ZX Spectrum ~ Rak, Patrik (2012-12-08):\n"
				"    10  Tests all flags and registers.\n"
				"    11  Tests all registers, but only officially documented flags.\n"
				"    12  Tests all flags, ignores registers.\n"
				"    13  Tests documented flags only, ignores registers.\n"
				"    14  Tests all flags after executing `ccf` after each instruction.\n"
				"    15  Tests all flags after executing `bit N,(hl)` after each instruction.\n"
				"  Zilog Z80 CPU Test Suite v1.2 ~ ZX Spectrum ~ Rak, Patrik (2022-01-26):\n"
				"    16  Tests all flags and registers.\n"
				"    17  Tests all registers, but only officially documented flags.\n"
				"    18  Tests all flags, ignores registers.\n"
				"    19  Tests documented flags only, ignores registers.\n"
				"    20  Tests all flags after executing `ccf` after each instruction.\n"
				"    21  Tests all flags after executing `bit N,(hl)` after each instruction.\n"
				"\n"
				"Email bug reports and questions to <manuel@zxe.io>\n"
				"Open issues at <https://github.com/redcode/Z80>");

			goto exit_without_error;
			}

		else if (is_option(argv[i], "-v", "--version"))
			{
			puts(	"test-Z80 v" Z80_LIBRARY_VERSION_STRING "\n"
				"Copyright (C) 2021-2023 Manuel Sainz de Baranda y Goñi.\n"
				"Released under the terms of the GNU General Public License v3.");

			goto exit_without_error;
			}

		else if (is_option(argv[i], "-V", "--verbosity"))
			{
			if (++i == argc) goto incomplete_option;

			if (!to_uint8(argv[i], 4, &verbosity))
				{
				invalid = "verbosity level";
				goto invalid_argument;
				}
			}

		else if (is_option(argv[i], "-0", "--even-in"))
			{
			if (++i == argc) goto incomplete_option;
			if (!to_uint8(argv[i], 255, &in_values[0])) goto invalid_io_value;
			}

		else if (is_option(argv[i], "-1", "--odd-in"))
			{
			if (++i == argc) goto incomplete_option;
			if (!to_uint8(argv[i], 255, &in_values[1])) goto invalid_io_value;
			}

		else if (is_option(argv[i], "-m", "--model"))
			{
			if (++i == argc) goto incomplete_option;

			for (j = 0; j < Z_ARRAY_SIZE(cpu_models); j++)
				if (!strcmp(argv[i], cpu_models[j].key))
					{
					cpu.options = cpu_models[j].options;
					break;
					}

			if (j == 4)
				{
				invalid = "CPU model";
				goto invalid_argument;
				}
			}

		else if (is_option(argv[i], "-p", "--path"))
			{
			zusize s;
			char **p;

			if (++i == argc || !(s = strlen(argv[i]))) goto incomplete_option;
			if (s > maximum_search_path_size) maximum_search_path_size = s;

			if ((p = realloc(search_paths, (search_path_count + 1) * sizeof(char *))) == Z_NULL)
				goto not_enough_memory_available;

			search_paths = p;
			search_paths[search_path_count++] = argv[i];
			}

		else if (is_option(argv[i], "-a", "--all"))
			all = TRUE;

		else	{
			invalid = "option";
			goto invalid_argument;
			}
		}

	/*------------------------------------------------.
	| The user must specify at least one test number, |
	| or the `-a` option in its absence.		  |
	'================================================*/
	if (i == argc && !all)
		{
		fputs("No test specified.\n", stderr);
		goto bad_syntax;
		}

	/*------------------------------------------.
	| All test numbers specified must be valid. |
	'==========================================*/
	for (j = i; i < argc; i++)
		{
		char const *string = argv[i];
		char *end;

		if (strtoul(string, &end, 10) >= Z_ARRAY_SIZE(tests) || end == string || *end)
			{
			invalid = "test number";
			goto invalid_argument;
			}
		}

	if (	search_path_count &&
		(path_buffer = malloc(maximum_search_path_size + 110)) == Z_NULL
	)
		{
		not_enough_memory_available:
		fputs("Error: not enough memory available.", stderr);
		goto exit_with_error;
		}

	/*---------------------------------------------------------------.
	| Disable output buffering if verbosity is enabled, so that the  |
	| messages are visible immediately rather than after each ENTER. |
	'===============================================================*/
	if (verbosity) setvbuf(stdout, Z_NULL, _IONBF, 0);
	show_test_output = verbosity == 4;

	/* Configure the Z80 CPU emulator. */

	/*---------------------------------------------------------------.
	| No CPU context is needed, since we are using global variables. |
	'===============================================================*/
	cpu.context = Z_NULL;

	/*---------------------------------------------------.
	| No test program requires these optional callbacks. |
	'===================================================*/
	cpu.nmia      =
	cpu.inta      =
	cpu.int_fetch = Z_NULL;
	cpu.ld_i_a    =
	cpu.ld_r_a    =
	cpu.reti      =
	cpu.retn      = Z_NULL;
	cpu.illegal   = Z_NULL;

	/*---------------------------------------------------------------------.
	| It is not necessary to distinguish between memory read and internal  |
	| NOP, as no test program require precise timing or memory contention. |
	'=====================================================================*/
	cpu.fetch =
	cpu.read  =
	cpu.nop	  = cpu_read;

	cpu.in	  = cpu_in;
	cpu.out	  = cpu_out;

	/*-------------------------------------------------------------------.
	| Entering the HALT state means that the test program has completed. |
	'===================================================================*/
	cpu.halt = cpu_halt;

	/*------------------------------------------------------------.
	| Run the tests whose numbers have been explicitly specified. |
	'============================================================*/
	while (j < argc)
		{
		tests_run |= Z_UINT32(1) << (i = atoi(argv[j++]));
		results[run_test(i)]++;
		}

	/*-----------------------------------------------------.
	| If the `-a` option has been specified, run all tests |
	| without repeating those that have already been run.  |
	'=====================================================*/
	if (all) for (i = 0; i < (int)Z_ARRAY_SIZE(tests); i++)
		if (!(tests_run & (Z_UINT32(1) << i))) results[run_test(i)]++;

	/*---------------------------.
	| Print the results summary. |
	'===========================*/
	printf(	"%sResults%s: %u test%s passed, %u failed\n",
		&new_line[!verbosity || show_test_output],
		show_test_output ? " summary" : "",
		results[1],
		results[1] == 1 ? "" : "s",
		results[0]);

	exit_without_error:
	free(search_paths);
	free(path_buffer);
	return results[0] ? -1 : 0;

	incomplete_option:
	fprintf(stderr, "Incomplete option: '%s'\n", argv[i - 1]);
	goto bad_syntax;

	invalid_io_value:
	invalid = "I/O value";

	invalid_argument:
	fprintf(stderr, "Invalid %s: '%s'\n", invalid, argv[i]);

	bad_syntax:
	fputs("Type 'test-Z80 -h' for help.\n", stderr);

	exit_with_error:
	free(search_paths);
	free(path_buffer);
	return -1;
	}


/* test-Z80.c EOF */
