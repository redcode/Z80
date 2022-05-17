# Z80 - FindZ80.cmake
#     ______  ______ ______
#    /\___  \/\  __ \\  __ \
#    \/__/  /\_\  __ \\ \/\ \
#       /\_____\\_____\\_____\
# Zilog \/_____//_____//_____/ CPU Emulator
# Copyright (C) 1999-2022 Manuel Sainz de Baranda y Goñi.
# Released under the terms of the GNU Lesser General Public License v3.
# This "find module" is DISTRIBUTED AS PUBLIC DOMAIN. No restrictions apply.

include(FindPackageHandleStandardArgs)

find_path(
	Z80_INCLUDE_DIR
	"Z80.h"
	HINTS	${PROJECT_BINARY_DIR}
		${PROJECT_SOURCE_DIR}
	PATH_SUFFIXES
		"deps/Z80/API"
		"dependencies/Z80/API"
		"extern/Z80/API"
		"external/Z80/API"
		"externals/Z80/API"
		"kits/Z80/API/"
		"Z80/API"
		"../Z80/API"
)

if(Z80_INCLUDE_DIR)
	file(READ "${Z80_INCLUDE_DIR}/Z80.h" _h)

	if(${_h} MATCHES ".*Z80_LIBRARY_VERSION_STRING \"([^\n]*)\".*")
		set(Z80_VERSION ${CMAKE_MATCH_1})
	endif()
endif()

find_package_handle_standard_args(
	Z80
	REQUIRED_VARS Z80_INCLUDE_DIR
	VERSION_VAR Z80_VERSION
)

if(Z80_FOUND AND NOT TARGET Z80)
	add_library(Z80 IMPORTED)
	target_include_directories(Z80 INTERFACE ${Z80_INCLUDE_DIR})
endif()

mark_as_advanced(Z80_INCLUDE_DIR)

# FindZ80.cmake EOF
