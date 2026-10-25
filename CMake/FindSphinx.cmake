# FindSphinx.cmake
# Copyright (C) 2021-2026 Manuel Sainz de Baranda y Goñi.
# This "find module" is distributed as public domain software.

#[=======================================================================[.rst:
FindSphinx
----------

Find the Sphinx documentation tools.

Optional components
^^^^^^^^^^^^^^^^^^^

This module supports 4 optional ``COMPONENTS``.  Component names are matched
case-insensitively.

``apidoc``
  Looks for the ``sphinx-apidoc`` executable.

``autogen``
  Looks for the ``sphinx-autogen`` executable.

``build``
  Looks for the ``sphinx-build`` executable.  This component is searched for by
  default if no components are specified.

``quickstart``
  Looks for the ``sphinx-quickstart`` executable.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``Sphinx_FOUND``
  ``TRUE`` if all required Sphinx tools were found.

``Sphinx_<COMPONENT>_FOUND``
  ``TRUE`` if the component was found, where ``<COMPONENT>`` is the uppercase
  name of the component (e.g. ``Sphinx_BUILD_FOUND``).

``Sphinx_<COMPONENT>_VERSION``
  The version of the ``sphinx-<component>`` executable that was found, where
  ``<COMPONENT>`` and ``<component>`` are the uppercase and lowercase names of
  the component, respectively.

``Sphinx_VERSION``
  The version of Sphinx that was found.  It is the same as
  ``Sphinx_BUILD_VERSION`` if the ``build`` component was requested; otherwise,
  it is the same as the first requested component.

Cache variables
^^^^^^^^^^^^^^^

Search results are saved persistently in CMake cache entries:

``Sphinx_<COMPONENT>_EXECUTABLE``
  The full path to the ``sphinx-<component>`` executable, where ``<COMPONENT>``
  and ``<component>`` are the uppercase and lowercase names of the component,
  respectively.

#]=======================================================================]

include(FindPackageHandleStandardArgs)

unset(Sphinx_VERSION)
set(_Sphinx_required_vars)

if(NOT Sphinx_FIND_COMPONENTS)
	set(Sphinx_FIND_COMPONENTS build)
	set(Sphinx_FIND_REQUIRED_build TRUE)
endif()

foreach(_Sphinx_component ${Sphinx_FIND_COMPONENTS})
	string(TOLOWER "${_Sphinx_component}" _Sphinx_component_lowercase)

	if(_Sphinx_component_lowercase MATCHES "^(apidoc|autogen|build|quickstart)$")
		string(TOUPPER ${_Sphinx_component} _Sphinx_component_uppercase)
		set(_Sphinx_component_executable_var Sphinx_${_Sphinx_component_uppercase}_EXECUTABLE)
		set(_Sphinx_component_version_var Sphinx_${_Sphinx_component_uppercase}_VERSION)
		unset(${_Sphinx_component_version_var})
		set(Sphinx_${_Sphinx_component_uppercase}_FOUND FALSE)

		if(_Sphinx_component_lowercase STREQUAL "build")
			set(_Sphinx_build_requested TRUE)
		endif()

		if(Sphinx_FIND_REQUIRED_${_Sphinx_component})
			list(	APPEND _Sphinx_required_vars
				${_Sphinx_component_executable_var} ${_Sphinx_component_version_var})
		endif()

		find_program(
			${_Sphinx_component_executable_var}
			NAMES sphinx-${_Sphinx_component_lowercase}
			DOC "Path to the `sphinx-${_Sphinx_component_lowercase}` executable.")

		if(${_Sphinx_component_executable_var})
			execute_process(
				COMMAND "${${_Sphinx_component_executable_var}}" --version
				RESULT_VARIABLE _Sphinx_result
				OUTPUT_VARIABLE _Sphinx_output
				OUTPUT_STRIP_TRAILING_WHITESPACE
				ERROR_QUIET)

			if(	_Sphinx_result EQUAL 0 AND
				"${_Sphinx_output}" MATCHES "([0-9]+(\\.[0-9]+(\\.[0-9]+)?)?)"
			)
				set(${_Sphinx_component_version_var} "${CMAKE_MATCH_1}")
				set(Sphinx_${_Sphinx_component_uppercase}_FOUND TRUE)

				if(NOT DEFINED Sphinx_VERSION)
					set(Sphinx_VERSION ${${_Sphinx_component_version_var}})
				endif()
			endif()
		endif()

		set(Sphinx_${_Sphinx_component}_FOUND ${Sphinx_${_Sphinx_component_uppercase}_FOUND})
		mark_as_advanced(${_Sphinx_component_executable_var})
	else()
		message(WARNING "Invalid component: ${_Sphinx_component}")
	endif()
endforeach()

if(_Sphinx_build_requested AND DEFINED Sphinx_BUILD_VERSION)
	set(Sphinx_VERSION ${Sphinx_BUILD_VERSION})
endif()

if(NOT _Sphinx_required_vars)
	set(_Sphinx_no_required_components TRUE)
	set(_Sphinx_required_vars _Sphinx_no_required_components)
endif()

find_package_handle_standard_args(
	Sphinx
	REQUIRED_VARS ${_Sphinx_required_vars}
	VERSION_VAR Sphinx_VERSION
	HANDLE_COMPONENTS)

unset(_Sphinx_required_vars)
unset(_Sphinx_component)
unset(_Sphinx_component_lowercase)
unset(_Sphinx_component_uppercase)
unset(_Sphinx_component_executable_var)
unset(_Sphinx_component_version_var)
unset(_Sphinx_build_requested)
unset(_Sphinx_result)
unset(_Sphinx_output)
unset(_Sphinx_no_required_components)

# FindSphinx.cmake EOF
