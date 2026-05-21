add_library(duolingoreminder_lib STATIC
    duolingo_client.cpp
    config.cpp
    checker.cpp
    notifier.cpp
)

target_compile_options(duolingoreminder_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(duolingoreminder_lib PUBLIC
    CURL::libcurl
    nlohmann_json::nlohmann_json
    tomlplusplus::tomlplusplus
)

add_executable(duolingoreminder main.cpp)

target_compile_options(duolingoreminder PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_compile_definitions(duolingoreminder PRIVATE
    ICON_PATH="${CMAKE_INSTALL_PREFIX}/share/duolingoreminder/duolingo.png"
    SOUND_PATH="${CMAKE_INSTALL_PREFIX}/share/duolingoreminder/notification.mp3"
)

target_link_libraries(duolingoreminder PRIVATE duolingoreminder_lib)

install(TARGETS duolingoreminder RUNTIME DESTINATION bin)
install(FILES
    ${CMAKE_SOURCE_DIR}/assets/duolingo.png
    ${CMAKE_SOURCE_DIR}/assets/notification.mp3
    DESTINATION share/duolingoreminder
)
