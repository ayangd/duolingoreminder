find_package(GTest REQUIRED)

add_executable(duolingoreminder_tests
    duolingo_client_test.cpp
    config_test.cpp
    checker_test.cpp
    notifier_test.cpp
)

target_include_directories(duolingoreminder_tests PRIVATE ../src)

target_compile_options(duolingoreminder_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(duolingoreminder_tests PRIVATE
    GTest::gtest_main
    duolingoreminder_lib
)

include(GoogleTest)
gtest_discover_tests(duolingoreminder_tests)
