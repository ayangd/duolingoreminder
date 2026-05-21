#include "duolingo_client.hpp"

#include <cstdlib>
#include <format>
#include <iostream>
#include <string>

auto main(int argc, char* argv[]) -> int {
    if (argc < 2) {
        std::cerr << std::format("Usage: {} <username>\n", argv[0]);
        return EXIT_FAILURE;
    }

    auto username = std::string(argv[1]);

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

        std::cout << std::format("\nCourses ({}):\n", profile.courses.size());
        for (const auto& course : profile.courses) {
            std::cout << std::format("  {} ({}) — {} XP\n", course.title,
                                     course.learningLanguage, course.xp);
        }
    } catch (const std::exception& e) {
        std::cerr << std::format("Error: {}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
