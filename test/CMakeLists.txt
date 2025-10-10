# SPDX-FileCopyrightText: 2025 https://github.com/friendlyanon
# SPDX-License-Identifier: Unlicense

cmake_minimum_required(VERSION 3.14)

project(citescoopTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(citescoop REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(citescoop_test src/citescoop_test.cc)
target_link_libraries(
    citescoop_test PRIVATE
    wikiopencite::citescoop
    Catch2::Catch2WithMain
)
target_compile_features(citescoop_test PRIVATE cxx_std_20)

catch_discover_tests(citescoop_test)

# ---- End-of-file commands ----

add_folders(Test)
