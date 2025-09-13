if(BUILD_EXAMPLES)
    function(add_example EXAMPLE_NAME)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.c)
        target_link_libraries(${EXAMPLE_NAME} ${PROJECT_NAME}-objects)
        target_include_directories(${EXAMPLE_NAME} PRIVATE ../include)
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        target_compile_options(${EXAMPLE_NAME} PRIVATE
            $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
            -Wno-initializer-overrides>
        )
    endfunction(add_example example_name)

    add_example(01-default_logger)
    add_example(02-simple_logger)
    add_example(03-custom_sink)
    add_example(03-logger_registry)
    add_example(05-custom-formatter)

    set(EXAMPLE_NAME header_only)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.c)
    target_include_directories(${EXAMPLE_NAME} PRIVATE ../include)
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
