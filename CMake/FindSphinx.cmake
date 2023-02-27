# FindSphinx.cmake
# Copyright (C) 2021-2023 Manuel Sainz de Baranda y Goñi.
# This "find module" is DISTRIBUTED AS PUBLIC DOMAIN. No restrictions apply.

include(FindPackageHandleStandardArgs)

find_program(
	SPHINX_BUILD_EXECUTABLE
	NAMES sphinx-build
	DOC "Sphinx Documentation Builder")

if(SPHINX_BUILD_EXECUTABLE)
	execute_process(
		COMMAND "${SPHINX_BUILD_EXECUTABLE}" --version
		OUTPUT_VARIABLE _output)

	if("${_output}" MATCHES ".* ([^\n]+)\n")
		set(SPHINX_BUILD_VERSION "${CMAKE_MATCH_1}")
	endif()

	unset(_output)
endif()

find_package_handle_standard_args(
	Sphinx
	REQUIRED_VARS SPHINX_BUILD_EXECUTABLE
	VERSION_VAR SPHINX_BUILD_VERSION)

mark_as_advanced(SPHINX_BUILD_EXECUTABLE)

# FindSphinx.cmake EOF
