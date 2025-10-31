if(BUILD_EXAMPLES)
    function(add_example EXAMPLE_NAME)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.c)
        target_link_libraries(${EXAMPLE_NAME} ${PROJECT_NAME}::static)
        target_include_directories(${EXAMPLE_NAME} PRIVATE ../include)
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/
        )
        apply_compile_flags(${EXAMPLE_NAME})

    endfunction(add_example example_name)

    add_example(01-default_logger)
    add_example(02-simple_logger)
    add_example(02-graylog)
    add_example(03-custom_sink)
    add_example(03-logger_registry)
    add_example(05-custom-formatter)

endif()
