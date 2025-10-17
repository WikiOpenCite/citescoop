# SPDX-FileCopyrightText: 2025 https://github.com/friendlyanon
# SPDX-License-Identifier: Unlicense

set(citescoop_FOUND YES)

include(CMakeFindDependencyMacro)
find_dependency(fmt)

if(citescoop_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/citescoopTargets.cmake")
endif()
