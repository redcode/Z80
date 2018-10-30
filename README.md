# Zilog Z80 CPU Emulator
Copyright © 1999-2018 Manuel Sainz de Baranda y Goñi.  
Released under the terms of the [GNU General Public License v3](http://www.gnu.org/copyleft/gpl.html).

This is a very accurate [Z80](http://en.wikipedia.org/wiki/Zilog_Z80) [emulator](http://en.wikipedia.org/wiki/Emulator) I wrote many years ago. It has been used in several machine emulators by other people and it has been extensivelly tested. It's very fast and small (although there are faster ones written in assembly), its structure is very clear and the code is commented.

If you are looking for an accurate Zilog Z80 CPU emulator for your project maybe you have found the correct one. I use this core in the [ZX Spectrum emulator](http://github.com/redcode/mZX) I started as hobby.


# Building

In order to compile you must install [Z](http://github.com/redcode/Z), a **header only** library which provides types, macros, inline functions, and a lot of utilities to detect the particularities of the compiler and the target system at compile time. This is the only dependency, the standard C library and its headers are not used and the emulator doesn't need to be dynamically linked against any library.

A Xcode project is provided to build the emulator. It has the following targets:

#### Targets

Name | Description
--- | ---
dynamic | Shared library.
dynamic module  | Shared library with a module ABI to be used in modular multi-machine emulators.
static | Static library.
static module | Static library with a descriptive ABI to be used in monolithic multi-machine emulators.

#### Constants used in Z80.h

Name | Description
--- | ---
CPU_Z80_STATIC | You need to define this if you are using the emulator as a static library or if you have added its sources to your project.

#### Constants used in Z80.c
Name | Description
--- | ---
CPU_Z80_BUILD_ABI | Builds the ABI of type `ZCPUEmulatorABI` declared in the header with the identifier `abi_emulation_cpu_z80`.
CPU_Z80_BUILD_MODULE_ABI | Builds a generic module ABI of type `ZModuleABI`. This constant enables `CPU_Z80_BUILD_ABI` automatically so `abi_emulation_cpu_z80` will be build too. This option is intended to be used when building a true module loadable at runtime with `dlopen()`, `LoadLibrary()` or similar. The module ABI can be accessed retrieving the **weak** symbol `__module_abi__`.
CPU_Z80_HIDE_API | Makes the API functions private.
CPU_Z80_HIDE_ABI | Makes the `abi_emulation_cpu_z80` private.
CPU_Z80_USE_LOCAL_HEADER | Use this if you have imported _Z80.h_ and _Z80.c_ to your project. _Z80.c_ will include `"Z80.h"` instead of `<emulation/CPU/Z80.h>`.


# API

### `z80_power`
Changes the CPU power status.

**Prototype**
```C
void z80_power(Z80 *object, zboolean state);
```
**Parameters**
* `object` → A pointer to a Z80 emulator instance object.
* `state` → `TRUE` for power ON, `FALSE` otherwise.

### `z80_reset`
Resets the CPU by reinitializing its variables and sets its registers to the state they would be in a real Z80 CPU after a pulse in the `RESET` line.

**Prototype**
```C
void z80_reset(Z80 *object);
```

**Parameters**
* `object` → A pointer to a Z80 emulator instance object.

### `z80_run`
Runs the CPU for a given number of ```cycles```.

**Prototype**
```C
zusize z80_run(Z80 *object, zusize cycles);
```

**Parameters**
* `object` → A pointer to a Z80 emulator instance object.
* `cycles` → The number of cycles to be executed.

**Return value**
The number of cycles executed.

**Discusion**
Given the fact that one Z80 instruction needs between 4 and 23 cycles to be executed, it is not always possible to run the CPU the exact number of cycles specfified.

### `z80_nmi`
Performs a non-maskable interrupt. This is equivalent to a pulse in the `NMI` line of a real Z80 CPU.

**Prototype**
```C
void z80_nmi(Z80 *object);
```

**Parameters**
* `object` → A pointer to a Z80 emulator instance object.

### `z80_int`
Changes the state of the maskable interrupt. This is equivalent to a change in the `INT` line of a real Z80 CPU.

**Prototype**
```C
void z80_int(Z80 *object, zboolean state);
```

**Parameters**
* `object` → A pointer to a Z80 emulator instance object.
* `state` → `TRUE` = set line high, `FALSE` = set line low.


# Callbacks
Before using an instance of the Z80 emulator, its callback pointers must be initialized with the pointers to the functions that your program must provide in order to make possible for the CPU to access the emulated machine's resources. All callbacks are mandatory except `halt`, which is optional and must be set to `NULL` if not used.

### `read` 
Called when the CPU needs to read 8 bits from memory.

**Prototype**
```C
zuint8 (* read)(void *context, zuint16 address);
```

**Parameters**
* `context` → A pointer to the calling emulator instance.
* `address` → The memory address to read.

**Return value**
The 8 bits read from memory.

### `write`
Called when the CPU needs to write 8 bits to memory.

**Prototype**
```C
void (* write)(void *context, zuint16 address, zuint8 value);
```

**Parameters**
* `context` → A pointer to the calling emulator instance.
* `address` → The memory address to write.
* `value` → The value to write in `address`.

### `in`
Called when the CPU needs to read 8 bits from an I/O port.

**Prototype**
```C
zuint8 (* in)(void *context, zuint16 port);
```

**Parameters**
* `context` → A pointer to the calling emulator instance.
* `port` → The number of the I/O port.

**Return value**
The 8 bits read from the I/O port.

### `out`
Called when the CPU needs to write 8 bits to an I/O port.

**Prototype**
```C
void (* out)(void *context, zuint16 port, zuint8 value);
```

**Parameters**
* `context` → A pointer to the calling emulator instance.
* `port` → The number of the I/O port.
* `value` → The value to write.

### `int_data`
Called when the CPU starts executing a maskable interrupt and the interruption mode is 0. This callback must return the instruction that the CPU would read from the data bus in this case.

**Prototype**
```C
zuint32 (* int_data)(void *context);
```

**Parameters**
* `context` → A pointer to the calling emulator instance.

**Return value**
A 32-bit value containing the bytes of an instruction. The instruction must begin at the most significant byte of the value.

### `halt`
Called when the CPU enters or exits the halt state.

**Prototype**
```C
void (* halt)(void *context, zboolean state);
```

**Parameters**
* `context` → A pointer to the calling emulator instance.
* `state` →  `TRUE` if halted, `FALSE` otherwise.
