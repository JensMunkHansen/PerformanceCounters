set(TARGET_NAME ${CMAKE_PROJECT_NAME}Test)

# Find Catch2 - required for testing
find_package(Catch2 3 REQUIRED)

# Test sources
set(SOURCES
  ${CMAKE_PROJECT_NAME}Test.cpp
)

# Create test executable
add_executable(${TARGET_NAME} ${SOURCES})

# Link to main library and Catch2
target_link_libraries(${TARGET_NAME}
  PRIVATE
    ${CMAKE_PROJECT_NAME}
    Catch2::Catch2WithMain
)

# Register tests with CTest
include(Catch)
catch_discover_tests(${TARGET_NAME})
