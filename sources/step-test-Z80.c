/* step-test-Z80
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator - Step Testing Tool               |
|  Copyright (C) 2024-2026 Manuel Sainz de Baranda y Goñi.                     |
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

#define INPUT_BLOCK_SIZE (Z_USIZE(512) * Z_USIZE(1024))


/* MARK: - Types */

typedef struct {
	char const *caption;   /* Name used in the mismatch reports. */
	char const *key;       /* JSON key.			     */
	zusize offset;	       /* Offset within the Z80 structure.   */
	zuint16 maximum_value; /* Maximum value the member can hold. */
} Member;

/*--------------------------------------------------------------------.
| `Cycle` and `Port` store information about a clock cycle and an I/O |
| operation, respectively. These structures are packed to avoid any   |
| padding bytes, so that they can be compared using `memcmp`.	      |
'====================================================================*/

typedef Z_PACKED_STRUCTURE_BEGIN {
	zuint16 address; /* Value on the address bus.		       */
	zsint16 value;   /* Value on the data bus (-1 = do not check). */
	char pins[4];    /* State of the pins: /RD, WR, /MREQ, /IORQ.  */
} Z_PACKED_STRUCTURE_END Cycle;

typedef Z_PACKED_STRUCTURE_BEGIN {
	zuint16 port;   /* I/O port number.			       */
	zuint8 value;   /* Value read from or written to the I/O port. */
	char direction; /* 'r' = read, 'w' = write.		       */
} Z_PACKED_STRUCTURE_END Port;


/* MARK: - Global Variables */

static struct {char const *key; zuint8 options;} const cpu_models[] = {
	{"zilog-nmos", Z80_MODEL_ZILOG_NMOS},
	{"zilog-cmos", Z80_MODEL_ZILOG_CMOS},
	{"nec-nmos",   Z80_MODEL_NEC_NMOS  }};

static Member const members[] = {
	{"A",	 "a",	 Z_MEMBER_OFFSET(Z80, af.uint8_values.at_1 ), Z_UINT8_MAXIMUM },
	{"AF'",	 "af_",	 Z_MEMBER_OFFSET(Z80, af_.uint16_value     ), Z_UINT16_MAXIMUM},
	{"B",	 "b",	 Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_1 ), Z_UINT8_MAXIMUM },
	{"BC'",	 "bc_",	 Z_MEMBER_OFFSET(Z80, bc_.uint16_value     ), Z_UINT16_MAXIMUM},
	{"C",	 "c",	 Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_0 ), Z_UINT8_MAXIMUM },
	{"D",	 "d",	 Z_MEMBER_OFFSET(Z80, de.uint8_values.at_1 ), Z_UINT8_MAXIMUM },
	{"DE'",	 "de_",	 Z_MEMBER_OFFSET(Z80, de_.uint16_value     ), Z_UINT16_MAXIMUM},
	{"E",	 "e",	 Z_MEMBER_OFFSET(Z80, de.uint8_values.at_0 ), Z_UINT8_MAXIMUM },
	{"F",	 "f",	 Z_MEMBER_OFFSET(Z80, af.uint8_values.at_0 ), Z_UINT8_MAXIMUM },
	{"H",	 "h",	 Z_MEMBER_OFFSET(Z80, hl.uint8_values.at_1 ), Z_UINT8_MAXIMUM },
	{"HL'",	 "hl_",	 Z_MEMBER_OFFSET(Z80, hl_.uint16_value     ), Z_UINT16_MAXIMUM},
	{"I",	 "i",	 Z_MEMBER_OFFSET(Z80, i			   ), Z_UINT8_MAXIMUM },
	{"IFF1", "iff1", Z_MEMBER_OFFSET(Z80, iff1		   ), 1		      },
	{"IFF2", "iff2", Z_MEMBER_OFFSET(Z80, iff2		   ), 1		      },
	{"IM",	 "im",	 Z_MEMBER_OFFSET(Z80, im		   ), 2		      },
	{"IX",	 "ix",	 Z_MEMBER_OFFSET(Z80, ix_iy[0].uint16_value), Z_UINT16_MAXIMUM},
	{"IY",	 "iy",	 Z_MEMBER_OFFSET(Z80, ix_iy[1].uint16_value), Z_UINT16_MAXIMUM},
	{"L",	 "l",	 Z_MEMBER_OFFSET(Z80, hl.uint8_values.at_0 ), Z_UINT8_MAXIMUM },
	{"PC",	 "pc",	 Z_MEMBER_OFFSET(Z80, pc.uint16_value	   ), Z_UINT16_MAXIMUM},
	{"Q",	 "q",	 Z_MEMBER_OFFSET(Z80, q			   ), Z_UINT8_MAXIMUM },
	{"SP",	 "sp",	 Z_MEMBER_OFFSET(Z80, sp.uint16_value	   ), Z_UINT16_MAXIMUM},
	{"R",	 "r",	 Z_MEMBER_OFFSET(Z80, r			   ), Z_UINT8_MAXIMUM },
	{"WZ",	 "wz",	 Z_MEMBER_OFFSET(Z80, memptr.uint16_value  ), Z_UINT16_MAXIMUM},
	{"EI",	 "ei",	 0,					      1		      },
	{"P",	 "p",	 0,					      1		      }};

static Z80 cpu;
static Z80InsnClock insn_clock;
static zuint8 memory[Z_USIZE(65536)];

static Cycle* cycles;
static zuint  cycles_size;
static zuint  cycles_index;
static Port*  ports;
static zuint  ports_size;
static zuint  ports_index;
static cJSON* expected_ports;
static zuint  expected_port_count;

static zbool file_failed, test_failed, array_failed;
static char const *field_separator = "";

static zbool cpu_break;


/*----------------------------------------------------------------------------.
| `add_cycle` and `add_port` add entries to the lists of recorded cycles and  |
| I/O port operations, respectively. If memory reallocation fails, they break |
| the CPU emulation and ignore further entries.				      |
'============================================================================*/

static void add_cycle(zuint16 address, zsint16 value, char const *pins)
	{
	Cycle *c;

	if (cycles_index == cycles_size)
		{
		if (cpu_break) return;
		cycles_size += Z80_MAXIMUM_CYCLES_PER_STEP;

		if ((c = realloc(cycles, cycles_size * sizeof(Cycle))) == Z_NULL)
			{
			z80_break(&cpu);
			cpu_break = Z_TRUE;
			return;
			}

		cycles = c;
		}

	c		    = cycles + cycles_index;
	c->address	    = address;
	c->value	    = value;
	*(zuint32 *)c->pins = *(zuint32 const *)pins;
	cycles_index++;
	}


static void add_extra_cycles(zuint16 address, zuint8 count)
	{while (count--) add_cycle(address, -1, "----");}


static void add_port(zuint16 port, zuint8 value, char direction)
	{
	Port *p;

	if (ports_index == ports_size)
		{
		if (cpu_break) return;

		if ((p = realloc(ports, ++ports_size * sizeof(Port))) == Z_NULL)
			{
			z80_break(&cpu);
			cpu_break = Z_TRUE;
			return;
			}
		}

	p	     = ports + ports_index;
	p->port	     = port;
	p->value     = value;
	p->direction = direction;
	ports_index++;
	}


/* MARK: - CPU Callbacks */

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
	zuint8 value = (zuint8)cJSON_GetNumberValue(cJSON_GetArrayItem(
		cJSON_GetArrayItem(expected_ports, (int)ports_index), 1));

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
	zuint8 extra = z80_insn_clock_extra_out(&insn_clock);

	Z_UNUSED(context)
	add_cycle(port,	-1,    "----");
	add_cycle(port, value, "-w-i");
	add_cycle(port, value, "-w-i");
	add_cycle(port, value, "-w-i");
	add_extra_cycles(port, extra);
	add_port (port, value, 'w');
	}


/* MARK: - Instruction Clock Callback */

static zuint8 insn_clock_read(void *context, zuint16 address)
	{
	Z_UNUSED(context)
	return memory[address];
	}


/* MARK: - JSON Validation */

static zbool is_number_between(cJSON *number, int minimum, int maximum)
	{
	double value;

	return	cJSON_IsNumber(number)				  &&
		(value = cJSON_GetNumberValue(number)) >= minimum &&
		value <= maximum;
	}


static zbool validate_test_state(cJSON *state)
	{
	cJSON *item, *subitem;
	Member const *member;

	/* The state object must contain all CPU members plus the `"ram"` array. */
	if (	!cJSON_IsObject(state)							||
		cJSON_GetArraySize(state) != (int)Z_ARRAY_SIZE(members) + /* "ram" */ 1 ||
		(item = cJSON_GetObjectItem(state, "ram")) == Z_NULL			||
		!cJSON_IsArray(item)
	)
		return Z_FALSE;

	/* The `"ram"` array must contain arrays of two numbers: 16-bit address and
	   8-bit value. */
	cJSON_ArrayForEach(subitem, item) if (
		!cJSON_IsArray(subitem)							||
		cJSON_GetArraySize(subitem) != 2					||
		!is_number_between(cJSON_GetArrayItem(subitem, 0), 0, Z_UINT16_MAXIMUM) ||
		!is_number_between(cJSON_GetArrayItem(subitem, 1), 0, Z_UINT8_MAXIMUM)
	)
		return Z_FALSE;

	/* Each CPU member must be present and hold a valid number according to its type. */
	for (member = members; member != members + Z_ARRAY_SIZE(members) - 2; member++)
		if (	(item = cJSON_GetObjectItem(state, member->key)) == Z_NULL ||
			!is_number_between(item, 0, member->maximum_value)
		)
			return Z_FALSE;

	return Z_TRUE;

	/*--------------------------------------------------------------.
	| `"ei"` and `"p"` are mutually exclusive, as the previous	|
	| instruction cannot be `ei` and `ld a,{i|r}` at the same time. |
	'==============================================================
	return	2.0 !=
		cJSON_GetNumberValue(cJSON_GetObjectItem(state, "ei")) +
		cJSON_GetNumberValue(cJSON_GetObjectItem(state, "p" ));*/
	}


static zbool validate_tests(cJSON *tests)
	{
	cJSON *test;

	if (!cJSON_IsArray(tests)) return Z_FALSE;
	cycles_size = ports_size = 0;

	cJSON_ArrayForEach(test, tests)
		{
		cJSON *item, *array;
		int size;

		if (	!cJSON_IsObject(test)					||
			(item = cJSON_GetObjectItem(test, "name"   )) == Z_NULL	||
			!cJSON_IsString(item)					||
			(item = cJSON_GetObjectItem(test, "initial")) == Z_NULL ||
			!validate_test_state(item)				||
			(item = cJSON_GetObjectItem(test, "final"  )) == Z_NULL ||
			!validate_test_state(item)				||
			(item = cJSON_GetObjectItem(test, "cycles" )) == Z_NULL ||
			!cJSON_IsArray(item)					||
			!(size = cJSON_GetArraySize(item))
		)
			return Z_FALSE;

		if (size > (int)cycles_size) cycles_size = (zuint)size;

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

		if ((item = cJSON_GetObjectItem(test, "ports")) != Z_NULL)
			{
			if (!cJSON_IsArray(item) || !(size = cJSON_GetArraySize(item)))
				return Z_FALSE;

			if (size > (int)ports_size) ports_size = (zuint)size;

			cJSON_ArrayForEach(array, item)
				{
				cJSON *io_item;

				if (	!cJSON_IsArray(array)						      ||
					cJSON_GetArraySize(array) != 3					      ||
					!is_number_between(cJSON_GetArrayItem(array, 0), 0, Z_UINT16_MAXIMUM) ||
					!is_number_between(cJSON_GetArrayItem(array, 1), 0, Z_UINT8_MAXIMUM ) ||
					!cJSON_IsString(io_item = cJSON_GetArrayItem(array, 2))		      ||
					(strcmp(io_item->valuestring, "r") && strcmp(io_item->valuestring, "w"))
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


static zuint16 data_bus_value_to_string(zsint16 value)
	{
	char string[3];

	if (value == -1) return (zuint16)('-' | ((zuint16)'-' << 8));
	sprintf(string, "%02X", (zuint)value);
	return *(zuint16 *)string;
	}


static void print_cycle_mismatch(cJSON *test, zuint index, Cycle const *actual, Cycle const *expected)
	{
	char actual_value[2], expected_value[2];

	mismatch_found(test);

	if (array_failed) printf(", ");

	else	{
		array_failed = Z_TRUE;
		printf("%s Cycles: [", field_separator);
		field_separator = ",";
		}

	if (actual != Z_NULL)
		*(zuint16 *)actual_value = data_bus_value_to_string(actual  ->value);

	if (expected != Z_NULL)
		*(zuint16 *)expected_value = data_bus_value_to_string(expected->value);

	printf("%u", index + 1);

	if (actual == Z_NULL)
		printf(	" ****""/%04X **""/%.2s ****""/%.4s",
			expected->address, expected_value, expected->pins);

	else if (expected == Z_NULL)
		printf(	" %04X/""**** %.2s/""** %.4s/""****",
			actual->address, actual_value, actual->pins);

	else	{
		if (actual->address != expected->address)
			printf(" %04X/%04X", actual->address, expected->address);

		if (actual->value != expected->value)
			printf(" %.2s/%.2s", actual_value, expected_value);

		if (*(zuint32 *)actual->pins != *(zuint32 *)expected->pins)
			printf(" %.4s/%.4s", actual->pins, expected->pins);
		}
	}


static void print_port_mismatch(cJSON *test, zuint index, Port const *actual, Port const *expected)
	{
	mismatch_found(test);

	if (array_failed) printf(", ");

	else	{
		array_failed = Z_TRUE;
		printf("%s Ports: [", field_separator);
		field_separator = ",";
		}

	printf("%u", index + 1);

	if (actual == Z_NULL)
		printf(	" ****""/%04X **""/%02X *""/%c",
			expected->port, expected->value, expected->direction);

	else if (expected == Z_NULL)
		printf(	" %04X/""**** %02X/""** %c/""*",
			actual->port, actual->value, actual->direction);

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
		? -1 : (zsint16)cJSON_GetNumberValue(item);

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


static zbool string_is_option(char const *string, char const *option)
	{return (*string == *option && string[1] == '\0') || !strcmp(string, &option[1]);}


int main(int argc, char **argv)
	{
	zbool test_format_and_exit = Z_FALSE;
	zbool produce_json_output  = Z_FALSE;
	zbool test_pins		   = Z_FALSE;
	zbool read_from_stdin	   = Z_FALSE;
	zuint8 verbosity	   = 2;
	int i			   = 0;
	zuint j;
	zuint file_count;
	zuint read_error_count;
	zuint bad_file_count;
	zuint passed_file_count;
	zuint failed_file_count;
	zuint test_count;
	zuint passed_test_count;
	zuint failed_test_count;
	char const *string;

#	define option  string
#	define invalid string
#	define error   string

	/* The default CPU model to emulate is Zilog NMOS. */
	cpu.options = Z80_MODEL_ZILOG_NMOS;

	/* Parse command line arguments. */
	while (++i < argc && *argv[i] == '-' && *(option = &argv[i][1]) != '\0')
		{
		if (string_is_option(option, "V-version"))
			{
			puts(	"step-test-Z80 v" Z80_LIBRARY_VERSION_STRING "\n"
				"Copyright (C) 2024-2026 Manuel Sainz de Baranda y Goñi.\n"
				"Released under the terms of the GNU General Public License v3.");

			return 0;
			}

		else if (string_is_option(option, "h-help"))
			{
			printf(	"Usage: step-test-Z80 [options] <JSON-file>...\n"
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
				"Email bug reports to <manuel@zxe.io>\n"
				"Open issues at <https://github.com/redcode/Z80>\n");

			return 0;
			}

		else if (string_is_option(option, "j-json-output"))
			produce_json_output = Z_TRUE;

		else if (string_is_option(option, "m-model"))
			{
			if (++i == argc) goto incomplete_option;

			for (j = 0; j < Z_ARRAY_SIZE(cpu_models); j++)
				if (!strcmp(argv[i], cpu_models[j].key))
					{
					cpu.options = cpu_models[j].options;
					goto cpu_model_found;
					}

			invalid = "CPU model";
			goto invalid_argument;
			cpu_model_found: continue;
			}

		else if (string_is_option(option, "p-pins"))
			test_pins = Z_TRUE;

		else if (string_is_option(option, "t-test-format"))
			test_format_and_exit = Z_TRUE;

		else if (string_is_option(option, "v-verbosity"))
			{
			char *end;
			zulong parsed;

			if (++i == argc) goto incomplete_option;
			parsed = strtoul(argv[i], &end, 0);

			if (end == argv[i] || *end || parsed > 2)
				{
				invalid = "verbosity level";
				goto invalid_argument;
				}

			verbosity = (zuint8)parsed;
			}

		else	{
			invalid = "option";
			goto invalid_argument;
			}
		}

	if (!(file_count = (zuint)(argc - i)))
		{
		fputs("step-test-Z80: No file specified.\n", stderr);
		goto bad_syntax;
		}

	for (j = (zuint)i; j < argc; j++) if (*argv[j] == '\0')
		{
		i = (int)j;
		invalid = "file path";
		goto invalid_argument;
		}

	/* Initialize the result counters. */
	read_error_count  =
	bad_file_count	  =
	passed_file_count =
	failed_file_count =
	passed_test_count =
	failed_test_count = 0;

	/* Initialize the CPU emulator and the instruction clock. */
	cpu.context	 = Z_NULL;
	cpu.fetch_opcode = cpu_fetch_opcode;
	cpu.nop		 = cpu_nop;
	cpu.fetch	 =
	cpu.read	 = cpu_read;
	cpu.write	 = cpu_write;
	cpu.in		 = cpu_in;
	cpu.out		 = cpu_out;
	cpu.halt	 = Z_NULL;
	cpu.ld_i_a	 =
	cpu.ld_r_a	 =
	cpu.reti	 =
	cpu.retn	 = Z_NULL;
	cpu.hook	 = Z_NULL;
	cpu.illegal	 = Z_NULL;

	z80_insn_clock_initialize(&insn_clock, &cpu.af, &cpu.bc, &cpu.hl, Z_NULL, insn_clock_read);

	cycles = Z_NULL;
	ports  = Z_NULL;

	if (verbosity && !produce_json_output)
		setvbuf(stdout, Z_NULL, _IONBF, 0);

	for (; !read_from_stdin && i != argc; i++)
		{
		char const *parse_end = Z_NULL;
		char *json = Z_NULL;
		zusize json_size;
		cJSON *tests = Z_NULL;

		error = Z_NULL;

		if (argv[i][0] == '-' && argv[i][1] == '\0')
			{
			zusize buffer_size = 0;

			read_from_stdin = Z_TRUE;
			printf("-: ");

			do	{
				if ((json = realloc(json, buffer_size += INPUT_BLOCK_SIZE)) == Z_NULL)
					goto cannot_allocate_memory;

				json_size = fread(json + buffer_size - INPUT_BLOCK_SIZE, 1, INPUT_BLOCK_SIZE, stdin);

				if (ferror(stdin))
					{
					error = "I/O error";
					break;
					}
				}
			while (!feof(stdin));

			json_size += buffer_size - INPUT_BLOCK_SIZE;
			}

		else	{
			FILE *file = Z_NULL;
			long file_size;

			printf("%s: ", argv[i]);

			if ((file = fopen(argv[i], "r")) == Z_NULL)
				error = "Cannot open file";

			else	{
				if (	fseek(file, 0, SEEK_END)      ||
					(file_size = ftell(file)) < 0 ||
					fseek(file, 0, SEEK_SET)
				)
					error = "Cannot determine file size";

				else if ((json_size = (zusize)file_size))
					{
					if ((json = malloc(json_size)) == Z_NULL)
						{
						fclose(file);
						goto cannot_allocate_memory;
						}

					if (fread(json, json_size, 1, file) != 1)
						error = "I/O error";
					}

				fclose(file);
				}
			}

		if (error != Z_NULL)
			{
			free(json);
			printf("Error: %s\n", error);
			read_error_count++;
			}

		else if (!json_size)
			{
			free(json);
			invalid_format:
			puts("Invalid format");
			bad_file_count++;
			}

		else if ((tests = cJSON_ParseWithLengthOpts(json, json_size, &parse_end, Z_FALSE)) == Z_NULL)
			{
			free(json);
			if (parse_end == Z_NULL) goto cannot_allocate_memory;
			goto invalid_format;
			}

		else	{
			free(json);

			if (!validate_tests(tests))
				{
				cJSON_Delete(tests);
				goto invalid_format;
				}

			else if (test_format_and_exit)
				{
				passed_file_count++;
				puts("OK");
				}

			else	{
				cJSON *test;

				if (cycles_size && (cycles = malloc(cycles_size * sizeof(Cycle))) == Z_NULL)
					goto cannot_allocate_memory;

				if (ports_size && (ports = malloc(ports_size * sizeof(Port))) == Z_NULL)
					goto cannot_allocate_memory;

				file_failed = Z_FALSE;

				cJSON_ArrayForEach(test, tests)
					{
					cJSON *item, *subitem;
					cJSON *initial		   = cJSON_GetObjectItem(test, "initial");
					cJSON *final		   = cJSON_GetObjectItem(test, "final"  );
					cJSON *expected_cycles	   = cJSON_GetObjectItem(test, "cycles" );
					zuint expected_cycle_count = (zuint)cJSON_GetArraySize(expected_cycles);
					zuint actual, expected;
					zuint32 address;

					/* Clean memory, power on CPU and start instruction clock. */
					memset(memory, 0, sizeof(memory));
					z80_power(&cpu, Z_TRUE);
					z80_insn_clock_start(&insn_clock, cpu.resume);

					/* Set initial CPU state. */
					for (j = 0; j != Z_ARRAY_SIZE(members) - 2; j++)
						{
						Member const *member = members + j;
						double v = cJSON_GetNumberValue(cJSON_GetObjectItem(initial, member->key));

						if (member->maximum_value == Z_UINT16_MAXIMUM)
							*(zuint16 *)(void *)((char *)&cpu + member->offset) = (zuint16)v;

						else *(zuint8 *)(void *)((char *)&cpu + member->offset) = (zuint8)v;
						}

					if (cJSON_GetNumberValue(cJSON_GetObjectItem(initial, "ei")) == 1.0)
						cpu.data.uint8_array[0] = 0xFB; /* `ei` */

					else if (cJSON_GetNumberValue(cJSON_GetObjectItem(initial, "p")) == 1.0)
						{
						cpu.data.uint8_array[0] = 0xED; /* `ld a,i` */
						cpu.data.uint8_array[1] = 0x57;
						}

					/* Set initial memory state. */
					item = cJSON_GetObjectItem(initial, "ram");

					cJSON_ArrayForEach(subitem, item) memory[
						(zuint16)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem, 0))] =
						(zuint8 )cJSON_GetNumberValue(cJSON_GetArrayItem(subitem, 1));

					cycles_index = ports_index = 0;

					expected_port_count = (expected_ports = cJSON_GetObjectItem(test, "ports")) != Z_NULL
						? (zuint)cJSON_GetArraySize(expected_ports) : 0U;

					/* Run the test. */
					cpu_break = Z_FALSE;

#					ifdef STEP_TEST_Z80_WITH_EXECUTE
						z80_execute(&cpu, expected_cycle_count);
#					else
						z80_run(&cpu, expected_cycle_count);
#					endif

					if (cpu_break) goto cannot_allocate_memory;

					test_failed = array_failed = Z_FALSE;
					field_separator = "";

					/* Check final CPU state. */
					for (j = 0; j != Z_ARRAY_SIZE(members) - 2; j++)
						{
						Member const *member = members + j;

						expected = (zuint)cJSON_GetNumberValue(cJSON_GetObjectItem(final, member->key));

						if (member->maximum_value == Z_UINT16_MAXIMUM)
							actual = *(zuint16 *)(void *)((char *)&cpu + member->offset);

						else actual = *(zuint8 *)(void *)((char *)&cpu + member->offset);

						if (actual != expected)
							{
							mismatch_found(test);

							printf(	member->maximum_value == Z_UINT16_MAXIMUM
								? "%s %s: %04X/%04X"
								: (member->maximum_value == Z_UINT8_MAXIMUM
									? "%s %s: %02X/%02X" : "%s %s: %u/%u"),
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
						printf("%s P: %u/%u", field_separator, cpu.data.uint8_array[0], 0xFBU);
						field_separator = ",";
						}

					/* Check final memory state. */
					item = cJSON_GetObjectItem(final, "ram");

					cJSON_ArrayForEach(subitem, item)
						{
						address	 = (zuint32)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem, 0));
						actual	 = memory[address];
						expected = (zuint)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem, 1));

						if (actual == expected) memory[address] = 0;
						else if (!actual) memory[address] = (zuint8)expected;
						}

					for (address = 0; address != Z_UINT32(65536); address++)
						if (memory[address])
							{
							expected = 0;
							mismatch_found(test);

							if (array_failed) printf(", ");

							else	{
								printf("%s RAM: [", field_separator);
								array_failed = Z_TRUE;
								field_separator = ",";
								}

							cJSON_ArrayForEach(subitem, item)
								if ((zuint32)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem, 0)) == address)
									{
									expected = (zuint)cJSON_GetNumberValue(cJSON_GetArrayItem(subitem, 1));
									break;
									}

							printf(	"%04X %02X/%02X",
								address,
								memory[address] == expected ? 0U : memory[address],
								expected);
							}

					array_check_end();

					/* Check cycles. */
					if (test_pins)
						{
						for (j = 0; j != expected_cycle_count; j++)
							{
							Cycle expected_cycle = read_cycle_item(cJSON_GetArrayItem(expected_cycles, (int)j));

							if (j >= cycles_index)
								print_cycle_mismatch(test, j, Z_NULL, &expected_cycle);

							else if (memcmp(cycles + j, &expected_cycle, sizeof(Cycle)))
								print_cycle_mismatch(test, j, cycles + j, &expected_cycle);
							}

						if (j < cycles_index)
							for (; j != cycles_index; j++)
								print_cycle_mismatch(test, j, cycles + j, Z_NULL);

						array_check_end();
						}

					else if (cycles_index != expected_cycle_count)
						{
						mismatch_found(test);
						printf(	"%s Cycles: %u/%u", field_separator, cycles_index, expected_cycle_count);
						field_separator = ",";
						}

					/* Check ports. */
					for (j = 0; j != expected_port_count; j++)
						{
						Port expected_port = read_port_item(cJSON_GetArrayItem(expected_ports, (int)j));

						if (j >= ports_index)
							print_port_mismatch(test, j, Z_NULL, &expected_port);

						else if (memcmp(ports + j, &expected_port, sizeof(Port)))
							print_port_mismatch(test, j, ports + j, &expected_port);
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
					puts("Passed");
					}
				}

			cJSON_Delete(tests ); tests  = Z_NULL;
			free	    (ports ); ports  = Z_NULL;
			free	    (cycles); cycles = Z_NULL;
			}

		continue;

		cannot_allocate_memory:
		free(ports );
		free(cycles);
		if (tests != Z_NULL) cJSON_Delete(tests);
		puts("Error");
		fputs("step-test-Z80: Cannot allocate memory.\n", stderr);
		return -1;
		}

	/* Print the results. */
	printf(	"\nResults:%c%u file%s in total",
		test_format_and_exit ? ' ' : '\n',
		file_count,
		file_count > 1 ? "s" : "");

	if (file_count == 1) printf(
		", %s",
		passed_file_count
			? "passed"
			: (failed_file_count
				? "failed"
				: (bad_file_count ? "invalid" : "unreadable")));

	else if (file_count == passed_file_count) printf(", all passed"	   );
	else if (file_count == failed_file_count) printf(", all failed"	   );
	else if (file_count == bad_file_count	) printf(", all invalid"   );
	else if (file_count == read_error_count	) printf(", all unreadable");

	else	{
		if (passed_file_count) printf(", %u passed",	 passed_file_count);
		if (failed_file_count) printf(", %u failed",	 failed_file_count);
		if (bad_file_count   ) printf(", %u invalid",	 bad_file_count	  );
		if (read_error_count ) printf(", %u unreadable", read_error_count );
		}

	if (!test_format_and_exit && (test_count = passed_test_count + failed_test_count))
		{
		printf(	".\n%u%s test%s in total",
			test_count,
			bad_file_count || read_error_count ? " valid" : "",
			test_count > 1 ? "s" : "");

		if (!failed_test_count) printf(",%s passed", passed_test_count > 1 ? " all" : "");
		else if (!passed_test_count) printf(",%s failed", failed_test_count > 1 ? " all" : "");
		else printf(", %u passed, %u failed", passed_test_count, failed_test_count);
		}

	puts(".");
	return read_error_count + bad_file_count + failed_file_count == 0 ? 0 : -1;

	incomplete_option:
	fprintf(stderr, "step-test-Z80: Incomplete option: %s\n", argv[i - 1]);
	goto bad_syntax;

	invalid_argument:
	if (*argv[i] != '\0') fprintf(stderr, "step-test-Z80: Invalid %s: %s\n", invalid, argv[i]);
	else fprintf(stderr, "step-test-Z80: Bad syntax: Empty %s.\n", invalid);

	bad_syntax:
	fputs("step-test-Z80: Type 'step-test-Z80 -h' for help.\n", stderr);
	return -1;
	}


/* step-test-Z80.c EOF */
