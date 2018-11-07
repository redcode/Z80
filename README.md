![al-tag](http://upload.wikimedia.org/wikipedia/commons/1/19/Zilog_Z80.jpg)

<br>

# Zilog Z80 CPU Emulator
Copyright © 1999-2018 Manuel Sainz de Baranda y Goñi.  
Released under the terms of the [GNU General Public License v3](http://www.gnu.org/copyleft/gpl.html).

This is a very accurate [Z80](http://en.wikipedia.org/wiki/Zilog_Z80) [emulator](http://en.wikipedia.org/wiki/Emulator) I wrote many years ago. It has been used in several machine emulators by other people and it has been extensivelly tested. It is fast, small (33 KB when compiled as a x86-64 dynamic library), easy to understand, and **the code is profusely commented**.

If you are looking for a Zilog Z80 CPU emulator for your project maybe you have found the correct one. I use this core in the [ZX Spectrum emulator](http://github.com/redcode/MicroZX) I started as hobby.

<br>

## Building

You must first install [Z](http://zeta.st), a **header-only** library that provides types and macros. This is the only dependency, the emulator does not use the C standard library or its headers. Then add `Z80.h` and `Z80.c` to your project and configure its build system so that `CPU_Z80_STATIC` and `CPU_Z80_USE_LOCAL_HEADER` are predefined when compiling the sources.

If you preffer to build the emulator as a library, you can use [premake4](http://premake.github.io):
```console
$ cd building
$ premake4 gmake                         # generate Makefile
$ make help                              # list available targets
$ make [config=<configuration>] [target] # build the emulator
```

There is also an Xcode project in `development/Xcode` with several targets:

Target | Description
--- | ---
Z80 (dynamic) | Shared library.
Z80 (dynamic module)  | Shared library with a generic module ABI to be used in modular multi-machine emulators.
Z80 (static) | Static library.
Z80 (static module) | Static library with a generic CPU emulator ABI to be used in monolithic multi-machine emulators.

<br>

## Code configuration

There are some predefined macros that control the compilation:

Name | Description
--- | ---
`CPU_Z80_DEPENDENCIES_H` | If defined, it replaces the inclusion of any external header with this one. If you don't want to use Z, you can provide your own header with the types and macros used by the emulator.
`CPU_Z80_HIDE_ABI` | Makes the generic CPU emulator ABI private.
`CPU_Z80_HIDE_API` | Makes the public functions private.
`CPU_Z80_STATIC` | You need to define this to compile or use the emulator as a static library or if you have added `Z80.h` and `Z80.c` to your project.
`CPU_Z80_USE_LOCAL_HEADER` | Use this if you have imported `Z80.h` and `Z80.c` to your project. `Z80.c` will `#include "Z80.h"` instead of `<emulation/CPU/Z80.h>`.
`CPU_Z80_WITH_ABI` | Builds the generic CPU emulator ABI and declares its prototype in `Z80.h`.
`CPU_Z80_WITH_MODULE_ABI` | Builds the generic module ABI. This macro also enables `CPU_Z80_WITH_ABI`, so the generic CPU emulator ABI will be built too. This option is intended to be used when building a true module loadable at runtime with `dlopen()`, `LoadLibrary()` or similar. The ABI module can be accessed via the [weak symbol](http://en.wikipedia.org/wiki/Weak_symbol) `__module_abi__`.

<br>

## API: `Z80` emulator instance

This structure contains the state of the emulated CPU and callback pointers necessary to interconnect the emulator with external logic. There is no constructor function, so, before using an object of this type, some of its members must be initialized, in particular the following: `context`, `read`, `write`, `in`, `out`, `int_data` and `halt`.  

```C
zusize cycles;
```
**Description**  
Number of cycles executed in the current call to `z80_run`.  
**Details**  
`z80run` sets this variable to `0` before starting to execute instructions and its value persists after returning. The callbacks can use this variable to know during what cycle they are being called.  

```C
void *context;
```
**Description**  
The value used as the first argument when calling a callback.  
**Details**  
This variable should be initialized before using the emulator and can be used to reference the context/instance of the machine being emulated.  

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

```C
void (* write)(void *context, zuint16 address, zuint8 value);
```
**Description**  
Callback: Called when the CPU needs to write 8 bits to memory.  
**Parameters**  
`context` → The value of the member `context`.  
`address` → The memory address to write to.  
`value` → The value to write.  

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

```C
void (* out)(void *context, zuint16 port, zuint8 value);
```
**Description**  
Callback: Called when the CPU needs to write 8 bits to an I/O port.  
**Parameters**  
`context` → The value of the member `context`.  
`port` → The number of the I/O port to write to.  
`value` → The value to write.  

```C
zuint32 (* int_data)(void *context);
```
**Description**  
Callback: Called when the CPU needs to read one instruction from the data bus to service a maskable interrupt (`INT`) in mode 0.  
**Parameters**  
`context` → The value of the member `context`.  
**Returns**  
A 32-bit value containing the bytes of an instruction. The instruction must begin at the most significant byte (big endian).  

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

```C
ZZ80State state;
```
**Description**  
CPU registers and internal bits.  
**Details**  
It contains the state of the registers, as well as the interrupt flip-flops, variables related to interrupts and other necessary flags. This is what a debugger should use as data source.  

```C
zuint8 r7;
```
**Description**  
Backup of the 7th bit of the R register.  
**Details**  
The value of the R register is incremented as instructions are executed, but its most significant bit remains unchanged. For optimization reasons, this bit is saved at the beginning of the execution of `z80_run` and restored before returning. If an instruction directly affects the R register, this variable is also updated accordingly.  

```C
Z16Bit xy;
```
**Description**  
Temporay IX/IY register for instructions with `DDh`/`FDh` prefix.  
**Details**  
Since instructions with prefix `DDh` and `FDh` behave similarly, differing only in the use of register IX or IY, for reasons of size optimization, a single register is used that acts as both. During opcode analysis, the IX or IY register is copied to this variable and, once the instruction emulation is complete, its contents are copied back to the appropriate register.  

```C
Z32Bit data;
```
**Description**  
Temporary storage for opcode fetching.  
**Details**  
This is an internal private variable.  

<br>

## API: Public Functions

```C
void z80_power(Z80 *object, zboolean state);
```
**Description**  
Changes the CPU power status.  
**Parameters**  
`object` → A pointer to a Z80 emulator instance.  
`state` → `TRUE` = power ON; `FALSE` = power OFF.  

```C
void z80_reset(Z80 *object);
```
**Description**  
Resets the CPU.  
**Details**  
This is equivalent to a pulse on the `RESET` line of a real Z80.  
**Parameters**  
`object` → A pointer to a Z80 emulator instance.  

```C
zusize z80_run(Z80 *object, zusize cycles);
```
**Description**  
Runs the CPU for a given number of `cycles`.  
**Note**  
Given the fact that one Z80 instruction needs between 4 and 23 cycles to be executed, it's not always possible to run the CPU the exact number of `cycles` specfified.  
**Parameters**  
`object` → A pointer to a Z80 emulator instance.  
`cycles` → The number of cycles to be executed.  
**Returns**  
The number of cycles executed.  

```C
void z80_nmi(Z80 *object);
```
**Description**  
Performs a non-maskable interrupt (NMI).  
**Details**  
This is equivalent to a pulse on the `NMI` line of a real Z80.  
**Parameters**  
`object` → A pointer to a Z80 emulator instance.  

```C
void z80_int(Z80 *object, zboolean state);
```
**Description**  
Changes the state of the maskable interrupt (INT).  
**Details**  
This is equivalent to a change on the `INT` line of a real Z80.  
**Parameters**  
`object` → A pointer to a Z80 emulator instance.  
`state` → `TRUE` = line high; `FALSE` = line low.  

<br>

## Use in Proprietary Software
This library is released under the terms of the [GNU General Public License v3](http://www.gnu.org/copyleft/gpl.html), but I can license it for non-free/propietary projects if you contact [me](mailto:contact@zxe.io?subject=Z80%20non-free%20licensing).
