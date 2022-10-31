============
Installation
============

.. only:: html

   .. |br| raw:: html

      <br />

.. only:: latex

   .. |nl| raw:: latex

      \newline

You will need `CMake <https://cmake.org>`_ v3.14 or later to build the package and, optionally, recent versions of `Doxygen <https://www.doxygen.nl>`_, `Sphinx <https://www.sphinx-doc.org>`_ and `Breathe <https://github.com/michaeljones/breathe>`_ to compile the documentation. Also make sure you have `LaTeX <https://www.latex-project.org>`_ with PDF support installed on your system if you want to generate the documentation in PDF format.

The emulator requires some types and macros included in `Zeta <https://zeta.st>`_, a dependency-free, `header-only <https://en.wikipedia.org/wiki/Header-only>`_ library used to retain compatibility with most C compilers. Install Zeta or extract its `official source code package <https://zeta.st/download>`_ to the directory of the Z80 project or its parent directory. Zeta is the sole dependency; the emulator is a freestanding implementation and as such does not depend on the
`C standard library <https://en.wikipedia.org/wiki/C_standard_library>`_.

Once all requirements are met, create a directory and run ``cmake`` from there to prepare the build system:

.. code-block:: sh

   mkdir build
   cd build
   cmake <path to the Z80 project> [options]

The resulting build files can be configured by passing options to ``cmake``. To show a complete list of those available along with their current settings, type the following:

.. code-block:: sh

   cmake -LAH

If in doubt, read the `CMake documentation <https://cmake.org/documentation/>`_ for more information on configuration options. The following are some of the most relevant standard options of CMake:

.. option:: -DBUILD_SHARED_LIBS=(YES|NO)

   Build the emulator as a shared library, rather than static. |br| |nl|
   The default is ``NO``.

.. option:: -DCMAKE_BUILD_TYPE=(Debug|Release|RelWithDebInfo|MinSizeRel)

   Choose the type of build (configuration) to generate. |br| |nl|
   The default is ``Release``.

.. option:: -DCMAKE_INSTALL_PREFIX="<path>"

   Specify the installation prefix on `UNIX <https://en.wikipedia.org/wiki/Unix>`_ and `UNIX-like <https://en.wikipedia.org/wiki/Unix-like>`_ operating systems. |br| |nl|
   The default is ``"/usr/local"``.

Package-specific options are prefixed with ``Z80_`` and can be divided into two groups. The first one controls aspects not related to the source code of the library:

.. option:: -DZ80_DOWNLOAD_TEST_FILES=(YES|NO)

   Download the firmware and software used by the testing tool. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_INSTALL_CMAKEDIR="<path>"

   Specify the directory in which to install the CMake `config-file package <https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages>`_. |br| |nl|
   The default is ``"${CMAKE_INSTALL_LIBDIR}/cmake/Z80"``.

.. option:: -DZ80_INSTALL_PKGCONFIGDIR="<path>"

   Specify the directory in which to install the `pkg-config <https://www.freedesktop.org/wiki/Software/pkg-config>`_ `file <https://people.freedesktop.org/~dbn/pkg-config-guide.html>`_. |br| |nl|
   The default is ``"${CMAKE_INSTALL_LIBDIR}/pkgconfig"``.

.. option:: -DZ80_NOSTDLIB_FLAGS=(Auto|[<flag>[;<flag>...]])

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

   Generate and install the CMake `config-file package <https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages>`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_HTML_DOCUMENTATION=(YES|NO)

   Build and install the documentation in HTML format. |br| |nl|
   It requires Doxygen, Sphinx and Breathe. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_PDF_DOCUMENTATION=(YES|NO)

   Build and install the documentation in PDF format. |br| |nl|
   It requires Doxygen, Sphinx, Breathe and LaTeX with PDF support. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_PKGCONFIG_SUPPORT=(YES|NO)

   Generate and install the `pkg-config <https://www.freedesktop.org/wiki/Software/pkg-config>`_ `file <https://people.freedesktop.org/~dbn/pkg-config-guide.html>`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_STANDARD_DOCUMENTS=(YES|NO)

   Install the standard text documents distributed with the package: :file:`AUTHORS`, :file:`COPYING`, :file:`COPYING.LESSER`, :file:`HISTORY`, :file:`README` and :file:`THANKS`. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_TESTS=(YES|NO)

   Build the testing tool. |br| |nl|
   The default is ``NO``.

The second group of package-specific options configures the source code of the library by predefining macros that enable optional implementations:

.. option:: -DZ80_WITH_EXECUTE=(YES|NO)

   Build the implementation of the :c:func:`z80_execute` function. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_FULL_IM0=(YES|NO)

   Build the full implementation of the interrupt mode 0 rather than the reduced one. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_Q=(YES|NO)

   Build the implementation of the `Q "register" <https://worldofspectrum.org/forums/discussion/41704>`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_SPECIAL_RESET=(YES|NO)

   Build the implementation of the `special RESET <http://www.primrosebank.net/computers/z80/z80_special_reset.htm>`_. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_UNOFFICIAL_RETI=(YES|NO)

   Configure the ``ED5Dh``, ``ED6Dh`` and ``ED7Dh`` undocumented instructions as ``reti`` instead of ``retn``. |br| |nl|
   The default is ``NO``.

.. option:: -DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=(YES|NO)

   Build the implementation of the bug affecting the Zilog Z80 NMOS, which causes the P/V flag to be reset when a maskable interrupt is accepted during the execution of the ``ld a,{i|r}`` instructions. |br| |nl|
   The default is ``NO``.

Package maintainers should use at least the following options for the shared library:

.. code-block:: sh

   -DZ80_WITH_EXECUTE=YES
   -DZ80_WITH_FULL_IM0=YES
   -DZ80_WITH_Q=YES
   -DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=YES

Finally, once the build system is configured according to your needs, build and install the package:

.. code-block:: sh

   make
   make install/strip
