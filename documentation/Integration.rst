===========
Integration
===========

.. |BUILD_SHARED_LIBS| replace:: ``BUILD_SHARED_LIBS``
.. _BUILD_SHARED_LIBS: https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html

.. |find_package| replace:: ``find_package``
.. _find_package: https://cmake.org/cmake/help/latest/command/find_package.html

.. |add_subdirectory| replace:: ``add_subdirectory``
.. _add_subdirectory: https://cmake.org/cmake/help/latest/command/add_subdirectory.html

As an external dependency in CMake-based projects
=================================================

The Z80 library includes a `config-file package <https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages>`_ for integration into CMake-based projects that must be installed for development. Use |find_package|_ to find the ``Z80`` package. This creates the ``Z80`` imported library target, which carries the necessary transitive link dependencies. Optionally, the linking method can be selected by specifying either the ``Shared`` or ``Static`` component.

Example:

.. code-block:: cmake

   find_package(Z80 REQUIRED Shared)
   target_link_libraries(your-target Z80)

When not specified as a component, the linking method is selected according to :option:`Z80_SHARED_LIBS<-DZ80_SHARED_LIBS>`. If this option is not defined, the config-file uses the type of library that is installed on the system and, if it finds both the shared and the static versions, |BUILD_SHARED_LIBS|_ determines which one to link against.

As a CMake subproject
=====================

To embed the Z80 library as a CMake subproject, extract the source code packages of `Zeta <https://zeta.st/download>`__ and `Z80 <https://zxe.io/software/Z80/download>`_ (or clone their respective repositories) into a subdirectory of another project. Then use |add_subdirectory|_ in the parent project to add the Z80 source code tree to the build process (N.B., the Z80 subproject will automatically find Zeta and import it as an `interface library <https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#interface-libraries>`_).

It is advisable to configure the Z80 library in the :file:`CMakeLists.txt` of the parent project. This will prevent the user from having to specify :ref:`configuration options for the Z80 subproject <cmake_package_options>` through the command line when building the main project.

Example:

.. code-block:: cmake

   set(Z80_SHARED_LIBS                 NO  CACHE BOOL "")
   set(Z80_WITH_Q                      YES CACHE BOOL "")
   set(Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG YES CACHE BOOL "")

   add_subdirectory(dependencies/Z80)
   target_link_libraries(your-target Z80)

It is important to set the :option:`Z80_SHARED_LIBS<-DZ80_SHARED_LIBS>` option. Otherwise, CMake will build the library type indicated by |BUILD_SHARED_LIBS|_, which may not be the desired one.

Integrating the source code
===========================

There are several macros that can be used to configure the source code of the library. You can define those you need in your build system or at the beginning of the :file:`Z80.c` file. The following ones allow you to configure the integration of :file:`Z80.h` and :file:`Z80.c` into the project:

.. c:macro:: Z80_DEPENDENCIES_HEADER

   Specifies the only external header to ``#include``, replacing those of `Zeta <https://github.com/redcode/Zeta>`__. This header must define the following:

   * Macros: ``Z_API_EXPORT``, ``Z_API_IMPORT``, ``Z_EMPTY``, ``Z_EXTERN_C_BEGIN``, ``Z_EXTERN_C_END``, ``Z_INLINE``, ``Z_MEMBER_OFFSET``, ``Z_NULL``, ``Z_UINT8_ROTATE_LEFT``, ``Z_UINT8_ROTATE_RIGHT``, ``Z_UINT16``, ``Z_UINT16_BIG_ENDIAN``, ``Z_UINT32``, ``Z_UINT32_BIG_ENDIAN``, ``Z_UNUSED`` and ``Z_USIZE``.

   * Types: ``zboolean``, ``zsint``, ``zsint8``, ``zuint``, ``zuint8``, ``zuint16``, ``zuint32``, ``zusize``, ``ZInt16`` and ``ZInt32``.

   If you compile :file:`Z80.c` with this macro defined, you must also define it before including ``"Z80.h"`` or ``<Z80.h>``.

.. c:macro:: Z80_STATIC

   Required to compile and/or use the emulator as a static library or as an internal part of another project. If you compile :file:`Z80.c` with this macro defined, you must also define it before including ``"Z80.h"`` or ``<Z80.h>``.

.. c:macro:: Z80_WITH_LOCAL_HEADER

   Tells :file:`Z80.c` to ``#include "Z80.h"`` instead of ``<Z80.h>``.

:ref:`The second group of package-specific options <cmake_source_code_options>`, explained in the :doc:`Installation` section of this document, activates various :ref:`optional features <Introduction:Optional features>` in the source code by predefining the following macros:

.. c:macro:: Z80_WITH_EXECUTE

   Enables the implementation of the :c:func:`z80_execute` function.

.. c:macro:: Z80_WITH_FULL_IM0

   Enables the full implementation of the interrupt mode 0.

.. c:macro:: Z80_WITH_IM0_RETX_NOTIFICATIONS

   Enables optional notifications for any ``reti`` or ``retn`` instruction executed during the interrupt mode 0 response.

.. c:macro:: Z80_WITH_Q

   Enables the implementation of `Q <https://worldofspectrum.org/forums/discussion/41704>`_.

.. c:macro:: Z80_WITH_SPECIAL_RESET

   Enables the implementation of the `special RESET <http://www.primrosebank.net/computers/z80/z80_special_reset.htm>`_.

.. c:macro:: Z80_WITH_UNOFFICIAL_RETI

   Configures the ``ED5Dh``, ``ED6Dh`` and ``ED7Dh`` undocumented instructions as ``reti`` instead of ``retn``.

.. c:macro:: Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG

   Enables the implementation of the bug affecting the Zilog Z80 NMOS, which causes the P/V flag to be reset when a maskable interrupt is accepted during the execution of the ``ld a,{i|r}`` instructions.

Except for :c:macro:`Z80_DEPENDENCIES_HEADER`, the above macros do not need to be defined as any value; the source code only checks whether they are defined.
