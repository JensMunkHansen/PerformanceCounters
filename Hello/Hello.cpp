#include "Hello.h"

namespace hello {

std::string greet(const std::string& name) {
    return "Hello, " + name + "!";
}

std::string version() {
    return "1.0.0";
}

int add(int a, int b) {
    return a + b;
}

} // namespace hello
