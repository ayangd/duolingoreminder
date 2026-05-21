find_package(GTest REQUIRED)

add_executable(duolingoreminder_tests duolingo_client_test.cpp ../src/duolingo_client.cpp)

target_include_directories(duolingoreminder_tests PRIVATE ../src)

target_compile_options(duolingoreminder_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(duolingoreminder_tests PRIVATE
    GTest::gtest_main
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

include(GoogleTest)
gtest_discover_tests(duolingoreminder_tests)
