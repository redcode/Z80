# Zeta - FindZeta.cmake
#  ______  ______________  ___
# |__   / |  ___|___  ___|/   \
#   /  /__|  __|   |  |  /  -  \
#  /______|_____|  |__| /__/ \__\
# Copyright (C) 2006-2024 Manuel Sainz de Baranda y Goñi.
# This "find module" is DISTRIBUTED AS PUBLIC DOMAIN. No restrictions apply.

include(FindPackageHandleStandardArgs)

if(NOT DEFINED Zeta_FIND_RELATIVE OR Zeta_FIND_RELATIVE)
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

find_path(
	Zeta_INCLUDE_DIR "Z/version.h"
	HINTS	ENV CPATH
		ENV C_INCLUDE_PATH
		ENV CPLUS_INCLUDE_PATH
		ENV OBJC_INCLUDE_PATH)

if(Zeta_INCLUDE_DIR AND EXISTS "${Zeta_INCLUDE_DIR}/Z/version.h")
	file(READ "${Zeta_INCLUDE_DIR}/Z/version.h" _)

	if(_ MATCHES ".*Z_LIBRARY_VERSION_STRING \"([^\n]*)\".*")
		set(Zeta_VERSION ${CMAKE_MATCH_1})

		if(Zeta_VERSION MATCHES "^([0-9]+)\\.([0-9]+)")
			set(Zeta_VERSION_MAJOR ${CMAKE_MATCH_1})
			set(Zeta_VERSION_MINOR ${CMAKE_MATCH_2})

			if(Zeta_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")
				set(Zeta_VERSION_PATCH ${CMAKE_MATCH_3})
			else()
				set(Zeta_VERSION_PATCH 0)
			endif()
		endif()
	endif()

	unset(_)
endif()

find_package_handle_standard_args(
	Zeta
	FOUND_VAR Zeta_FOUND
	REQUIRED_VARS Zeta_INCLUDE_DIR Zeta_VERSION
	VERSION_VAR Zeta_VERSION)

if(Zeta_FOUND AND NOT TARGET Zeta)
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
