#include "config.hpp"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <stdexcept>
#include <system_error>

#include <toml++/toml.hpp>

namespace reminder {

namespace {

auto extractConfig(const toml::table& tbl) -> Config {
    auto config = Config{};

    auto username = tbl["username"].value<std::string>();
    if (!username || username->empty()) {
        throw std::runtime_error("Config: 'username' is required and must be non-empty");
    }
    config.username = *username;

    if (auto* arr = tbl["messages"].as_array()) {
        for (const auto& elem : *arr) {
            if (auto val = elem.value<std::string>()) {
                config.messages.push_back(*val);
            }
        }
    }
    if (config.messages.empty()) {
        config.messages.emplace_back("Time for Duolingo!");
    }

    if (auto sound = tbl["sound"].value<std::string>()) {
        config.sound = *sound;
    }

    return config;
}

} // namespace

auto parseConfig(std::string_view tomlContent) -> Config {
    try {
        return extractConfig(toml::parse(tomlContent));
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(std::format("Invalid config: {}", e.what()));
    }
}

auto loadConfig(const std::string& path) -> Config {
    auto ec = std::error_code{};
    if (!std::filesystem::exists(path, ec)) {
        throw std::runtime_error(std::format("Config file not found: {}", path));
    }
    try {
        return extractConfig(toml::parse_file(path));
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(std::format("Invalid config '{}': {}", path, e.what()));
    }
}

auto defaultConfigPath() -> std::string {
    auto* home = std::getenv("HOME"); // NOLINT(concurrency-mt-unsafe)
    if (home == nullptr) {
        throw std::runtime_error("HOME environment variable not set");
    }
    return std::format("{}/.config/duolingoreminder/config.toml", home);
}

} // namespace reminder
