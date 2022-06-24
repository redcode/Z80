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

find_path(Z80_INCLUDE_DIR "Z80.h")

if(Z80_INCLUDE_DIR AND EXISTS "${Z80_INCLUDE_DIR}/Z80.h")
	file(READ "${Z80_INCLUDE_DIR}/Z80.h" _)

	if(_ MATCHES ".*Z80_LIBRARY_VERSION_STRING \"([^\n]*)\".*")
		set(Z80_VERSION ${CMAKE_MATCH_1})
	endif()

	unset(_)
endif()

find_library(Z80_LIBRARY Z80)

find_package_handle_standard_args(
	Z80
	FOUND_VAR Z80_FOUND
	REQUIRED_VARS Z80_INCLUDE_DIR Z80_VERSION Z80_LIBRARY
	VERSION_VAR Z80_VERSION)

if(Z80_FOUND AND NOT TARGET Z80)
	find_package(Zeta QUIET)

	add_library(Z80 SHARED IMPORTED)

	set_target_properties(
		Z80 PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${Z80_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES "Zeta"
		IMPORTED_LOCATION "${Z80_LIBRARY}")
endif()

mark_as_advanced(Z80_INCLUDE_DIR Z80_LIBRARY)

# FindZ80.cmake EOF
