# SPDX-FileCopyrightText: 2025 https://github.com/friendlyanon
# SPDX-License-Identifier: Unlicense

if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/citescoop"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
# should match the name of variable set in the install-config.cmake script
set(package citescoop)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT citescoop_Development
)

install(
    TARGETS citescoop_citescoop
    EXPORT citescoopTargets
    RUNTIME #
    COMPONENT citescoop_Runtime
    LIBRARY #
    COMPONENT citescoop_Runtime
    NAMELINK_COMPONENT citescoop_Development
    ARCHIVE #
    COMPONENT citescoop_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    citescoop_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE citescoop_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(citescoop_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${citescoop_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT citescoop_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${citescoop_INSTALL_CMAKEDIR}"
    COMPONENT citescoop_Development
)

install(
    EXPORT citescoopTargets
    NAMESPACE wikiopencite::
    DESTINATION "${citescoop_INSTALL_CMAKEDIR}"
    COMPONENT citescoop_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
