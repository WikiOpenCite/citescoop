# SPDX-FileCopyrightText: 2025 https://github.com/friendlyanon
# SPDX-License-Identifier: Unlicense

cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    citescoop
    VERSION 0.1.0
    DESCRIPTION "Wikipedia citation extractor"
    HOMEPAGE_URL "https://github.com/WikiOpenCite/citescoop"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    citescoop_lib OBJECT
    src/extract.cc
)

add_library(wikiopencite::citescoop ALIAS citescoop_lib)

include(GenerateExportHeader)
generate_export_header(
    citescoop_lib
    BASE_NAME citescoop
    EXPORT_FILE_NAME export/citescoop/citescoop_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(citescoop_lib PUBLIC CITESCOOP_STATIC_DEFINE)
endif()

set_target_properties(
    citescoop_lib PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME citescoop
    OUTPUT_NAME citescoop
)

target_include_directories(
    citescoop_lib ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    citescoop_lib SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(citescoop_lib PUBLIC cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(citescoop_lib PRIVATE fmt::fmt)

# ---- Declare executable ----

add_executable(citescoop_exe
  src/cli/main.cc
)
add_executable(citescoop::exe ALIAS citescoop_exe)

set_property(TARGET citescoop_exe PROPERTY OUTPUT_NAME citescoop)

target_compile_features(citescoop_exe PRIVATE cxx_std_20)

target_link_libraries(citescoop_exe PRIVATE citescoop_lib)

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
