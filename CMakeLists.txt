cmake_minimum_required(VERSION 3.25)
project(duolingoreminder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_CLANG_TIDY "Run clang-tidy during build" ON)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
    # Nix clang wrapper injects include paths internally that clang-tidy can't see.
    # Extract them from the compiler and forward via --extra-arg.
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -v -x c++ /dev/null -fsyntax-only
        ERROR_VARIABLE COMPILER_VERBOSE_OUTPUT
        OUTPUT_QUIET
    )
    set(CLANG_TIDY_EXTRA_ARGS "")
    string(REGEX MATCHALL "-isystem [^ ]+" ISYSTEM_FLAGS "${COMPILER_VERBOSE_OUTPUT}")
    foreach(FLAG ${ISYSTEM_FLAGS})
        string(REGEX REPLACE "-isystem " "" INCLUDE_PATH "${FLAG}")
        list(APPEND CLANG_TIDY_EXTRA_ARGS "--extra-arg=-isystem${INCLUDE_PATH}")
    endforeach()
    string(REGEX MATCHALL "-cxx-isystem [^ ]+" CXX_ISYSTEM_FLAGS "${COMPILER_VERBOSE_OUTPUT}")
    foreach(FLAG ${CXX_ISYSTEM_FLAGS})
        string(REGEX REPLACE "-cxx-isystem " "" INCLUDE_PATH "${FLAG}")
        list(APPEND CLANG_TIDY_EXTRA_ARGS "--extra-arg=-cxx-isystem${INCLUDE_PATH}")
    endforeach()
    string(REGEX MATCHALL "-internal-isystem [^ ]+" INTERNAL_ISYSTEM_FLAGS "${COMPILER_VERBOSE_OUTPUT}")
    foreach(FLAG ${INTERNAL_ISYSTEM_FLAGS})
        string(REGEX REPLACE "-internal-isystem " "" INCLUDE_PATH "${FLAG}")
        list(APPEND CLANG_TIDY_EXTRA_ARGS
            "--extra-arg=-Xclang" "--extra-arg=-internal-isystem"
            "--extra-arg=-Xclang" "--extra-arg=${INCLUDE_PATH}")
    endforeach()
    string(REGEX MATCHALL "-idirafter [^ ]+" IDIRAFTER_FLAGS "${COMPILER_VERBOSE_OUTPUT}")
    foreach(FLAG ${IDIRAFTER_FLAGS})
        string(REGEX REPLACE "-idirafter " "" INCLUDE_PATH "${FLAG}")
        list(APPEND CLANG_TIDY_EXTRA_ARGS "--extra-arg=-idirafter" "--extra-arg=${INCLUDE_PATH}")
    endforeach()
    string(REGEX MATCH "-resource-dir [^ ]+" RESOURCE_DIR_FLAG "${COMPILER_VERBOSE_OUTPUT}")
    if(RESOURCE_DIR_FLAG)
        string(REGEX REPLACE "-resource-dir " "" RESOURCE_DIR "${RESOURCE_DIR_FLAG}")
        list(APPEND CLANG_TIDY_EXTRA_ARGS "--extra-arg=-resource-dir=${RESOURCE_DIR}")
    endif()
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" ${CLANG_TIDY_EXTRA_ARGS})
endif()

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(tomlplusplus REQUIRED)

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
