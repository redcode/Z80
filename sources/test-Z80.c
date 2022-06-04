/* test-Z80
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator Test                              |
|  Copyright (C) 2021-2022 Manuel Sainz de Baranda y Goñi.                     |
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

	/*-----------------------------------------.
	| Z_NULL is also defined by Zeta, so it is |
	| undefined to avoid conflicts with zlib.  |
	'=========================================*/
#	ifdef Z_NULL
#		undef Z_NULL
#	endif

#	include <Z/formats/archive/TAR.h>
#endif

#include <Z/constants/pointer.h>
#include <Z/macros/aggregate.h>
#include <Z80.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* MARK: - Constants */

#define OPCODE_NOP	 0x00
#define OPCODE_RET	 0xC9
#define OPCODE_HALT	 0x76
#define OPCODE_CALL_WORD 0xCD
#define OPCODE_JP_WORD	 0xC3

/* CP/M program in COM format. */
#define TEST_FORMAT_CPM 0

/* ZX Spectrum TAP image. Different versions of the Z80 Instruction Set
 * Exerciser adapted and improved by Jonathan Graham Harston and others. */
#define TEST_FORMAT_HARSTON 1

/* ZX Spectrum TAP image. Tapes of the Zilog Z80 CPU Test Suite written by
 * Patrik Rak. */
#define TEST_FORMAT_RAK 2

/* ZX Spectrum TAP image. Z80 Test Suite, written by Mark Woodmass. */
#define TEST_FORMAT_WOODMASS 3


/* MARK: - Types */

typedef struct {
	/* Name of the archive if the test is compressed; Z_NULL otherwise. */
	char const* archive_name;

	/* Name of the test program file, or the path to the file inside the
	 * archive if the file is compressed. */
	char const* file_path;

	/* Size of the program file. */
	zuint16 file_size;

	/* Offset of the program code inside the file. */
	zuint16 code_offset;

	/* Size of the program code. */
	zuint16 code_size;

	/* Memory address where to jump to start executing the test code. */
	zuint16 start_address; /* */

	/* Value of the PC register once the test ends. */
	zuint16 exit_address;

	/* Format of the program file. */
	zuint8 format;

	/* Total number of lines printed by the test when it passes. */
	zuint8 lines_expected;
} Test;


/* MARK: - Global Variables */

static Test const tests[22] = {
	{"Yaze v1.14 (2004-04-23)(Cringle, Frank D.)(Sources)[!].tar.gz", "yaze-1.14/test/zexdoc.com",				 8704,   0,  8704, 0x0100,	0, TEST_FORMAT_CPM,	  68},
	{Z_NULL, "Z80 Documented Instruction Set Exerciser for Spectrum (2018)(Harston, Jonathan Graham)[!].tap",		 8716,  91,  8624, 0x8000, 0x803D, TEST_FORMAT_HARSTON,	  69},
	{"Yaze v1.14 (2004-04-23)(Cringle, Frank D.)(Sources)[!].tar.gz", "yaze-1.14/test/zexall.com",				 8704,   0,  8704, 0x0100,	0, TEST_FORMAT_CPM,	  68},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2009)(Bobrowski, Jan)[!].tap",				 8656, 108,  8547, 0x8000, 0x803D, TEST_FORMAT_HARSTON,	  69},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2011)(Bobrowski, Jan)(Narrowed to BIT Instructions)[!].tap",  8656, 108,  8547, 0x8000, 0x803D, TEST_FORMAT_HARSTON,	   4},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2017-0x)(Harston, Jonathan Graham)[!].tap",			 8704,  91,  8612, 0x8000, 0x803D, TEST_FORMAT_HARSTON,	  69},
	{Z_NULL, "Z80 Full Instruction Set Exerciser for Spectrum (2018)(Harston, Jonathan Graham)[!].tap",			 8716,  91,  8624, 0x8000, 0x803D, TEST_FORMAT_HARSTON,	  69},
	{"Z80 Instruction Set Exerciser for Spectrum 2 v0.1 (2012-11-27)(Rak, Patrik)[!].zip", "zexall2-0.1/zexall2.tap",	 9316,  87,  9228, 0x8000, 0x8040, TEST_FORMAT_HARSTON,	  76},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80full.tap",				13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,	 156},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80doc.tap",				13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,	 156},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80flags.tap",				13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,	 156},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80docflags.tap",			13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,	 156},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80ccf.tap",				14219,  91, 14127, 0x8000, 0x7003, TEST_FORMAT_RAK,	 156},
	{"Zilog Z80 CPU Test Suite v1.0 (2012-12-08)(Rak, Patrik)[!].zip", "z80test-1.0/z80memptr.tap",				13758,  91, 13666, 0x8000, 0x7003, TEST_FORMAT_RAK,	 156},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80full.tap",				14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,	 164},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80doc.tap",				14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,	 164},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80flags.tap",				14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,	 164},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80docflags.tap",			14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,	 164},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80ccf.tap",				14875,  91, 14783, 0x8000, 0x7003, TEST_FORMAT_RAK,	 164},
	{"Zilog Z80 CPU Test Suite v1.2 (2022-01-26)(Rak, Patrik)[!].zip", "z80test-1.2/z80memptr.tap",				14390,  91, 14298, 0x8000, 0x7003, TEST_FORMAT_RAK,	 164},
	{Z_NULL, "Z80 Test Suite (2008)(Woodmass, Mark)[!].tap",								 5573, 120,  5452, 0x8057, 0x80E6, TEST_FORMAT_WOODMASS,  50},
	{Z_NULL, "Z80 Test Suite (2008)(Woodmass, Mark)[!].tap",								 5573, 120,  5452, 0x8049, 0x80E6, TEST_FORMAT_WOODMASS,  61}
};

static struct {char const *key; zuint8 options;} const cpu_models[4] = {
	{"zilog-nmos", Z80_MODEL_ZILOG_NMOS},
	{"zilog-cmos", Z80_MODEL_ZILOG_CMOS},
	{"nec-nmos",   Z80_MODEL_NEC_NMOS  },
	{"st-cmos",    Z80_MODEL_ST_CMOS   }
};

/* Instance of the Z80 CPU emulator and 64 KB of memory. */
static Z80 cpu;
static zuint8 memory[65536];

/* Whether or not the current test has been completed. */
static zboolean test_completed;

/* Whether or not the last character printed was a TAB.
 * Only used in ZX Spectrum tests. */
static zboolean zx_spectrum_tab;

/* X position of the cursor inside screen paper (in characters).
 * Only used in ZX Spectrum tests. */
static zuint zx_spectrum_column;

/* Number of text lines printed by the current test program. It is used
 * to know whether the test has been passed or has produced errors. */
static zuint lines;

/* Address where to place a trap to intercept the PRINT routine used by
 * the test program. */
static zuint16 print_hook_address;

/* [0] = Value read from even I/O ports.
 * [1] = Value read from odd I/O ports. */
static zuint8 in_values[2];

/* Verbosity level. */
static zuint8 verbosity = 4;

/* Wheter or not to print the output of the test program being run. This is
 * TRUE only if the verbosity level is 4. It is used to simplify the code. */
static zboolean show_test_output;

static char*  path_buffer	= Z_NULL;
static char** search_paths	= Z_NULL;
static zuint  search_path_count = 0;

/* [0] = Number of failed tests.
 * [1] = Number of passed tests. */
static zuint results[2];

/* String containing what has been detected as invalid
 * when parsing the command line. */
static const char *invalid;


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


static void cpu_halt(void *context, zboolean state)
	{
	Z_UNUSED(context) Z_UNUSED(state)
	cpu.cycles = Z80_CYCLE_LIMIT;
	test_completed = TRUE;
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

	if (Z80_C(cpu) == 2) switch ((character = Z80_E(cpu)))
		{
		case 0x0D: break;
		case 0x0A: character = '\n';
		case 0x3A: lines++;

		default:
		if (show_test_output) putchar(character);
		}

	else if (Z80_C(cpu) == 9)
		{
		zuint16 i = Z80_DE(cpu);
		zuint   c = 0;

		while (memory[i] != '$')
			{
			if (c++ > 100)
				{
				putchar('\n');
				fputs("FATAL ERROR: String to print is too long!\n", stderr);
				exit(EXIT_FAILURE);
				}

			switch ((character = memory[i++]))
				{
				case 0x0D: break;
				case 0x0A: character = '\n';
				case 0x3A: lines++;

				default:
				if (show_test_output) putchar(character);
				}
			}
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
	if (address != print_hook_address) return OPCODE_NOP;

	if (zx_spectrum_tab)
		{
		if (show_test_output)
			{
			zuint c = (Z80_A(cpu) % 32) - (zx_spectrum_column % 32);

			while (c--) putchar(' ');
			}

		zx_spectrum_tab = FALSE;
		}

	else switch (Z80_A(cpu))
		{
		case 0x0D: /* CR */
		if (show_test_output) putchar('\n');
		lines++;
		zx_spectrum_column = 0;
		break;

		case 0x7F: /* © */
		if (show_test_output) printf("©");
		zx_spectrum_column++;
		break;

		case 0x17: /* TAB */
		zx_spectrum_tab = TRUE;
		break;

		default:
		if (Z80_A(cpu) >= 32 && Z80_A(cpu) < 127)
			{
			if (show_test_output) putchar(Z80_A(cpu));
			lines += ++zx_spectrum_column > 32;
			}
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


static char const *compose_path(char const *base_path, const char *file_path)
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
	char const* path,
	char const* file_path,
	zuint32     file_size,
	zuint16	    offset,
	zuint16	    size,
	void*	    buffer
)
	{
	zboolean status = FALSE;
	FILE *file = fopen(compose_path(path, file_path), "rb");

	if (file != Z_NULL)
		{
		fseek(file, 0, SEEK_END);

		if (ftell(file) == file_size)
			{
			fseek(file, offset, SEEK_SET);
			status = fread(buffer, size, 1, file) == 1;
			}

		fclose(file);
		}

	return status;
	}


static zboolean load_test(const char *path, Test const *test, void *buffer)
	{
#	ifdef TEST_Z80_WITH_ARCHIVE_EXTRACTION
		zboolean status = load_file(
			path, test->file_path, test->file_size,
			test->code_offset, test->code_size, buffer);

		if (!status && test->archive_name != Z_NULL)
			{
			path = compose_path(path, test->archive_name);

			/* .tar.gz */
			if (strrchr(test->archive_name, '.')[1] == 'g')
				{
				union {zuint8 data[Z_TAR_BLOCK_SIZE]; Z_TARHeader fields;} header;
				gzFile gz = gzopen(path, "rb");

				if (gz != Z_NULL)
					{
					while (!gzeof(gz))
						{
						char *end;
						zulong file_size, block_tail_size;

						if (gzread(gz, header.data, Z_TAR_BLOCK_SIZE) != Z_TAR_BLOCK_SIZE) break;
						file_size = strtoul((char *)header.fields.size, &end, 8);

						if (!strcmp(test->file_path, (char *)header.fields.name))
							{
							status =
								file_size				== test->file_size &&
								gzseek(gz, test->code_offset, SEEK_CUR) != -1		   &&
								gzread(gz, buffer, test->code_size)	== test->code_size;

							break;
							}

						if (	(zuint8 *)end == header.fields.size || *end ||
							-1 == gzseek(gz, (block_tail_size = (file_size % Z_TAR_BLOCK_SIZE))
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
				zip_t *zip = zip_open(path, ZIP_RDONLY | ZIP_CHECKCONS, &error);

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
							status = TRUE;

						zip_fclose(file);
						}

					zip_close(zip);
					}
				}
			}

		return status;

#	else
		return load_file(
			path, test->file_path, test->file_size,
			test->code_offset, test->code_size, buffer);
#	endif
	}


static zuint8 run_test(zuint test_index)
	{
	Test const *test = &tests[test_index];
	zuint16 start_address = test->start_address;
	zuint i = 0;

	if (verbosity)
		{
		printf(verbosity == 1 ? "%02u" : "[%02u] ", test_index);

		if (verbosity >= 2)
			{
			if (test->archive_name == Z_NULL) printf("%s", test->file_path);
			else printf("%s/%s", test->archive_name, test->file_path);
			if (verbosity >= 3) printf("\n* Loading program");
			}

		printf("...");
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
		if (verbosity) puts(" ERROR (test skipped)\n");
		return FALSE;
		}

	if (verbosity >= 3) puts(" OK");
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
			if (verbosity >= 3) printf("* Loading firmware...");

			for (	i = 0;
				i < search_path_count &&
				!load_file(search_paths[i], "ZX Spectrum.rom", 16384, 0, 16384, memory);
				i++
			);

			if (	i == search_path_count &&
				!load_file(Z_NULL, "ZX Spectrum.rom", 16384, 0, 16384, memory)
			)
				{
				if (verbosity) puts(" ERROR (test skipped)\n");
				return FALSE;
				}

			if (verbosity >= 3) puts(" OK");
			Z80_SP(cpu) = 0x7FE8;
			Z80_AF(cpu) = 0x3222;

			cpu.fetch_opcode = zx_spectrum_cpu_fetch_opcode;

			/* 0010: THE 'PRINT A CHARACTER' RESTART */
			memory[0x0010] = OPCODE_JP_WORD; /* jp PRINT */
			memory[0x0011] = 0xF2;
			memory[0x0012] = 0x70;

			/* 70F2: PRINT */
			memory[print_hook_address = 0x70F2] = Z80_HOOK;
			}

		else	{
			cpu.fetch_opcode = cpu_read;

			/* 0010: THE 'PRINT A CHARACTER' RESTART */
			memory[print_hook_address = 0x0010] = Z80_HOOK;

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
	lines			   = 0;
	zx_spectrum_column	   = 0;
	zx_spectrum_tab		   = FALSE;
	test_completed		   = FALSE;

	if (verbosity >= 3) printf("* Running program...%s", show_test_output ? "\n\n" : "");

	do
#	ifdef TEST_Z80_WITH_EXECUTE
		z80_execute(&cpu, Z80_CYCLE_LIMIT);
#	else
		z80_run(&cpu, Z80_CYCLE_LIMIT);
#	endif
	while (!test_completed);

	if (verbosity) puts(show_test_output
		? (test->format == TEST_FORMAT_RAK ? "" : "\n")
		: (lines == test->lines_expected ? " OK" : " FAILED"));

	return lines == test->lines_expected;
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
	zusize longest_search_path_size = 0;
	int ii, i = 0;

	cpu.options  = Z80_MODEL_ZILOG_NMOS;
	in_values[0] = 191;
	in_values[1] = 255;
	results  [1] =
	results  [0] = 0;

	while (++i < argc && *argv[i] == '-')
		{
		if (is_option(argv[i], "-h", "--help"))
			{
			puts(	"Usage: test-Z80 [options] (--all | <test>...)\n"
				"\n"
				"Options:\n"
				"  -V, --verbosity (0..4)  Set the verbosity level (the default level is 4).\n"
				"  -0, --in-even <value>   Set the 8-bit value to be read from even I/O ports.\n"
				"  -1, --in-odd <value>    Set the 8-bit value to be read from odd I/O ports.\n"
				"  -a, --all               Run all tests.\n"
				"  -h, --help              Show this help message.\n"
				"  -m, --model <model>     Specify the CPU model to emulate.\n"
				"  -p, --path <path>       Add a path where to look for the required files.\n"
				"  -v, --version           Show version and copyright.\n"
				"\n"
				"CPU models:\n"
				"  zilog-nmos  Zilog NMOS (default)\n"
				"  zilog-cmos  Zilog CMOS\n"
				"  nec-nmos    NEC NMOS\n"
				"  st-cmos     SGS-Thomson CMOS\n"
				"\n"
				"Tests:\n"
				"  Versions of the Z80 Documented Instruction Set Exerciser:\n"
				"    00  CP/M        ~ Cringle, Frank D. (2004-04-23).\n"
				"    01  ZX Spectrum ~ Harston, Jonathan Graham (2018).\n"
				"  Versions of the Z80 Full Instruction Set Exerciser:\n"
				"    02  CP/M        ~ Cringle, Frank D. (2004-04-23).\n"
				"    03  ZX Spectrum ~ Bobrowski, Jan (2009).\n"
				"    04  ZX Spectrum ~ Bobrowski, Jan (2011). Narrowed to BIT instructions.\n"
				"    05  ZX Spectrum ~ Harston, Jonathan Graham (2017).\n"
				"    06  ZX Spectrum ~ Harston, Jonathan Graham (2018).\n"
				"    07  ZX Spectrum ~ Rak, Patrik (2012-11-27).\n"
				"  Zilog Z80 CPU Test Suite v1.0 ~ ZX Spectrum ~ Rak, Patrik (2012-12-08):\n"
				"    08  Tests all flags and registers.\n"
				"    09  Tests all registers, but only officially documented flags.\n"
				"    10  Tests all flags, ignores registers.\n"
				"    11  Tests documented flags only, ignores registers.\n"
				"    12  Tests all flags after executing CCF after each instruction tested.\n"
				"    13  Tests all flags after executing BIT N,(HL) after each instruction tested.\n"
				"  Zilog Z80 CPU Test Suite v1.2 ~ ZX Spectrum ~ Rak, Patrik (2022-01-26):\n"
				"    14  Tests all flags and registers.\n"
				"    15  Tests all registers, but only officially documented flags.\n"
				"    16  Tests all flags, ignores registers.\n"
				"    17  Tests documented flags only, ignores registers.\n"
				"    18  Tests all flags after executing CCF after each instruction tested.\n"
				"    19  Tests all flags after executing BIT N,(HL) after each instruction tested.\n"
				"  Z80 Test Suite ~ ZX Spectrum ~ Woodmass, Mark (2008):\n"
				"    20  Tests flags.\n"
				"    21  Tests MEMPTR.\n"
				"\n"
				"Email bug reports and questions to <manuel@zxe.io>\n"
				"Open issues at <https://github.com/redcode/Z80>");

			goto exit_without_error;
			}

		else if (is_option(argv[i], "-v", "--version"))
			{
			puts(	"test-Z80 v" Z80_LIBRARY_VERSION_STRING "\n"
				"Copyright (C) 2021-2022 Manuel Sainz de Baranda y Goñi.\n"
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

		else if (is_option(argv[i], "-0", "--in-even"))
			{
			if (++i == argc) goto incomplete_option;
			if (!to_uint8(argv[i], 255, &in_values[0])) goto invalid_io_value;
			}

		else if (is_option(argv[i], "-1", "--in-odd"))
			{
			if (++i == argc) goto incomplete_option;
			if (!to_uint8(argv[i], 255, &in_values[1])) goto invalid_io_value;
			}

		else if (is_option(argv[i], "-m", "--model"))
			{
			if (++i == argc) goto incomplete_option;
			for (ii = 0; ii < 4; ii++) if (!strcmp(argv[i], cpu_models[ii].key)) break;

			if (ii == 4)
				{
				invalid = "CPU model";
				goto invalid_argument;
				}

			cpu.options = cpu_models[ii].options;
			}

		else if (is_option(argv[i], "-p", "--path"))
			{
			zusize s;
			char **p;

			if (++i == argc || !(s = strlen(argv[i]))) goto incomplete_option;
			if (s > longest_search_path_size) longest_search_path_size = s;

			if ((p = realloc(search_paths, (search_path_count + 1) * sizeof(char *))) == Z_NULL)
				{
				fputs("Error: Not enough memory available.", stderr);
				goto exit_with_error;
				}

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

	if (i == argc && !all)
		{
		fputs("No test specified.\n", stderr);
		goto bad_syntax;
		}

	if (search_path_count) path_buffer = malloc(longest_search_path_size + 110);

	/* Configure stdout as unbuffered. */
	if (verbosity) setvbuf(stdout, Z_NULL, _IONBF, 0);

	/* Configure the Z80 emulator */
	cpu.context   = Z_NULL;
	cpu.fetch     =
	cpu.read      =
	cpu.nop	      = cpu_read;
	cpu.in	      = cpu_in;
	cpu.out	      = cpu_out;
	cpu.halt      = cpu_halt;
	cpu.nmia      =
	cpu.inta      =
	cpu.int_fetch = Z_NULL;
	cpu.reset     = Z_NULL;
	cpu.ld_i_a    =
	cpu.ld_r_a    =
	cpu.reti      =
	cpu.retn      = Z_NULL;
	cpu.illegal   = Z_NULL;

	/* Ensure that all specified test numbers are valid. */
	for (ii = i; i < argc; i++)
		{
		char const *string = argv[i];
		char *end;

		if (strtoul(string, &end, 10) >= Z_ARRAY_SIZE(tests) || end == string || *end)
			{
			invalid = "test number";
			goto invalid_argument;
			}
		}

	show_test_output = verbosity == 4;

	while (ii < argc)
		{
		tests_run |= Z_UINT32(1) << (i = atoi(argv[ii++]));
		results[run_test(i)]++;
		}

	if (all) for (i = 0; i < (int)Z_ARRAY_SIZE(tests); i++)
		if (!(tests_run & (Z_UINT32(1) << i))) results[run_test(i)]++;

	printf(	"%sResults: %u test%s passed, %u failed\n",
		(verbosity && verbosity < 4) ? "\n" : "",
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
