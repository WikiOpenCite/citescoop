# SPDX-FileCopyrightText: 2025 https://github.com/friendlyanon
# SPDX-License-Identifier: Unlicense

install(
    TARGETS citescoop_exe
    RUNTIME COMPONENT citescoop_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
