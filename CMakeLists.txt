cmake_minimum_required(VERSION 3.15)

project(timber VERSION 0.0.1
    DESCRIPTION "C logging library"
    LANGUAGES C)

include(GNUInstallDirs)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE LIB_SOURCES "src/*.c")
set(public_headers include/tmb.h)
set(soversion 0)

option(BUILD_SHARED_LIBS "Build shared libraries, builds static when off, can be overwriten with BUILD_SHARED_AND_STATIC_LIBS" ON)
option(BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

function(setup_shared_lib)


    add_library(${PROJECT_NAME} SHARED)

    target_link_libraries(${PROJECT_NAME} PRIVATE timber-objects)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${soversion}
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
    target_link_libraries(${PROJECT_NAME}-static PRIVATE timber-objects)
endfunction(setup_static_lib)


add_library(${PROJECT_NAME}-objects OBJECT ${LIB_SOURCES})
target_include_directories(${PROJECT_NAME}-objects
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(${PROJECT_NAME}-objects PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS ${PROJECT_NAME}-objects
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
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
    if(BUILD_TESTS)
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
