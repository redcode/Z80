===============
Version history
===============

Z80 v0.2-pre
============

*Released on 2024-09-21*

This is an important update that addresses a number of issues and also includes new features. Please note that the changes introduced in this release break the binary compatibility with the previous version.

**Changes:**

1. Changed the :doc:`license <license>` from GPL to LGPL (by popular request).
2. Moved the public header from ``<emulation/CPU/Z80.h>`` to ``<Z80.h>``.
3. Removed the Xcode project.
4. Switched the build system from Premake to `CMake <https://cmake.org>`_.
5. Switched to `Zeta <https://zeta.st>`_ v0.1.
6. Added `pkg-config <https://www.freedesktop.org/wiki/Software/pkg-config>`_ support.
7. Added the following files to the project: :file:`.vimrc`, :file:`CITATION.cff` and :file:`THANKS`.
8. Added detailed documentation.
9. Added Pascal binding, courtesy of Zoran Vučenović.
10. Added tests.
11. Renamed the macros ``CPU_Z80_DEPENDENCIES_H`` and ``CPU_Z80_STATIC`` to :c:macro:`Z80_EXTERNAL_HEADER` and :c:macro:`Z80_STATIC`, respectively.
12. Added :ref:`public macros <api-reference:Macros>` for checking the library version, working with flags, accessing the 16-bit registers and other purposes.
13. Added the ability to end the emulation loop immediately, and the :c:func:`z80_break` function.
14. Added the :c:func:`z80_execute` function for running a simplified emulation without RESET and interrupts.
15. Added the :c:func:`z80_in_cycle` and :c:func:`z80_out_cycle` functions for obtaining the clock cycle at which the I/O M-cycle begins, relative to the start of the instruction.
16. Added the :c:func:`z80_refresh_address` function for getting the refresh address of the current M1 cycle.
17. Renamed the ``z80_reset`` function to :c:func:`z80_instant_reset`.
18. Added optional emulation of the special RESET, and the :c:func:`z80_special_reset` function.
19. Added the :c:data:`Z80::fetch_opcode<Z80.fetch_opcode>` and :c:data:`Z80::fetch<Z80.fetch>` callbacks for performing, respectively, opcode fetch operations and memory read operations on instruction data.
20. Added the :c:data:`Z80::nop<Z80.nop>` callback for performing disregarded opcode fetch operations during internal NOP M-cycles.
21. Added emulation of the NMI acknowledge M-cycle through the new :c:data:`Z80::nmia<Z80.nmia>` callback.
22. Added emulation of the INT acknowledge M-cycle through the new :c:data:`Z80::inta<Z80.inta>` callback, which replaces ``Z80::int_data``.
23. Added optional full emulation of the interrupt mode 0, along with the new :c:data:`Z80::int_fetch<Z80.int_fetch>` callback for performing bus read operations on instruction data. If not enabled at compile-time, the old simplified emulation is built, which supports only the most typical instructions.
24. Added four callbacks for notifying the execution of important instructions: :c:data:`Z80::ld_i_a<Z80.ld_i_a>`, :c:data:`Z80::ld_r_a<Z80.ld_r_a>`, :c:data:`Z80::reti<Z80.reti>` and :c:data:`Z80::retn<Z80.retn>`.
25. Added hooking functionality through the ``ld h,h`` instruction and the new :c:data:`Z80::hook<Z80.hook>` callback.
26. Added the :c:data:`Z80::illegal<Z80.illegal>` callback for delegating the emulation of illegal instructions.
27. Added :ref:`emulation options <api-reference:Configuration>` that can be configured at runtime.
28. Removed ``Z80::state``. Replaced with individual members for the registers, the interrupt enable flip-flops and the interrupt mode.
29. Removed the superfluous EI flag. The previous opcode is checked instead, which is faster and makes the :c:type:`Z80` object smaller.
30. Removed all module-related stuff.
31. Added emulation of the additional flag changes performed during the extra 5 clock cycles of the following instructions: ``ldir``, ``lddr``, ``cpir``, ``cpdr``, ``inir``, ``indr``, ``otir`` and ``otdr``.
32. Added emulation of the interrupt acceptance deferral that occurs during the ``reti`` and ``retn`` instructions.
33. Added MEMPTR emulation. The ``bit N,(hl)`` instruction now produces a correct value of F.
34. Added optional emulation of Q. If enabled at compile-time, the ``ccf`` and ``scf`` instructions will produce a correct value of F.
35. Added emulation of the ``out (c),255`` instruction (Zilog Z80 CMOS).
36. Added optional emulation of the bug affecting the ``ld a,{i|r}`` instructions (Zilog Z80 NMOS). If enabled at compile-time and configured at runtime, the P/V flag will be reset if an INT is accepted during the execution of these instructions.
37. Added an optional implementation of the parity calculation for the P/V flag that performs the actual computation instead of using a table of precomputed values.
38. Added an optional implementation of the ``daa`` instruction that uses a table of precomputed values.
39. Increased granularity. The emulator can now stop directly after fetching a prefix ``DDh`` or ``FDh`` if it runs out of clock cycles. This also works during the INT response in mode 0.
40. Reimplemented the HALT state. The emulation should now be fully accurate. HALTskip optimization is also supported.
41. Fixed a bug in the ``sll`` instruction.
42. Fixed a bug in the ``INX`` and ``OUTX`` macros affecting the S and N flags.
43. Fixed a bug in the ``OUTX`` macro affecting the MSByte of the port number.
44. Fixed the clock cycles of the ``dec XY`` and ``in (c)`` instructions.
45. Fixed the ``read_16`` function so that the order of the memory read operations is not determined by the order in which the compiler evaluates expressions.
46. Fixed the order in which the memory write operations are performed when the SP register is involved. This affects the NMI response, the INT response in modes 1 and 2, and the following instructions: ``ex (sp),{hl|XY}``, ``push TT``, ``push XY``, ``call WORD``, ``call Z,WORD`` and ``rst N``.
47. Fixed the handling of illegal instructions to avoid stack overflows in long sequences of ``DDh/FDh`` prefixes.
48. Fixed several implicit conversions to avoid warnings about loss of sign and precision.
49. Fixed some bitwise operations to avoid undefined behavior and arithmetic right shifts on signed integers.
50. Fixed violations of the C standard in several identifiers.
51. Renamed the 8-bit register lists: ``X/Y`` to ``J/K``; ``J/K`` and ``P/Q`` to ``O/P``.
52. Replaced all P/V overflow computation functions with a single, faster macro.
53. Replaced all register resolution functions with macros.
54. Replaced all ``ld {J,K|O,P}`` instructions that have the same destination and source register with NOPs. In addition, the "illegal" forms of the following instructions are now executed without using the illegal instruction handler: ``ld O,P``, ``ld O,BYTE``, ``U [a,]P`` and ``V O``.
55. Optimizations in flag computation and condition evaluation.
56. New source code comments and improvements to existing ones.
57. Improved code aesthetics.
58. Other improvements, optimizations and minor changes.

Z80 v0.1
========

*Released on 2018-11-10*

Initial public release.
