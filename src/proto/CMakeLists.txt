# SPDX-FileCopyrightText: 2025 The University of St Andrews
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(protobuf CONFIG REQUIRED)

# Since protoc is only in vcpkg, let's find it properly
find_program(PROTOC_EXECUTABLE
    NAMES protoc
    PATHS
        "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-linux/tools/protobuf"
        "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/tools/protobuf"
        "${CMAKE_SOURCE_DIR}/build/dev/vcpkg_installed/x64-linux/tools/protobuf"
    NO_DEFAULT_PATH
    REQUIRED
)

message(STATUS "Found protoc: ${PROTOC_EXECUTABLE}")
if(CMAKE_CXX_CPPCHECK)
# This is a bit of a hack to suppress the weird error because cppcheck
# can't include it properly.
    list(APPEND CMAKE_CXX_CPPCHECK
        "--suppress=preprocessorErrorDirective"
    )
endif()

# Verify version
execute_process(
    COMMAND ${PROTOC_EXECUTABLE} --version
    OUTPUT_VARIABLE PROTOC_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Protoc version: ${PROTOC_VERSION_STRING}")

add_library(wikiopencite_protobuf
  citation_group.proto
  citation.proto
  extracted_citation.proto
  identifiers.proto
  language.proto
  revision_citations.proto
  revision.proto
  revisions_group.proto
  url.proto
)
add_library(wikiopencite::protobuf ALIAS wikiopencite_protobuf)

set_target_properties(wikiopencite_protobuf PROPERTIES
    CXX_CLANG_TIDY ""
)

protobuf_generate(
    TARGET wikiopencite_protobuf
    LANGUAGE cpp
    PROTOC_EXECUTABLE ${PROTOC_EXECUTABLE}
)

target_link_libraries(wikiopencite_protobuf PUBLIC protobuf::libprotobuf)
target_include_directories(wikiopencite_protobuf PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
