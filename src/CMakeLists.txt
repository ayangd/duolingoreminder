add_executable(duolingoreminder main.cpp duolingo_client.cpp)

target_compile_options(duolingoreminder PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(duolingoreminder PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)
