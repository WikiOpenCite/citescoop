# SPDX-FileCopyrightText: 2025 https://github.com/friendlyanon
# SPDX-License-Identifier: Unlicense

cmake_minimum_required(VERSION 3.28)

include(cmake/prelude.cmake)

project(
    citescoop
    VERSION 0.1.1
    DESCRIPTION "A library for extracting citations from Wikitext"
    HOMEPAGE_URL "https://github.com/WikiOpenCite/citescoop"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    citescoop_citescoop
    src/parser/parser.cc
    src/parser/parser_impl.cc
    src/parser/exceptions.cc
)
add_library(wikiopencite::citescoop ALIAS citescoop_citescoop)

include(GenerateExportHeader)
generate_export_header(
    citescoop_citescoop
    BASE_NAME citescoop
    EXPORT_FILE_NAME export/citescoop/citescoop_export.h
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(citescoop_citescoop PUBLIC CITESCOOP_STATIC_DEFINE)
endif()

set_target_properties(
    citescoop_citescoop PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME citescoop
    OUTPUT_NAME citescoop
)

target_include_directories(
    citescoop_citescoop ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    citescoop_citescoop SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(citescoop_citescoop PUBLIC cxx_std_20)

find_package(Boost REQUIRED COMPONENTS parser algorithm)
find_package(citescoop-proto REQUIRED)

target_link_libraries(citescoop_citescoop PRIVATE
  Boost::parser
  Boost::algorithm
  wikiopencite::citescoop-proto
)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT citescoop_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of citescoop"
  )
endif()

include(cmake/dev-mode.cmake)
