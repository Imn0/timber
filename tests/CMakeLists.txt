if(BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES "*_test.c")
    list(FILTER TEST_SOURCES EXCLUDE REGEX ".*/lib/.*")

    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

        # add_executable(${TEST_NAME} ${TEST_SOURCE} lib/tmb_test.c)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} PRIVATE timber::static)
        target_include_directories(${TEST_NAME}
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../src
            ${CMAKE_CURRENT_SOURCE_DIR}/../include
            lib
        )
        set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/
        )
        apply_compile_flags(${TEST_NAME})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()


    add_executable(aaaa aaaa.c lib/tmb_test.c)
    target_link_libraries(aaaa PRIVATE timber::static)
    target_include_directories(aaaa
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        lib
    )
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/
    )
    apply_compile_flags(${TEST_NAME})

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
