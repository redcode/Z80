![al-tag](https://upload.wikimedia.org/wikipedia/commons/1/19/Zilog_Z80.jpg)

<br>

# Zilog Z80 CPU Emulator
Copyright © 1999-2018 Manuel Sainz de Baranda y Goñi.  
Released under the terms of the [GNU General Public License v3](https://www.gnu.org/copyleft/gpl.html).

This is a very accurate [Z80](https://en.wikipedia.org/wiki/Zilog_Z80) [emulator](http://en.wikipedia.org/wiki/Emulator) I wrote many years ago. It has been used in several machine emulators by other people and it has been extensivelly tested. It's fast and small (33 KB when compiled as a x86-64 dynamic library), its structure is clear and the code is **profusely commented**.

If you are looking for an accurate Zilog Z80 CPU emulator for your project maybe you have found the correct one. I use this core in the [ZX Spectrum emulator](https://github.com/redcode/mZX) I started as hobby.

<br>

## Building

You must first install [Z](https://zeta.st), a **header-only** library that provides types and macros. This is the only dependency, the emulator does not use the standard C library or its headers, nor does it need to be dynamically linked against any library. Then add `Z80.h` and `Z80.c` to your project and configure your build system so that the compiler predefines the `CPU_Z80_STATIC` and `CPU_Z80_USE_LOCAL_HEADER` macros when compiling your sources.

If you preffer to compile the emulator as a library, you can use premake4:
```console
$ cd building
$ premake4 gmake                         # generate Makefile
$ make help                              # list available targets
$ make [config=<configuration>] <target> # build the emulator
```

There is also an Xcode project in `development/Xcode` with several targets:

Target | Description
--- | ---
dynamic | Shared library.
dynamic-module  | Shared library with a generic module ABI to be used in modular multi-machine emulators.
static | Static library.
static-module | Static library with a generic CPU emulator ABI to be used in monolithic multi-machine emulators.

<br>

## Code configuration

There are some predefined macros that control the compilation:

Name | Description
--- | ---
`CPU_Z80_DEPENDENCIES_H` | If defined, `Z80.h` will `#include` only this header as dependency. If you don't want to use Z, you can provide your own header with the types and macros used by the emulator.
`CPU_Z80_BUILD_ABI` | Builds the generic CPU emulator ABI.
`CPU_Z80_BUILD_MODULE_ABI` | Builds the generic module ABI. This macro also enables CPU_Z80_BUILD_ABI, so the generic CPU emulator ABI will be built too. This option is intended to be used when building a true module loadable at runtime with `dlopen()`, `LoadLibrary()` or similar. The ABI module can be accessed via the [weak symbol](https://en.wikipedia.org/wiki/Weak_symbol) `__module_abi__`.
`CPU_Z80_HIDE_ABI` | Makes the generic CPU emulator ABI private.
`CPU_Z80_HIDE_API` | Makes the public functions private.
`CPU_Z80_STATIC` | You need to define this to compile or use the emulator as a static library or if you have added `Z80.h` and `Z80.c` to your project.
`CPU_Z80_USE_ABI` | Tells `Z80.h` to declare the prototype of the CPU emulator ABI.
`CPU_Z80_USE_LOCAL_HEADER` | Use this if you have imported `Z80.h` and `Z80.c` to your project. `Z80.c` will `#include "Z80.h"` instead of `<emulation/CPU/Z80.h>`.

<br>

## API

### The `Z80` emulator instance object

This structure contains the state of the emulated CPU and callback pointers necessary to interconnect the emulator with external logic. There is no constructor function, so, before using an object of this type, some of its members must be initialized, in particular the following: `context`, `read`, `write`, `in`, `out`, `int_data` and `halt`.  

Descriptions of each member follow:  
<br>
```C
zusize cycles;
```
**Description**  
Number of cycles executed in the current call to `z80_run`.  
**Details**  
`z80run` sets this variable to 0 before starting to execute instructions and its value persists after returning. The callbacks can use this variable to know during what cycle they are being called.  
<br>
```C
void *context;
```
**Description**  
The value used as the first argument when calling a callback.  
**Details**  
This variable should be initialized before using the emulator and can be used to reference the context or instance of the machine being emulated.  
<br>
```C
ZZ80State state;
```
**Description**  
CPU registers and internal bits.  
**Details**  
It contains the values of the registers, as well as the interruption flip-flops, variables related to interruptions and other necessary flags. This is what a debugger should use as data source.  
<br>
```C
Z16Bit xy;
```
**Description**  
Temporay IX/IY register for instructions with DDh/FDh prefix.  
**Details**  
Since instructions with prefix DD and FD behave similarly, differing only in the use of register IX or IY, for reasons of size optimization, a single register is used that acts as both. During opcode analysis, the IX or IY register is copied to this variable and, once the instruction emulation is complete, its contents are copied back to the appropriate register.  
<br>
```C
zuint8 r7;
```
**Description**  
Backup of the 7th bit of the R register.  
**Details**  
The value of the R register is incremented as instructions are executed, but its most significant bit remains unchanged. For optimization reasons, this bit is saved at the beginning of the execution of `z80_run` and restored before returning. If an instruction directly affects the R register, this variable is also updated accordingly.  
<br>
```C
Z32Bit data;
```
**Description**  
Temporary storage for opcode fetching.  
**Details**  
This is an internal private variable.  
<br>
```C
zuint8 (* read)(void *context, zuint16 address);
```
**Description**  
Callback: Called when the CPU needs to read 8 bits from memory.  
**Parameters**  
`context` → The value of the member `context`.  
`address` → The memory address to read from.  
**Returns**  
The 8 bits read from memory.  
<br>
```C
void (* write)(void *context, zuint16 address, zuint8 value);
```
**Description**  
Callback: Called when the CPU needs to write 8 bits to memory.  
**Parameters**  
`context` → The value of the member `context`.  
`address` → The memory address to write to.  
`value` → The value to write.
<br>
```C
zuint8 (* in)(void *context, zuint16 port);
```
**Description**  
Callback: Called when the CPU needs to read 8 bits from an I/O port.  
**Parameters**  
`context` → The value of the member `context`.  
`port` → The number of the I/O port to read from.  
**Returns**  
The 8 bits read from the I/O port.  
<br>
```C
void (* out)(void *context, zuint16 port, zuint8 value);
```
**Description**  
Callback: Called when the CPU needs to write 8 bits to an I/O port.  
**Parameters**  
`context` → The value of the member `context`.  
`port` → The number of the I/O port to write to.  
`value` → The value to write.  
<br>
```C
zuint32 (* int_data)(void *context);
```
**Description**  
Callback: Called when the CPU starts executing a maskable interrupt and the interruption mode is 0. This callback must return the instruction that the CPU would read from the data bus in this case.  
**Parameters**  
`context` → The value of the member `context`.  
**Returns**  
A 32-bit value containing the bytes of an instruction. The instruction must begin at the most significant byte (big endian).  
<br>
```C
void (* halt)(void *context, zboolean state);
```
**Description**  
Callback: Called when the CPU enters or exits the halt state.  
**Note**  
This callback is **optional** and must be set to `NULL` if not used.  
**Parameters**  
`context` → The value of the member `context`.  
`state` → `TRUE` if halted; `FALSE` otherwise.
<br><br>

### Functions

```C
void z80_power(Z80 *object, zboolean state);
```
**Description**  
Changes the CPU power status.  
**Parameters**  
`object` → A pointer to a `Z80` emulator instance object.  
`state` → `TRUE` = power ON; `FALSE` = power OFF.  
<br>
```C
void z80_reset(Z80 *object);
```
**Description**  
Resets the CPU by reinitializing its variables and sets its registers to the state they would be in a real Z80 CPU after a pulse in the `RESET` line.  
**Parameters**  
`object` → A pointer to a `Z80` emulator instance object.  
<br>
```C
zusize z80_run(Z80 *object, zusize cycles);
```
**Description**  
Runs the CPU for a given number of `cycles`.  
**Note**  
Given the fact that one Z80 instruction needs between 4 and 23 cycles to be executed, it's not always possible to run the CPU the exact number of `cycles` specfified.  
**Parameters**  
`object` → A pointer to a `Z80` emulator instance object.  
`cycles` → The number of `cycles` to be executed.  
**Returns**  
The number of cycles executed.  
<br>
```C
void z80_nmi(Z80 *object);
```
**Description**  
Performs a non-maskable interrupt.  
**Details**  
This is equivalent to a pulse in the `NMI` line of a real Z80 CPU.  
**Parameters**  
`object` → A pointer to a `Z80` emulator instance object.  
<br>
```C
void z80_int(Z80 *object, zboolean state);
```
**Description**  
Changes the state of the maskable interrupt.  
**Details**  
This is equivalent to a change in the `INT` line of a real Z80 CPU.  
**Parameters**  
`object` → A pointer to a `Z80` emulator instance object.  
`state` → `TRUE` = line high; `FALSE` = line low.  

<br>

## Use in Non-Free / Proprietary Software
This library is released under the terms of the [GNU General Public License v3](https://www.gnu.org/copyleft/gpl.html), but I can license it for non-free projects if you contact [me](mailto:contact@zxe.io?subject=Z80%20non-free%20licensing).
