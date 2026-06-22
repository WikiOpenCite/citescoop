// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TEST_SRC_UTIL_H_
#define TEST_SRC_UTIL_H_

#include <string>  // NOLINT(misc-include-cleaner)

#define FILE(filename)                                    \
  std::string(__FILE__).substr(                           \
      0, std::string(__FILE__).find_last_of("/\\") + 1) + \
      filename

#endif  // TEST_SRC_UTIL_H_
