# DummyLib - Separate DLL for testing cross-module timing aggregation

set(TARGET_NAME DummyLib)

# Source files
set(SOURCES
    DummyLib.cpp
)

set(HEADERS
    DummyLib.h
)

# Create shared library (always shared for this test)
add_library(${TARGET_NAME} SHARED ${SOURCES} ${HEADERS})

# Link to PerformanceCounters
target_link_libraries(${TARGET_NAME}
    PRIVATE
        ${CMAKE_PROJECT_NAME}
        $<BUILD_INTERFACE:$<LINK_ONLY:build>>
)

# Include directories
target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# Generate export header
include(GenerateExportHeader)
generate_export_header(${TARGET_NAME}
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/dummylib_export.h
)
