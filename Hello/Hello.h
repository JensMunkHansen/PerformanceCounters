#pragma once

#include "hello_export.h"
#include <string>

namespace hello {

/**
 * @brief Get a greeting message
 * @param name The name to greet
 * @return A greeting string
 */
HELLO_EXPORT std::string greet(const std::string& name);

/**
 * @brief Get the library version
 * @return Version string in format "major.minor.patch"
 */
HELLO_EXPORT std::string version();

/**
 * @brief Simple addition function for demonstration
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
HELLO_EXPORT int add(int a, int b);

} // namespace hello
