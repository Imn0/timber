cmake_minimum_required(VERSION 3.15)

project(timber VERSION 0.0.1
DESCRIPTION "Very nice project"
LANGUAGES C)

include(GNUInstallDirs)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE LIB_SOURCES "src/*.c")


option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED)
else()
    add_library(${PROJECT_NAME} STATIC)
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC TMB_EXPORTS
        PUBLIC TMB_SHARED
    )
endif()

target_sources(${PROJECT_NAME}
    PRIVATE
    ${LIB_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(public_headers
    include/tmb.h
)


set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)


option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE TEST_SOURCES "tests/*_test.c")

    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} ${PROJECT_NAME})

        set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
        add_test(NAME ${TEST_NAME} COMMAND ${CMAKE_BINARY_DIR}/tests/${TEST_NAME})
    endforeach()

    if(TEST_SOURCES)
        message(STATUS "Found tests:")
        foreach(TEST_SOURCE ${TEST_SOURCES})
            get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
            message(STATUS "    ${TEST_NAME}")
        endforeach()
    else()
        message(AUTHOR_WARNING "No test files found in tests/ directory")
    endif()
endif()

if(BUILD_EXAMPLES)
    file(GLOB_RECURSE EXAMPLE_SOURCES "examples/*.c")

    foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
        target_link_libraries(${EXAMPLE_NAME} ${PROJECT_NAME})

        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
        )
    endforeach()

    if(EXAMPLE_SOURCES)
        message(STATUS "Found examples:")
        foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
            get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
            message(STATUS "    ${EXAMPLE_NAME}")
        endforeach()
    else()
        message(STATUS "No example files found in examples/ directory")
    endif()
endif()
