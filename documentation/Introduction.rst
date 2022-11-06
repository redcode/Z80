============
Introduction
============

The Z80 library implements a fast, small and accurate `emulator <https://en.wikipedia.org/wiki/Emulator>`_ of the `Zilog Z80 <https://en.wikipedia.org/wiki/Zilog_Z80>`_. It emulates all that is known to date about this CPU, including the undocumented behaviors, `MEMPTR <https://zxpress.ru/zxnet/zxnet.pc/5909>`_, `Q <https://worldofspectrum.org/forums/discussion/41704>`_ and the `special RESET <http://www.primrosebank.net/computers/z80/z80_special_reset.htm>`_. It also has the honor of having been the first open source project to provide full emulation of the interrupt mode 0.

Optional implementations
========================

:c:func:`z80_execute`
---------------------

This function performs a partial emulation of the Z80 CPU that ignores interrupts and the special RESET. It is somewhat faster than :c:func:`z80_run`, since the main emulation loop does not have to check each iteration for pending interrupts before executing the next instruction.

Not enabling the implementation of this function allows a slight reduction in the size of the library, but it is usually marginal.

Package maintainers should enable the implementation of this function.

Interrupt mode 0
----------------

Package maintainers should enable the full implementation of the interrupt mode 0.

Q "register"
------------

Q is an abstraction of certain ALU data involved in the computation of the flags. Instructions that do not modify the flags set Q to ``0``, while those that do so copy the final value of the F register to Q. The state of Q affects the undocumented flags X and Y in the ``ccf`` and ``scf`` instructions on Z80 chips from Zilog and most other manufacturers.

Enabling the implementation of this feature adds code to update the value of Q in the emulation of each instruction and interrupt response, which slightly increases the size of the library and, depending on the target ISA and the specific microarchitecture of the host CPU, can result in a performance loss of up to 2.4%.

Package maintainers should however enable the implementation of the Q "register", as it is necessary for the emulator to pass all tests.

Special RESET
-------------

The special RESET is a little-known debug function of the Z80 CPU described in US Patent `4486827 <https://zxe.io/depot/patents/US4486827.pdf>`_. It was first `documented <http://www.primrosebank.net/computers/z80/z80_special_reset.htm>`_ in detail by Tony Brewer. It is not of particular interest for the emulation of commercial systems, as no machine is known to make use of it except for a few development systems such as the Zilog In-Circuit Emulator (ICE).

Enabling its implementation is discouraged because it adds additional operations that may very slightly affect the speed of interrupt responses and the resumption of emulation when the CPU is in the HALT state.

Package maintainers should not enable the implementation of the special RESET.

Unofficial ``reti`` opcodes
---------------------------

The Z80 Counter/Timer Circuit (CTC) detects the two-byte ``ED4Dh`` opcode when it is fetched by the Z80 CPU. This instruction is represented with the ``reti`` assembly mnemonic and is used to return from an ISR and signal that the computer should initialize the daisy-chain enable lines for control of nested priority interrupt handling. However, as far as the CPU is concerned, this instruction is exactly the same as ``retn``.

Package maintainers should not enable unofficial ``reti`` opcodes.

Zilog NMOS bug of the ``ld a,i`` and ``ld a,r`` instructions
------------------------------------------------------------

The Zilog Z80 CPU models that use NMOS technology have a bug which causes the P/V flag to be reset when a maskable interrupt is accepted during the execution of these instructions. This affects processors manufactured by second sources and can also be found in CMOS models from SGS-Thomson (and possibly other companies).

Enabling the implementation of this bug adds code inside the maskable interrupt response to check whether or not the previous instruction was ``la a,i`` or ``ld a,r``, thus making it slightly slower.

Package maintainers should enable the implementation of this bug, as there are firmware and software that depend on it.

Limitations
===========

Granularity
-----------

Fetch/execute overlapping
-------------------------

Normal RESET
------------

BUSREQ and WAIT
---------------
