# RunExample.cmake - Helper script to build and test examples

# Required variables
if(NOT source)
  message(FATAL_ERROR "source not set")
endif()
if(NOT example_dir)
  message(FATAL_ERROR "example_dir not set")
endif()
if(NOT lib_dir)
  message(FATAL_ERROR "lib_dir not set")
endif()
if(NOT example_binary_dir)
  message(FATAL_ERROR "example_binary_dir not set")
endif()
if(NOT project_name)
  message(FATAL_ERROR "project_name not set")
endif()

# Set defaults
if(NOT generator)
  set(generator "Ninja")
endif()
if(NOT config)
  set(config "Release")
endif()
if(NOT build_type)
  set(build_type "Release")
endif()

set(example_source_dir "${source}/${example_dir}")

message(STATUS "Building example: ${example_dir}")
message(STATUS "  Source: ${example_source_dir}")
message(STATUS "  Binary: ${example_binary_dir}")
message(STATUS "  ${project_name}_DIR: ${lib_dir}")
message(STATUS "  Generator: ${generator}")
message(STATUS "  Config: ${config}")

# Build CMake configure arguments
set(cmake_args
  "-S${example_source_dir}"
  "-B${example_binary_dir}"
  "-G${generator}"
  "-D${project_name}_DIR=${lib_dir}"
)

# Add compilers if specified
if(c_compiler)
  list(APPEND cmake_args "-DCMAKE_C_COMPILER=${c_compiler}")
endif()
if(cxx_compiler)
  list(APPEND cmake_args "-DCMAKE_CXX_COMPILER=${cxx_compiler}")
endif()

# Add platform/toolset for Visual Studio
if(platform)
  list(APPEND cmake_args "-A${platform}")
endif()
if(toolset)
  list(APPEND cmake_args "-T${toolset}")
endif()

# Add build type for single-config generators
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT isMultiConfig AND build_type)
  list(APPEND cmake_args "-DCMAKE_BUILD_TYPE=${build_type}")
endif()

# Add shared libs setting
if(shared)
  list(APPEND cmake_args "-DBUILD_SHARED_LIBS=${shared}")
endif()

# Configure
message(STATUS "Configuring...")
execute_process(
  COMMAND ${CMAKE_COMMAND} ${cmake_args}
  RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Configure failed")
endif()

# Build
message(STATUS "Building...")
execute_process(
  COMMAND ${CMAKE_COMMAND} --build "${example_binary_dir}" --config "${config}"
  RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Build failed")
endif()

# Run tests if ctest is available
if(ctest)
  message(STATUS "Running tests...")
  execute_process(
    COMMAND ${ctest} --test-dir "${example_binary_dir}" -C "${config}" --output-on-failure
    RESULT_VARIABLE result
  )
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Tests failed")
  endif()
endif()

message(STATUS "Example ${example_dir} completed successfully")
