# FindZeta.cmake
#  ______  ______________  ___
# |__   / |  ___|___  ___|/   \
#   /  /__|  __|   |  |  /  -  \
#  /______|_____|  |__| /__/ \__\
# Copyright (C) 2006-2024 Manuel Sainz de Baranda y Goñi.
# This "find module" is distributed as public domain software.

#[=======================================================================[.rst:
FindZeta
--------

Find the Zeta library.

Search behavior
^^^^^^^^^^^^^^^

By default, this module will search for Zeta in the directory specified by the
``ZETA_DIR`` environment variable. If this variable is not defined, it will
search in ``PROJECT_BINARY_DIR`` and the following paths relative to
``PROJECT_SOURCE_DIR`` (in the listed order):

* :file:`.`
* :file:`3rd`
* :file:`3rd-party`
* :file:`3rd-parties`
* :file:`3rd_party`
* :file:`3rd_parties`
* :file:`3rdparty`
* :file:`3rdparties`
* :file:`third-party`
* :file:`third-parties`
* :file:`third_party`
* :file:`third_parties`
* :file:`thirdparty`
* :file:`thirdparties`
* :file:`dependencies`
* :file:`deps`
* :file:`extern`
* :file:`external`
* :file:`externals`
* :file:`..`

Finally, if the relaive search is unsuccessful, the module will search for the
headers of the Zeta library in system include directories.

Input variables
^^^^^^^^^^^^^^^

The search process can be controlled by using the following variables:

``Zeta_IGNORE_ZETA_DIR``
  Set this variable to ``TRUE`` to ignore the ``ZETA_DIR`` environment variable.

``Zeta_SKIP_RELATIVE_SEARCH``
  If set to ``TRUE``, the module will not search in ``PROJECT_BINARY_DIR`` and
  paths relative to ``PROJECT_BINARY_DIR`` before trying system include
  directories.

``Zeta_SKIP_SYSTEM_SEARCH``
  If set to ``TRUE``, the module will not search in system include directories.

Imported targets
^^^^^^^^^^^^^^^^

This module defines the following ``INTERFACE IMPORTED`` target:

``Zeta``
  The Zeta library, if found.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``Zeta_FOUND``
  ``TRUE`` if the Zeta library was found.

``Zeta_INCLUDE_DIRS``
  The include directory needed to use Zeta.

``Zeta_VERSION``
  The version of the Zeta library that was found.

``Zeta_VERSION_MAJOR``
  First version number component of the ``Zeta_VERSION`` variable.

``Zeta_VERSION_MINOR``
  Second version number component of the ``Zeta_VERSION`` variable.

``Zeta_VERSION_MICRO``
  Third version number component of the ``Zeta_VERSION`` variable.

``Zeta_VERSION_PATCH``
  Same as ``Zeta_VERSION_MICRO``.

Cache Variables
^^^^^^^^^^^^^^^

Search results are saved persistently in CMake cache entries:

``Zeta_INCLUDE_DIR``
  Same as ``Zeta_INCLUDE_DIRS``.

Hints
^^^^^

The user can set the ``ZETA_DIR`` environment variable to specify the include
directory of the Zeta library or the directory where its tarball has been
extracted. However, if this variable is defined and Zeta is not found in the
specified directory, the module will fail and will not look anywhere else.

#]=======================================================================]

include(FindPackageHandleStandardArgs)

if(	DEFINED ENV{ZETA_DIR} AND
	(NOT DEFINED Zeta_IGNORE_ZETA_DIR OR NOT Zeta_IGNORE_ZETA_DIR)
)
	find_path(
		Zeta_INCLUDE_DIR "Z/version.h"
		PATHS "$ENV{ZETA_DIR}"
		NO_DEFAULT_PATH)

else()
	if(NOT DEFINED Zeta_SKIP_RELATIVE_SEARCH OR NOT Zeta_SKIP_RELATIVE_SEARCH)
		find_path(
			Zeta_INCLUDE_DIR "Z/version.h"
			PATHS	"${PROJECT_BINARY_DIR}/Zeta/API"
				"${PROJECT_BINARY_DIR}"
				"${PROJECT_SOURCE_DIR}/Zeta/API"
				"${PROJECT_SOURCE_DIR}"
				"${PROJECT_SOURCE_DIR}/3rd/Zeta/API"
				"${PROJECT_SOURCE_DIR}/3rd"
				"${PROJECT_SOURCE_DIR}/3rd-party/Zeta/API"
				"${PROJECT_SOURCE_DIR}/3rd-party"
				"${PROJECT_SOURCE_DIR}/3rd-parties/Zeta/API"
				"${PROJECT_SOURCE_DIR}/3rd-parties"
				"${PROJECT_SOURCE_DIR}/3rd_party/Zeta/API"
				"${PROJECT_SOURCE_DIR}/3rd_party"
				"${PROJECT_SOURCE_DIR}/3rd_parties/Zeta/API"
				"${PROJECT_SOURCE_DIR}/3rd_parties"
				"${PROJECT_SOURCE_DIR}/3rdparty/Zeta/API"
				"${PROJECT_SOURCE_DIR}/3rdparty"
				"${PROJECT_SOURCE_DIR}/3rdparties/Zeta/API"
				"${PROJECT_SOURCE_DIR}/3rdparties"
				"${PROJECT_SOURCE_DIR}/third-party/Zeta/API"
				"${PROJECT_SOURCE_DIR}/third-party"
				"${PROJECT_SOURCE_DIR}/third-parties/Zeta/API"
				"${PROJECT_SOURCE_DIR}/third-parties"
				"${PROJECT_SOURCE_DIR}/third_party/Zeta/API"
				"${PROJECT_SOURCE_DIR}/third_party"
				"${PROJECT_SOURCE_DIR}/third_parties/Zeta/API"
				"${PROJECT_SOURCE_DIR}/third_parties"
				"${PROJECT_SOURCE_DIR}/thirdparty/Zeta/API"
				"${PROJECT_SOURCE_DIR}/thirdparty"
				"${PROJECT_SOURCE_DIR}/thirdparties/Zeta/API"
				"${PROJECT_SOURCE_DIR}/thirdparties"
				"${PROJECT_SOURCE_DIR}/dependencies/Zeta/API"
				"${PROJECT_SOURCE_DIR}/dependencies"
				"${PROJECT_SOURCE_DIR}/deps/Zeta/API"
				"${PROJECT_SOURCE_DIR}/deps"
				"${PROJECT_SOURCE_DIR}/extern/Zeta/API"
				"${PROJECT_SOURCE_DIR}/extern"
				"${PROJECT_SOURCE_DIR}/external/Zeta/API"
				"${PROJECT_SOURCE_DIR}/external"
				"${PROJECT_SOURCE_DIR}/externals/Zeta/API"
				"${PROJECT_SOURCE_DIR}/externals"
				"${PROJECT_SOURCE_DIR}/../zeta-src/API"
				"${PROJECT_SOURCE_DIR}/../Zeta/API"
				"${PROJECT_SOURCE_DIR}/.."
			NO_DEFAULT_PATH)
	endif()

	if(NOT DEFINED Zeta_SKIP_SYSTEM_SEARCH OR NOT Zeta_SKIP_SYSTEM_SEARCH)
		find_path(
			Zeta_INCLUDE_DIR "Z/version.h"
			HINTS	ENV CPATH
				ENV C_INCLUDE_PATH
				ENV CPLUS_INCLUDE_PATH
				ENV OBJC_INCLUDE_PATH)
	endif()
endif()

if(Zeta_INCLUDE_DIR AND EXISTS "${Zeta_INCLUDE_DIR}/Z/version.h")
	file(READ "${Zeta_INCLUDE_DIR}/Z/version.h" _Zeta_Z_version_h)

	if(_Zeta_Z_version_h MATCHES ".*Z_LIBRARY_VERSION_STRING \"([^\n]*)\".*")
		set(Zeta_VERSION ${CMAKE_MATCH_1})

		if(Zeta_VERSION MATCHES "^([0-9]+)\\.([0-9]+)")
			set(Zeta_VERSION_MAJOR ${CMAKE_MATCH_1})
			set(Zeta_VERSION_MINOR ${CMAKE_MATCH_2})

			if(Zeta_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")
				set(Zeta_VERSION_MICRO ${CMAKE_MATCH_3})
				set(Zeta_VERSION_PATCH ${Zeta_VERSION_MICRO})
			else()
				set(Zeta_VERSION_MICRO 0)
				set(Zeta_VERSION_PATCH 0)
			endif()
		endif()
	endif()

	unset(_Zeta_Z_version_h)
endif()

find_package_handle_standard_args(
	Zeta
	FOUND_VAR Zeta_FOUND
	REQUIRED_VARS Zeta_INCLUDE_DIR Zeta_VERSION
	VERSION_VAR Zeta_VERSION)

if(Zeta_FOUND AND NOT TARGET Zeta)
	set(Zeta_INCLUDE_DIRS "${Zeta_INCLUDE_DIR}")
	add_library(Zeta INTERFACE IMPORTED)

	if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_LESS 3.11)
		set_property(
			TARGET Zeta
			PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${Zeta_INCLUDE_DIR}")
	else()
		target_include_directories(Zeta INTERFACE "${Zeta_INCLUDE_DIR}")
	endif()
endif()

mark_as_advanced(Zeta_INCLUDE_DIR)

# FindZeta.cmake EOF
