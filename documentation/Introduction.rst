============
Introduction
============

The Z80 library implements a fast, small and accurate `emulator <https://en.wikipedia.org/wiki/Emulator>`_ of the `Zilog Z80 <https://en.wikipedia.org/wiki/Zilog_Z80>`_. It emulates all that is known to date about this CPU, including the undocumented behaviors, `MEMPTR <https://zxpress.ru/zxnet/zxnet.pc/5909>`_, `Q <https://worldofspectrum.org/forums/discussion/41704>`_ and the `special RESET <http://www.primrosebank.net/computers/z80/z80_special_reset.htm>`_. It also has the honor of having been the first open-source project to provide full emulation of the interrupt mode 0.

Limitations
===========

Granularity
-----------

Fetch/execute overlapping
-------------------------

Real Z80 CPUs overlap some operations of certain instructions with the fetch phase of the next instruction or interrupt response. For example, the effective modification of registers A and F. The library does not emulate this feature for speed reasons and only guarantees register coherence at the instruction level, not at the T-state level.

Normal RESET
------------

BUSREQ and WAIT
---------------

Optional features
=================

:c:func:`z80_execute`
---------------------

This function performs a partial emulation of the Z80 CPU that ignores interrupts and the special RESET. It is somewhat faster than :c:func:`z80_run`, since the main emulation loop does not have to check each iteration for pending interrupts before executing the next instruction.

Not enabling the implementation of this function allows a slight reduction in the size of the library, but it is usually marginal.

Package maintainers should enable the implementation of this function.

Interrupt mode 0
----------------

The interrupt mode 0 executes an instruction supplied to the CPU via the data bus. Real Z80 chips can execute any instruction, even illegal ones, and also long sequences of ``DDh/FDh`` prefixes, although most existing hardware only uses a few instructions from the control transfer groups.

The library includes two different implementations of this interrupt mode: full, which emulates everything, and reduced, which emulates only the ``jp WORD``, ``call WORD`` and ``rst N`` instructions.

Enabling the full implementation of the interrupt mode 0 can increase the size of the library by 1 or 2 KB approximately (depending on the target ISA and the executable format).

Package maintainers should enable the full implementation of the interrupt mode 0.

Q
-

Q is an abstraction of certain ALU data related to flag computation. Instructions that modify the flags copy the final value of the F register to Q, while those that do not modify the flags set Q to zero. The state of Q affects the undocumented flags X and Y in the ``ccf`` and ``scf`` instructions. This occurs on Z80 CPUs from Zilog and most other manufacturers.

Enabling the implementation of this feature adds code to update the value of Q in each instruction and interrupt response, which slightly increases the size of the library and, depending on the target ISA and the specific microarchitecture of the host CPU, can result in a performance loss of up to 2.4%.

Package maintainers should however enable the implementation of Q, as it is necessary for the emulator to pass all tests.

Special RESET
-------------

The special RESET is a little-known pseudo interrupt described in U.S. Patent No. `4,486,827 <https://zxe.io/depot/patents/US4486827.pdf>`_. It was first `documented <http://www.primrosebank.net/computers/z80/z80_special_reset.htm>`_ in detail by Tony Brewer. It is not of particular interest for the emulation of commercial systems, as no machine is known to make use of it except for a few development boards such as the Zilog In-Circuit Emulator (ICE).

Enabling its implementation is discouraged because it adds additional operations that may very slightly affect the speed of interrupt responses and the resumption of emulation when the CPU is in the HALT state.

Package maintainers should not enable the implementation of the special RESET.


Notifications of ``reti`` and ``retn`` in the interrupt mode 0
--------------------------------------------------------------

Unofficial ``reti`` opcodes
---------------------------

The ``retn`` instruction has 8 different opcodes: 1 documented, 6 undocumented and ``reti``, which is just the specific ``retn`` opcode whose execution is monitored by the Z80 CTC chip to know whether the CPU is returning from an ISR.

Some Z80 CPU emulators assign the 4 opcodes whose bit 3 is ``0`` to ``retn``, and the other 4 to ``reti``. However, as far as is known, the Z80 CTC and other similar circuits only monitor the execution of the two-byte instruction ``ED4Dh``, (i.e. the one that the official documentation designates as ``reti``).

Enabling unofficial ``reti`` opcodes will configure the ``ED5Dh``, ``ED6Dh`` and ``ED7Dh`` instructions as ``reti``, causing their execution to be notified through the :c:data:`Z80::reti<Z80.reti>` callback rather than :c:data:`Z80::retn<Z80.retn>`.

Package maintainers should never enable unofficial ``reti`` opcodes.

Zilog NMOS bug of the ``ld a,i`` and ``ld a,r`` instructions
------------------------------------------------------------

The Z80 CPU models that use NMOS technology have a bug which causes the P/V flag to be reset when a maskable interrupt is accepted during the execution of these instructions. This affects processors manufactured by Zilog, second sources, NEC and can also be found in CMOS models from SGS-Thomson (and possibly other companies).

Enabling the implementation of this bug adds code inside the maskable interrupt response to check whether or not the previous instruction was ``ld a,i`` or ``ld a,r``, thus making it slightly slower.

Package maintainers should enable the implementation of this bug, as there are firmware and software that depend on it.
