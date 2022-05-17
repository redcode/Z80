# Z80 - CMakeLists.txt
#     ______  ______ ______
#    /\___  \/\  __ \\  __ \
#    \/__/  /\_\  __ \\ \/\ \
#       /\_____\\_____\\_____\
# Zilog \/_____//_____//_____/ CPU Emulator
# Copyright (C) 1999-2022 Manuel Sainz de Baranda y Goñi.
# Released under the terms of the GNU Lesser General Public License v3.

cmake_minimum_required(VERSION 3.14)

if(	CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR AND
	NOT CMAKE_BUILD_TYPE				   AND
	NOT CMAKE_CONFIGURATION_TYPES
)
	set(CMAKE_BUILD_TYPE Release)
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/API/Z80.h" _)
string(REGEX MATCH ".*Z80_LIBRARY_VERSION_STRING \"([^\n]*)\".*" _ ${_})

project(Z80
	VERSION ${CMAKE_MATCH_1}
	LANGUAGES C
	DESCRIPTION "Zilog Z80 CPU emulator")

unset(_)
message("${PROJECT_NAME} v${PROJECT_VERSION}")

include(GNUInstallDirs)

if(DEFINED ${PROJECT_NAME}_SHARED_LIBS)
	set(BUILD_SHARED_LIBS "${${PROJECT_NAME}_SHARED_LIBS}")
endif()

option(${PROJECT_NAME}_DOWNLOAD_TEST_FILES
"Download the firmware and software used by the testing tool."
NO)

set(${PROJECT_NAME}_INSTALL_CMAKEDIR
"${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING
"Specify the directory in which to install the CMake config-file package.")

set(${PROJECT_NAME}_INSTALL_PKGCONFIGDIR
"${CMAKE_INSTALL_LIBDIR}/pkgconfig" CACHE STRING
"Specify the directory in which to install the pkg-config file.")

set(${PROJECT_NAME}_NOSTDLIB_FLAGS
"Auto" CACHE STRING
"Specify the linker flags used to avoid linking against system libraries.")

set(${PROJECT_NAME}_SPHINX_HTML_THEME
"" CACHE STRING
"Specify the Sphinx theme for the documentation in HTML format.")

option(${PROJECT_NAME}_WITH_CMAKE_SUPPORT
"Generate and install the CMake config-file package."
NO)

option(${PROJECT_NAME}_WITH_HTML_DOCUMENTATION
"Build and install the documentation in HTML format."
NO)

option(${PROJECT_NAME}_WITH_PDF_DOCUMENTATION
"Build and install the documentation in PDF format."
NO)

option(${PROJECT_NAME}_WITH_PKGCONFIG_SUPPORT
"Generate and install the pkg-config file."
NO)

option(${PROJECT_NAME}_WITH_STANDARD_DOCUMENTS
"Install the standard text documents distributed with the package: \
AUTHORS, COPYING, COPYING.LESSER, HISTORY, README and THANKS."
NO)

option(${PROJECT_NAME}_WITH_TESTS
"Build the testing tool."
NO)

option(${PROJECT_NAME}_WITH_EXECUTE
"Build the implementation of the z80_execute function."
NO)

option(${PROJECT_NAME}_WITH_FULL_IM0
"Build the full implementation of the interrupt mode 0 rather than the \
reduced one."
NO)

option(${PROJECT_NAME}_WITH_Q
"Build the implementation of the Q \"register\"."
NO)

option(${PROJECT_NAME}_WITH_RESET_SIGNAL
"Build the implementation of the normal RESET signal."
NO)

option(${PROJECT_NAME}_WITH_SPECIAL_RESET_SIGNAL
"Build the implementation of the special RESET signal."
NO)

option(${PROJECT_NAME}_WITH_UNOFFICIAL_RETI
"Configure the ED5Dh, ED6Dh and ED7Dh undocumented instructions as \"reti\" \
instead of \"retn\"."
NO)

option(${PROJECT_NAME}_WITH_ZILOG_NMOS_LD_A_IR_BUG
"Build the implementation of the bug affecting the Zilog Z80 NMOS, which \
causes the P/V flag to be reset when a maskable interrupt is accepted \
during the execution of the \"ld a,{i|r}\" instructions."
NO)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
find_package(Zeta REQUIRED)

add_library(
	${PROJECT_NAME}
	"${CMAKE_CURRENT_SOURCE_DIR}/API/Z80.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/sources/Z80.c")

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED NO
	VERSION	${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	DEBUG_POSTFIX "-debug"
	PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/API/Z80.h")

target_link_libraries(${PROJECT_NAME} PUBLIC Zeta)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/API>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(BUILD_SHARED_LIBS)
	if(WIN32)
		configure_file(
			"${CMAKE_CURRENT_SOURCE_DIR}/sources/Z80.rc.in"
			"${PROJECT_BINARY_DIR}/Z80.rc"
			@ONLY)

		target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}/Z80.rc")
	endif()

	if (${PROJECT_NAME}_NOSTDLIB_FLAGS STREQUAL "Auto")
		if(MSVC)
			target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_WINDOWS_DLL_MAIN)
			target_link_options(${PROJECT_NAME} PRIVATE "/NODEFAULTLIB")
		elseif(APPLE)
			if(CMAKE_C_COMPILER_ID MATCHES "^(AppleClang|Clang)$")
				target_link_options(${PROJECT_NAME} PRIVATE "LINKER:-dead_strip_dylibs")
			elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
				target_link_options(${PROJECT_NAME} PRIVATE "LINKER:-dead_strip_dylibs;-nolibc")
			endif()
		elseif(CMAKE_C_COMPILER_ID MATCHES "^(ARMClang|Clang|GNU|Intel|TinyCC|XL|XLClang)$")
			target_link_options(${PROJECT_NAME} PRIVATE "-nostdlib")
		elseif(CMAKE_C_COMPILER_ID MATCHES "PGI")
			target_link_options(${PROJECT_NAME} PRIVATE "-Mnostdlib")
		elseif(CMAKE_C_COMPILER_ID MATCHES "SunPro")
			target_link_options(${PROJECT_NAME} PRIVATE "-xnolib")
		endif()
	elseif(NOT ${PROJECT_NAME}_NOSTDLIB_FLAGS STREQUAL "")
		target_link_options(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_NOSTDLIB_FLAGS})
	endif()
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC Z80_STATIC)
endif()

if(${PROJECT_NAME}_WITH_EXECUTE)
	target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_EXECUTE)
endif()

if(${PROJECT_NAME}_WITH_FULL_IM0)
	target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_FULL_IM0)
endif()

if(${PROJECT_NAME}_WITH_Q)
	target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_Q)
endif()

if(${PROJECT_NAME}_WITH_RESET_SIGNAL)
	target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_RESET_SIGNAL)
endif()

if(${PROJECT_NAME}_WITH_SPECIAL_RESET_SIGNAL)
	target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_SPECIAL_RESET_SIGNAL)
endif()

if(${PROJECT_NAME}_WITH_UNOFFICIAL_RETI)
	target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_UNOFFICIAL_RETI)
endif()

if(${PROJECT_NAME}_WITH_ZILOG_NMOS_LD_A_IR_BUG)
	target_compile_definitions(${PROJECT_NAME} PRIVATE Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG)
endif()

install(TARGETS ${PROJECT_NAME}
	EXPORT "${PROJECT_NAME}_Targets"
	RUNTIME COMPONENT "${PROJECT_NAME}_Runtime"
	LIBRARY COMPONENT "${PROJECT_NAME}_Runtime"
		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		NAMELINK_COMPONENT "${PROJECT_NAME}_Development"
	ARCHIVE COMPONENT "${PROJECT_NAME}_Development"
	PUBLIC_HEADER
		COMPONENT "${PROJECT_NAME}_Development"
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

if(${PROJECT_NAME}_WITH_CMAKE_SUPPORT OR ${PROJECT_NAME}_WITH_PKGCONFIG_SUPPORT)
	include(CMakePackageConfigHelpers)

	if(${PROJECT_NAME}_WITH_CMAKE_SUPPORT)
		if (BUILD_SHARED_LIBS)
			set(_type Shared)
		else()
			set(_type Static)
		endif()

		install(EXPORT "${PROJECT_NAME}_Targets"
			DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
			FILE ${PROJECT_NAME}${_type}Targets.cmake
			COMPONENT ${PROJECT_NAME}_Development)

		unset(_type)

		write_basic_package_version_file(
			"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
			VERSION ${PROJECT_VERSION}
			COMPATIBILITY SameMajorVersion)

		configure_package_config_file(
			"${CMAKE_CURRENT_SOURCE_DIR}/support/${PROJECT_NAME}Config.cmake.in"
			"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
			INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR})

		install(FILES
			"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
			"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
			DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
			COMPONENT ${PROJECT_NAME}_Development)
	endif()

	if(${PROJECT_NAME}_WITH_PKGCONFIG_SUPPORT)
		configure_file(
			"${CMAKE_CURRENT_SOURCE_DIR}/support/${PROJECT_NAME}.pc.in"
			"${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc"
			@ONLY)

		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
			DESTINATION ${${PROJECT_NAME}_INSTALL_PKGCONFIGDIR}
			COMPONENT ${PROJECT_NAME}_Development)
	endif()
endif()

if(${PROJECT_NAME}_WITH_STANDARD_DOCUMENTS)
	install(FILES	"${CMAKE_CURRENT_SOURCE_DIR}/AUTHORS"
			"${CMAKE_CURRENT_SOURCE_DIR}/COPYING"
			"${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER"
			"${CMAKE_CURRENT_SOURCE_DIR}/HISTORY"
			"${CMAKE_CURRENT_SOURCE_DIR}/README"
			"${CMAKE_CURRENT_SOURCE_DIR}/THANKS"
		DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()

if(${PROJECT_NAME}_WITH_HTML_DOCUMENTATION OR ${PROJECT_NAME}_WITH_PDF_DOCUMENTATION)
	add_subdirectory(documentation)
endif()

if(${PROJECT_NAME}_WITH_TESTS)
	include(CTest)

	find_package(ZLIB   QUIET)
	find_package(libzip QUIET)

	add_executable("test-${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/sources/test-Z80.c")
	target_link_libraries("test-${PROJECT_NAME}" PRIVATE ${PROJECT_NAME})

	if(ZLIB_FOUND AND libzip_FOUND)
		target_link_libraries("test-${PROJECT_NAME}" PRIVATE z libzip::zip)
		target_compile_definitions("test-${PROJECT_NAME}" PRIVATE TEST_Z80_WITH_ARCHIVE_EXTRACTION)
	endif()

	if(${PROJECT_NAME}_WITH_EXECUTE)
		target_compile_definitions("test-${PROJECT_NAME}" PRIVATE TEST_Z80_WITH_EXECUTE)
	endif()

	if(${PROJECT_NAME}_DOWNLOAD_TEST_FILES)
		function(_download_files sha512sum_file base_url downloads_dir)
			file(STRINGS "${sha512sum_file}" _lines)

			foreach(_line ${_lines})
				string(STRIP ${_line} _line)
				string(SUBSTRING ${_line} 0 128 _sha512)
				string(SUBSTRING ${_line} 130 -1 _file_path)
				get_filename_component(_file_full_name ${_file_path} NAME)

				set(_file_url "${base_url}/${_file_path}")
				message(STATUS "Downloading \"${_file_full_name}\"")
				string(REPLACE " " "%20" _file_url ${_file_url})
				string(REPLACE "!" "%21" _file_url ${_file_url})
				string(REPLACE "(" "%28" _file_url ${_file_url})
				string(REPLACE ")" "%29" _file_url ${_file_url})
				string(REPLACE "[" "%5B" _file_url ${_file_url})
				string(REPLACE "]" "%5D" _file_url ${_file_url})

				file(	DOWNLOAD
					"${_file_url}"
					"${downloads_dir}/${_file_path}"
					EXPECTED_HASH SHA512=${_sha512})

				if (	NOT "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.18 AND
					(NOT ZLIB_FOUND OR NOT libzip_FOUND)
				)
					get_filename_component(_file_extension ${_file_full_name} LAST_EXT)

					if(_file_extension STREQUAL ".gz" OR _file_extension STREQUAL ".zip")
						get_filename_component(_subdirectory ${_file_path} DIRECTORY)

						if(_file_extension STREQUAL ".gz")
							set(_extract_pattern "*zex*.com")
						else()
							set(_extract_pattern "*.tap")
						endif()

						file(	ARCHIVE_EXTRACT
							INPUT "${downloads_dir}/${_file_path}"
							DESTINATION "${downloads_dir}/${_subdirectory}"
							PATTERNS ${_extract_pattern}
							VERBOSE)
					endif()
				endif()
			endforeach()
		endfunction()

		_download_files(
			"${CMAKE_CURRENT_SOURCE_DIR}/support/firmware.sha512sum"
			"http://zxe.io/downloads/support/firmware"
			"${CMAKE_CURRENT_BINARY_DIR}/downloads/firmware")

		_download_files(
			"${CMAKE_CURRENT_SOURCE_DIR}/support/software.sha512sum"
			"http://zxe.io/downloads/support/software"
			"${CMAKE_CURRENT_BINARY_DIR}/downloads/software")

		if (	"${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.18 AND
			(NOT ZLIB_FOUND OR NOT libzip_FOUND)
		)
			message(WARNING
				"libzip or zlib was not found on your system, which will cause the "
				"test-Z80 tool to be built without archive extraction support. When "
				"this happens, the build script extracts those downloaded tests that "
				"are compressed so that test-Z80 can use them later, but this has "
				"failed because the version of CMake you are using is too old and "
				"does not support archive extraction either.\n"
				"To fix this, extract all files with \".tar.gz\" or \".zip\" "
				"extension located in "
				"\"${CMAKE_CURRENT_BINARY_DIR}/downloads/software/**\" "
				"to the same directory.")
		endif()
	endif()

	add_test(
		NAME "test-${PROJECT_NAME}"
		COMMAND "test-${PROJECT_NAME}"
			--path "${CMAKE_CURRENT_BINARY_DIR}/downloads/firmware"
			--path "${CMAKE_CURRENT_BINARY_DIR}/downloads/software/POSIX"
			--path "${CMAKE_CURRENT_BINARY_DIR}/downloads/software/ZX Spectrum"
			--all
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# CMakeLists.txt EOF