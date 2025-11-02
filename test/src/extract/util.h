// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TEST_SRC_EXTRACT_UTIL_H_
#define TEST_SRC_EXTRACT_UTIL_H_

#include <string>

inline std::string GetTestFilePath(const std::string& filename) {
  // __FILE__ gives the current source file path
  std::string path = __FILE__;
  auto pos = path.find_last_of("/\\");
  return path.substr(0, pos + 1) + "data/" + filename;
}

#endif
