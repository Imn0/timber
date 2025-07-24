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
set(CMAKE_BUILD_TYPE Debug)


file(GLOB_RECURSE LIB_SOURCES "src/*.c")
set(public_headers include/tmb.h)

option(BUILD_SHARED_LIBS "Build shared libraries, builds static when off, can be overwriten with BUILD_SHARED_AND_STATIC_LIBS" ON)
option(BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

function(setup_shared_lib)

    add_library(${PROJECT_NAME} SHARED)

    target_compile_definitions(${PROJECT_NAME} PRIVATE TMB_BUILD_DLL=1)
    target_compile_definitions(${PROJECT_NAME}-objects PRIVATE TMB_BUILD_DLL=1)

    target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}-objects)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${SOVERSION}
        PUBLIC_HEADER ${public_headers}
        OUTPUT_NAME ${PROJECT_NAME}
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${public_headers})
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

endfunction(setup_shared_lib)

function(setup_static_lib)
    add_library(${PROJECT_NAME}-static STATIC)
    if(WIN32)
        set_target_properties(${PROJECT_NAME}-static PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}_static
        )
    else()
        set_target_properties(${PROJECT_NAME}-static PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}
        )
    endif()
    target_link_libraries(${PROJECT_NAME}-static PRIVATE ${PROJECT_NAME}-objects)
endfunction(setup_static_lib)


add_library(${PROJECT_NAME}-objects OBJECT ${LIB_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/version.c)
target_include_directories(${PROJECT_NAME}-objects
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_options(${PROJECT_NAME}-objects PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
    -Wall -Wextra -pedantic -D_CRT_SECURE_NO_WARNINGS -fvisibility=hidden>
    $<$<C_COMPILER_ID:MSVC>:
    /W4 /D_CRT_SECURE_NO_WARNINGS>)
set_target_properties(${PROJECT_NAME}-objects PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    UNITY_BUILD ON)
install(TARGETS ${PROJECT_NAME}-objects
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_version.c
    ${CMAKE_CURRENT_BINARY_DIR}/version.c
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
    VERBATIM
)

if(BUILD_SHARED_AND_STATIC_LIBS)
    setup_shared_lib()
    setup_static_lib()
    install(TARGETS ${PROJECT_NAME}-static
        EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
    )
elseif(BUILD_SHARED_LIBS)
    if(BUILD_TESTS OR BUILD_EXAMPLES)
        setup_static_lib()
    endif()
    setup_shared_lib()
else()
    setup_static_lib()
    install(TARGETS ${PROJECT_NAME}-static
        EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
    )
    set_target_properties(${PROJECT_NAME}-static PROPERTIES PUBLIC_HEADER "${public_headers}")
    install(TARGETS ${PROJECT_NAME}-static
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
endif()


option(BUILD_PYTHON_BINDINGS "" OFF)
if(BUILD_PYTHON_BINDINGS)
    set(Python_ADDITIONAL_VERSIONS 3)
    find_package(PythonLibs REQUIRED)
    find_package(PythonInterp REQUIRED)
    add_library(${PROJECT_NAME}-python MODULE bindings/python/_timber.c)
    target_link_libraries(${PROJECT_NAME}-python PRIVATE ${PROJECT_NAME}-objects)
    include_directories("${PYTHON_INCLUDE_DIRS}")
    set_target_properties(${PROJECT_NAME}-python PROPERTIES PREFIX "")
    set_target_properties(${PROJECT_NAME}-python PROPERTIES OUTPUT_NAME "_timber")
    if(WIN32)
        set_target_properties(${PROJECT_NAME}-python PROPERTIES SUFFIX ".pyd")
    else()
        set_target_properties(${PROJECT_NAME}-python PROPERTIES SUFFIX ".so")
    endif()
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/library_config/Config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake @ONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/library_config/ConfigVersion.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake @ONLY)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/library_config/libtmb.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libtmb.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtmb.pc DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)

if(BUILD_TESTS)
    enable_testing()
endif()
add_subdirectory(tests)
add_subdirectory(examples)
