===========
Integration
===========

.. only:: html

	.. |br| raw:: html

		<br />

.. only:: latex

	.. |nl| raw:: latex

		\newline

As an external dependency in CMake-based projects
-------------------------------------------------

The Z80 library includes a `config-file package <https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages>`_ for integration into CMake-based projects, which should be installed on development environments. As usual, just call ``find_package`` to find the library. This creates the imported ``Z80`` target carrying the necessary transitive link dependencies. The linking method can optionally be selected by specifying the ``Shared`` or ``Static`` component of the ``Z80`` package.

Example:

.. code-block:: cmake

	find_package(Z80 REQUIRED [Shared|Static])
	target_link_libraries(your-target Z80)

When not specified as a component, the linking method is selected according to ``Z80_SHARED_LIBS``. If this option is not defined, the config-file package uses the type of library that is installed on the system and, if it finds both the shared and the static versions, ``BUILD_SHARED_LIBS`` determines which one to link against.

As a CMake subproject
---------------------

To embed the library as a CMake subproject, just place its entire source tree into a subdirectory of your project.

It is advisable to configure the library in the ``CMakeLists.txt`` of your project. This will prevent the user from having to specify configuration options of the Z80 subproject through the CMake command line when building the main project. As noted in the Installation section of this document, all package-specific options are prefixed with ``Z80_``, so, in a normal scenario, there should be no risk of name collision with the options and variables of the parent project.

Example:

.. code-block:: cmake

	set(Z80_SHARED_LIBS                 NO  CACHE BOOL "")
	set(Z80_WITH_Q                      YES CACHE BOOL "")
	set(Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG YES CACHE BOOL "")

	add_subdirectory(dependencies/Z80)
	target_link_libraries(your-target Z80)

It is important to set the ``Z80_SHARED_LIBS`` option. Otherwise CMake will build the library type indicated by ``BUILD_SHARED_LIBS``, which may not be the desired one.

Manual integration
------------------

There are several macros that can be used to configure the source code of the library. You can define those you need in your build system or at the beginning of the ``Z80.c`` file. The following ones allow you to configure the integration of ``Z80.h`` and ``Z80.c`` into the project:

.. c:macro:: Z80_DEPENDENCIES_HEADER

	Specifies the only external header to ``#include``, replacing those of Zeta. |br| |nl|
	If used, it must also be defined before including the ``Z80.h`` header.

.. c:macro:: Z80_STATIC

	Needed for compiling and/or using the emulator as a static library or as an internal part of other project. |br| |nl|
	If used, it must also be defined before including the ``Z80.h`` header.

.. c:macro:: Z80_WITH_LOCAL_HEADER

	Tells ``Z80.c`` to ``#include Z80.h`` instead of ``<Z80.h>``.

The second group of package-specific options, explained in the "Installation" section of this document, activates various optional implementations in the source code by predefining the following macros:

.. c:macro:: Z80_WITH_EXECUTE
.. c:macro:: Z80_WITH_FULL_IM0
.. c:macro:: Z80_WITH_Q
.. c:macro:: Z80_WITH_RESET_SIGNAL
.. c:macro:: Z80_WITH_SPECIAL_RESET_SIGNAL
.. c:macro:: Z80_WITH_UNOFFICIAL_RETI
.. c:macro:: Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG

Except for ``Z80_DEPENDENCIES_HEADER``, the above macros do not need to be set to a particular token when used, as the source code only checks whether or not they are defined.
