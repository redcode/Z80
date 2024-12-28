# FindSphinx.cmake
# Copyright (C) 2021-2024 Manuel Sainz de Baranda y Goñi.
# This "find module" is distributed as public domain software.

#[=======================================================================[.rst:
FindSphinx
----------

Find the Sphinx documentation tools.

Optional components
^^^^^^^^^^^^^^^^^^^

This module supports 4 optional ``COMPONENTS``:

``apidoc``
  Looks for the ``sphinx-apidoc`` executable.

``autogen``
  Looks for the ``sphinx-autogen`` executable.

``build``
  Looks for the ``sphinx-build`` executable. This component is used by default
  if no components are specified.

``quickstart``
  Looks for the ``sphinx-quickstart`` executable.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``Sphinx_FOUND``
  ``TRUE`` if all requested Sphinx tools were found.

``Sphinx_<COMPONENT>_VERSION``
  The version of the ``sphinx-<component>`` executable that was found, where
  ``<COMPONENT>`` and ``<component>`` are the uppercase and lowercase names of
  the component, respectively.

``Sphinx_VERSION``
  The version of Sphinx that was found. It is the same as
  ``Sphinx_BUILD_VERSION`` if the ``build`` component was requested; otherwise,
  it is the same as the first one requested.

Cache variables
^^^^^^^^^^^^^^^

Search results are saved persistently in CMake cache entries:

``Sphinx_<COMPONENT>_EXECUTABLE``
  The full path to the ``sphinx-<component>`` executable, where ``<COMPONENT>``
  and ``<component>`` are the uppercase and lowercase names of the component,
  respectively.

#]=======================================================================]

set(_Sphinx_required_vars)

if(NOT Sphinx_FIND_COMPONENTS)
	set(Sphinx_FIND_COMPONENTS build)
endif()

foreach(_Sphinx_tool IN LISTS Sphinx_FIND_COMPONENTS)
	if(NOT _Sphinx_tool MATCHES "^(apidoc|autogen|build|quickstart)$")
		message(FATAL_ERROR "Invalid components: ${Sphinx_FIND_COMPONENTS}")
	endif()

	string(TOUPPER ${_Sphinx_tool} _Sphinx_tool_uppercase)
	set(_Sphinx_tool_executable_var Sphinx_${_Sphinx_tool_uppercase}_EXECUTABLE)
	set(_Sphinx_tool_version_var Sphinx_${_Sphinx_tool_uppercase}_VERSION)

	list(	APPEND _Sphinx_required_vars
		${_Sphinx_tool_executable_var} ${_Sphinx_tool_version_var})

	if(NOT DEFINED ${_Sphinx_tool_executable_var})
		find_program(
			${_Sphinx_tool_executable_var}
			NAMES sphinx-${_Sphinx_tool}
			DOC "`sphinx-${_Sphinx_tool}` executable.")

		if(${_Sphinx_tool_executable_var})
			execute_process(
				COMMAND "${${_Sphinx_tool_executable_var}}" --version
				OUTPUT_VARIABLE _Sphinx_output)

			if("${_Sphinx_output}" MATCHES ".* ([0-9]+(\\.[0-9]+(\\.[0-9]+)?)?).*\n")
				set(${_Sphinx_tool_version_var} "${CMAKE_MATCH_1}")

				if(NOT DEFINED Sphinx_VERSION)
					set(Sphinx_VERSION ${${_Sphinx_tool_version_var}})
				endif()
			endif()

			unset(_Sphinx_output)
		endif()

		mark_as_advanced(${_Sphinx_tool_executable_var})
	endif()

	unset(_Sphinx_tool_version_var)
	unset(_Sphinx_tool_executable_var)
	unset(_Sphinx_tool_uppercase)
endforeach()

if(DEFINED Sphinx_BUILD_VERSION)
	set(Sphinx_VERSION ${Sphinx_BUILD_VERSION})
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
	Sphinx
	REQUIRED_VARS ${_Sphinx_required_vars}
	VERSION_VAR Sphinx_VERSION)

unset(_Sphinx_required_vars)

# FindSphinx.cmake EOF
