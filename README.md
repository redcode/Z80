# Zilog Z80 CPU Emulator
Copyright © 1999-2016 Manuel Sainz de Baranda y Goñi.  
Released under the terms of the [GNU General Public License v3](http://www.gnu.org/copyleft/gpl.html).

This is a very accurate Z80 emulator I wrote many years ago. It has been used in several machine emulators by other people and has been extensivelly tested. It is very fast and small (although there are faster ones written in assembly), its structure is very clear and the code is commented.

If you are looking for an accurate Zilog Z80 CPU emulator for your project maybe you have found the correct one. I use this core in the [ZX Spectrum](http://github.com/redcode/mZX) emulator I started as hobby.

In order to compile you must install [Z](http://github.com/redcode/Z), its only dependency, a **header only** library which provides types, macros, inline functions, and a lot of facilities to detect the particularities of the compiler and the target system.

### API

```
void z80_power(Z80 *object, zboolean state)
```
**Parameters:**   
```object```: A pointer to an emulator instance.   
```state```: ```ON``` / ```OFF```   
**Return value**:   
none   
**Description:**   
Switchs the core power status.   
   
```
zsize z80_run(Z80 *object, zsize cycles)
```
**Return value**:   
The actual number of cycles executed.   
**Description:**   
Runs the core for the given amount of ```cycles```.   
**Discusion:**   
Given the fact that one Z80 instruction needs between 4 and 23 cycles to be executed, it is not always possible to run the core the exact number of cycles specfified.   


### History

* __[v1.0.0](http://github.com/Z80/releases/tag/v1.0.0)__ _(2016-05-01)_
    * Initial release.
