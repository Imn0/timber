if(BUILD_EXAMPLES)
    file(GLOB_RECURSE EXAMPLE_SOURCES "*.c")

    foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
        target_link_libraries(${EXAMPLE_NAME} ${PROJECT_NAME}-static)
        target_include_directories(${EXAMPLE_NAME} PRIVATE ../include)

        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        if(MSVC)
            target_compile_definitions(${EXAMPLE_NAME} PRIVATE TMB_WIN_USE_STAIC=1)
        endif()
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
