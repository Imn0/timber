if(BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES "*_test.c")
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} PRIVATE timber-objects)
        target_include_directories(${TEST_NAME}
            PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
        set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
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
