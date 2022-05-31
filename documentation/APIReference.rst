=============
API reference
=============

Library version macros
----------------------

.. doxygendefine:: Z80_LIBRARY_VERSION_MAJOR
.. doxygendefine:: Z80_LIBRARY_VERSION_MINOR
.. doxygendefine:: Z80_LIBRARY_VERSION_MICRO
.. doxygendefine:: Z80_LIBRARY_VERSION_STRING

Types
-----

.. doxygentypedef:: Z80Read
.. doxygentypedef:: Z80Write
.. doxygentypedef:: Z80HALT
.. doxygentypedef:: Z80Notify
.. doxygentypedef:: Z80Illegal
.. doxygentypedef:: Z80Reset
.. doxygenstruct:: Z80
   :members:

Configuration options
---------------------

.. doxygendefine:: Z80_OPTION_HALT_SKIP
.. doxygendefine:: Z80_OPTION_LD_A_IR_BUG
.. doxygendefine:: Z80_OPTION_OUT_VC_255
.. doxygendefine:: Z80_OPTION_XQ
.. doxygendefine:: Z80_OPTION_YQ
.. doxygendefine:: Z80_MODEL_ZILOG_NMOS
.. doxygendefine:: Z80_MODEL_ZILOG_CMOS
.. doxygendefine:: Z80_MODEL_NEC_NMOS
.. doxygendefine:: Z80_MODEL_ST_CMOS

Flag bitmasks
-------------

.. doxygendefine:: Z80_SF
.. doxygendefine:: Z80_ZF
.. doxygendefine:: Z80_YF
.. doxygendefine:: Z80_HF
.. doxygendefine:: Z80_XF
.. doxygendefine:: Z80_PF
.. doxygendefine:: Z80_NF
.. doxygendefine:: Z80_CF

Register accessors
------------------

.. doxygendefine:: Z80_MEMPTR
.. doxygendefine:: Z80_PC
.. doxygendefine:: Z80_SP
.. doxygendefine:: Z80_XY
.. doxygendefine:: Z80_IX
.. doxygendefine:: Z80_IY
.. doxygendefine:: Z80_AF
.. doxygendefine:: Z80_BC
.. doxygendefine:: Z80_DE
.. doxygendefine:: Z80_HL
.. doxygendefine:: Z80_AF_
.. doxygendefine:: Z80_BC_
.. doxygendefine:: Z80_DE_
.. doxygendefine:: Z80_HL_
.. doxygendefine:: Z80_MEMPTRH
.. doxygendefine:: Z80_MEMPTRL
.. doxygendefine:: Z80_PCH
.. doxygendefine:: Z80_PCL
.. doxygendefine:: Z80_SPH
.. doxygendefine:: Z80_SPL
.. doxygendefine:: Z80_XYH
.. doxygendefine:: Z80_XYL
.. doxygendefine:: Z80_IXH
.. doxygendefine:: Z80_IXL
.. doxygendefine:: Z80_IYH
.. doxygendefine:: Z80_IYL
.. doxygendefine:: Z80_A
.. doxygendefine:: Z80_F
.. doxygendefine:: Z80_B
.. doxygendefine:: Z80_C
.. doxygendefine:: Z80_D
.. doxygendefine:: Z80_E
.. doxygendefine:: Z80_H
.. doxygendefine:: Z80_L
.. doxygendefine:: Z80_A_
.. doxygendefine:: Z80_F_
.. doxygendefine:: Z80_B_
.. doxygendefine:: Z80_C_
.. doxygendefine:: Z80_D_
.. doxygendefine:: Z80_E_
.. doxygendefine:: Z80_H_
.. doxygendefine:: Z80_L_

Limits
------

.. doxygendefine:: Z80_CYCLE_LIMIT

Hooking
-------

.. doxygendefine:: Z80_HOOK


Functions
---------

.. doxygenfunction:: z80_in_cycle
.. doxygenfunction:: z80_instant_reset
.. doxygenfunction:: z80_out_cycle
.. doxygenfunction:: z80_execute
.. doxygenfunction:: z80_int
.. doxygenfunction:: z80_nmi
.. doxygenfunction:: z80_power
.. doxygenfunction:: z80_refresh_address
.. doxygenfunction:: z80_reset
.. doxygenfunction:: z80_run
.. doxygenfunction:: z80_special_reset
