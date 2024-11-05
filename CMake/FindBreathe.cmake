# FindBreathe.cmake
# Copyright (C) 2021-2024 Manuel Sainz de Baranda y Goñi.
# This "find module" is distributed as public domain software.

#[=======================================================================[.rst:
FindBreathe
-----------

Find Breathe.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``Breathe_FOUND``
  ``TRUE`` if Breathe was found.

``Breathe_APIDOC_VERSION``
  The version of the ``breathe-apidoc`` executable that was found.

``Breathe_VERSION``
  Same as ``Breathe_APIDOC_VERSION``.

Cache variables
^^^^^^^^^^^^^^^

Search results are saved persistently in CMake cache entries:

``Breathe_APIDOC_EXECUTABLE``
  The full path to the ``breathe-apidoc`` executable.

#]=======================================================================]

include(FindPackageHandleStandardArgs)

find_program(
	Breathe_APIDOC_EXECUTABLE
	NAMES breathe-apidoc
	DOC "`breathe-apidoc` executable.")

if(Breathe_APIDOC_EXECUTABLE)
	execute_process(
		COMMAND "${Breathe_APIDOC_EXECUTABLE}" --version
		OUTPUT_VARIABLE _Breathe_output)

	if("${_Breathe_output}" MATCHES ".* ([0-9]+(\\.[0-9]+(\\.[0-9]+)?)?).*\n")
		set(Breathe_APIDOC_VERSION "${CMAKE_MATCH_1}")
		set(Breathe_VERSION ${Breathe_APIDOC_VERSION})
	endif()

	unset(_Breathe_output)
endif()

find_package_handle_standard_args(
	Breathe
	REQUIRED_VARS Breathe_APIDOC_EXECUTABLE Breathe_APIDOC_VERSION
	VERSION_VAR Breathe_VERSION)

mark_as_advanced(Breathe_APIDOC_EXECUTABLE)

# FindBreathe.cmake EOF
