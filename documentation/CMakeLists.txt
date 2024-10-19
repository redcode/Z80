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

set(_api_xml_output "${CMAKE_CURRENT_BINARY_DIR}/API-XML")

set(DOXYGEN_BRIEF_MEMBER_DESC	  YES			     )
set(DOXYGEN_ENABLE_PREPROCESSING  YES			     )
set(DOXYGEN_EXPAND_ONLY_PREDEF	  NO			     )
set(DOXYGEN_FILE_PATTERNS	  "*.h"			     )
set(DOXYGEN_FULL_PATH_NAMES	  YES			     )
set(DOXYGEN_GENERATE_DOCBOOK	  NO			     )
set(DOXYGEN_GENERATE_HTML	  NO			     )
set(DOXYGEN_GENERATE_LATEX	  NO			     )
set(DOXYGEN_GENERATE_MAN	  NO			     )
set(DOXYGEN_GENERATE_RTF	  NO			     )
set(DOXYGEN_GENERATE_XML	  YES			     )
set(DOXYGEN_INLINE_INFO		  YES			     )
set(DOXYGEN_INPUT_ENCODING	  "UTF-8"		     )
set(DOXYGEN_MACRO_EXPANSION	  YES			     )
set(DOXYGEN_MARKDOWN_SUPPORT	  YES			     )
set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES			     )
set(DOXYGEN_PREDEFINED		  "Z_ALWAYS_INLINE=inline"   )
set(DOXYGEN_REPEAT_BRIEF	  YES			     )
set(DOXYGEN_SEARCH_INCLUDES	  YES			     )
set(DOXYGEN_SHOW_INCLUDE_FILES	  YES			     )
set(DOXYGEN_SKIP_FUNCTION_MACROS  NO			     )
set(DOXYGEN_STRIP_FROM_PATH	  "${PROJECT_SOURCE_DIR}/API")
set(DOXYGEN_WARN_IF_DOC_ERROR	  YES			     )
set(DOXYGEN_WARN_IF_UNDOCUMENTED  YES			     )
set(DOXYGEN_WARNINGS		  YES			     )
set(DOXYGEN_XML_OUTPUT		  "API-XML"		     )
set(DOXYGEN_XML_PROGRAMLISTING	  YES			     )

get_target_property(DOXYGEN_INCLUDE_PATH Zeta INTERFACE_INCLUDE_DIRECTORIES)

doxygen_add_docs(
	${PROJECT_NAME}-API-XML
	"${PROJECT_SOURCE_DIR}/API"
	ALL
	COMMENT "Extracting API reference from ${PROJECT_NAME}")

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
			-Dproject="${PROJECT_NAME}"
			-Dversion="${PROJECT_VERSION}"
			-Drelease="${PROJECT_VERSION}"
			${_html_theme_option}
			-Dhtml_title="${PROJECT_NAME}"
			-Dbreathe_default_project="${PROJECT_NAME}"
			-Dbreathe_projects.${PROJECT_NAME}="${_api_xml_output}"
			"${CMAKE_CURRENT_SOURCE_DIR}"
			"${_html_documentation_output}"
		DEPENDS ${PROJECT_NAME}-API-XML
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
			-Dproject="${PROJECT_NAME}"
			-Dversion="${PROJECT_VERSION}"
			-Drelease="${PROJECT_VERSION}"
			-Dhtml_title="${PROJECT_NAME}"
			-Dbreathe_default_project="${PROJECT_NAME}"
			-Dbreathe_projects.${PROJECT_NAME}="${_api_xml_output}"
			"${CMAKE_CURRENT_SOURCE_DIR}"
			"${_latex_documentation_output}"
		DEPENDS ${PROJECT_NAME}-API-XML
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
		RENAME "${PROJECT_NAME}-${PROJECT_VERSION}-documentation.pdf"
		COMPONENT ${PROJECT_NAME}_Documentation)
endif()

# documentation/CMakeLists.txt EOF
