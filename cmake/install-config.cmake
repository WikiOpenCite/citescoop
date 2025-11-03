# SPDX-FileCopyrightText: 2025 https://github.com/friendlyanon
# SPDX-License-Identifier: Unlicense

set(citescoop_FOUND YES)

include(CMakeFindDependencyMacro)

find_dependency(Boost REQUIRED COMPONENTS parser algorithm iostreams)
find_dependency(citescoop-proto REQUIRED)
find_dependency(PkgConfig REQUIRED)
# Then use pkg-config for locate specific package
pkg_check_modules(LIBXMLXX REQUIRED IMPORTED_TARGET libxml++-5.0)

if(citescoop_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/citescoopTargets.cmake")
endif()
