# InstallHello.cmake - Helper script to install library for testing
# Note: This file should be renamed to Install${PROJECT_NAME}.cmake

if(NOT Hello_BINARY_DIR)
  message(FATAL_ERROR "Hello_BINARY_DIR not set")
endif()

if(NOT Hello_INSTALL_DIR)
  message(FATAL_ERROR "Hello_INSTALL_DIR not set")
endif()

if(NOT config)
  set(config "Release")
endif()

message(STATUS "Installing Hello to ${Hello_INSTALL_DIR}")
message(STATUS "  Binary dir: ${Hello_BINARY_DIR}")
message(STATUS "  Config: ${config}")

execute_process(
  COMMAND ${CMAKE_COMMAND} --install "${Hello_BINARY_DIR}" --prefix "${Hello_INSTALL_DIR}" --config "${config}"
  RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "Installation failed with error code: ${result}")
endif()

message(STATUS "Installation completed successfully")
