#pragma once

#include <optional>
#include <string>
#include <vector>

namespace reminder {

struct Config {
    std::string username;
    std::vector<std::string> messages;
    std::optional<std::string> sound;
};

auto loadConfig(const std::string& path) -> Config;
auto parseConfig(std::string_view tomlContent) -> Config;
auto defaultConfigPath() -> std::string;

} // namespace reminder
