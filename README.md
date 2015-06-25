# Zilog Z80 CPU Emulator
Copyright © 1999-2015 Manuel Sainz de Baranda y Goñi.  
Released under the terms of the [GNU General Public License v3](http://www.gnu.org/copyleft/gpl.html).

This is a very accurate Z80 emulator I developed many years ago. I did not test it nor use it until recently. Some friends of mine have used it in their computer and console emulators.

In 2013 I thought it was a shame to let this code untested so I replaced the FBZX emulator's core of [Sergio Costas Rodríguez](http://www.rastersoft.com/fbzx.html) with this and it worked great without any error!!! As a result of this I started to scream and ran out of the house like possesed shaking my arms up. After this I ate two tablets of chocolate and drank a glass of natural orange juice to celebrate that this long forgotten masterpiece of programming was born pure, without original sin.

Then I used it in the [ZX Spectrum](http://github.com/redcode/mZX) emulator I started as hobby.

I will provide instructions to use this emulator in the future, but I think any decent C programmer can understand how to use it in its project.

This emulator is very accurate, very fast (there are others faster written in assembly, yes) and what it's the most important thing of all IMHO: Its structure is very clear, elegant and it is small. This is not the typical emulator which generates tables in runtime nor expands a lot of macros to create one function or piece of code per every instruction precise case.

In order to compile you must know that it uses my [Q API](http://github.com/redcode/Q), which is header only and provides types, macros, inline functions, and a lot of facilities to detect the particularities of the compiler and the target machine. The included Xcode project expects to find Q in "/usr/local/include/Q". This is a header only dependency. The emulator doesn't depend on anything.

If you want precise details about how the code works, you want to use this emulator in your project (privative software projects too), or you have any doubt about it, please, send me an email.
