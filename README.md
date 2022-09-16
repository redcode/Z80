<h1 align="center">
	<img src="https://upload.wikimedia.org/wikipedia/commons/1/19/Zilog_Z80.jpg"><br>
	<b>Zilog Z80 CPU Emulator</b>
</h1>
<p align="center">
	<a href="https://github.com/redcode/Z80/actions/workflows/library-ci.yml">
		<img src="https://github.com/redcode/Z80/actions/workflows/library-ci.yml/badge.svg">
	</a>
	<a href="https://github.com/redcode/Z80/actions/workflows/documentation-ci.yml">
		<img src="https://github.com/redcode/Z80/actions/workflows/documentation-ci.yml/badge.svg">
	</a>
	<a href="https://discord.gg/zU4dkzp8Mv">
		<img alt="Discord" src="https://img.shields.io/discord/848534208129138738?color=5865F2&label=Discord&logo=discord&logoColor=white"/>
	</a>
</p>

# Introduction

The [Z80 library](https://zxe.io/software/Z80) implements a fast, small and accurate [emulator](https://en.wikipedia.org/wiki/Emulator) of the [Zilog Z80](https://en.wikipedia.org/wiki/Zilog_Z80). It emulates all that is known to date about this CPU, including the undocumented behaviors, [MEMPTR](https://zxpress.ru/zxnet/zxnet.pc/5909), [Q](https://worldofspectrum.org/forums/discussion/41704) and the [special RESET](http://www.primrosebank.net/computers/z80/z80_special_reset.htm). It also has the honor of having been the first open source project to provide full emulation of the interrupt mode 0.

The source code is written in [ANSI C](https://en.wikipedia.org/wiki/ANSI_C) for maximum portability and is extensively commented. The aim has been to write a well-structured, easy to understand piece of software; something solid and elegant that can stand the test of time with no need for major changes.


# Emulation accuracy

The Zilog Z80 emulator has a classic design with instruction-level granularity. This provides the best performance when speed is a critical factor, while still offering a reasonable flexibility to achieve precision down to the T-state level when accuracy is imperative.

Instruction-level granularity means that, except in a few well-defined cases, the execution of a given instruction cannot stop until all its internal M-cycles are completed. In addition, this kind of emulation has been optimized for speed, which implies that any pertinent register is modified only once per instruction and the T-state counter is updated only after a whole instruction is executed.

That said, instructions, flags, memory accesses, interrupt and reset responses, clock cycles, etc. are accurately emulated as far as is known, according to the technical documentation available, the findings made after decades of research on the Z80 and electronic simulations. And, of course, the emulator passes the most exhaustive tests written to date:

### <span id="major_test_suites">[Zilog Z80 CPU Test Suite](https://github.com/raxoft/z80test), by [Patrik Rak](https://github.com/raxoft)</span>

This set of programs is intended to help the emulator authors to reach the desired level of the CPU emulation authenticity. Each of the included programs performs an exhaustive computation using each of the tested Z80 instructions, compares the results with values obtained from a real [Sinclair ZX Spectrum 48K](https://en.wikipedia.org/wiki/ZX_Spectrum) with Zilog Z80 CPU, and reports any deviations detected.

<details>
	<summary>Results (v1.0)</summary>
	<table>
		<tr>
			<td align="center" valign="top" align="center">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80full.tap</b>
				</a>
				<br>
				<sup>Tests all flags and registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80doc.tap</b>
				</a>
				<br>
				<sup>Tests all registers, but only officially documented flags.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Full.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Doc.gif"/>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80flags.tap</b>
				</a>
				<br>
				<sup>Tests all flags, ignores registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80docflags.tap</b>
				</a>
				<br>
				<sup>Tests documented flags only, ignores registers.</sup>
			</td>
		</tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Flags.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Doc%20Flags.gif"/>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80ccf.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>ccf</code> after each instruction tested.</sup>
			</td>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80memptr.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>bit n,(hl)</code> after each instruction tested.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20CCF.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20MEMPTR.gif"/>
			</td>
		</tr>
	</table>
</details>
<details>
	<summary>Results (v1.2)</summary>
	<table>
		<tr>
			<td align="center" valign="top" align="center">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80full.tap</b>
				</a>
				<br>
				<sup>Tests all flags and registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80doc.tap</b>
				</a>
				<br>
				<sup>Tests all registers, but only officially documented flags.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Full.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Doc.gif"/>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80flags.tap</b>
				</a>
				<br>
				<sup>Tests all flags, ignores registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80docflags.tap</b>
				</a>
				<br>
				<sup>Tests documented flags only, ignores registers.</sup>
			</td>
		</tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Flags.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Doc%20Flags.gif"/>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80ccf.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>ccf</code> after each instruction tested.</sup>
			</td>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80memptr.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>bit n,(hl)</code> after each instruction tested.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20CCF.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20MEMPTR.gif"/>
			</td>
		</tr>
	</table>
</details>

### Z80 Test Suite, by Mark Woodmass

This suite performs a series of tests to verify the MEMPTR documents <sub>_([English](http://zx.pk.ru/attachment.php?attachmentid=2989), [Russian](http://zx.pk.ru/attachment.php?attachmentid=2984))_</sub>, which are spot on, as well as a brief run through several of the `CBh`/`DDh`/`FDh` opcode ranges. The test results in the program are compared against those from a [NEC D780C-1](https://www.cpu-world.com/CPUs/Z80/NEC-D780C-1.html) CPU, but Simon Conway kindly tested several other Z80 clones, confirming the same results.

<details>
	<summary>Results</summary>
	<table>
		<tr>
			<td align="center" valign="top" colspan="2">
				<a href="https://web.archive.org/web/20130511214919if_/http://homepage.ntlworld.com/mark.woodmass/z80tests.tap">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>z80tests.tap</b>
				</a>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Z80%20Test%20Suite%20%282008%29%28Woodmass,%20Mark%29%20-%20Flags.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Z80%20Test%20Suite%20%282008%29%28Woodmass,%20Mark%29%20-%20MEMPTR.gif"/>
			</td>
		</tr>
	</table>
</details>

### Z80 Instruction Set Exerciser, by Frank D. Cringle

Frank Cringle's _Z80 Instruction Set Exerciser_ attempts to execute every Z80 opcode, putting them through a cycle of tests and comparing the results to actual results from running the code on a real Z80. The exerciser is supplied with Frank's [YAZE](ftp://ftp.ping.de/pub/misc/emulators/yaze-1.10.tar.gz) (Yet Another Z80 Emulator). It is often difficult to track down, so [Jonathan Graham Harston](http://mdfs.net/User/JGH/) put it together [here](http://mdfs.net/Software/Z80/Exerciser/), as well as [some conversions](http://mdfs.net/Software/Z80/Exerciser/Spectrum/). The latest release of YAZE is available at [Andreas Gerlich's website](http://www.mathematik.uni-ulm.de/users/ag/yaze-ag/).

<details>
	<summary>Results (Jonathan Graham Harston's version)</summary>
	<table>
		<tr>
			<td align="center" valign="top">
				<a href="http://mdfs.net/Software/Z80/Exerciser/Spectrum/zexdoc.tap">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>zexdoc.tap</b>
				</a>
				<br>
				<sup>Tests officially documented flag effects.</sup>
			</td>
			</td>
			<td align="center" valign="top">
				<a href="http://mdfs.net/Software/Z80/Exerciser/Spectrum/zexall.tap">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>zexall.tap</b>
				</a>
				<br>
				<sup>Tests all flags changes.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Z80%20Documented%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282018%29%28Harston,%20Jonathan%20Graham%29.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Z80%20Full%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282018%29%28Harston,%20Jonathan%20Graham%29.gif"/>
			</td>
		</tr>
	</table>
</details>
<details>
	<summary>Results (Jan Bobrowski's version)</summary>
	<table>
		<tr>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20120527195857if_/http://wizard.ae.krakow.pl:80/~jb/qaop/t/zexfix.tap">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>zexfix.tap</b>
				</a>
				<br>
				<sup>Tests all flags changes.</sup>
			</td>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20120527200327if_/http://wizard.ae.krakow.pl:80/~jb/qaop/t/zexbit.tap">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>zexbit.tap</b>
				</a>
				<br>
				<sup>Tests all flags changes of <code>bit</code> instructions.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Z80%20Full%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282009%29%28Bobrowski,%20Jan%29.gif"/>
			</td>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Z80%20Full%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282011%29%28Bobrowski,%20Jan%29%28Narrowed%20to%20BIT%20Instructions%29.gif"/>
			</td>
		</tr>
	</table>
</details>
<details>
	<summary>Results (Patrik Rak's version)</summary>
	<table>
		<tr>
			<td align="center" valign="top">
				<a href="http://zxds.raxoft.cz/taps/misc/zexall2.zip">
					<img src="http://zxe.io/software/Z80/images/TAP.png"/>
					<br>
					<b>zexall2.tap</b>
				</a>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="http://zxe.io/software/Z80/images/tests/Z80%20Instruction%20Set%20Exerciser%20for%20Spectrum%202%20v0.1%20%282012-11-27%29%28Rak,%20Patrik%29.gif"/>
			</td>
		</tr>
	</table>
</details>

# Installation

### Debian/Ubuntu

First add the ZXE repository:

```shell
apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv FE214A38D6A0C01D9AF514EE841EA3BD3A7E1487
add-apt-repository 'deb [arch=amd64] https://zxe.io/repos/apt stable main'
```

Next, install the Z80 library package:

```shell
apt install libz80
```

In case you need to build software that requires the Z80 library, install the development package too:

```shell
apt install libz80-dev
````

### Gentoo Linux

First add the ZXE overlay:

```
eselect repository add zxe git https://github.com/redcode/zxe-gentoo-overlay.git
emaint sync --repo zxe
```

Next, install the Z80 library:

```
emerge z80
```

# Installation from sources

You will need [CMake](https://cmake.org) v3.14 or later to build the package and, optionally, recent versions of [Doxygen](https://www.doxygen.nl), [Sphinx](https://www.sphinx-doc.org) and [Breathe](https://github.com/michaeljones/breathe) to compile the documentation. Also make sure you have [LaTeX](https://www.latex-project.org) with PDF support installed on your system if you want to generate the documentation in PDF format.

The emulator requires some types and macros included in [Zeta](https://github.com/redcode/Zeta), a dependency-free, [header-only](https://en.wikipedia.org/wiki/Header-only) library used to retain compatibility with most C compilers. Install Zeta or extract its official source code package to the same directory of this `README.md` or its parent directory. Zeta is the sole dependency; the emulator is a freestanding implementation and as such does not depend on the [C standard library](https://en.wikipedia.org/wiki/C_standard_library).

Once all requirements are met, create a directory and run `cmake` from there to prepare the build system:

```shell
mkdir build
cd build
cmake <directory of this README.md> [options]
```

The resulting build files can be configured by passing options to `cmake`. To show a complete list of those available along with their current settings, type the following:

```shell
cmake -LAH
```

If in doubt, read the [CMake documentation](https://cmake.org/documentation/) for more information on configuration options. The following are some of the most relevant standard options of CMake:

* <span id="option_build_shared_libs">**`-DBUILD_SHARED_LIBS=(YES|NO)`**</span>  
	Build the project as a shared library rather than a static one.  
	The default is `NO`.

* <span id="option_cmake_build_type">**`-DCMAKE_BUILD_TYPE=(Debug|Release|RelWithDebInfo|MinSizeRel)`**</span>  
	Choose the type of build (configuration) to generate.  
	The default is `Release`.

* <span id="option_cmake_install_prefix">**`-DCMAKE_INSTALL_PREFIX="<path>"`**</span>  
	Specify the installation prefix on [UNIX](https://en.wikipedia.org/wiki/Unix) and [UNIX-like](https://en.wikipedia.org/wiki/Unix-like) operating systems.  
	The default is `"/usr/local"`.

Package-specific options are prefixed with `Z80_` and can be divided into two groups. The first one controls aspects not related to the source code of the library:

* <span id="option_z80_download_test_files">**`-DZ80_DOWNLOAD_TEST_FILES=(YES|NO)`**</span>  
	Download the firmware and software used by the testing tool.  
	The default is `NO`.

* <span id="option_z80_install_cmakedir">**`-DZ80_INSTALL_CMAKEDIR="<path>"`**</span>  
	Specify the directory in which to install the CMake [config-file package](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages).  
	The default is `"${CMAKE_INSTALL_LIBDIR}/cmake/Z80"`.

* <span id="option_z80_install_pkgconfigdir">**`-DZ80_INSTALL_PKGCONFIGDIR="<path>"`**</span>  
	Specify the directory in which to install the [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config) [file](https://people.freedesktop.org/~dbn/pkg-config-guide.html).  
	The default is `"${CMAKE_INSTALL_LIBDIR}/pkgconfig"`.

* <span id="option_z80_nostdlib_flags">**`-DZ80_NOSTDLIB_FLAGS=(Auto|[<flag>[;<flag>...]])`**</span>  
	Specify the linker flags used to avoid linking against system libraries.  
	The default is `Auto` (autoconfigure flags). If you get linker errors, set this option to `""`.

* <span id="option_z80_shared_libs">**`-DZ80_SHARED_LIBS=(YES|NO)`**</span>  
	Build the project as a shared library rather than a static one.  
	This option takes precedence over [`BUILD_SHARED_LIBS`](#option_build_shared_libs).  
	Not defined by default.

* <span id="option_z80_sphinx_html_theme">**`-DZ80_SPHINX_HTML_THEME="[<name>]"`**</span>  
	Specify the Sphinx theme for the documentation in HTML format.  
	The default is `""` (use the default theme).

* <span id="option_z80_with_cmake_support">**`-DZ80_WITH_CMAKE_SUPPORT=(YES|NO)`**</span>  
	Generate and install the CMake [config-file package](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages).  
	The default is `NO`.

* <span id="option_z80_with_html_documentation">**`-DZ80_WITH_HTML_DOCUMENTATION=(YES|NO)`**</span>  
	Build and install the documentation in HTML format.  
	It requires Doxygen, Sphinx and Breathe.  
	The default is `NO`.

* <span id="option_z80_with_pdf_documentation">**`-DZ80_WITH_PDF_DOCUMENTATION=(YES|NO)`**</span>  
	Build and install the documentation in PDF format.  
	It requires Doxygen, Sphinx, Breathe and LaTeX with PDF support.  
	The default is `NO`.

* <span id="option_z80_with_pkgconfig_support">**`-DZ80_WITH_PKGCONFIG_SUPPORT=(YES|NO)`**</span>  
	Generate and install the [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config) [file](https://people.freedesktop.org/~dbn/pkg-config-guide.html).  
	The default is `NO`.

* <span id="option_z80_with_standard_documents">**`-DZ80_WITH_STANDARD_DOCUMENTS=(YES|NO)`**</span>  
	Install the standard text documents distributed with the package: `AUTHORS`, `COPYING`, `COPYING.LESSER`, `HISTORY`, `README` and `THANKS`.  
	The default is `NO`.

* <span id="option_z80_with_tests">**`-DZ80_WITH_TESTS=(YES|NO)`**</span>  
	Build the testing tool.  
	The default is `NO`.

<span id="cmake_source_code_options">The second group of package-specific options</span> configures the source code of the library by predefining macros that enable optional implementations:

* <span id="option_z80_with_execute">**`-DZ80_WITH_EXECUTE=(YES|NO)`**</span>  
	Build the implementation of the `z80_execute` function.  
	The default is `NO`.

* <span id="option_z80_with_full_im0">**`-DZ80_WITH_FULL_IM0=(YES|NO)`**</span>  
	Build the full implementation of the interrupt mode 0 rather than the reduced one.  
	The default is `NO`.

* <span id="option_z80_with_q">**`-DZ80_WITH_Q=(YES|NO)`**</span>  
	Build the implementation of the [Q "register"](https://worldofspectrum.org/forums/discussion/41704).  
	The default is `NO`.

* <span id="option_z80_with_special_reset">**`-DZ80_WITH_SPECIAL_RESET=(YES|NO)`**</span>  
	Build the implementation of the [special RESET](http://www.primrosebank.net/computers/z80/z80_special_reset.htm).  
	The default is `NO`.

* <span id="option_z80_with_unofficial_reti">**`-DZ80_WITH_UNOFFICIAL_RETI=(YES|NO)`**</span>  
        Configure the `ED5Dh`, `ED6Dh` and `ED7Dh` undocumented instructions as `reti` instead of `retn`.  
        The default is `NO`.

* <span id="option_z80_with_zilog_nmos_ld_a_ir_bug">**`-DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=(YES|NO)`**</span>  
	Build the implementation of the bug affecting the Zilog Z80 NMOS, which causes the P/V flag to be reset when a maskable interrupt is accepted during the execution of the `ld a,{i|r}` instructions.  
	The default is `NO`.

Package maintainers should use at least the following options for the shared library:

```shell
-DZ80_WITH_EXECUTE=YES
-DZ80_WITH_FULL_IM0=YES
-DZ80_WITH_Q=YES
-DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=YES
```

Finally, once the build system is configured according to your needs, build and install the package:

```shell
make
make install/strip
```

# Running the tests

The package includes a tool called `test-Z80` capable of running all [CP/M](https://en.wikipedia.org/wiki/CP/M) and [ZX Spectrum](https://en.wikipedia.org/wiki/ZX_Spectrum) versions of [the major test suites](#major_test_suites). Configure the build system with [`-DZ80_WITH_TESTS=YES`](#option_z80_with_tests) to enable its compilation and [`-DZ80_DOWNLOAD_TEST_FILES=YES`](#option_z80_download_test_files) to download the firmware and software required. Also note that the Z80 library must be built with [`-DZ80_WITH_Q=YES`](#option_z80_with_q) to be able to pass Patrik Rak's tests.

Once you have built the package, type the following to run all tests:
```shell
./test-Z80 -p downloads/firmware -p downloads/software/POSIX -p "downloads/software/ZX Spectrum" -a
```

The tool supports options and can run the tests individually. Type `./test-Z80 -h` for help. If you prefer to run all tests through Make, just type `make tests`. For [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html), use `ctest` or `ctest -V` instead.

These are the complete logs generated by `test-Z80` emulating the different CPU variants:

* [Zilog NMOS](https://zxe.io/software/Z80/Z80%20v0.2%20Test%20Results%20%28Zilog%20NMOS%29.txt)
* [Zilog CMOS](https://zxe.io/software/Z80/Z80%20v0.2%20Test%20Results%20%28Zilog%20CMOS%29.txt)
* [NEC NMOS](https://zxe.io/software/Z80/Z80%20v0.2%20Test%20Results%20%28NEC%20NMOS%29.txt)
* [ST CMOS](https://zxe.io/software/Z80/Z80%20v0.2%20Test%20Results%20%28ST%20CMOS%29.txt)

The CRC errors in the latter two are normal and match the values obtained on real hardware.

# Integration

### As an external dependency in CMake-based projects

The Z80 library [includes](#option_z80_with_cmake_support) a [config-file package](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages) for integration into CMake-based projects, which should be installed for development. Use [`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html) to find the `Z80` package. This creates the `Z80` imported library target that carries the necessary transitive link dependencies. The linking method can optionally be selected by specifying the `Shared` or the `Static` component.

Example:

```cmake
find_package(Z80 REQUIRED Shared)
target_link_libraries(your-target Z80)
```

When not specified as a component, the linking method is selected according to [`Z80_SHARED_LIBS`](#option_z80_shared_libs). If this option is not defined, the config-file uses the type of library that is installed on the system and, if it finds both the shared and the static versions, [`BUILD_SHARED_LIBS`](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html) determines which one to link against.

### As a CMake subproject

To embed the Z80 library as a CMake subproject, place its entire source tree into a subdirectory of another project and use [`add_subdirectory`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html) in the parent project to add this subdirectory to the build process.

It is advisable to configure the library in the `CMakeLists.txt` of the parent project. This will prevent the user from having to specify configuration options for the Z80 subproject through the command line when building the main project.

Example:

```cmake
set(Z80_SHARED_LIBS                 NO  CACHE BOOL "")
set(Z80_WITH_Q                      YES CACHE BOOL "")
set(Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG YES CACHE BOOL "")

add_subdirectory(dependencies/Z80)
target_link_libraries(your-target Z80)
```

It is important to set the [`Z80_SHARED_LIBS`](#option_z80_shared_libs) option. Otherwise CMake will build the library type indicated by [`BUILD_SHARED_LIBS`](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html), which may not be the desired one.

### Integrating the source code

There are several macros that can be used to configure the source code of the library. You can define those you need in your build system or at the beginning of the `Z80.c` file. The following ones allow you to configure the integration of `Z80.h` and `Z80.c` into the project:

* **`#define Z80_DEPENDENCIES_HEADER "header name.h"`**  
	Specifies the only external header to `#include`, replacing those of Zeta.  
	If used, it must also be defined before including the `Z80.h` header.

* **`#define Z80_STATIC`**  
	Required to compile and/or use the emulator as a static library or as an internal part of another project.  
	If used, it must also be defined before including the `Z80.h` header.

* **`#define Z80_WITH_LOCAL_HEADER`**  
	Tells `Z80.c` to `#include "Z80.h"` instead of `<Z80.h>`.

[The second group of package-specific options](#cmake_source_code_options), explained in the _[Installation from sources](#installation-from-sources)_ section of this document, activates various optional implementations in the source code by predefining the following macros:

* **[`#define Z80_WITH_EXECUTE`](#option_z80_with_execute)**
* **[`#define Z80_WITH_FULL_IM0`](#option_z80_with_full_im0)**
* **[`#define Z80_WITH_Q`](#option_z80_with_q)**
* **[`#define Z80_WITH_SPECIAL_RESET`](#option_z80_with_special_reset)**
* **[`#define Z80_WITH_UNOFFICIAL_RETI`](#option_z80_with_unofficial_reti)**
* **[`#define Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG`](#option_z80_with_zilog_nmos_ld_a_ir_bug)**

Except for `Z80_DEPENDENCIES_HEADER`, the above macros do not need to be defined as any value; the source code only checks whether or not they are defined.

Please note that the activation of some of the optional implementations affects the speed of the emulator due to various factors (read the documentation for more details).

# Showcase

This emulator has been used by the following projects (listed in alphabetical order):

* [Augmentinel](https://simonowen.com/spectrum/augmentinel/), _by [Simon Owen](https://simonowen.com/)_ - [GitHub](https://github.com/simonowen/augmentinel)
* CPM-Emulator, _by [Marc Sibert](https://github.com/Marcussacapuces91)_ - [GitHub](https://github.com/Marcussacapuces91/CPM-Emulator)
* [tihle: a unique TI calculator emulator](https://www.taricorp.net/2020/introducing-tihle/), _by [Peter Marheine](https://www.taricorp.net/about/)_ - [GitHub](https://github.com/tari/tihle), [GitLab](https://gitlab.com/taricorp/tihle)
* [TileMap](https://simonowen.com/spectrum/tilemap/), _by [Simon Owen](https://simonowen.com/)_ - [GitHub](https://github.com/simonowen/tilemap)
* Z80Plus, _by [Sofía Ortega Sosa](https://github.com/Agaxia)_ - [GitHub](https://github.com/Agaxia/Z80Plus)
* [Zemu](https://github.com/jayvalentine/zemu), _by [Jay Valentine](https://jayvalentine.github.io/)_ - [GitHub](https://github.com/jayvalentine/zemu), [RubyGems](https://rubygems.org/gems/zemu)

# Thanks

Many thanks to the following individuals (in alphabetical order):

* **Akimov, Vadim (lvd)**
	* For testing the library on many different platforms and CPU architectures.
* **azesmbog**
	1. For validating tests on real hardware. <sup>[1](#r1)</sup>
	2. For his research on the unstable flag behavior of the `ccf/scf` instructions.
	3. For his invaluable help.
* **Banks, David (hoglet)**
	1. For cracking the flag behavior of the block instructions. <sup>[2](#r2), [3](#r3)</sup>
	2. For his research on the flag behavior of the `ccf/scf` instructions. <sup>[3](#r3)</sup>
* **Beliansky, Anatoly (Tolik_Trek)**
	* For validating tests on real hardware. <sup>[4](#r4)</sup>
* **Bobrowski, Jan**  
	* For fixing the _"Z80 Full Instruction Set Exerciser for Spectrum"_. <sup>[5](#r5)</sup>
* **boo_boo**
	* For cracking the behavior of the MEMPTR register. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Brady, Stuart**
	* For his research on the flag behavior of the `ccf/scf` instructions. <sup>[10](#r10)</sup>
* **Brewer, Tony**
	1. For his research on the special RESET. <sup>[11](#r11), [12](#r12)</sup>
	2. For helping to crack the flag behavior of the block instructions. <sup>[2](#r2)</sup>
	3. For conducting low-level tests on real hardware. <sup>[2](#r2)</sup>
	4. For helping me to test different undocumented behaviors of the Zilog Z80.
* **Bystrov, Dmitry (Alone Coder)**
	* For validating tests on real hardware. <sup>[4](#r4)</sup>
* **Chunin, Roman (CHRV)**
	* For testing the behavior of the MEMPTR register on real Z80 chips. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Conway, Simon (BadBeard)**
	* For validating the _"Z80 Test Suite"_ on several Z80 clones. <sup>[13](#r13)</sup>
* **Cooke, Simon**
	* For finding out how the `out (c),0` instruction behaves on the Zilog Z80 CMOS. <sup>[14](#r14)</sup>
* **Cringle, Frank D.**
	* For writing the _"Z80 Instruction Set Exerciser"_. <sup>[15](#r15)</sup>
* **Devic, Goran**
	* For his research on undocumented behaviors of the Z80 CPU. <sup>[16](#r16)</sup>
* **Flammenkamp, Achim**
	* For his article on Z80 interrupts. <sup>[17](#r17)</sup>
* **Gimeno Fortea, Pedro**
	1. For his research work. <sup>[18](#r18)</sup>
	2. For writing the first-ever ZX Spectrum emulator. <sup>[19](#r19), [20](#r20)</sup>
* **goodboy**
	* For testing the behavior of the MEMPTR register on real Z80 chips. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Greenway, Ian**
	* For testing the flag behavior of the `ccf/scf` instructions on real hardware. <sup>[10](#r10), [21](#r21)</sup>
* **Harston, Jonathan Graham**
	1. For his research work.
	2. For his technical documents about the Zilog Z80. <sup>[22](#r22), [23](#r23), [24](#r24)</sup>
	3. For porting the _"Z80 Instruction Set Exerciser"_ to the ZX Spectrum. <sup>[25](#r25)</sup>
* **Helcmanovsky, Peter (Ped7g)**
	1. For helping me to write the _"IN-MEMPTR"_ test.
	2. For writing the _"Z80 Block Flags Test"_. <sup>[26](#r26), [27](#r27)</sup>
	3. For writing the _"Z80 CCF SCF Outcome Stability"_ test. <sup>[27](#r27)</sup>
	4. For writing the _"Z80 INT Skip"_ test. <sup>[27](#r27)</sup>
	5. For his research on the unstable flag behavior of the `ccf/scf` instructions.
	6. For his invaluable help.
* **icebear**
	* For testing the behavior of the MEMPTR register on real Z80 chips. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Kladov, Vladimir**
	* For cracking the behavior of the MEMPTR register. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Krook, Magnus**
	* For validating tests on real hardware. <sup>[28](#r28)</sup>
* **London, Matthew**
	* For validating tests on real hardware.
* **Martínez Cantero, Ricardo (Kyp)**
	* For validating tests on real hardware.
* **Molodtsov, Aleksandr**
	* For testing the behavior of the MEMPTR register on real Z80 chips. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Nair, Arjun**
	* For validating tests on real hardware. <sup>[26](#r26)</sup>
* **Nicolás-González, César**
	* For helping me to research the unstable flag behavior of the `ccf/scf` instructions.
* **Ortega Sosa, Sofía**
	* For her support.
* **Owen, Simon**
	* For the idea of the hooking method used in this emulator.
* **Ownby, Matthew P.**
	* For his reseach on the state of the registers after POWER-ON. <sup>[29](#r29)</sup>
* **Rak, Patrik**
	1. For improving the _"Z80 Instruction Set Exerciser for Spectrum"_. <sup>[30](#r30)</sup>
	2. For cracking the flag behavior of the `ccf/scf` instructions. <sup>[13](#r13), [30](#r30)</sup>
	3. For writing the _"Zilog Z80 CPU Test Suite"_. <sup>[30](#r30), [31](#r31)</sup>
	4. For his research on the unstable flag behavior of the `ccf/scf` instructions.
* **Rodríguez Jódar, Miguel Ángel (mcleod_ideafix)**
	* For his reseach on the state of the registers after POWER-ON/RESET. <sup>[32](#r32)</sup>
* **Rodríguez Palomino, Mario (r-lyeh)**
	* For teaching me how emulators work.
* **Sainz de Baranda y Romero, Manuel**
	* For teaching me programming and giving me my first computer.
* **Sánchez Ordiñana, José Ismael (Vaporatorius)**
	* For validating tests on real hardware. <sup>[33](#r33), [34](#r34)</sup>
* **Stevenson, Dave**
	1. For testing the special RESET on real hardware. <sup>[11](#r11)</sup>
	2. For conducting low-level tests on real hardware. <sup>[35](#r35)</sup>
* **Weissflog, Andre (Floh)**
	1. For finding out that the `reti/retn` instructions defer the acceptance of the maskable interrupt. <sup>[36](#r36)</sup>
	2. For writing the _"Visual Z80 Remix"_ simulator. <sup>[37](#r37)</sup>
* **Wilkinson, Oli (evolutional)**
	* For validating tests on real hardware. <sup>[26](#r26)</sup>
* **Wlodek**
	* For testing the behavior of the MEMPTR register on real Z80 chips. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Woodmass, Mark (Woody)**
	1. For his invaluable contributions to the emuscene.
	2. For writing the _"Z80 Test Suite"_. <sup>[13](#r13)</sup>
	3. For his research on the flag behavior of the `ccf/scf` instructions. <sup>[38](#r38)</sup>
	4. For writing the _"HALT2INT"_ test.
	5. For writing the _"EIHALT"_ test.
* **Young, Sean**
	1. For his research work.
	2. For his technical documents about the Zilog Z80. <sup>[18](#r18), [29](#r29), [39](#r39)</sup>
* **ZXGuesser**
	* For validating tests on real hardware.

### References

1. <span id="r1">https://spectrumcomputing.co.uk/forums/viewtopic.php?p=83384#p83384</span>
2. <span id="r2">https://stardot.org.uk/forums/viewtopic.php?t=15464</span>
	* https://stardot.org.uk/forums/viewtopic.php?p=211042#p211042
	* https://stardot.org.uk/forums/viewtopic.php?p=212021#p212021
3. <span id="r3">Banks, David (2018-08-21). _"Undocumented Z80 Flags"_  revision 1.0.</span>
	* https://github.com/hoglet67/Z80Decoder/wiki/Undocumented-Flags
	* https://stardot.org.uk/forums/download/file.php?id=39831
4. <span id="r4">https://spectrumcomputing.co.uk/forums/viewtopic.php?p=83041#p83041</span>
5. <span id="r5">http://wizard.ae.krakow.pl/~jb/qaop/tests.html</span>
6. <span id="r6">https://zxpress.ru/zxnet/zxnet.pc/5909</span>
7. <span id="r7">https://zx-pk.ru/threads/2506-komanda-bit-n-(hl).html</span>
8. <span id="r8">https://zx-pk.ru/threads/2586-prosba-realshchikam-ot-emulyatorshchikov.html</span>
9. <span id="r9">boo_boo; Kladov, Vladimir (2006-03-29). _"MEMPTR, Esoteric Register of the Zilog Z80 CPU"_.</span>
	* http://zx.pk.ru/showpost.php?p=43688
	* http://zx.pk.ru/attachment.php?attachmentid=2984
	* http://zx.pk.ru/showpost.php?p=43800
	* http://zx.pk.ru/attachment.php?attachmentid=2989
10. <span id="r10">https://sourceforge.net/p/fuse-emulator/mailman/message/6929573</span>
11. <span id="r11">Brewer, Tony (2014-12). _"Z80 Special Reset"_.</span>
	* http://primrosebank.net/computers/z80/z80_special_reset.htm
12. <span id="r12">https://stardot.org.uk/forums/viewtopic.php?p=357136#p357136</span>
13. <span id="r13">https://worldofspectrum.org/forums/discussion/20345</span>
14. <span id="r14">https://groups.google.com/g/comp.os.cpm/c/HfSTFpaIkuU/m/KotvMWu3bZoJ</span>
15. <span id="r15">Cringle, Frank D. (1998-01-28). _"Yaze - Yet Another Z80 Emulator"_ v1.10.</span>
	* ftp://ftp.ping.de/pub/misc/emulators/yaze-1.10.tar.gz
16. <span id="r16">https://baltazarstudios.com/zilog-z80-undocumented-behavior</span>
17. <span id="r17">Flammenkamp, Achim. _"Interrupt Behaviour of the Z80 CPU"_.</span>
	* http://z80.info/interrup.htm
18. <span id="r18">Young, Sean (1998-10). _"Z80 Undocumented Features (in Software Behaviour)"_ v0.3.</span>
	* http://www.msxnet.org/tech/Z80/z80undoc.txt
19. <span id="r19">https://elmundodelspectrum.com/desenterrando-el-primer-emulador-de-spectrum</span>
20. <span id="r20">https://elmundodelspectrum.com/con-vosotros-el-emulador-de-pedro-gimeno-1989</span>
21. <span id="r21">https://sourceforge.net/p/fuse-emulator/mailman/message/4502844</span>
22. <span id="r22">Harston, Jonathan Graham (2008). _"Full Z80 Opcode List Including Undocumented Opcodes"_ v0.11 (revised).</span>
	* https://mdfs.net/Docs/Comp/Z80/OpList
23. <span id="r23">Harston, Jonathan Graham (2012). _"Z80 Microprocessor Undocumented Instructions"_ v0.15.</span>
	* https://mdfs.net/Docs/Comp/Z80/UnDocOps
24. <span id="r24">Harston, Jonathan Graham (2014). _"Z80 Opcode Map"_ v0.10 (revised).</span>
	* https://mdfs.net/Docs/Comp/Z80/OpCodeMap
25. <span id="r25">https://mdfs.net/Software/Z80/Exerciser/Spectrum</span>
26. <span id="r26">https://spectrumcomputing.co.uk/forums/viewtopic.php?t=6102</span>
27. <span id="r27">https://github.com/MrKWatkins/ZXSpectrumNextTests</span>
28. <span id="r28">https://spectrumcomputing.co.uk/forums/viewtopic.php?p=83157#p83157</span>
29. <span id="r29">Young, Sean (2005-09-18). _"Undocumented Z80 Documented, The"_ v0.91.</span>
	* http://www.myquest.nl/z80undocumented
	* http://www.myquest.nl/z80undocumented/z80-documented-v0.91.pdf
30. <span id="r30">https://worldofspectrum.org/forums/discussion/41704</span>
	* http://zxds.raxoft.cz/taps/misc/zexall2.zip
31. <span id="r31">https://worldofspectrum.org/forums/discussion/41834</span>
	* http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip
	* https://github.com/raxoft/z80test
32. <span id="r32">https://worldofspectrum.org/forums/discussion/34574</span>
33. <span id="r33">https://worldofspectrum.org/forums/discussion/comment/668760/#Comment_668760</span>
34. <span id="r34">https://jisanchez.com/test-a-dos-placas-de-zx-spectrum</span>
35. <span id="r35">https://stardot.org.uk/forums/viewtopic.php?p=212360#p212360</span>
36. <span id="r36">https://floooh.github.io/2021/12/17/cycle-stepped-z80.html</span>
37. <span id="r37">https://github.com/floooh/v6502r</span>
38. <span id="r38">http://groups.google.co.uk/group/comp.sys.sinclair/msg/56dd1fd4ccb5fb3b</span>
39. <span id="r39">Young, Sean (1997-09-21). _"Zilog Z80 CPU Specifications"_.
	* http://www.msxnet.org/tech/Z80/z80.zip

# License

Copyright © 1999-2022 Manuel Sainz de Baranda y Goñi.

<img src="http://zxe.io/software/Z80/images/lgplv3-147x51.png" align="right">

This library is [free software](https://www.gnu.org/philosophy/free-sw.html): you can redistribute it and/or modify it under the terms of the **[GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.en.html)** as published by the [Free Software Foundation](https://www.fsf.org), either version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but **WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE**. See the GNU Lesser General Public License for more details.

You should have received a [copy](COPYING.LESSER) of the GNU Lesser General Public License along with this library. If not, see <https://www.gnu.org/licenses/>.

# Special licensing

Projects where the terms of the GNU Lesser General Public License prevent the use of this library, or require unwanted publication of the source code of commercial products, may [apply for a special license](mailto:manuel@zxe.io?subject=Z80).
