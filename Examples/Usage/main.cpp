#include <iostream>
#include "Hello.h"

int main() {
    // Demonstrate greeting function
    std::cout << hello::greet("World") << std::endl;
    std::cout << hello::greet("Template User") << std::endl;

    // Demonstrate version function
    std::cout << "Hello library version: " << hello::version() << std::endl;

    // Demonstrate add function
    int a = 5, b = 3;
    std::cout << a << " + " << b << " = " << hello::add(a, b) << std::endl;

    return 0;
}
