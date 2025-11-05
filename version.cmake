find_package(Git QUIET)

set(GIT_OK OFF)

if(GIT_FOUND)
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE T
        RESULT_VARIABLE GIT_OK
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(GIT_OK EQUAL 0)
        set(GIT_OK ON)
    else()
        set(GIT_OK OFF)
    endif()
endif()

file(READ "${CMAKE_CURRENT_LIST_DIR}/src/tmb.c" VERSION_FILE_CONTENT)
string(REGEX MATCH "TMB_MAJOR_V\\ *=\\ *\"([0-9]+)\"" _major_match "${VERSION_FILE_CONTENT}")
set(MAJOR "${CMAKE_MATCH_1}")

string(REGEX MATCH "TMB_MINOR_V\\ *=\\ *\"([0-9]+)\"" _minor_match "${VERSION_FILE_CONTENT}")
set(MINOR "${CMAKE_MATCH_1}")

string(REGEX MATCH "TMB_PATCH_V\\ *=\\ *\"([0-9]+)\"" _patch_match "${VERSION_FILE_CONTENT}")
set(PATCH "${CMAKE_MATCH_1}")

string(REGEX MATCH "TMB_SO_V\\ *=\\ *\"([0-9]+)\"" _so_match "${VERSION_FILE_CONTENT}")
set(GIT_SOVERSION "${CMAKE_MATCH_1}")

set(PROJECT_VERSION "${MAJOR}.${MINOR}.${PATCH}")
message(STATUS "Parsed version: ${PROJECT_VERSION}")


if(NOT (GIT_OK))
    set(PROJECT_VERSION "0.0.0")
    set(GIT_SOVERSION "0")
    set(GIT_REV "N/A")
    set(GIT_DIFF "")
else()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REV
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND git status --porcelain
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_STATUS_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(GIT_STATUS_OUTPUT)
        set(GIT_DIFF "-dirty")
    endif()

endif()

set(PROJECT_VERSION "${PROJECT_VERSION}")
set(GIT_SOVERSION "${GIT_SOVERSION}")

set(VERSION
    "
const char* const TMB_GIT_REV=\"${GIT_REV}${GIT_DIFF}\";
"
)

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/version.c)
    file(READ ${CMAKE_CURRENT_BINARY_DIR}/version.c VERSION_)
else()
    set(VERSION_ "")
endif()

if(NOT "${VERSION}" STREQUAL "${VERSION_}")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version.c "${VERSION}")
endif()
