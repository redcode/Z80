=========================
Installation from sources
=========================

.. only:: html

   .. |br| raw:: html

      <br />

.. only:: latex

   .. |nl| raw:: latex

      \newline

.. |cmake_option_install_component| replace:: ``--component``
.. _cmake_option_install_component: https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-install-component

.. |cmake_option_build_config| replace:: ``--config``
.. _cmake_option_build_config: https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-build-config

.. |cmake_option_install_strip| replace:: ``--strip``
.. _cmake_option_install_strip: https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-install-strip

.. _config-file package: https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages
.. _CP/M: https://en.wikipedia.org/wiki/CP/M
.. _file: https://people.freedesktop.org/~dbn/pkg-config-guide.html
.. _pkg-config: https://www.freedesktop.org/wiki/Software/pkg-config
.. _unit tests in JSON format: https://github.com/SingleStepTests/z80
.. _ZX Spectrum: https://en.wikipedia.org/wiki/ZX_Spectrum

Prerequisites
=============

You will need `CMake <https://cmake.org>`_ v3.14 or later to build the package and, optionally, recent versions of `Doxygen <https://www.doxygen.nl>`_, `Sphinx <https://www.sphinx-doc.org>`_ and `Breathe <https://www.breathe-doc.org>`_ to compile the documentation. Also, make sure that you have `LaTeX <https://www.latex-project.org>`_ with PDF support installed on your system if you want to generate the documentation in PDF format.

The Z80 library requires some types and macros included in `Zeta <https://zxe.io/software/Zeta>`_, a `header-only <https://en.wikipedia.org/wiki/Header-only>`_, dependency-free library used for portability reasons. Install Zeta or extract its `source code tarball <https://zxe.io/software/Zeta/download>`_ to the root directory of the Z80 project or its parent directory. Zeta is the sole dependency; the emulator does not depend on the `C standard library <https://en.wikipedia.org/wiki/C_standard_library>`_.

Lastly, the package includes two testing tools, which are optional to build. The first one runs various Z80-specific tests for `CP/M`_ and `ZX Spectrum`_ and will use `libzip <https://libzip.org>`_ and `zlib <https://zlib.net>`_ if they are available on your system. The second tool is for `unit tests in JSON format`_ and requires the `cJSON <https://github.com/DaveGamble/cJSON>`_ and `Z80InsnClock <https://zxe.io/software/Z80InsnClock>`_ libraries.

Configure
=========

Once the prerequisites are met, create a directory and run ``cmake`` from there to prepare the build system:

.. code-block:: sh

   mkdir build
   cd build
   cmake [options] <Z80-project-directory>

The resulting build files can be configured by passing options to ``cmake``. To show a complete list of those available along with their current settings, type the following:

.. code-block:: sh

   cmake -LAH -N -B .

If in doubt, read the `CMake documentation <https://cmake.org/documentation/>`_ for more information on configuration options. The following are some of the most relevant standard options of CMake:

.. option:: -DBUILD_SHARED_LIBS=(YES|NO)

   Generate shared libraries rather than static libraries. |br| |nl|
   The default is ``NO``.

.. option:: -DCMAKE_BUILD_TYPE=(Debug|Release|RelWithDebInfo|MinSizeRel)

   Choose the type of build (configuration) to generate. |br| |nl|
   The default is ``Release``.

.. option:: -DCMAKE_INSTALL_NAME_DIR="<path>"

   Specify the `directory portion <https://developer.apple.com/documentation/xcode/build-settings-reference#Dynamic-Library-Install-Name-Base>`_ of the `dynamic library install name <https://developer.apple.com/documentation/xcode/build-settings-reference#Dynamic-Library-Install-Name>`_ on Apple platforms (for installed shared libraries). |br| |nl|
   Not defined by default.

.. option:: -DCMAKE_INSTALL_PREFIX="<path>"

   Specify the installation prefix. |br| |nl|
   The default is ``"/usr/local"`` (on `UNIX <https://en.wikipedia.org/wiki/Unix>`_ and `UNIX-like <https://en.wikipedia.org/wiki/Unix-like>`_ operating systems).

.. _cmake_package_options:

Package-specific options are prefixed with ``Z80_`` and can be divided into two groups. The first one controls aspects not related to the source code of the library:

.. option:: -DZ80_DEPOT_LOCATION="<location>"

   Specify the directory or URL of the depot containing the test files (i.e., the firmware and software required by the :file:`test-Z80` tool). |br| |nl|
   The default is ``"http://zxe.io/depot"``.

.. option:: -DZ80_FETCH_TEST_FILES=(YES|NO)

   If :option:`Z80_WITH_TESTING_TOOL<-DZ80_WITH_TESTING_TOOL>` is ``YES``, copy or download the test files from the depot to the build directory. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_INSTALL_CMAKEDIR="<path>"

   Specify the directory in which to install the CMake `config-file package`_. |br| |nl|
   The default is ``"${CMAKE_INSTALL_LIBDIR}/cmake/Z80"``.

.. option:: -DZ80_INSTALL_PKGCONFIGDIR="<path>"

   Specify the directory in which to install the `pkg-config`_ `file`_. |br| |nl|
   The default is ``"${CMAKE_INSTALL_LIBDIR}/pkgconfig"``.

.. option:: -DZ80_NOSTDLIB_FLAGS=(Auto|"[<flag>[;<flag>...]]")

   Specify the linker flags used to avoid linking against system libraries. |br| |nl|
   The default is ``Auto`` (autoconfigure flags). If you get linker errors, set this option to ``""``.

.. option:: -DZ80_OBJECT_LIBS=(YES|NO)

   Build the emulator as an `object library <https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#object-libraries>`_. |br| |nl|
   This option takes precedence over :option:`BUILD_SHARED_LIBS<-DBUILD_SHARED_LIBS>` and :option:`Z80_SHARED_LIBS<-DZ80_SHARED_LIBS>`. If enabled, the build system will ignore :option:`Z80_WITH_CMAKE_SUPPORT<-DZ80_WITH_CMAKE_SUPPORT>` and :option:`Z80_WITH_PKGCONFIG_SUPPORT<-DZ80_WITH_PKGCONFIG_SUPPORT>`, as no libraries or support files will be installed. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_SHARED_LIBS=(YES|NO)

   Build the emulator as a shared library, rather than static. |br| |nl|
   This option takes precedence over :option:`BUILD_SHARED_LIBS<-DBUILD_SHARED_LIBS>`. |br| |nl|
   Not defined by default.

.. option:: -DZ80_SPHINX_HTML_THEME="[<name>]"

   Specify the Sphinx theme for the documentation in HTML format. |br| |nl|
   The default is ``""`` (use the default theme).

.. option:: -DZ80_WITH_CMAKE_SUPPORT=(YES|NO)

   Generate and install the CMake `config-file package`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_HTML_DOCUMENTATION=(YES|NO)

   Build and install the documentation in HTML format. |br| |nl|
   It requires Doxygen, Sphinx and Breathe. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_PDF_DOCUMENTATION=(YES|NO)

   Build and install the documentation in PDF format. |br| |nl|
   It requires Doxygen, Sphinx, Breathe, and LaTeX with PDF support. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_PKGCONFIG_SUPPORT=(YES|NO)

   Generate and install the `pkg-config`_ `file`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_STANDARD_DOCUMENTS=(YES|NO)

   Install the standard text documents distributed with the package: :file:`AUTHORS`, :file:`COPYING`, :file:`COPYING.LESSER`, :file:`HISTORY`, :file:`README` and :file:`THANKS`. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_STEP_TESTING_TOOL=(YES|NO)

   Build :file:`step-test-Z80`, a tool for `unit tests in JSON format`_. |br| |nl|
   It requires cJSON and Z80InsnClock. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_TESTING_TOOL=(YES|NO)

   Build :file:`test-Z80`, a tool that runs various Z80-specific tests for `CP/M`_ and `ZX Spectrum`_. |br| |nl|
   The default is ``NO``.

.. _cmake_package_source_code_options:

The second group of package-specific options configures the source code of the library by predefining macros that enable :ref:`optional features <introduction:Optional features>`:

.. option:: -DZ80_WITH_EXECUTE=(YES|NO)

   Build the implementation of the :c:func:`z80_execute` function. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_FULL_IM0=(YES|NO)

   Build the full implementation of the interrupt mode 0 rather than the reduced one. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_IM0_RETX_NOTIFICATIONS=(YES|NO)

   Enable optional notifications for any ``reti`` or ``retn`` instruction executed during the interrupt mode 0 response. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_PARITY_COMPUTATION=(YES|NO)

   Enable actual parity calculation for the P/V flag instead of using a table of precomputed values (NOT RECOMMENDED for production builds). |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_PRECOMPUTED_DAA=(YES|NO)

   Use a table of precomputed values to emulate the ``daa`` instruction (NOT RECOMMENDED for production builds). |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_Q=(YES|NO)

   Build the implementation of `Q <https://worldofspectrum.org/forums/discussion/41704>`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_SPECIAL_RESET=(YES|NO)

   Build the implementation of the `special RESET <http://www.primrosebank.net/computers/z80/z80_special_reset.htm>`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_UNOFFICIAL_RETI=(YES|NO)

   Configure the undocumented instructions ``ED5Dh``, ``ED6Dh`` and ``ED7Dh`` as ``reti`` instead of ``retn``. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=(YES|NO)

   Build the implementation of the bug affecting the Zilog Z80 NMOS, which causes the P/V flag to be reset when a maskable interrupt is accepted during the execution of the ``ld a,{i|r}`` instructions. |br| |nl|
   The default is ``NO``.

Package maintainers are encouraged to use at least the following options for the shared library:

.. code-block:: sh

   -DZ80_WITH_EXECUTE=YES
   -DZ80_WITH_FULL_IM0=YES
   -DZ80_WITH_IM0_RETX_NOTIFICATIONS=YES
   -DZ80_WITH_Q=YES
   -DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=YES

Build and install
=================

Finally, once the build system is configured according to your needs, build and install the package:

.. code-block:: sh

   cmake --build . [--config (Debug|Release|RelWithDebInfo|MinSizeRel)]
   cmake --install . [--config <configuration>] [--strip] [--component <component>]

The |cmake_option_build_config|_ option is only necessary for those `CMake generators <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`_ that ignore :option:`CMAKE_BUILD_TYPE<-DCMAKE_BUILD_TYPE>` (e.g., Xcode and Visual Studio). Use |cmake_option_install_strip|_ to remove debugging information and non-public symbols when installing non-debug builds of the shared library. To install only a specific component of the package, use the |cmake_option_install_component|_ option.

The project defines the following components:

.. option:: Z80_Runtime

   * Shared library.
   * Symbolic link for the compatibility version of the shared library.
   * Standard text documents.

.. option:: Z80_Development

   * Static library.
   * Unversioned symbolic link of the shared library.
   * Public header.
   * CMake config-file package.
   * pkg-config file.

.. option:: Z80_Documentation

   * Documentation in HTML format.
   * Documentation in PDF format.

.. option:: Z80_Testing

   * Testing tools.

By default, the build system will install ``Z80_Runtime``, ``Z80_Development`` and ``Z80_Documentation``. The ``Z80_Testing`` component can only be installed explicitly.
