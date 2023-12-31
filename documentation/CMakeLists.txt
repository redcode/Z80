# Z80 - documentation/CMakeLists.txt
#     ______  ______ ______
#    /\___  \/\  __ \\  __ \
#    \/__/  /\_\  __ \\ \/\ \
#       /\_____\\_____\\_____\
# Zilog \/_____//_____//_____/ CPU Emulator
# Copyright (C) 1999-2024 Manuel Sainz de Baranda y Goñi.
# Released under the terms of the GNU Lesser General Public License v3.

find_package(Doxygen REQUIRED)
find_package(Sphinx  REQUIRED)
find_package(Breathe REQUIRED)

set(DOXYGEN_INPUT_DIR  "${PROJECT_SOURCE_DIR}/API"		)
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}"		)
set(DOXYGEN_XML_OUTPUT "API-XML"				)
set(_doxyfile_in       "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
set(_doxyfile	       "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"	)

get_target_property(DOXYGEN_INCLUDE_PATH Zeta INTERFACE_INCLUDE_DIRECTORIES)
configure_file("${_doxyfile_in}" "${_doxyfile}" @ONLY)

file(GLOB_RECURSE _public_headers "${DOXYGEN_INPUT_DIR}/*.h")
set(_api_xml_output "${DOXYGEN_OUTPUT_DIR}/${DOXYGEN_XML_OUTPUT}")

add_custom_command(
	OUTPUT  "${_api_xml_output}"
	DEPENDS ${_public_headers}
	COMMAND "${DOXYGEN_EXECUTABLE}" "${_doxyfile}"
	MAIN_DEPENDENCY "${_doxyfile}"
	COMMENT "Extracting API reference from ${PROJECT_NAME}")

add_custom_target(
	${PROJECT_NAME}-API-XML ALL
	DEPENDS "${_api_xml_output}")

if(${PROJECT_NAME}_WITH_HTML_DOCUMENTATION)
	set(_html_documentation_output "${CMAKE_CURRENT_BINARY_DIR}/HTML")

	if(${PROJECT_NAME}_SPHINX_HTML_THEME STREQUAL "")
		set(_html_theme_option "")
	else()
		set(_html_theme_option "-Dhtml_theme=${${PROJECT_NAME}_SPHINX_HTML_THEME}")
	endif()

	add_custom_command(
		OUTPUT  "${_html_documentation_output}"
		COMMAND "${SPHINX_BUILD_EXECUTABLE}"
			-b html
			${_html_theme_option}
			"-Dbreathe_projects.${PROJECT_NAME}=${_api_xml_output}"
			"${CMAKE_CURRENT_SOURCE_DIR}"
			"${_html_documentation_output}"
		MAIN_DEPENDENCY "${_api_xml_output}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Generating documentation in HTML format for ${PROJECT_NAME}")

	add_custom_target(
		${PROJECT_NAME}-Documentation-HTML ALL
		DEPENDS "${_html_documentation_output}")

	add_dependencies(${PROJECT_NAME}-Documentation-HTML ${PROJECT_NAME}-API-XML)

	install(DIRECTORY "${_html_documentation_output}/"
		DESTINATION "${CMAKE_INSTALL_DOCDIR}/documentation"
		COMPONENT ${PROJECT_NAME}_Documentation)
endif()

if(${PROJECT_NAME}_WITH_PDF_DOCUMENTATION)
	find_package(LATEX REQUIRED COMPONENTS PDFLATEX)

	set(_latex_documentation_output "${CMAKE_CURRENT_BINARY_DIR}/LaTeX")

	add_custom_command(
		OUTPUT  "${_latex_documentation_output}"
		COMMAND "${SPHINX_BUILD_EXECUTABLE}"
			-b latex
			"-Dbreathe_projects.${PROJECT_NAME}=${_api_xml_output}"
			"${CMAKE_CURRENT_SOURCE_DIR}"
			"${_latex_documentation_output}"
		MAIN_DEPENDENCY "${_api_xml_output}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Generating documentation in LaTeX format for ${PROJECT_NAME}")

	add_custom_target(
		${PROJECT_NAME}-Documentation-LaTeX ALL
		DEPENDS "${_latex_documentation_output}")

	add_dependencies(${PROJECT_NAME}-Documentation-LaTeX ${PROJECT_NAME}-API-XML)

	string(TOLOWER ${PROJECT_NAME} _pdf_documentation_output)
	set(_pdf_documentation_output "${_latex_documentation_output}/${_pdf_documentation_output}.pdf")

	add_custom_command(
		OUTPUT  "${_pdf_documentation_output}"
		COMMAND "${CMAKE_MAKE_PROGRAM}"
		MAIN_DEPENDENCY "${_latex_documentation_output}"
		WORKING_DIRECTORY "${_latex_documentation_output}"
		COMMENT "Generating documentation in PDF format for ${PROJECT_NAME}")

	add_custom_target(
		${PROJECT_NAME}-Documentation-PDF ALL
		DEPENDS "${_pdf_documentation_output}")

	add_dependencies(${PROJECT_NAME}-Documentation-PDF ${PROJECT_NAME}-Documentation-LaTeX)

	install(FILES "${_pdf_documentation_output}"
		DESTINATION "${CMAKE_INSTALL_DOCDIR}"
		RENAME "${PROJECT_NAME} v${PROJECT_VERSION}.pdf"
		COMPONENT ${PROJECT_NAME}_Documentation)
endif()

# documentation/CMakeLists.txt EOF
