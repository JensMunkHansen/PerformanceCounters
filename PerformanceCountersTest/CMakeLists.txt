set(TARGET_NAME ${CMAKE_PROJECT_NAME}Test)

# Find Catch2 - required for testing
find_package(Catch2 3 REQUIRED)

# Build the DummyLib shared library for cross-module testing
add_subdirectory(DummyLib)

# Test sources
set(SOURCES
  ${CMAKE_PROJECT_NAME}Test.cpp
)

# Create test executable
add_executable(${TARGET_NAME} ${SOURCES})

# Link to main library, DummyLib, and Catch2
target_link_libraries(${TARGET_NAME}
  PRIVATE
    ${CMAKE_PROJECT_NAME}
    DummyLib
    Catch2::Catch2WithMain
)

# Register tests with CTest
include(Catch)
catch_discover_tests(${TARGET_NAME})

# Standalone cross-module test executable
add_executable(CrossModuleTest CrossModuleTest.cpp)
target_link_libraries(CrossModuleTest
  PRIVATE
    ${CMAKE_PROJECT_NAME}
    DummyLib
)

# Register as a CTest
add_test(NAME CrossModuleAggregationTest COMMAND CrossModuleTest)
