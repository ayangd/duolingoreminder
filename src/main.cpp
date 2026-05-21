#include "checker.hpp"
#include "config.hpp"
#include "duolingo_client.hpp"
#include "notifier.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>

namespace {

auto todayString() -> std::string {
    auto now = std::chrono::system_clock::now();
    auto days = std::chrono::floor<std::chrono::days>(now);
    auto ymd = std::chrono::year_month_day{days};
    return std::format("{:04d}-{:02d}-{:02d}", static_cast<int>(ymd.year()),
                       static_cast<unsigned>(ymd.month()), static_cast<unsigned>(ymd.day()));
}

auto runCheck(const std::string& configPath) -> int {
    try {
        auto config = reminder::loadConfig(configPath);
        auto profile = reminder::fetchWithRetry(config.username);
        auto today = todayString();
        auto result = reminder::checkStreak(profile, today);

        if (result != reminder::CheckResult::DONE) {
            auto notification = reminder::formatNotification(profile, config.messages);
            auto iconPath = std::optional<std::string>{};
            auto soundPath = config.sound;
#ifdef ICON_PATH
            if (std::filesystem::exists(ICON_PATH)) {
                iconPath = ICON_PATH;
            }
#endif
#ifdef SOUND_PATH
            if (!soundPath && std::filesystem::exists(SOUND_PATH)) {
                soundPath = SOUND_PATH;
            }
#endif
            reminder::sendNotification(notification, soundPath, iconPath);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << std::format("Error: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

auto printProfile(const std::string& username) -> int {
    try {
        auto profile = duolingo::fetchPublicProfile(username);

        std::cout << std::format("User: {}\n", profile.username);
        std::cout << std::format("Streak: {}\n", profile.streak);
        std::cout << std::format("Total XP: {}\n", profile.totalXp);
        std::cout << std::format("Plus: {}\n", profile.hasPlus ? "yes" : "no");
        std::cout << std::format("Recent activity: {}\n",
                                 profile.hasRecentActivity15 ? "yes" : "no");

        if (profile.currentStreak.length.has_value()) {
            std::cout << std::format("Current streak length: {}\n",
                                     profile.currentStreak.length.value());
        }
        if (profile.currentStreak.endDate.has_value()) {
            std::cout << std::format("Streak end date: {}\n",
                                     profile.currentStreak.endDate.value());
        }

        std::cout << std::format("\nCourses ({}):\n", profile.courses.size());
        for (const auto& course : profile.courses) {
            std::cout << std::format("  {} ({}) — {} XP\n", course.title, course.learningLanguage,
                                     course.xp);
        }
    } catch (const std::exception& e) {
        std::cerr << std::format("Error: {}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

} // namespace

auto main(int argc, char* argv[]) -> int {
    if (argc < 2) {
        std::cerr << std::format("Usage: {} <username> | --check [--config <path>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    auto arg = std::string(argv[1]);

    if (arg == "--check") {
        auto configPath = reminder::defaultConfigPath();
        if (argc >= 4 && std::string(argv[2]) == "--config") {
            configPath = argv[3];
        }
        return runCheck(configPath);
    }

    return printProfile(arg);
}
