# InstallPerformanceCounters.cmake - Helper script to install library for testing

if(NOT PerformanceCounters_BINARY_DIR)
  message(FATAL_ERROR "PerformanceCounters_BINARY_DIR not set")
endif()

if(NOT PerformanceCounters_INSTALL_DIR)
  message(FATAL_ERROR "PerformanceCounters_INSTALL_DIR not set")
endif()

if(NOT config)
  set(config "Release")
endif()

message(STATUS "Installing PerformanceCounters to ${PerformanceCounters_INSTALL_DIR}")
message(STATUS "  Binary dir: ${PerformanceCounters_BINARY_DIR}")
message(STATUS "  Config: ${config}")

execute_process(
  COMMAND ${CMAKE_COMMAND} --install "${PerformanceCounters_BINARY_DIR}" --prefix "${PerformanceCounters_INSTALL_DIR}" --config "${config}"
  RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "Installation failed with error code: ${result}")
endif()

message(STATUS "Installation completed successfully")
