=====
Usage
=====

.. code-block:: c

	#include <Z/constants/pointer.h> /* Z_NULL */
	#include <Z80.h>

	#define CYCLES_PER_FRAME 69888
	#define CYCLES_AT_INT    24
	#define CYCLES_PER_INT   32
	#define ROM_SIZE         0x4000 /* 16 KiB */
	#define MEMORY_SIZE      0x8000 /* 32 KiB */

	typedef struct {
		void* context;
		zuint8 (* read)(void *context);
		void (* write)(void *context, zuint8 value);
		zuint16 assigned_port;
	} Device;

	typedef struct {
		zusize  cycles;
		zuint8  memory[65536];
		Z80     cpu;
		Device* devices;
		zusize  device_count;
	} Machine;


	Device *machine_find_device(Machine *self, zuint16 port)
		{
		zusize index = 0;

		for (; index < device_count; index++)
			if (self->devices[index]->assigned_port == port)
				return &self->devices[index];

		return Z_NULL;
		}


	static zuint8 machine_cpu_read(Machine *self, zuint16 address)
		{
		return address < MEMORY_SIZE ? self->memory[address] : 0xFF;
		}


	static void machine_cpu_write(Machine *self, zuint16 address, zuint8 value)
		{
		if (address >= ROM_SIZE && address < MEMORY_SIZE)
			self->memory[address] = value;
		}


	static zuint8 machine_cpu_in(Machine *self, zuint16 port)
		{
		Device *device = machine_find_device(self, port);

		return device != Z_NULL ? device->read(device->context) : 0xFF;
		}


	static void machine_cpu_out(Machine *self, zuint16 port, zuint8 value)
		{
		Device *device = machine_find_device(self, port);

		if (device != Z_NULL) device->write(device->context, value);
		}


	void machine_initialize(Machine *self)
		{
		self->cycles           = 0;
		self->cpu.context      = self;
		self->cpu.fetch_opcode =
		self->cpu.fetch        =
		self->cpu.nop          =
		self->cpu.read         = (Z80Read )machine_cpu_read;
		self->cpu.write        = (Z80Write)machine_cpu_write;
		self->cpu.in           = (Z80Read )machine_cpu_in;
		self->cpu.out          = (Z80Write)machine_cpu_out;
		self->cpu.halt         = Z_NULL;
		self->cpu.nmia         = Z_NULL;
		self->cpu.inta         = Z_NULL;
		self->cpu.int_fetch    = Z_NULL;
		self->cpu.ld_i_a       = Z_NULL;
		self->cpu.ld_r_a       = Z_NULL;
		self->cpu.reti         = Z_NULL;
		self->cpu.retn         = Z_NULL;
		self->cpu.hook         = Z_NULL;
		self->cpu.illegal      = Z_NULL;
		self->cpu.options      = Z80_MODEL_ZILOG_NMOS;

		/* Create and initialize devices... */
		}


	void machine_power(Machine *self, zboolean state)
		{
		if (state) memset(self->memory, 0, 65536);
		z80_power(&self->cpu, state);
		}


	void machine_reset(Machine *self)
		{
		z80_instant_reset(&cpu);
		}


	void machine_run_frame(Machine *self)
		{
		self->cycles += z80_execute(&self->cpu, CYCLES_AT_INT - self->cycles);
		z80_int(&self->cpu, Z_TRUE);
		self->cycles += z80_run(&self->cpu, (CYCLES_AT_INT + CYCLES_PER_INT) - self->cycles);
		z80_int(&self->cpu, Z_FALSE);
		self->cycles += z80_execute(&self->cpu, CYCLES_PER_FRAME - self->cycles);
		self->cycles -= CYCLES_PER_FRAME;
		}
