<h1 align="center">
	<img src="https://upload.wikimedia.org/wikipedia/commons/1/19/Zilog_Z80.jpg"><br>
	<b>Zilog Z80 CPU Emulator</b>
</h1>
<p align="center">
	<a href="https://zxe.io/software/Z80/documentation/latest">
		<img alt="Documentation" src="https://img.shields.io/badge/Documentation-informational?color=2675f5">
	</a>
	<a href="https://github.com/redcode/Z80/actions/workflows/build_and_test_library.yml">
		<img src="https://github.com/redcode/Z80/actions/workflows/build_and_test_library.yml/badge.svg">
	</a>
	<a href="https://github.com/redcode/Z80/actions/workflows/build_documentation.yml">
		<img src="https://github.com/redcode/Z80/actions/workflows/build_documentation.yml/badge.svg">
	</a>
	<a href="https://discord.gg/zU4dkzp8Mv">
		<img alt="Discord" src="https://img.shields.io/discord/848534208129138738?color=5865F2&label=Discord&logo=discord&logoColor=white">
	</a>
	<img src="https://awesome.re/mentioned-badge.svg">
</p>

# Introduction

The [Z80 library](https://zxe.io/software/Z80) implements a fast, small and accurate [emulator](https://en.wikipedia.org/wiki/Emulator) of the [Zilog Z80](https://en.wikipedia.org/wiki/Zilog_Z80). It emulates all that is known to date about this CPU, including the undocumented behaviors, [MEMPTR](https://zxpress.ru/zxnet/zxnet.pc/5909), [Q](https://worldofspectrum.org/forums/discussion/41704) and the [special RESET](http://www.primrosebank.net/computers/z80/z80_special_reset.htm). It also has the honor of having been the first open-source project to provide full emulation of the interrupt mode 0.

The source code is written in [ANSI C](https://en.wikipedia.org/wiki/ANSI_C) for maximum portability and is extensively commented. The aim has been to write a well-structured, easy to understand piece of software; something solid and elegant that can stand the test of time with no need for major changes.

# Accuracy

The Zilog Z80 emulator has a classic design with instruction-level granularity. This provides the best performance while still offering a reasonable flexibility to achieve [precision down to the T-state level](https://github.com/Agaxia/Z80Plus).

Instruction-level granularity means that, except in a few well-defined cases, the execution of a given instruction cannot stop until all its internal M-cycles are processed. In addition, this type of emulation has been optimized for speed, which implies that the registers are modified only once per instruction and the [T-state counter](https://zxe.io/software/Z80/documentation/latest/APIReference.html#c.Z80.cycles) is normally updated only after a full instruction has been executed.

That said, instructions, flags, memory accesses, interrupt and reset responses, clock cycles, etc., are accurately emulated as far as is known, according to the [technical documentation](https://github.com/redcode/Z80/wiki/Technical-literature) available, the findings made after decades of research on the Z80 and [electronic simulations](https://github.com/redcode/Z80/wiki/Simulators). And, of course, the emulator passes the most exhaustive [tests](https://github.com/redcode/Z80/wiki/Tests) written to date, including all three major test suites:

### <span id="major_test_suites">[Zilog Z80 CPU Test Suite](https://github.com/raxoft/z80test), by [Patrik Rak](https://github.com/raxoft)</span>

This set of programs is intended to help the emulator authors to reach the desired level of the CPU emulation authenticity. Each of the included programs performs an exhaustive computation using each of the tested Z80 instructions, compares the results with values obtained from a real [Sinclair ZX Spectrum 48K](https://en.wikipedia.org/wiki/ZX_Spectrum) with Zilog Z80 CPU, and reports any deviations detected.

<details>
	<summary>Results (v1.0)</summary>
	<table>
		<tr>
			<td align="center" valign="top" align="center">
				<a href="https://web.archive.org/web/20210401091605if_/http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80full.tap</b>
				</a>
				<br>
				<sup>Tests all flags and registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20210401091605if_/http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80doc.tap</b>
				</a>
				<br>
				<sup>Tests all registers, but only officially documented flags.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Full.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Doc.gif">
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20210401091605if_/http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80flags.tap</b>
				</a>
				<br>
				<sup>Tests all flags, ignores registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20210401091605if_/http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80docflags.tap</b>
				</a>
				<br>
				<sup>Tests documented flags only, ignores registers.</sup>
			</td>
		</tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Flags.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20Doc%20Flags.gif">
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20210401091605if_/http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80ccf.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>ccf</code> after each instruction tested.</sup>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20210401091605if_/http://zxds.raxoft.cz/taps/misc/z80test-1.0.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80memptr.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>bit n,(hl)</code> after each instruction tested.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20CCF.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.0%20%282012-12-08%29%28Rak,%20Patrik%29%20-%20MEMPTR.gif">
			</td>
		</tr>
	</table>
</details>
<details>
	<summary>Results (v1.2)</summary>
	<table>
		<tr>
			<td align="center" valign="top" align="center">
				<a href="https://web.archive.org/web/20221202204115if_/http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80full.tap</b>
				</a>
				<br>
				<sup>Tests all flags and registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20221202204115if_/http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80doc.tap</b>
				</a>
				<br>
				<sup>Tests all registers, but only officially documented flags.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Full.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Doc.gif">
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20221202204115if_/http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80flags.tap</b>
				</a>
				<br>
				<sup>Tests all flags, ignores registers.</sup>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20221202204115if_/http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80docflags.tap</b>
				</a>
				<br>
				<sup>Tests documented flags only, ignores registers.</sup>
			</td>
		</tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Flags.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20Doc%20Flags.gif">
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20221202204115if_/http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80ccf.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>ccf</code> after each instruction tested.</sup>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20221202204115if_/http://zxds.raxoft.cz/taps/misc/z80test-1.2.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80memptr.tap</b>
				</a>
				<br>
				<sup>Tests all flags after executing <code>bit n,(hl)</code> after each instruction tested.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20CCF.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Zilog%20Z80%20CPU%20Test%20Suite%20v1.2%20%282022-01-26%29%28Rak,%20Patrik%29%20-%20MEMPTR.gif">
			</td>
		</tr>
	</table>
</details>

### Z80 Test Suite, by Mark Woodmass

This suite performs a series of tests to verify the MEMPTR documents <sup>_([English](https://zx-pk.ru/attachment.php?attachmentid=2989), [Russian](https://zx-pk.ru/attachment.php?attachmentid=2984))_</sup>, which are spot on, as well as a brief run through several of the `CBh/DDh/FDh` opcode ranges. The test results in the program are compared against those from a [NEC D780C-1](https://www.cpu-world.com/CPUs/Z80/NEC-D780C-1.html) CPU, but Simon Conway kindly tested several other Z80 clones, confirming the same results.

<details>
	<summary>Results</summary>
	<table>
		<tr>
			<td align="center" valign="top" colspan="2">
				<a href="https://web.archive.org/web/20130511214919if_/http://homepage.ntlworld.com/mark.woodmass/z80tests.tap">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>z80tests.tap</b>
				</a>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Z80%20Test%20Suite%20%282008%29%28Woodmass,%20Mark%29%20-%20Flags.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Z80%20Test%20Suite%20%282008%29%28Woodmass,%20Mark%29%20-%20MEMPTR.gif">
			</td>
		</tr>
	</table>
</details>

### Z80 Instruction Set Exerciser, by Frank D. Cringle

Frank Cringle's _Z80 Instruction Set Exerciser_ attempts to execute every Z80 opcode, putting them through a cycle of tests and comparing the results to actual results from running the code on a real Z80. The exerciser is supplied with Frank's [Yaze](ftp://ftp.ping.de/pub/misc/emulators/yaze-1.10.tar.gz) (Yet Another Z80 Emulator). It is often difficult to track down, so [Jonathan Graham Harston](https://mdfs.net/User/JGH) put it together [here](https://mdfs.net/Software/Z80/Exerciser), as well as [some conversions](https://mdfs.net/Software/Z80/Exerciser/Spectrum). The latest release of Yaze is available at [Andreas Gerlich's website](https://www.mathematik.uni-ulm.de/users/ag/yaze-ag).

<details>
	<summary>Results (Jonathan Graham Harston's version)</summary>
	<table>
		<tr>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20200806155334if_/http://mdfs.net/Software/Z80/Exerciser/Spectrum/zexdoc.tap">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>zexdoc.tap</b>
				</a>
				<br>
				<sup>Tests officially documented flag effects.</sup>
			</td>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20181216034526if_/http://mdfs.net/Software/Z80/Exerciser/Spectrum/zexall.tap">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>zexall.tap</b>
				</a>
				<br>
				<sup>Tests all flag changes.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Z80%20Documented%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282018%29%28Harston,%20Jonathan%20Graham%29.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Z80%20Full%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282018%29%28Harston,%20Jonathan%20Graham%29.gif">
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
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>zexfix.tap</b>
				</a>
				<br>
				<sup>Tests all flag changes.</sup>
			</td>
			</td>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20120527200327if_/http://wizard.ae.krakow.pl:80/~jb/qaop/t/zexbit.tap">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>zexbit.tap</b>
				</a>
				<br>
				<sup>Tests all flag changes of the <code>bit</code> instructions.</sup>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Z80%20Full%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282009%29%28Bobrowski,%20Jan%29.gif">
			</td>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Z80%20Full%20Instruction%20Set%20Exerciser%20for%20Spectrum%20%282011%29%28Bobrowski,%20Jan%29%28Narrowed%20to%20BIT%20Instructions%29.gif">
			</td>
		</tr>
	</table>
</details>
<details>
	<summary>Results (Patrik Rak's version)</summary>
	<table>
		<tr>
			<td align="center" valign="top">
				<a href="https://web.archive.org/web/20221202204406if_/http://zxds.raxoft.cz/taps/misc/zexall2.zip">
					<img src="https://zxe.io/software/Z80/images/TAP.png" width="88"><br>
					<b>zexall2.tap</b>
				</a>
			</td>
		</tr>
		<tr>
			<td align="center" valign="top">
				<img src="https://zxe.io/software/Z80/tests/screenshots/ZX%20Spectrum/Z80%20Instruction%20Set%20Exerciser%20for%20Spectrum%202%20v0.1%20%282012-11-27%29%28Rak,%20Patrik%29.gif">
			</td>
		</tr>
	</table>
</details>

# Installation

### <sub><img src="https://zxe.io/software/Z80/images/debian-icon.svg" height="24"></sub> Debian/Ubuntu

First, add the `zxe` repository and update the package index:

```shell
sudo mkdir -pm700 /root/.gnupg
sudo mkdir -pm755 /etc/apt/keyrings
sudo gpg --no-default-keyring --keyring /etc/apt/keyrings/zxe-archive-keyring.gpg --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys FE214A38D6A0C01D9AF514EE841EA3BD3A7E1487
echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/zxe-archive-keyring.gpg] https://zxe.io/repos/apt stable main" | sudo tee /etc/apt/sources.list.d/zxe.list
sudo apt update
```

Next, install the library package:

```shell
sudo apt install libz80
```

In case you need to build software that requires the Z80 library, install the development package too:

```shell
sudo apt install libz80-dev
````

### <sub><img src="https://zxe.io/software/Z80/images/gentoo-icon.svg" height="24"></sub> Gentoo Linux

First, add and sync the [`zxe`](https://github.com/redcode/zxe-gentoo-overlay) overlay:

```shell
eselect repository add zxe git https://github.com/redcode/zxe-gentoo-overlay.git
emaint sync --repo zxe
```

Then, install the library:

```shell
emerge emulation-libs/z80
```

### <sub><img src="https://zxe.io/software/Z80/images/homebrew-icon.svg" height="24"></sub> Homebrew

```shell
brew install redcode/zxe/z80
```

# Installation from sources

You will need [CMake](https://cmake.org) v3.14 or later to build the package and, optionally, recent versions of [Doxygen](https://www.doxygen.nl), [Sphinx](https://www.sphinx-doc.org) and [Breathe](https://www.breathe-doc.org) to compile the documentation. Also, make sure that you have [LaTeX](https://www.latex-project.org) with PDF support installed on your system if you want to generate the documentation in PDF format.

The emulator requires some types and macros included in [Zeta](https://github.com/redcode/Zeta), a dependency-free, [header-only](https://en.wikipedia.org/wiki/Header-only) library used to retain compatibility with most C compilers. Install Zeta or extract its [source code package](https://zeta.st/download) to the root directory of the Z80 project or its parent directory. Zeta is the sole dependency; the emulator is a freestanding implementation and as such does not depend on the [C standard library](https://en.wikipedia.org/wiki/C_standard_library).

Once the prerequisites are met, create a directory and run `cmake` from there to prepare the build system:

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

* <span id="option_BUILD_SHARED_LIBS">**<code>-D[BUILD_SHARED_LIBS](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)=(YES|NO)</code>**</span>  
	Build the emulator as a shared library, rather than static.  
	The default is `NO`.

* <span id="option_CMAKE_BUILD_TYPE">**<code>-D[CMAKE_BUILD_TYPE](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)=(Debug|Release|RelWithDebInfo|MinSizeRel)</code>**</span>  
	Choose the type of build (configuration) to generate.  
	The default is `Release`.

* <span id="option_CMAKE_INSTALL_PREFIX">**<code>-D[CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html)="\<path\>"</code>**</span>  
	Specify the installation prefix on [UNIX](https://en.wikipedia.org/wiki/Unix) and [UNIX-like](https://en.wikipedia.org/wiki/Unix-like) operating systems.  
	The default is `"/usr/local"`.

<span id="cmake_package_options">Package-specific options</span> are prefixed with `Z80_` and can be divided into two groups. The first one controls aspects not related to the source code of the library:

* <span id="option_Z80_DEPOT_LOCATION">**`-DZ80_DEPOT_LOCATION="<location>"`**</span>  
	Specify the directory or URL of the depot containing the test files (i.e., the firmware and software required by the [testing tool](#running-the-tests)).  
	The default is `"http://zxe.io/depot"`.

* <span id="option_Z80_FETCH_TEST_FILES">**`-DZ80_FETCH_TEST_FILES=(YES|NO)`**</span>  
	Copy or download the test files from the depot to the build directory.  
	The default is `NO`.

* <span id="option_Z80_INSTALL_CMAKEDIR">**`-DZ80_INSTALL_CMAKEDIR="<path>"`**</span>  
	Specify the directory in which to install the CMake [config-file package](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages).  
	The default is <code>"${[CMAKE_INSTALL_LIBDIR](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)}/cmake/Z80"</code>.

* <span id="option_Z80_INSTALL_PKGCONFIGDIR">**`-DZ80_INSTALL_PKGCONFIGDIR="<path>"`**</span>  
	Specify the directory in which to install the [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config) [file](https://people.freedesktop.org/~dbn/pkg-config-guide.html).  
	The default is <code>"${[CMAKE_INSTALL_LIBDIR](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)}/pkgconfig"</code>.

* <span id="option_Z80_NOSTDLIB_FLAGS">**`-DZ80_NOSTDLIB_FLAGS=(Auto|"[<flag>[;<flag>...]]")`**</span>  
	Specify the linker flags used to avoid linking against system libraries.  
	The default is `Auto` (autoconfigure flags). If you get linker errors, set this option to `""`.

* <span id="option_Z80_OBJECT_LIBS">**`-DZ80_OBJECT_LIBS=(YES|NO)`**</span>  
	Build the emulator as an [object library](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#object-libraries).  
	This option takes precedence over [`BUILD_SHARED_LIBS`](#option_BUILD_SHARED_LIBS) and [`Z80_SHARED_LIBS`](#option_Z80_SHARED_LIBS). If enabled, the build system will ignore [`Z80_WITH_CMAKE_SUPPORT`](#option_Z80_WITH_CMAKE_SUPPORT) and [`Z80_WITH_PKGCONFIG_SUPPORT`](#option_Z80_WITH_PKGCONFIG_SUPPORT), as no libraries or support files will be installed.  
	The default is `NO`.

* <span id="option_Z80_SHARED_LIBS">**`-DZ80_SHARED_LIBS=(YES|NO)`**</span>  
	Build the emulator as a shared library, rather than static.  
	This option takes precedence over [`BUILD_SHARED_LIBS`](#option_BUILD_SHARED_LIBS).  
	Not defined by default.

* <span id="option_Z80_SPHINX_HTML_THEME">**`-DZ80_SPHINX_HTML_THEME="[<name>]"`**</span>  
	Specify the Sphinx theme for the documentation in HTML format.  
	The default is `""` (use the default theme).

* <span id="option_Z80_WITH_CMAKE_SUPPORT">**`-DZ80_WITH_CMAKE_SUPPORT=(YES|NO)`**</span>  
	Generate and install the CMake [config-file package](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages).  
	The default is `NO`.

* <span id="option_Z80_WITH_HTML_DOCUMENTATION">**`-DZ80_WITH_HTML_DOCUMENTATION=(YES|NO)`**</span>  
	Build and install the documentation in HTML format.  
	It requires Doxygen, Sphinx and Breathe.  
	The default is `NO`.

* <span id="option_Z80_WITH_PDF_DOCUMENTATION">**`-DZ80_WITH_PDF_DOCUMENTATION=(YES|NO)`**</span>  
	Build and install the documentation in PDF format.  
	It requires Doxygen, Sphinx, Breathe and LaTeX with PDF support.  
	The default is `NO`.

* <span id="option_Z80_WITH_PKGCONFIG_SUPPORT">**`-DZ80_WITH_PKGCONFIG_SUPPORT=(YES|NO)`**</span>  
	Generate and install the [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config) [file](https://people.freedesktop.org/~dbn/pkg-config-guide.html).  
	The default is `NO`.

* <span id="option_Z80_WITH_STANDARD_DOCUMENTS">**`-DZ80_WITH_STANDARD_DOCUMENTS=(YES|NO)`**</span>  
	Install the standard text documents distributed with the package: [`AUTHORS`](AUTHORS), [`COPYING`](COPYING), [`COPYING.LESSER`](COPYING.LESSER), [`HISTORY`](HISTORY), [`README`](README) and [`THANKS`](THANKS).  
	The default is `NO`.

* <span id="option_Z80_WITH_TESTS">**`-DZ80_WITH_TESTS=(YES|NO)`**</span>  
	Build the [testing tool](#running-the-tests).  
	The default is `NO`.

<span id="cmake_source_code_options">The second group of package-specific options</span> configures the source code of the library by predefining macros that enable [optional features](https://zxe.io/software/Z80/documentation/latest/Introduction.html#optional-features):

* <span id="option_Z80_WITH_EXECUTE">**`-DZ80_WITH_EXECUTE=(YES|NO)`**</span>  
	Build the implementation of the [`z80_execute`](https://zxe.io/software/Z80/documentation/latest/APIReference.html#c.z80_execute) function.  
	The default is `NO`.

* <span id="option_Z80_WITH_FULL_IM0">**`-DZ80_WITH_FULL_IM0=(YES|NO)`**</span>  
	Build the full implementation of the interrupt mode 0 rather than the reduced one.  
	The default is `NO`.

* <span id="option_Z80_WITH_IM0_RETX_NOTIFICATIONS">**`-DZ80_WITH_IM0_RETX_NOTIFICATIONS=(YES|NO)`**</span>  
	Enable optional notifications for any `reti` or `retn` instruction executed during the interrupt mode 0 response.  
	The default is `NO`.

* <span id="option_Z80_WITH_Q">**`-DZ80_WITH_Q=(YES|NO)`**</span>  
	Build the implementation of [Q](https://worldofspectrum.org/forums/discussion/41704).  
	The default is `NO`.

* <span id="option_Z80_WITH_SPECIAL_RESET">**`-DZ80_WITH_SPECIAL_RESET=(YES|NO)`**</span>  
	Build the implementation of the [special RESET](http://www.primrosebank.net/computers/z80/z80_special_reset.htm).  
	The default is `NO`.

* <span id="option_Z80_WITH_UNOFFICIAL_RETI">**`-DZ80_WITH_UNOFFICIAL_RETI=(YES|NO)`**</span>  
	Configure the `ED5Dh`, `ED6Dh` and `ED7Dh` undocumented instructions as `reti` instead of `retn`.  
	The default is `NO`.

* <span id="option_Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG">**`-DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=(YES|NO)`**</span>  
	Build the implementation of the bug affecting the Zilog Z80 NMOS, which causes the P/V flag to be reset when a maskable interrupt is accepted during the execution of the `ld a,{i|r}` instructions.  
	The default is `NO`.

Package maintainers are encouraged to use at least the following options for the shared library:

```shell
-DZ80_WITH_EXECUTE=YES
-DZ80_WITH_FULL_IM0=YES
-DZ80_WITH_Q=YES
-DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=YES
```

Finally, once the build system is configured according to your needs, build and install the package:

```shell
cmake --build . [--config (Debug|Release|RelWithDebInfo|MinSizeRel)]
cmake --install . [--strip]
```

The [`--config`](https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-build-config) option is only necessary for those [CMake generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) that ignore [`CMAKE_BUILD_TYPE`](#option_CMAKE_BUILD_TYPE) (e.g., Xcode and Visual Studio). For non-debug builds of the shared library, use [`--strip`](https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-install-strip) to discard debugging information and non-public symbols from the installed binary.

### TL;DR

Use the following to build the emulator as a shared library and install it along with the development files into `$HOME/.local`:

```shell
mkdir work && cd work
git clone https://github.com/redcode/Zeta.git
git clone https://github.com/redcode/Z80.git
cd Zeta
mkdir build && cd build
cmake .. \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$HOME/.local \
	-DZeta_WITH_CMAKE_SUPPORT=YES \
	-DZeta_WITH_PKGCONFIG_SUPPORT=YES
cmake --install .
cd ../../Z80
mkdir build && cd build
cmake .. \
	-DBUILD_SHARED_LIBS=YES \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$HOME/.local \
	-DZ80_WITH_CMAKE_SUPPORT=YES \
	-DZ80_WITH_PKGCONFIG_SUPPORT=YES \
	-DZ80_WITH_EXECUTE=YES \
	-DZ80_WITH_FULL_IM0=YES \
	-DZ80_WITH_Q=YES \
	-DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=YES
cmake --build . --config Release
cmake --install . --strip
```

# Running the tests

The package includes a tool called `test-Z80` capable of running the most relevant [CP/M](https://en.wikipedia.org/wiki/CP/M) and [ZX Spectrum](https://en.wikipedia.org/wiki/ZX_Spectrum) versions of [the major test suites](#major_test_suites). Configure the build system with <code>-D[Z80_WITH_TESTS](#option_Z80_WITH_TESTS)=YES</code> to enable its compilation and <code>-D[Z80_FETCH_TEST_FILES](#option_Z80_FETCH_TEST_FILES)=YES</code> to download the firmware and software required. Also note that the Z80 library must be built with <code>-D[Z80_WITH_Q](#option_Z80_WITH_Q)=YES</code> to be able to pass [Patrik Rak's tests](#zilog-z80-cpu-test-suite-by-patrik-rak).

Once you have built the package, type the following to run all tests:

```shell
./test-Z80 -p depot/firmware -p depot/software/POSIX -p "depot/software/ZX Spectrum" -a
```

The tool supports options and can run the tests individually (type `./test-Z80 -h` for help). If you prefer to run all tests through [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html), use this command:

```shell
ctest --verbose --build-config (Debug|Release|RelWithDebInfo|MinSizeRel)
```

The complete logs generated by `test-Z80` emulating different CPU variants are available here:

* [Zilog NMOS](https://zxe.io/software/Z80/tests/logs/test-Z80%20v0.2%20Results%20-%20Zilog%20NMOS.txt)
* [Zilog CMOS](https://zxe.io/software/Z80/tests/logs/test-Z80%20v0.2%20Results%20-%20Zilog%20CMOS.txt)
* [NEC NMOS](https://zxe.io/software/Z80/tests/logs/test-Z80%20v0.2%20Results%20-%20NEC%20NMOS.txt)
* [ST CMOS](https://zxe.io/software/Z80/tests/logs/test-Z80%20v0.2%20Results%20-%20ST%20CMOS.txt)

> **Note**: The CRC errors in the logs of the NEC NMOS variant **are normal** and match the [values obtained on real hardware](https://jisanchez.com/test-a-dos-placas-de-zx-spectrum). The ST CMOS variant is currently under investigation.

### TL;DR

Use the following to build and test the emulator:

```shell
mkdir work && cd work
git clone https://github.com/redcode/Zeta.git
git clone https://github.com/redcode/Z80.git
cd Z80
mkdir build && cd build
cmake .. \
	-DCMAKE_BUILD_TYPE=Release \
	-DZ80_FETCH_TEST_FILES=YES \
	-DZ80_WITH_TESTS=YES \
	-DZ80_WITH_EXECUTE=YES \
	-DZ80_WITH_FULL_IM0=YES \
	-DZ80_WITH_Q=YES \
	-DZ80_WITH_ZILOG_NMOS_LD_A_IR_BUG=YES
cmake --build . --config Release
ctest --verbose --build-config Release
```

# Integration

### As an external dependency in CMake-based projects

The Z80 library [includes](#option_Z80_WITH_CMAKE_SUPPORT) a [config-file package](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages) for integration into CMake-based projects that must be installed for development. Use [`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html) to find the `Z80` package. This creates the `Z80` imported library target, which carries the necessary transitive link dependencies. Optionally, the linking method can be selected by specifying either the `Shared` or `Static` component.

Example:

```cmake
find_package(Z80 REQUIRED Shared)
target_link_libraries(your-target Z80)
```

When not specified as a component, the linking method is selected according to [`Z80_SHARED_LIBS`](#option_Z80_SHARED_LIBS). If this option is not defined, the config-file uses the type of library that is installed on the system and, if it finds both the shared and the static versions, [`BUILD_SHARED_LIBS`](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html) determines which one to link against.

### As a CMake subproject

To embed the Z80 library as a CMake subproject, extract the source code packages of [Zeta](https://zeta.st/download) and [Z80](https://zxe.io/software/Z80/download) (or clone their respective repositories) into a subdirectory of another project. Then use [`add_subdirectory`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html) in the parent project to add the Z80 source code tree to the build process (N.B., the Z80 subproject will automatically find Zeta and import it as an [interface library](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#interface-libraries)).

It is advisable to configure the Z80 library in the `CMakeLists.txt` of the parent project. This will prevent the user from having to specify [configuration options for the Z80 subproject](#cmake_package_options) through the command line when building the main project.

Example:

```cmake
set(Z80_SHARED_LIBS                 NO  CACHE BOOL "")
set(Z80_WITH_Q                      YES CACHE BOOL "")
set(Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG YES CACHE BOOL "")

add_subdirectory(dependencies/Z80)
target_link_libraries(your-target Z80)
```

It is important to set the [`Z80_SHARED_LIBS`](#option_Z80_SHARED_LIBS) option. Otherwise, CMake will build the library type indicated by [`BUILD_SHARED_LIBS`](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html), which may not be the desired one.

### Integrating the source code

There are several macros that can be used to configure the source code of the library. You can define those you need in your build system or at the beginning of the `Z80.c` file. The following ones allow you to configure the integration of `Z80.h` and `Z80.c` into the project:

* **`#define Z80_DEPENDENCIES_HEADER "header name.h"`**  
	Specifies the only external header to `#include`, replacing those of Zeta. If you compile `Z80.c` with this macro defined, you must also define it before including `"Z80.h"` or `<Z80.h>`.

* **`#define Z80_STATIC`**  
	Required to compile and/or use the emulator as a static library or as an internal part of another project. If you compile `Z80.c` with this macro defined, you must also define it before including `"Z80.h"` or `<Z80.h>`.

* **`#define Z80_WITH_LOCAL_HEADER`**  
	Tells `Z80.c` to `#include "Z80.h"` instead of `<Z80.h>`.

[The second group of package-specific options](#cmake_source_code_options), explained in the _[Installation from sources](#installation-from-sources)_ section of this document, activates various optional features in the source code by predefining the following macros:

* **<code>#define [Z80_WITH_EXECUTE](#option_Z80_WITH_EXECUTE)</code>**
* **<code>#define [Z80_WITH_FULL_IM0](#option_Z80_WITH_FULL_IM0)</code>**
* **<code>#define [Z80_WITH_IM0_RETX_NOTIFICATIONS](#option_Z80_WITH_IM0_RETX_NOTIFICATIONS)</code>**
* **<code>#define [Z80_WITH_Q](#option_Z80_WITH_Q)</code>**
* **<code>#define [Z80_WITH_SPECIAL_RESET](#option_Z80_WITH_SPECIAL_RESET)</code>**
* **<code>#define [Z80_WITH_UNOFFICIAL_RETI](#option_Z80_WITH_UNOFFICIAL_RETI)</code>**
* **<code>#define [Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG](#option_Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG)</code>**

Except for `Z80_DEPENDENCIES_HEADER`, the above macros do not need to be defined as any value; the source code only checks whether they are defined.

> **Note**: The activation of some of the optional features affects the speed of the emulator due to various factors (read the [documentation](https://zxe.io/software/Z80/documentation/latest/Introduction.html#optional-features) for more details).

# Showcase

This emulator has been used by the following projects (listed in alphabetical order):

* **[Augmentinel](https://simonowen.com/spectrum/augmentinel/)** <sub>_by [Simon Owen](https://simonowen.com)_</sub> — [GitHub](https://github.com/simonowen/augmentinel)
* **CPM-Emulator** <sub>_by [Marc Sibert](https://github.com/Marcussacapuces91)_</sub> — [GitHub](https://github.com/Marcussacapuces91/CPM-Emulator)
* **F80: simple Z80 system running on an FPGA** <sub>_by [Richard J. Prinz](https://www.min.at/prinz)_</sub> — [GitHub](https://github.com/rprinz08/f80)
* **[Google Capture the Flag (2021)](https://capturetheflag.withgoogle.com)** <sub>_by [Google](https://www.google.com)_</sub> — [GitHub](https://github.com/google/google-ctf)
* **pico-sorcerer-2** <sub>_by [fruit-bat](https://github.com/fruit-bat)_</sub> — [GitHub](https://github.com/fruit-bat/pico-sorcerer-2)
* **pico-zxspectrum** <sub>_by [fruit-bat](https://github.com/fruit-bat)_</sub> — [GitHub](https://github.com/fruit-bat/pico-zxspectrum)
* **[tihle: a unique TI calculator emulator](https://www.taricorp.net/2020/introducing-tihle/)** <sub>_by [Peter Marheine](https://www.taricorp.net/about/)_</sub> — [GitHub](https://github.com/tari/tihle) · [GitLab](https://gitlab.com/taricorp/tihle)
* **[TileMap](https://simonowen.com/spectrum/tilemap/)** <sub>_by [Simon Owen](https://simonowen.com/)_</sub> — [GitHub](https://github.com/simonowen/tilemap)
* **<a href="https://eaw.app/tranzputer-fusionx">tranZPUter<sup>FusionX</sup></a>** <sub>_by [Philip Smart](https://eaw.app/about/)_</sub> — [GitHub](https://github.com/pdsmart/tzpuFusionX)
* **Z80Plus** <sub>_by [Sofía Ortega Sosa](https://github.com/Agaxia)_</sub> — [GitHub](https://github.com/Agaxia/Z80Plus)
* **Zemu** <sub>_by [Jay Valentine](https://jayvalentine.github.io/)_</sub> — [GitHub](https://github.com/jayvalentine/zemu) · [RubyGems](https://rubygems.org/gems/zemu)

# Thanks

Many thanks to the following individuals (in alphabetical order):

* **Akimov, Vadim (lvd)**
	* For testing the library on many different platforms and CPU architectures.
* **azesmbog**
	1. For validating tests on real hardware. <sup>[1](#r1)</sup>
	2. For his research on the unstable flag behavior of the `ccf/scf` instructions.
	3. For his invaluable help.
* **Banks, David (hoglet)** :trophy:
	1. For cracking the flag behavior of the block instructions. <sup>[2](#r2), [3](#r3)</sup>
	2. For his research on the flag behavior of the `ccf/scf` instructions. <sup>[3](#r3)</sup>
* **Beliansky, Anatoly (Tolik_Trek)**
	* For validating tests on real hardware. <sup>[4](#r4)</sup>
* **Bobrowski, Jan**  
	* For fixing the _"Z80 Full Instruction Set Exerciser for Spectrum"_. <sup>[5](#r5)</sup>
* **boo_boo** :trophy:
	* For cracking the behavior of the MEMPTR register. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Brady, Stuart**
	* For his research on the flag behavior of the `ccf/scf` instructions. <sup>[10](#r10)</sup>
* **Brewer, Tony** :trophy:
	1. For his research on the special RESET. <sup>[11](#r11), [12](#r12)</sup>
	2. For helping to crack the flag behavior of the block instructions. <sup>[2](#r2)</sup>
	3. For conducting low-level tests on real hardware. <sup>[2](#r2)</sup>
	4. For helping me to test different undocumented behaviors of the Zilog Z80.
* **Bystrov, Dmitry (Alone Coder)**
	* For validating tests on real hardware. <sup>[4](#r4)</sup>
* **Chandler, Richard**
	* For his corrections to the documentation.
* **Chunin, Roman (CHRV)**
	* For testing the behavior of the MEMPTR register on real Z80 chips. <sup>[6](#r6), [7](#r7), [8](#r8), [9](#r9)</sup>
* **Conway, Simon (BadBeard)**
	* For validating the _"Z80 Test Suite"_ on several Z80 clones. <sup>[13](#r13)</sup>
* **Cooke, Simon** :trophy:
	* For finding out how the `out (c),0` instruction behaves on the Zilog Z80 CMOS. <sup>[14](#r14)</sup>
* **Cringle, Frank D.**
	* For writing the _"Z80 Instruction Set Exerciser"_. <sup>[15](#r15)</sup>
* **Devic, Goran**
	* For his research on undocumented behaviors of the Z80 CPU. <sup>[16](#r16)</sup>
* **Equinox**
	* For his corrections to the documentation.
* **Flammenkamp, Achim**
	* For his article on Z80 interrupts. <sup>[17](#r17)</sup>
* **Gimeno Fortea, Pedro** :trophy:
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
* **ICEknight**
	* For validating tests on real hardware.
* **Kladov, Vladimir** :trophy:
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
	1. For optimizing the emulator.
	2. For her support.
* **Owen, Simon**
	* For the idea of the hooking method used in this emulator.
* **Ownby, Matthew P.**
	* For his research on the state of the registers after POWER-ON. <sup>[29](#r29)</sup>
* **Rak, Patrik** :trophy:
	1. For improving the _"Z80 Instruction Set Exerciser for Spectrum"_. <sup>[30](#r30)</sup>
	2. For cracking the flag behavior of the `ccf/scf` instructions. <sup>[13](#r13), [30](#r30)</sup>
	3. For writing the _"Zilog Z80 CPU Test Suite"_. <sup>[30](#r30), [31](#r31)</sup>
	4. For his research on the unstable flag behavior of the `ccf/scf` instructions.
* **Rodríguez Jódar, Miguel Ángel (mcleod_ideafix)**
	* For his research on the state of the registers after POWER-ON/RESET. <sup>[32](#r32)</sup>
* **Rodríguez Palomino, Mario (r-lyeh)**
	* For teaching me how emulators work.
* **Sainz de Baranda y Romero, Manuel**
	* For teaching me programming and giving me my first computer.
* **Sánchez Ordiñana, José Ismael (Vaporatorius)**
	* For validating tests on real hardware. <sup>[33](#r33), [34](#r34)</sup>
* **Stevenson, Dave**
	1. For testing the special RESET on real hardware. <sup>[11](#r11)</sup>
	2. For conducting low-level tests on real hardware. <sup>[35](#r35)</sup>
* **Vučenović, Zoran**
	* For writing the Pascal binding.
* **Weissflog, Andre (Floh)** :trophy:
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
* **Young, Sean** :trophy:
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
	* https://zx-pk.ru/showpost.php?p=43688
	* https://zx-pk.ru/attachment.php?attachmentid=2984
	* https://zx-pk.ru/showpost.php?p=43800
	* https://zx-pk.ru/attachment.php?attachmentid=2989
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
38. <span id="r38">https://groups.google.com/g/comp.sys.sinclair/c/WPsPr6j6w5k/m/O_u1zNQf3VYJ</span>
39. <span id="r39">Young, Sean (1997-09-21). _"Zilog Z80 CPU Specifications"_.
	* http://www.msxnet.org/tech/Z80/z80.zip

# License

Copyright © 1999-2023 Manuel Sainz de Baranda y Goñi.

<img src="https://zxe.io/software/Z80/images/lgplv3-147x51.png" width="147" align="right">

This library is [free software](https://www.gnu.org/philosophy/free-sw.html): you can redistribute it and/or modify it under the terms of the **[GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.html)** as published by the [Free Software Foundation](https://www.fsf.org), either version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but **WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE**. See the GNU Lesser General Public License for more details.

You should have received a [copy](COPYING.LESSER) of the GNU Lesser General Public License along with this library. If not, see <https://www.gnu.org/licenses/>.

# Special licensing

Projects where the terms of the GNU Lesser General Public License prevent the use of this library, or require unwanted publication of the source code of commercial products, may [apply for a special license](mailto:manuel@zxe.io?subject=Z80).
