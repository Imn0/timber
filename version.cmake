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


if(NOT (GIT_OK))
    set(PROJECT_VERSION "0.0.0")
    set(GIT_PROJECT_VERSION "0.0.0")
    set(GIT_REV "N/A")
    set(GIT_DIFF "")
    set(GIT_TAG "N/A")
    set(GIT_BRANCH "N/A")
    set(GIT_SOVERSION "0")
else()
    set(GIT_SOVERSION "0")
    set(PROJECT_VERSION "0.0.0")
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --long --dirty=-dirty
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        OUTPUT_VARIABLE GIT_REV
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(GIT_TAG AND GIT_DESCRIBE)
        string(REGEX MATCH "^([^-]+)-([0-9]+)-g([0-9a-f]+)(-dirty)?$" MATCH_RESULT ${GIT_DESCRIBE})
        if(CMAKE_MATCH_1) # tag in normal-ish form

            set(GIT_TAG ${CMAKE_MATCH_1})
            set(COMMITS_AHEAD ${CMAKE_MATCH_2})
            set(COMMIT_HASH ${CMAKE_MATCH_3})
            set(IS_DIRTY ${CMAKE_MATCH_4})
            string(REGEX REPLACE "^v" "" BASE_VERSION ${GIT_TAG})
            if(COMMITS_AHEAD EQUAL 0)
                # exactly on a tag
                set(PROJECT_VERSION ${BASE_VERSION})
                set(GIT_PROJECT_VERSION ${BASE_VERSION})
                if(IS_DIRTY)
                    set(GIT_PROJECT_VERSION "${GIT_PROJECT_VERSION}-dirty")
                endif()
            else()
                # commits since tag
                set(GIT_PROJECT_VERSION "${BASE_VERSION}-${COMMITS_AHEAD}-g${COMMIT_HASH}")
                if(IS_DIRTY)
                    set(GIT_PROJECT_VERSION "${GIT_PROJECT_VERSION}-dirty")
                endif()
            endif()


        endif()
    else()
        # No tags found, use commit hash
        if(GIT_REV)
            set(GIT_PROJECT_VERSION "${PROJECT_VERSION}-g${GIT_REV}")
            if(NOT GIT_DIRTY EQUAL 0)
                set(GIT_PROJECT_VERSION "${GIT_PROJECT_VERSION}-dirty")
            endif()
        else()
        endif()
    endif()
endif()

set(PROJECT_VERSION "${PROJECT_VERSION}")
set(GIT_SOVERSION "${GIT_SOVERSION}")

set(VERSION
    "
const char* GIT_REV=\"${GIT_REV}${GIT_DIFF}\";
const char* GIT_TAG=\"${GIT_TAG}\";
const char* GIT_BRANCH=\"${GIT_BRANCH}\";
const char* GIT_PROJECT_VERSION=\"${GIT_PROJECT_VERSION}\";
const char* PROJECT_VERSION=\"${PROJECT_VERSION}\";
const char* SOVERSION=\"${GIT_SOVERSION}\";
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
