cmake_minimum_required(VERSION 3.16)

include(version.cmake)

project(timber VERSION ${PROJECT_VERSION}
    DESCRIPTION "C logging library"
    LANGUAGES C)
set(soversion ${GIT_SOVERSION})

include(GNUInstallDirs)
if(MSVC)
    set(CMAKE_C_STANDARD 17)
else()
    set(CMAKE_C_STANDARD 23)
endif()
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries, builds static when off, can be overwriten with BUILD_SHARED_AND_STATIC_LIBS" ON)
option(BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(DEBUG_DEFINES "Enable developer build" OFF)

set(THREADING_BACKEND "C11" CACHE STRING "Threading backend to use (C11, PTHREADS, WINDOWS, NONE)")
set_property(CACHE THREADING_BACKEND PROPERTY STRINGS C11 PTHREADS WINDOWS NONE)

set(THREADING_LIBS "")
if(THREADING_BACKEND STREQUAL "C11")
    add_compile_definitions(TMB_THREADING_C11=1)
    message(STATUS "Threading backend: C11 threads")
elseif(THREADING_BACKEND STREQUAL "PTHREADS")
    find_package(Threads REQUIRED)
    if(NOT CMAKE_USE_PTHREADS_INIT)
        message(FATAL_ERROR "Pthreads not found but THREADING_BACKEND=PTHREADS")
    endif()
    add_compile_definitions(TMB_THREADING_PTHREADS=1)
    set(THREADING_LIBS Threads::Threads)
    message(STATUS "Threading backend: POSIX threads (pthreads)")
elseif(THREADING_BACKEND STREQUAL "WINDOWS")
    if(NOT WIN32)
        message(WARNING "THREADING_BACKEND=WINDOWS but not building on Windows")
    endif()
    add_compile_definitions(TMB_THREADING_WINDOWS=1)
    message(STATUS "Threading backend: Windows threads")
elseif(THREADING_BACKEND STREQUAL "NONE")
    add_compile_definitions(TMB_THREADING_NONE=1)
    message(STATUS "Threading backend: None (single-threaded)")
else()
    message(FATAL_ERROR "Invalid THREADING_BACKEND: ${THREADING_BACKEND}. Must be one of: C11, PTHREADS, WINDOWS, NONE")
endif()

function(apply_threading_to_target target_name)
    if(THREADING_LIBS)
        target_link_libraries(${target_name} PRIVATE ${THREADING_LIBS})
    endif()
endfunction()

if(DEBUG_DEFINES)
    add_compile_definitions(TMB_DEBUG=1)
endif()

file(GLOB_RECURSE LIB_SOURCES "src/*.c" ${CMAKE_CURRENT_BINARY_DIR}/version.c)

function(apply_compile_flags target_name)
    target_compile_options(${target_name} PRIVATE
        $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
        -Wall -Wextra -pedantic -Wshadow -Werror -D_CRT_SECURE_NO_WARNINGS -fvisibility=hidden -Wno-initializer-overrides -Wno-gnu-zero-variadic-macro-arguments -Wno-error=override-init -Wno-override-init
        -Wint-conversion -Wconversion>
        $<$<C_COMPILER_ID:MSVC>:
        /W4 /D_CRT_SECURE_NO_WARNINGS /std:clatest /experimental:c11atomics>
        $<$<C_COMPILER_ID:TinyCC>:
        -std=c23>)
endfunction()

function(setup_shared_lib)
    add_library(${PROJECT_NAME}-shared SHARED ${LIB_SOURCES})
    add_library(${PROJECT_NAME}::shared ALIAS ${PROJECT_NAME}-shared)

    set_target_properties(${PROJECT_NAME}-shared PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${GIT_SOVERSION}
        OUTPUT_NAME ${PROJECT_NAME}
    )
    target_include_directories(${PROJECT_NAME}-shared
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    apply_compile_flags(${PROJECT_NAME}-shared)
    target_compile_definitions(${PROJECT_NAME}-shared
        PRIVATE TMB_BUILD_SHARED=1
        INTERFACE TMB_USING_SHARED=1
    )

    apply_threading_to_target(${PROJECT_NAME}-shared)

    install(TARGETS ${PROJECT_NAME}-shared
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endfunction(setup_shared_lib)

function(setup_static_lib)
    add_library(${PROJECT_NAME}-static STATIC ${LIB_SOURCES})
    add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}-static)

    target_include_directories(${PROJECT_NAME}-static
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    apply_compile_flags(${PROJECT_NAME}-static)
    if(WIN32)
        set_target_properties(${PROJECT_NAME}-static PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}_static
        )
    else()
        set_target_properties(${PROJECT_NAME}-static PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}
        )
    endif()
    target_compile_definitions(${PROJECT_NAME}-static
        PRIVATE TMB_BUILD_STATIC=1
        INTERFACE TMB_USING_STATIC=1
    )

    apply_threading_to_target(${PROJECT_NAME}-static)

    install(TARGETS ${PROJECT_NAME}-static
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endfunction(setup_static_lib)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.c ${CMAKE_CURRENT_BINARY_DIR}/_version.c
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
    DEPENDS FORCE_REBUILD_DUMMY
    VERBATIM
    COMMENT "Generating version.c"
)

add_custom_target(FORCE_REBUILD_DUMMY)

if(BUILD_SHARED_AND_STATIC_LIBS)
    setup_shared_lib()
    setup_static_lib()
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}-shared)
elseif(BUILD_SHARED_LIBS)
    setup_shared_lib()
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}-shared)
else()
    setup_static_lib()
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}-static)
endif()


configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/library_config/Config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake @ONLY)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/library_config/libtimber.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libtimber.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtimber.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

if(BUILD_TESTS)
    enable_testing()
endif()
add_subdirectory(tests)
add_subdirectory(examples)
