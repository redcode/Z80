# Zilog Z80 CPU Emulator
Copyright © 1999-2016 Manuel Sainz de Baranda y Goñi.  
Released under the terms of the [GNU General Public License v3](http://www.gnu.org/copyleft/gpl.html).

This is a very accurate [Z80](http://en.wikipedia.org/wiki/Zilog_Z80) [emulator](http://en.wikipedia.org/wiki/Emulator) I wrote many years ago. It has been used in several machine emulators by other people and has been extensivelly tested. It is very fast and small (although there are faster ones written in assembly), its structure is very clear and the code is commented.

If you are looking for an accurate Zilog Z80 CPU emulator for your project maybe you have found the correct one. I use this core in the [ZX Spectrum emulator](http://github.com/redcode/mZX) I started as hobby.


## Building

In order to compile you must install [Z](http://github.com/redcode/Z), a **header only** library which provides types, macros, inline functions, and a lot of facilities to detect the particularities of the compiler and the target system. This is the only dependency, the standard C library and its headers are not used and the emulator doesn't need to be dynamically linked with any library.

A Xcode project file is provided to build the emulator. It has the following targets:

#### Targets

Name | Description
--- | ---
dynamic | Shared library.
module  | Module for A.C.M.E.
static | Static library.
static+ABI | Static library with a descriptive ABI to be used in modular multi-machine emulators. Building the ABI implies callback pointers with slots enabled.
static+ABI-API | The same as above but exposing only the symbol of the ABI, the API functions will be private.

#### Constants used in Z80.h

Name | Description
--- | ---
CPU_Z80_STATIC | You need to define this if you are using the emulator as a static library or if you have added its sources to your project.
CPU_Z80_USE_SLOTS | Always needed when using the ABI. Each callback function will have its own context pointer, which should store the address of the object assigned to the call.

#### Constants used in Z80.c
Name | Description
--- | ---
CPU_Z80_DYNAMIC | Needed to build a shared library. Exports the public symbols.
CPU_Z80_BUILD_ABI | Builds the ABI of type `ZCPUEmulatorABI` declared in the header with the identifier `abi_emulation_cpu_z80`.
CPU_Z80_BUILD_MODULE_ABI | Builds a generic module ABI of type `ZModuleABI`. This constant enables `CPU_Z80_BUILD_ABI` automatically so `abi_emulation_cpu_z80` will be build too. This option is intended to be used when building a true module loadable at runtime with `dlopen()`, `LoadLibrary()` or similar, and can be accessed retrieving the **weak** symbol `__module_abi__`.
CPU_Z80_HIDE_API | 
CPU_Z80_HIDE_ABI | 


## API

### `z80_run`

***Description***  
Runs the core for the given amount of ```cycles```.   

***Declaration***  
```C
zsize z80_run(Z80 *object, zsize cycles);
```

***Parameters***  

Name | Description
--- | ---
object | A pointer to an emulator instance.
cycles | The number of cycles to be executed.

***Return value***  
The actual number of cycles executed.   

***Discusion***  
Given the fact that one Z80 instruction needs between 4 and 23 cycles to be executed, it is not always possible to run the core the exact number of cycles specfified.   

### `z80_power`

***Description***  
Switchs the core power status.   

***Declaration***  
```C
void z80_power(Z80 *object, zboolean state);
```
***Parameters***  

Name | Description
--- | ---
object | A pointer to an emulator instance.
state  | `ON` / `OFF`

***Return value***  
none.   

### `z80_reset`

***Description***  
Resets the core by reinitializing its variables and sets its registers to the state they would be in a real Z80 CPU after a pulse in the `RESET` line.   

***Declaration***
```C
void z80_reset(Z80 *object);
```
***Parameters***  

Name | Description
--- | ---
object | A pointer to an emulator instance.

***Return value***  
none.   

### `z80_nmi`

***Description***  
Performs a non-maskable interrupt. This is equivalent to a pulse in the `NMI` line of a real Z80 CPU.   

***Declaration***  
```C
void z80_nmi(Z80 *object);
```

***Parameters***  

Name | Description
--- | ---
object | A pointer to an emulator instance.

***Return value***  
none.   


### `z80_int`

***Description***  
Switchs the state of the maskable interrupt. This is equivalent to a change in the `INT` line of a real Z80 CPU.   

***Declaration***  
```C
void z80_int(Z80 *object, zboolean state);
```

***Parameters***  

Name | Description
--- | ---
object | A pointer to an emulator instance.
state  | `ON` = set line high, `OFF` = set line low

***Return value***  
none.   


## History

* __[v1.0.0](http://github.com/Z80/releases/tag/v1.0.0)__ _(2016-05-02)_
    * Initial release.
