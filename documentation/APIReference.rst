=============
API reference
=============

.. code-block:: c

   #include <Z80.h>

Types
=====

Callback pointers
-----------------

.. doxygentypedef:: Z80Read
.. doxygentypedef:: Z80Write
.. doxygentypedef:: Z80Halt
.. doxygentypedef:: Z80Notify
.. doxygentypedef:: Z80Illegal

Emulator object
---------------

.. doxygenstruct:: Z80
   :members:

Functions
=========

Powering the CPU
----------------

.. doxygenfunction:: z80_power

Running the emulation
---------------------

.. doxygenfunction:: z80_execute
.. doxygenfunction:: z80_run

Requesting interrupts
---------------------

.. doxygenfunction:: z80_int
.. doxygenfunction:: z80_nmi
.. doxygenfunction:: z80_instant_reset
.. doxygenfunction:: z80_special_reset

Obtaining information
---------------------

.. doxygenfunction:: z80_in_cycle
.. doxygenfunction:: z80_out_cycle
.. doxygenfunction:: z80_r
.. doxygenfunction:: z80_refresh_address

Macros
======

Library version
---------------

.. doxygendefine:: Z80_LIBRARY_VERSION_MAJOR
.. doxygendefine:: Z80_LIBRARY_VERSION_MINOR
.. doxygendefine:: Z80_LIBRARY_VERSION_MICRO
.. doxygendefine:: Z80_LIBRARY_VERSION_STRING

Limits
------

.. doxygendefine:: Z80_MAXIMUM_CYCLES
.. doxygendefine:: Z80_MAXIMUM_CYCLES_PER_STEP

Opcodes
-------

.. doxygendefine:: Z80_HOOK

Flags
-----

.. doxygendefine:: Z80_SF
.. doxygendefine:: Z80_ZF
.. doxygendefine:: Z80_YF
.. doxygendefine:: Z80_HF
.. doxygendefine:: Z80_XF
.. doxygendefine:: Z80_PF
.. doxygendefine:: Z80_NF
.. doxygendefine:: Z80_CF

Configuration
-------------

.. doxygendefine:: Z80_OPTION_HALT_SKIP
.. doxygendefine:: Z80_OPTION_IM0_RETX_NOTIFICATIONS
.. doxygendefine:: Z80_OPTION_LD_A_IR_BUG
.. doxygendefine:: Z80_OPTION_OUT_VC_255
.. doxygendefine:: Z80_OPTION_XQ
.. doxygendefine:: Z80_OPTION_YQ
.. doxygendefine:: Z80_MODEL_ZILOG_NMOS
.. doxygendefine:: Z80_MODEL_ZILOG_CMOS
.. doxygendefine:: Z80_MODEL_NEC_NMOS
.. doxygendefine:: Z80_MODEL_ST_CMOS

Requests
--------

.. doxygendefine:: Z80_REQUEST_INT
.. doxygendefine:: Z80_REQUEST_NMI
.. doxygendefine:: Z80_REQUEST_REJECT_NMI
.. doxygendefine:: Z80_REQUEST_SPECIAL_RESET

Resume codes
------------

Sometimes the emulator runs out of clock cycles while performing long tasks that can exceed :c:macro:`Z80_MAXIMUM_CYCLES_PER_STEP`. In these cases, the emulation stops and :c:data:`Z80::resume<Z80.resume>` is set to one of the following values:

.. doxygendefine:: Z80_RESUME_HALT
.. doxygendefine:: Z80_RESUME_XY
.. doxygendefine:: Z80_RESUME_IM0_XY

HALT state codes
----------------

.. doxygendefine:: Z80_HALT_EXIT_EARLY
.. doxygendefine:: Z80_HALT_CANCEL

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
