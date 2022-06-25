# FindBreathe.cmake
# Copyright (C) 2021-2022 Manuel Sainz de Baranda y Goñi.
# This "find module" is DISTRIBUTED AS PUBLIC DOMAIN. No restrictions apply.

include(FindPackageHandleStandardArgs)

find_program(
	BREATHE_APIDOC_EXECUTABLE
	NAMES breathe-apidoc
	DOC "Breathe extension for Sphinx")

if(BREATHE_APIDOC_EXECUTABLE)
	execute_process(
		COMMAND ${BREATHE_APIDOC_EXECUTABLE} --version
		OUTPUT_VARIABLE _output)

	if("${_output}" MATCHES ".* ([^\n]+)\n")
		set(BREATHE_APIDOC_VERSION "${CMAKE_MATCH_1}")
	endif()

	unset(_output)
endif()

find_package_handle_standard_args(
	Breathe
	REQUIRED_VARS BREATHE_APIDOC_EXECUTABLE
	VERSION_VAR BREATHE_APIDOC_VERSION)

mark_as_advanced(BREATHE_APIDOC_EXECUTABLE)

# FindBreathe.cmake EOF
