set(TARGET_NAME ${PROJECT_NAME})

# Library sources
set(SOURCES
  ${PROJECT_NAME}.cpp
)

# Library headers (public API)
set(HEADERS
  ${PROJECT_NAME}.h
  ScopedTimer.h
)

# Create library (shared or static based on BUILD_SHARED_LIBS)
add_library(${TARGET_NAME} ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})

# Link to build interface for compiler flags (build-time only, not exported)
# Use $<BUILD_INTERFACE:$<LINK_ONLY:build>> to avoid export issues
target_link_libraries(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:$<LINK_ONLY:build>>)

# Include directories
target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Set properties for shared library versioning
set_target_properties(${TARGET_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  EXPORT_NAME ${TARGET_NAME}
)

# Generate export header for Windows DLL support
include(GenerateExportHeader)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
generate_export_header(${TARGET_NAME}
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}_export.h
)

# Install library
install(TARGETS ${TARGET_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# Install headers
install(FILES ${HEADERS}
  DESTINATION include/PerformanceCounters
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}_export.h
  DESTINATION include/PerformanceCounters
)

# Install CMake config files
include(CMakePackageConfigHelpers)

# Generate package config file
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

# Generate version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Install the config files
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install the export targets
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# Export from build tree
export(EXPORT ${PROJECT_NAME}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
  NAMESPACE ${PROJECT_NAME}::
)
