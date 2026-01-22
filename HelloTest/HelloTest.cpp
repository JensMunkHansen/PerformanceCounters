#include <catch2/catch_test_macros.hpp>
#include "Hello.h"

TEST_CASE("Greeting function", "[hello]") {
    SECTION("Greets with provided name") {
        REQUIRE(hello::greet("World") == "Hello, World!");
        REQUIRE(hello::greet("Alice") == "Hello, Alice!");
    }

    SECTION("Handles empty name") {
        REQUIRE(hello::greet("") == "Hello, !");
    }
}

TEST_CASE("Version function", "[hello]") {
    SECTION("Returns valid version string") {
        std::string ver = hello::version();
        REQUIRE_FALSE(ver.empty());
        REQUIRE(ver == "1.0.0");
    }
}

TEST_CASE("Add function", "[hello]") {
    SECTION("Adds positive numbers") {
        REQUIRE(hello::add(2, 3) == 5);
        REQUIRE(hello::add(10, 20) == 30);
    }

    SECTION("Adds negative numbers") {
        REQUIRE(hello::add(-2, -3) == -5);
    }

    SECTION("Adds mixed numbers") {
        REQUIRE(hello::add(-5, 10) == 5);
        REQUIRE(hello::add(5, -10) == -5);
    }

    SECTION("Handles zero") {
        REQUIRE(hello::add(0, 0) == 0);
        REQUIRE(hello::add(5, 0) == 5);
        REQUIRE(hello::add(0, 5) == 5);
    }
}
