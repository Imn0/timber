if(BUILD_EXAMPLES)
    set(EXAMPLE_NAME simple)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.c)
    target_link_libraries(${EXAMPLE_NAME} ${PROJECT_NAME}-static)
    target_include_directories(${EXAMPLE_NAME} PRIVATE ../include)
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    if(MSVC)
        target_compile_definitions(${EXAMPLE_NAME} PRIVATE TMB_WIN_USE_STAIC=1)
    endif()

    set(EXAMPLE_NAME header_only)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.c)
    target_include_directories(${EXAMPLE_NAME} PRIVATE ../include)
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    if(MSVC)
        target_compile_definitions(${EXAMPLE_NAME} PRIVATE TMB_WIN_USE_STAIC=1)
    endif()
endif()
