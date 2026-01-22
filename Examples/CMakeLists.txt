cmake_minimum_required(VERSION 3.25...3.31)
project(${CMAKE_PROJECT_NAME}Examples)

# Test that the library can install
add_custom_target(install-${CMAKE_PROJECT_NAME}
  COMMAND ${CMAKE_COMMAND}
    -Dconfig=$<CONFIGURATION>
    -D${CMAKE_PROJECT_NAME}_BINARY_DIR=${${CMAKE_PROJECT_NAME}_BINARY_DIR}
    -D${CMAKE_PROJECT_NAME}_INSTALL_DIR=${${CMAKE_PROJECT_NAME}_INSTALL_DIR}
    -P "${CMAKE_CURRENT_LIST_DIR}/Install${CMAKE_PROJECT_NAME}.cmake"
  COMMENT "Installing ${CMAKE_PROJECT_NAME} using CMake script"
  VERBATIM
)
add_test(NAME CMake-Install-Target
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install-${CMAKE_PROJECT_NAME} --config $<CONFIGURATION>)

set_tests_properties(CMake-Install-Target PROPERTIES
  ENVIRONMENT
    "CXX_COMPILER_PATH=${CXX_COMPILER_PATH};C_COMPILER_PATH=${C_COMPILER_PATH}"
  FIXTURES_SETUP CMake-Install-Fixture
)

# Examples using the library from build location and install location
if(${CMAKE_PROJECT_NAME}_BINARY_DIR)
  message(STATUS "Examples tested from ${${CMAKE_PROJECT_NAME}_BINARY_DIR}")

  function(_add_example_test test_name dir lib_dir lib_binary_dir fixture)
    # Determine unique binary output dir for this test
    if(fixture STREQUAL "CMake-Install-Fixture")
      set(example_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/${dir}-installed")
    else()
      set(example_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/${dir}")
    endif()

    add_test(
      NAME "${test_name}"
      COMMAND "${CMAKE_COMMAND}"
              "-Dconfig=$<CONFIGURATION>"
              "-Dgenerator=${CMAKE_GENERATOR}"
              "-Dsource=${CMAKE_CURRENT_SOURCE_DIR}"
              "-Dbinary=${CMAKE_CURRENT_BINARY_DIR}"
              "-Dexample_dir=${dir}"
              "-Dbuild_type=${CMAKE_BUILD_TYPE}"
              "-Dshared=${BUILD_SHARED_LIBS}"
              "-Dlib_dir=${lib_dir}"
              "-Dexample_binary_dir=${example_binary_dir}"
              "-Dctest=${CMAKE_CTEST_COMMAND}"
              "-Dplatform=${CMAKE_GENERATOR_PLATFORM}"
              "-Dtoolset=${CMAKE_GENERATOR_TOOLSET}"
              "-Dlib_binary_dir=${lib_binary_dir}"
              "-Dc_compiler=${CMAKE_C_COMPILER}"
              "-Dcxx_compiler=${CMAKE_CXX_COMPILER}"
              "-Dproject_name=${CMAKE_PROJECT_NAME}"
              -P "${CMAKE_CURRENT_LIST_DIR}/RunExample.cmake")

    set_property(TEST "${test_name}" APPEND
      PROPERTY SKIP_REGULAR_EXPRESSION "Skipping example")

    if(fixture)
      set_tests_properties("${test_name}" PROPERTIES FIXTURES_REQUIRED "${fixture}")
    endif()
  endfunction()

  function(add_example dir)
    if(${CMAKE_PROJECT_NAME}_BINARY_DIR)
      # For multi-config generators, use the config-specific output directory
      get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
      if(isMultiConfig)
        set(_runtime_dir "${CMAKE_BINARY_DIR}/$<CONFIG>/${CMAKE_INSTALL_BINDIR}")
      else()
        set(_runtime_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
      endif()
      _add_example_test(
        "Example-BuildTree-${dir}"
        "${dir}"
        "${${CMAKE_PROJECT_NAME}_BINARY_DIR}/${CMAKE_PROJECT_NAME}"
        "${_runtime_dir}"
        "")  # no fixture
    endif()
  endfunction()

  function(add_installed_example dir)
    if(${CMAKE_PROJECT_NAME}_INSTALL_DIR)
      _add_example_test(
        "Example-InstallTree-${dir}"
        "${dir}"
        "${${CMAKE_PROJECT_NAME}_INSTALL_DIR}/lib/cmake/${CMAKE_PROJECT_NAME}"
        "${${CMAKE_PROJECT_NAME}_INSTALL_DIR}/bin"
        "CMake-Install-Fixture")  # add fixture
    endif()
  endfunction()
else()
  macro(add_example dir)
    add_subdirectory("${dir}")
  endmacro()
endif()

add_example(Usage)
add_installed_example(Usage)
