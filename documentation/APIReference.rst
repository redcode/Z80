=============
API reference
=============

Macros
------

.. doxygendefine:: Z80_LIBRARY_VERSION_MAJOR
.. doxygendefine:: Z80_LIBRARY_VERSION_MINOR
.. doxygendefine:: Z80_LIBRARY_VERSION_MICRO
.. doxygendefine:: Z80_LIBRARY_VERSION_STRING
.. doxygendefine:: Z80_CYCLE_LIMIT
.. doxygendefine:: Z80_HOOK

.. doxygendefine:: Z80_MODEL_ZILOG_NMOS
.. doxygendefine:: Z80_MODEL_ZILOG_CMOS

.. doxygendefine:: Z80_REQUEST_SPECIAL_RESET
.. doxygendefine:: Z80_REQUEST_RESET
.. doxygendefine:: Z80_REQUEST_NMI
.. doxygendefine:: Z80_REQUEST_INT

.. doxygendefine:: Z80_RESUME_HALT
.. doxygendefine:: Z80_RESUME_XY
.. doxygendefine:: Z80_RESUME_IM0_XY

Callback Types
--------------

.. doxygentypedef:: Z80Read
.. doxygentypedef:: Z80Write
.. doxygentypedef:: Z80HALT
.. doxygentypedef:: Z80Notify

Objects
-------

.. doxygenstruct:: Z80
	:members:

Functions
---------

.. doxygenfunction:: z80_in_offset
.. doxygenfunction:: z80_instant_reset
.. doxygenfunction:: z80_out_offset
.. doxygenfunction:: z80_execute
.. doxygenfunction:: z80_int
.. doxygenfunction:: z80_nmi
.. doxygenfunction:: z80_normal_reset
.. doxygenfunction:: z80_power
.. doxygenfunction:: z80_refresh_address
.. doxygenfunction:: z80_run
.. doxygenfunction:: z80_special_reset
