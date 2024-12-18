/* step-test-Z80
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator - Step Testing Tool               |
|  Copyright (C) 2021-2024 Manuel Sainz de Baranda y Goñi.                     |
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

#include <cjson/cJSON.h>
#include <Z/constants/pointer.h>
#include <Z/macros/array.h>
#include <Z/macros/structure.h>
#include <Z80.h>
#include <Z80InsnClock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BLOCK_SIZE (750 * 1024)


/* MARK: - Types */

typedef struct {
	char const *caption;
	char const *key;
	zusize offset;
	zuint16 maximum_value;
} Member;

typedef struct {
	zuint16 address;
	zsint16 value;
	char pins[4];
} Cycle;

typedef struct {
	zuint16 port;
	zuint8 value;
	char direction;
} Port;


/* MARK: - Global variables */

static struct {char const *key; zuint8 options;} const cpu_models[] = {
	{"zilog-nmos", Z80_MODEL_ZILOG_NMOS},
	{"zilog-cmos", Z80_MODEL_ZILOG_CMOS},
	{"nec-nmos",   Z80_MODEL_NEC_NMOS  }};

static Member const members[] = {
	{"A",	 "a",	 Z_MEMBER_OFFSET(Z80, af.uint8_values.at_1 ),	255},
	{"AF'",	 "af_",	 Z_MEMBER_OFFSET(Z80, af_.uint16_value     ), 65535},
	{"B",	 "b",	 Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_1 ),	255},
	{"BC'",	 "bc_",	 Z_MEMBER_OFFSET(Z80, bc_.uint16_value     ), 65535},
	{"C",	 "c",	 Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_0 ),	255},
	{"D",	 "d",	 Z_MEMBER_OFFSET(Z80, de.uint8_values.at_1 ),	255},
	{"DE'",	 "de_",	 Z_MEMBER_OFFSET(Z80, de_.uint16_value     ), 65535},
	{"E",	 "e",	 Z_MEMBER_OFFSET(Z80, de.uint8_values.at_0 ),	255},
	{"F",	 "f",	 Z_MEMBER_OFFSET(Z80, af.uint8_values.at_0 ),	255},
	{"H",	 "h",	 Z_MEMBER_OFFSET(Z80, hl.uint8_values.at_1 ),	255},
	{"HL'",	 "hl_",	 Z_MEMBER_OFFSET(Z80, hl_.uint16_value     ), 65535},
	{"I",	 "i",	 Z_MEMBER_OFFSET(Z80, i			   ),	255},
	{"IFF1", "iff1", Z_MEMBER_OFFSET(Z80, iff1		   ),	  1},
	{"IFF2", "iff2", Z_MEMBER_OFFSET(Z80, iff2		   ),	  1},
	{"IM",	 "im",	 Z_MEMBER_OFFSET(Z80, im		   ),	  2},
	{"IX",	 "ix",	 Z_MEMBER_OFFSET(Z80, ix_iy[0].uint16_value), 65535},
	{"IY",	 "iy",	 Z_MEMBER_OFFSET(Z80, ix_iy[1].uint16_value), 65535},
	{"L",	 "l",	 Z_MEMBER_OFFSET(Z80, hl.uint8_values.at_0 ),	255},
	{"PC",	 "pc",	 Z_MEMBER_OFFSET(Z80, pc.uint16_value	   ), 65535},
	{"Q",	 "q",	 Z_MEMBER_OFFSET(Z80, q			   ),	255},
	{"SP",	 "sp",	 Z_MEMBER_OFFSET(Z80, sp.uint16_value	   ), 65535},
	{"R",	 "r",	 Z_MEMBER_OFFSET(Z80, r			   ),	255},
	{"WZ",	 "wz",	 Z_MEMBER_OFFSET(Z80, memptr.uint16_value  ), 65535},
	{"EI",	 "ei",	 0,						  1},
	{"P",	 "p",	 0,						  1}};

static Z80 cpu;
static Z80InsnClock insn_clock;
static zuint8 memory[65536];

static Cycle *cycles = Z_NULL;
static int cycles_size;
static int cycles_index;

static Port *ports = Z_NULL;
static int ports_size;
static int ports_index;

static cJSON *expected_ports = Z_NULL;
static int expected_port_count;

static zboolean file_failed, test_failed, array_failed;
static char const *field_separator = "";
static char const *my_name;


static void add_cycle(zuint16 address, zsint16 value, char const *pins)
	{
	Cycle *c;

	if (cycles_index == cycles_size)
		{
		if (cpu.cycles == cpu.cycle_limit) return;
		cycles_size += Z80_MAXIMUM_CYCLES_PER_STEP;

		if ((cycles = realloc(cycles, cycles_size * sizeof(Cycle))) == Z_NULL)
			{
			z80_break(&cpu);
			return;
			}
		}

	c = cycles + cycles_index;
	c->address = address;
	c->value   = value;
	memcpy(c->pins, pins, 4);
	cycles_index++;
	}


static void add_extra_cycles(zuint16 address, zuint8 count)
	{while (count--) add_cycle(address, -1, "----");}


static void add_port(zuint16 port, zuint8 value, char direction)
	{
	Port *p;

	if (ports_index == ports_size)
		{
		if (cpu.cycles == cpu.cycle_limit) return;
		ports_size += Z80_MAXIMUM_CYCLES_PER_STEP;

		if ((ports = realloc(ports, ports_size * sizeof(Port))) == Z_NULL)
			{
			z80_break(&cpu);
			return;
			}
		}

	p = ports + ports_index;
	p->port	     = port;
	p->value     = value;
	p->direction = direction;
	ports_index++;
	}


/* CPU callbacks */

static zuint8 cpu_fetch_opcode(void *context, zuint16 address)
	{
	zuint16 refresh_address = z80_refresh_address(&cpu);
	zuint8	opcode		= memory[address];
	zuint8	extra		= z80_insn_clock_extra_m1(&insn_clock, opcode);

	Z_UNUSED(context)
	z80_insn_clock_extra_add_m1(&insn_clock, extra);
	add_cycle(address,	   -1,	   "----");
	add_cycle(address,	   -1,	   "r-m-");
	add_cycle(refresh_address, opcode, "----");
	add_cycle(refresh_address, -1,	   "--m-");
	add_extra_cycles(refresh_address, extra);
	return memory[address];
	}


static zuint8 cpu_nop(void *context, zuint16 address)
	{
	zuint16 refresh_address = z80_refresh_address(&cpu);

	Z_UNUSED(context)
	add_cycle(address,	   -1,		    "----");
	add_cycle(address,	   -1,		    "r-m-");
	add_cycle(refresh_address, memory[address], "----");
	add_cycle(refresh_address, -1,		    "--m-");
	return 0;
	}


static zuint8 cpu_read(void *context, zuint16 address)
	{
	zuint8 extra = z80_insn_clock_extra_m(&insn_clock);

	Z_UNUSED(context)
	z80_insn_clock_extra_add_m(&insn_clock, extra);
	add_cycle(address, -1,		    "----");
	add_cycle(address, -1,		    "r-m-");
	add_cycle(address, memory[address], "r-m-");
	add_extra_cycles(address, extra);
	return memory[address];
	}


static void cpu_write(void *context, zuint16 address, zuint8 value)
	{
	Cycle* cycle = cycles + cycles_index;
	zuint8 extra = z80_insn_clock_extra_m(&insn_clock);

	Z_UNUSED(context)
	z80_insn_clock_extra_add_m(&insn_clock, extra);
	add_cycle(address, -1,	  "----");
	add_cycle(address, value, "--m-");
	add_cycle(address, value, "-wm-");
	add_extra_cycles(address, extra);
	memory[address] = value;
	}


static zuint8 cpu_in(void *context, zuint16 port)
	{
	Cycle* cycle = cycles + cycles_index;
	zuint8 value = (zuint8)cJSON_GetNumberValue(cJSON_GetArrayItem(
		cJSON_GetArrayItem(expected_ports, ports_index), 1));

	Z_UNUSED(context)
	z80_insn_clock_extra_add_in(&insn_clock);
	add_cycle(port,	-1,    "----");
	add_cycle(port,	-1,    "r--i");
	add_cycle(port,	-1,    "r--i");
	add_cycle(port, value, "r--i");
	add_port (port, value, 'r');
	return value;
	}


static void cpu_out(void *context, zuint16 port, zuint8 value)
	{
	Cycle* cycle = cycles + cycles_index;
	zuint8 extra = z80_insn_clock_extra_out(&insn_clock);

	Z_UNUSED(context)
	add_cycle(port,	-1,    "----");
	add_cycle(port, value, "-w-i");
	add_cycle(port, value, "-w-i");
	add_cycle(port, value, "-w-i");
	add_extra_cycles(port, extra);
	add_port (port, value, 'w');
	}


static zuint8 cpu_clock_direct_read(void *context, zuint16 address)
	{return memory[address];}


/* Member lookup */

static Member *find_member(char const *key)
	{
	zusize i;

	for (i = 0; i != Z_ARRAY_SIZE(members); i++)
		if (!strcmp(key, members[i].key))
			return (Member *)&members[i];

	return Z_NULL;
	}


static zuint8 expected_ram_address_value(cJSON *ram, zuint16 address)
	{
	cJSON *element;

	cJSON_ArrayForEach(element, ram)
		if ((zuint16)cJSON_GetNumberValue(cJSON_GetArrayItem(element, 0)) == address)
			return (zuint8)cJSON_GetNumberValue(cJSON_GetArrayItem(element, 1));

	return 0;
	}


/* JSON validation */

static zboolean is_number_between(cJSON *number, zuint min, zuint max)
	{
	int value;

	return	cJSON_IsNumber(number)			      &&
		(value = cJSON_GetNumberValue(number)) >= min &&
		value <= max;
	}


static zboolean validate_test_state(cJSON *state)
	{
	cJSON *item = Z_NULL;

	if (	!cJSON_IsObject(state) ||
		cJSON_GetArraySize(state) != Z_ARRAY_SIZE(members) + /* "ram" */ 1
	)
		return Z_FALSE;

	cJSON_ArrayForEach(item, state)
		{
		const char *key = item->string;

		if (!strcmp(key, "ram"))
			{
			cJSON *subitem;

			if (!cJSON_IsArray(item)) return Z_FALSE;

			cJSON_ArrayForEach(subitem, item) if (
				!cJSON_IsArray(subitem)					     ||
				cJSON_GetArraySize(subitem) != 2			     ||
				!is_number_between(cJSON_GetArrayItem(subitem, 0), 0, 65535) ||
				!is_number_between(cJSON_GetArrayItem(subitem, 1), 0,   255)
			)
				return Z_FALSE;
			}

		else	{
			Member *member;

			if (	(member = find_member(key)) == NULL ||
				!is_number_between(item, 0, member->maximum_value)
			)
				return Z_FALSE;
			}
		}

/*	return	cJSON_GetNumberValue(cJSON_GetObjectItem(state, "ei")) == 0.0 ||
		cJSON_GetNumberValue(cJSON_GetObjectItem(state, "p" )) == 0.0;*/

	return Z_TRUE;
	}


static zboolean validate_tests(cJSON *tests)
	{
	cJSON *test;

	if (!cJSON_IsArray(tests)) return Z_FALSE;
	cycles_size = ports_size = 0;

	cJSON_ArrayForEach(test, tests)
		{
		cJSON *item, *array, *subarray;
		int size;

		if (	!cJSON_IsObject(test)					||
			(item = cJSON_GetObjectItem(test, "initial")) == Z_NULL ||
			!validate_test_state(item)				||
			(item = cJSON_GetObjectItem(test, "final"  )) == Z_NULL ||
			!validate_test_state(item)				||
			(item = cJSON_GetObjectItem(test, "cycles" )) == Z_NULL ||
			!cJSON_IsArray(item)					||
			!(size = cJSON_GetArraySize(item))
		)
			return Z_FALSE;

		if (size > cycles_size) cycles_size = size;

		cJSON_ArrayForEach(array, item)
			{
			cJSON *cycle_item;
			char const *pins;

			if (	!cJSON_IsArray(array)					   ||
				cJSON_GetArraySize(array) != 3				   ||
				!cJSON_IsNumber(cycle_item = cJSON_GetArrayItem(array, 0)) ||
				(	!cJSON_IsNumber(cycle_item = cJSON_GetArrayItem(array, 1)) &&
					!cJSON_IsNull(cycle_item)) ||
				!cJSON_IsString(cycle_item = cJSON_GetArrayItem(array, 2)) ||
				strlen(pins = cycle_item->valuestring) != 4		   ||
				(pins[0] != '-' && pins[0] != 'r')			   ||
				(pins[1] != '-' && pins[1] != 'w')			   ||
				(pins[2] != '-' && pins[2] != 'm')			   ||
				(pins[3] != '-' && pins[3] != 'i')
			)
				return Z_FALSE;
			}

		if ((item = cJSON_GetObjectItem(test, "ports")) != NULL)
			{
			if (!cJSON_IsArray(item) || !(size = cJSON_GetArraySize(item)))
				return Z_FALSE;

			if (size > ports_size) ports_size = size;

			cJSON_ArrayForEach(array, item)
				{
				cJSON *io_item;

				if (	!cJSON_IsArray(array)					       ||
					cJSON_GetArraySize(array) != 3				       ||
					!is_number_between(cJSON_GetArrayItem(array, 0), 0.0, 65535.0) ||
					!is_number_between(cJSON_GetArrayItem(array, 1), 0.0,	255.0) ||
					!cJSON_IsString(io_item = cJSON_GetArrayItem(array, 2))	       ||
					(	strcmp(io_item->valuestring, "r") &&
						strcmp(io_item->valuestring, "w"))
				)
					return Z_FALSE;
				}
			}
		}

	return Z_TRUE;
	}


static void mismatch_found(cJSON *test)
	{
	if (!file_failed)
		{
		file_failed = Z_TRUE;
		puts("Failed");
		}

	if (!test_failed)
		{
		test_failed = Z_TRUE;
		printf("<%s>", cJSON_GetObjectItem(test, "name")->valuestring);
		}
	}


static zuint16 data_bus_value_to_string(int value)
	{
	char string[3];

	if (value == -1) return (zuint16)('-' | ((zuint16)'-' << 8));
	sprintf(string, "%02X", value);
	return *(zuint16 *)string;
	}


static void print_cycle_mismatch(cJSON *test, int index, Cycle const *actual, Cycle const *expected)
	{
	char actual_value[2], expected_value[2];

	mismatch_found(test);

	if (array_failed) printf(", ");

	else	{
		array_failed = Z_TRUE;
		printf("%s Cycles: [", field_separator);
		field_separator = ",";
		}

	if (actual   != Z_NULL) *(zuint16 *)actual_value   = data_bus_value_to_string(actual  ->value);
	if (expected != Z_NULL) *(zuint16 *)expected_value = data_bus_value_to_string(expected->value);

	printf("%d", index + 1);

	if (actual == Z_NULL)
		printf(" ****""/%04X **""/%.2s ****""/%.4s", expected->address, expected_value, expected->pins);

	else if (expected == Z_NULL)
		printf(" %04X/""**** %.2s/""** %.4s/""****", actual->address, actual_value, actual->pins);

	else	{
		if (actual->address != expected->address)
			printf(" %04X/%04X", actual->address, expected->address);

		if (actual->value != expected->value)
			printf(" %.2s/%.2s", actual_value, expected_value);

		if (memcmp(actual->pins, expected->pins, 4))
			printf(" %.4s/%.4s", actual->pins, expected->pins);
		}
	}


static void print_port_mismatch(cJSON *test, int index, Port const *actual, Port const *expected)
	{
	mismatch_found(test);

	if (array_failed) printf(", ");

	else	{
		array_failed = Z_TRUE;
		printf("%s Ports: [", field_separator);
		field_separator = ",";
		}

	printf("%d", index + 1);

	if (actual == Z_NULL)
		printf(" ****""/%04X **""/%02X *""/%c", expected->port, expected->value, expected->direction);

	else if (expected == Z_NULL)
		printf(" %04X/""**** %02X/""** %c/""*", actual->port, actual->value, actual->direction);

	else	{
		if (actual->port != expected->port)
			printf(" %04X/%04X", actual->port, expected->port);

		if (actual->value != expected->value)
			printf(" %02X/%02X", actual->value, expected->value);

		if (actual->direction != expected->direction)
			printf(" %c/%c", actual->direction, expected->direction);
		}
	}


static void array_check_end(void)
	{
	if (array_failed)
		{
		putchar(']');
		array_failed = Z_FALSE;
		}
	}


static Cycle read_cycle_item(cJSON const *item)
	{
	Cycle cycle;

	*(zuint32 *)cycle.pins = *(zuint32 *)cJSON_GetArrayItem(item, 2)->valuestring;
	cycle.address = (zuint16)cJSON_GetNumberValue(cJSON_GetArrayItem(item, 0));

	cycle.value = cJSON_IsNull(item = cJSON_GetArrayItem(item, 1))
		? -1
		: (zsint16)cJSON_GetNumberValue(item);

	return cycle;
	}


static Port read_port_item(cJSON const *item)
	{
	Port port;

	port.port      = (zuint16)cJSON_GetNumberValue(cJSON_GetArrayItem(item, 0));
	port.value     = (zuint8 )cJSON_GetNumberValue(cJSON_GetArrayItem(item, 1));
	port.direction = *cJSON_GetArrayItem(item, 2)->valuestring;
	return port;
	}


static zboolean string_is_option(char const* string, char const* short_option, char const* long_option)
	{return !strcmp(string, short_option) || !strcmp(string, long_option);}


static zboolean string_to_uint8(char const* string, zuint8 maximum, zuint8 *value)
	{
	char *end;
	zulong parsed = strtoul(string, &end, 0);

	if (end == string || *end || parsed > maximum) return Z_FALSE;
	*value = (zuint8)parsed;
	return Z_TRUE;
	}


int main(int argc, char **argv)
	{
	char const *invalid;
	int i = 0, j;
	int file_count;
	int read_error_count  = 0;
	int bad_file_count    = 0;
	int passed_file_count = 0;
	int failed_file_count = 0;
	int passed_test_count = 0;
	int failed_test_count = 0;
	zboolean test_format_and_exit = Z_FALSE;
	zboolean produce_json_output  = Z_FALSE;
	zboolean test_pins	      = Z_FALSE;
	zboolean stdin_read	      = Z_FALSE;
	zuint8 verbosity = 2;

	/* Get the name of the program. */
	my_name = strrchr(*argv, '/');
	my_name = my_name == Z_NULL || my_name[1] == '\0' ? *argv : &my_name[1];

	/* The emulator is set to Zilog NMOS by default */
	cpu.options = Z80_MODEL_ZILOG_NMOS;

	/* Parse the command line. */

	while (++i < argc && *argv[i] == '-' && argv[i][1] != '\0')
		{
		if (string_is_option(argv[i], "-V", "--version"))
			{
			puts(	"step-test-Z80 v" Z80_LIBRARY_VERSION_STRING "\n"
				"Copyright (C) 2024 Manuel Sainz de Baranda y Goñi.\n"
				"Released under the terms of the GNU General Public License v3.");

			return 0;
			}

		else if (string_is_option(argv[i], "-h", "--help"))
			{
			printf(	"Usage: %s [options] (<JSON-file>...|-)\n"
				"\n"
				"Options:\n"
				"  -V, --version           Print version information and exit.\n"
				"  -h, --help              Print this help message and exit.\n"
				"  -j, --json-output       Produce output in JSON format.\n"
				"  -m, --model <model>     Specify the CPU model to emulate.\n"
				"  -p, --pins              Test address bus, data bus and control pins.\n"
				"  -t, --test-format       Test format of the JSON file(s) and exit.\n"
				"  -v, --verbosity (0..2)  Set the verbosity level [default: 2].\n"
				"\n"
				"CPU models:\n"
				"  zilog-nmos  Zilog NMOS [default]\n"
				"  zilog-cmos  Zilog CMOS\n"
				"  nec-nmos    NEC NMOS\n"
				"\n"
				"Email bug reports and questions to <manuel@zxe.io>\n"
				"Open issues at <https://github.com/redcode/Z80>\n",
				my_name);

			return 0;
			}

		else if (string_is_option(argv[i], "-j", "--json-output"))
			produce_json_output = Z_TRUE;

		else if (string_is_option(argv[i], "-m", "--model"))
			{
			if (++i == argc) goto incomplete_option;

			for (j = 0; j < (int)Z_ARRAY_SIZE(cpu_models); j++)
				if (!strcmp(argv[i], cpu_models[j].key))
					{
					cpu.options = cpu_models[j].options;
					goto cpu_model_found;
					}

			invalid = "CPU model";
			goto invalid_argument;
			cpu_model_found: continue;
			}

		else if (string_is_option(argv[i], "-p", "--pins"))
			test_pins = Z_TRUE;

		else if (string_is_option(argv[i], "-t", "--test-format"))
			test_format_and_exit = Z_TRUE;

		else if (string_is_option(argv[i], "-v", "--verbosity"))
			{
			if (++i == argc) goto incomplete_option;

			if (!string_to_uint8(argv[i], 3, &verbosity))
				{
				invalid = "verbosity level";
				goto invalid_argument;
				}
			}
		}

	if (!(file_count = argc - i))
		{
		fprintf(stderr, "%s: No test file specified.\n", my_name);
		goto bad_syntax;
		}

	/* Initialize CPU */

	cpu.context	 = Z_NULL;
	cpu.fetch_opcode = (Z80Read )cpu_fetch_opcode;
	cpu.nop		 = (Z80Read )cpu_nop;
	cpu.fetch	 =
	cpu.read	 = (Z80Read )cpu_read;
	cpu.write	 = (Z80Write)cpu_write;
	cpu.in		 = (Z80Read )cpu_in;
	cpu.out		 = (Z80Write)cpu_out;
	cpu.halt	 = Z_NULL;
	cpu.ld_i_a	 =
	cpu.ld_r_a	 =
	cpu.reti	 =
	cpu.retn	 = Z_NULL;
	cpu.hook	 = Z_NULL;
	cpu.illegal	 = Z_NULL;

	z80_insn_clock_initialize(&insn_clock, &cpu.af, &cpu.bc, &cpu.hl, &cpu, cpu_clock_direct_read);

	if (verbosity && !produce_json_output)
		setvbuf(stdout, Z_NULL, _IONBF, 0);

	for (; !stdin_read && i != argc; i++)
		{
		char *json = Z_NULL;
		char const *error = Z_NULL;
		cJSON *tests;
		zusize json_size;

		if (argv[i][0] == '-' && argv[i][1] == '\0')
			{
			zusize buffer_size = 0;

			stdin_read = Z_TRUE;
			printf("-: ");

			do	{
				if ((json = realloc(json, (buffer_size += INPUT_BLOCK_SIZE) + 1)) == Z_NULL)
					{
					error = "cannot allocate memory";
					break;
					}

				json_size = fread(
					json + buffer_size - INPUT_BLOCK_SIZE,
					1, INPUT_BLOCK_SIZE, stdin);

				if (ferror(stdin))
					{
					error = "I/O error";
					break;
					}
				}
			while (!feof(stdin));

			json_size += buffer_size - INPUT_BLOCK_SIZE;
			}
		else
			{
			FILE *file = Z_NULL;
			long file_size;

			printf("%s: ", argv[i]);

			if ((file = fopen(argv[i], "r")) == Z_NULL)
				error = "cannot open file";

			else if (
				fseek(file, 0, SEEK_END)      ||
				(file_size = ftell(file)) < 0 ||
				fseek(file, 0, SEEK_SET)
			)
				error = "cannot determine file size";

			else if ((json = malloc(file_size + 1)) == Z_NULL)
				error = "cannot allocate memory";

			else if (fread(json, file_size, 1, file) != 1)
				error = "I/O error";

			if (file != Z_NULL) fclose(file);

			json_size = (zusize)file_size;
			}

		if (error)
			{
			printf("Error: %s\n", error);
			free(json);
			read_error_count++;
			continue;
			}

		json[json_size] = '\0';
		tests = cJSON_Parse(json);
		free(json);

		if (!validate_tests(tests))
			{
			puts("Invalid format");
			bad_file_count++;
			}

		else	{
			if (test_format_and_exit)
				{
				passed_file_count++;
				puts("OK");
				}

			else	{
				cJSON *test;

				if (	(cycles = realloc(cycles, cycles_size * sizeof(Cycle))) == Z_NULL ||
					(ports	= realloc(ports,  ports_size  * sizeof(Port ))) == Z_NULL
				)
					{
					cannot_allocate_memory:
					cJSON_Delete(tests);
					puts("Error");
					fprintf(stderr, "%s: Cannot allocate memory.\n", my_name);
					goto exit_with_error;
					}

				file_failed = Z_FALSE;

				cJSON_ArrayForEach(test, tests)
					{
					cJSON* item;
					cJSON* initial		    = cJSON_GetObjectItem(test, "initial");
					cJSON* final		    = cJSON_GetObjectItem(test, "final"  );
					cJSON* expected_cycles	    = cJSON_GetObjectItem(test, "cycles" );
					int    expected_cycle_count = cJSON_GetArraySize (expected_cycles);
					cJSON *subitem_1, *subitem_2;
					zuint address, actual, expected;

					/* Clean memory */
					memset(memory, 0, sizeof(memory));

					/* Power on CPU */
					z80_power(&cpu, Z_TRUE);
					z80_insn_clock_start(&insn_clock, cpu.resume);

					/* Set initial CPU state */
					for (j= 0; j != Z_ARRAY_SIZE(members) - 2; j++)
						{
						const Member *member = members + j;
						cJSON *value;
						double v = cJSON_GetNumberValue(cJSON_GetObjectItem(initial, member->key));

						if (member->maximum_value == 65535)
							*(zuint16 *)(void *)((char *)&cpu + member->offset) = (zuint16)v;

						else *(zuint8 *)(void *)((char *)&cpu + member->offset) = (zuint8)v;
						}

					if (cJSON_GetNumberValue(cJSON_GetObjectItem(initial, "ei")) == 1.0)
						cpu.data.uint8_array[0] = 0xFB;

					else if (cJSON_GetNumberValue(cJSON_GetObjectItem(initial, "p")) == 1.0)
						{
						cpu.data.uint8_array[0] = 0xED;
						cpu.data.uint8_array[1] = 0x57;
						}

					/* Set initial memory state */
					item = cJSON_GetObjectItem(initial, "ram");

					cJSON_ArrayForEach(subitem_1, item) memory[
						(zuint16)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem_1, 0))] =
						(zuint8 )cJSON_GetNumberValue(cJSON_GetArrayItem(subitem_1, 1));

					cycles_index = ports_index = 0;

					expected_port_count = (expected_ports = cJSON_GetObjectItem(test, "ports")) != Z_NULL
						? cJSON_GetArraySize(expected_ports)
						: 0;

					/* Run test */
					z80_run(&cpu, expected_cycle_count);

					if (cycles == Z_NULL || ports == Z_NULL)
						goto cannot_allocate_memory;

					test_failed = array_failed = Z_FALSE;
					field_separator = "";

					/* Check final CPU state */
					for (j = 0; j != Z_ARRAY_SIZE(members) - 2; j++)
						{
						const Member *member = members + j;

						expected = (zuint)cJSON_GetNumberValue(cJSON_GetObjectItem(final, member->key));

						if (member->maximum_value == 65535)
							actual = *(zuint16 *)(void *)((char *)&cpu + member->offset);

						else actual = *(zuint8 *)(void *)((char *)&cpu + member->offset);

						if (actual != expected)
							{
							mismatch_found(test);

							printf(	member->maximum_value == 65535
								? "%s %s: %04X/%04X"
								: (member->maximum_value == 255
									? "%s %s: %02X/%02X"
									: "%s %s: %u/%u"),
								field_separator, member->caption, actual, expected);

							field_separator = ",";
							}
						}

					if (	(expected = (zuint)cJSON_GetNumberValue(cJSON_GetObjectItem(final, "ei"))) !=
						(actual = cpu.data.uint8_array[0] == 0xFB)
					)
						{
						mismatch_found(test);
						printf("%s EI: %u/%u", field_separator, actual, expected);
						field_separator = ",";
						}

					if (	(expected = (zuint)cJSON_GetNumberValue(cJSON_GetObjectItem(final, "p"))) !=
						(actual =
							cpu.data.uint8_array[0] == 0xED && (
							cpu.data.uint8_array[1] == 0x57 || cpu.data.uint8_array[1] == 0x5F))
					)
						{
						mismatch_found(test);
						printf("%s P: %u/%u", field_separator, (zuint)cpu.data.uint8_array[0], 0xFB);
						field_separator = ",";
						}

					/* Check final memory state */
					item = cJSON_GetObjectItem(final, "ram");

					cJSON_ArrayForEach(subitem_1, item)
						{
						zuint address  = (zuint)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem_1, 0));
						zuint actual   = memory[address];
						zuint expected = (zuint)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem_1, 1));

						if (actual == expected) memory[address] = 0;
						else if (!actual) memory[address] = expected;
						}

					for (address = 0; address != 65536; address++)
						if (memory[address])
							{
							zuint8 expected = 0;

							mismatch_found(test);

							if (array_failed) printf(", ");

							else	{
								printf("%s RAM: [", field_separator);
								array_failed = Z_TRUE;
								field_separator = ",";
								}

							cJSON_ArrayForEach(subitem_1, item)
								if ((zuint)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem_1, 0)) == address)
									{
									expected = (zuint)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem_1, 1));
									break;
									}

							printf(	"%04X %02X/%02X",
								address,
								memory[address] == expected ? 0 : memory[address],
								expected);
							}

					array_check_end();

					/* Check cycles */

					for (j = 0; j != expected_cycle_count; j++)
						{
						cJSON *subitem_1 = cJSON_GetArrayItem(expected_cycles, j);
						Cycle expected = read_cycle_item(subitem_1);

						if (j >= cycles_index)
							print_cycle_mismatch(test, j, Z_NULL, &expected);

						else if (memcmp(cycles + j, &expected, sizeof(Cycle)))
							print_cycle_mismatch(test, j, cycles + j, &expected);
						}

					if (j < cycles_index)
						for (; j != cycles_index; j++)
							print_cycle_mismatch(test, j, cycles + j, Z_NULL);

					array_check_end();

					/* Check ports */

					for (j = 0; j != expected_port_count; j++)
						{
						cJSON *subitem_1 = cJSON_GetArrayItem(expected_ports, j);
						Port expected = read_port_item(subitem_1);

						if (j >= ports_index)
							print_port_mismatch(test, j, Z_NULL, &expected);

						else if (memcmp(ports + j, &expected, sizeof(Port)))
							print_port_mismatch(test, j, ports + j, &expected);
						}

					if (j < ports_index)
						for (; j != ports_index; j++)
							print_port_mismatch(test, j, ports + j, Z_NULL);

					array_check_end();

					if (test_failed)
						{
						failed_test_count++;
						putchar('\n');
						}

					else passed_test_count++;
					}

				if (file_failed) failed_file_count++;

				else	{
					passed_file_count++;
					puts("OK");
					}
				}
			}

		cJSON_Delete(tests);
		}

	/* Print results summary */

	printf("\nResults:%c%d files in total", test_format_and_exit ? ' ' : '\n', file_count);

	if (file_count == passed_file_count) printf(", all passed");

	else	{
		if (passed_file_count) printf(", %d passed",	 passed_file_count);
		if (failed_file_count) printf(", %d failed",	 failed_file_count);
		if (bad_file_count   ) printf(", %d invalid",	 bad_file_count	  );
		if (read_error_count ) printf(", %d unreadable", read_error_count );
		}

	if (!test_format_and_exit)
		{
		printf(".\n%d tests in total", passed_test_count + failed_test_count);

		if (!failed_test_count) printf(", all passed");

		else	{
			if (passed_test_count) printf(", %d passed", passed_test_count);
			if (failed_test_count) printf(", %d failed", failed_test_count);
			}
		}

	puts(".");

	exit_without_error:
	return read_error_count + bad_file_count + failed_file_count == 0 ? 0 : -1;

	incomplete_option:
	fprintf(stderr, "%s: Incomplete option: '%s'\n", my_name, argv[i - 1]);
	goto bad_syntax;

	invalid_argument:
	fprintf(stderr, "%s: Invalid %s: '%s'\n", my_name, invalid, argv[i]);

	bad_syntax:
	fprintf(stderr, "%s: Type '%s -h' for help.\n", my_name, my_name);

	exit_with_error:
	return -1;
	}


/* step-test-Z80.c EOF */
