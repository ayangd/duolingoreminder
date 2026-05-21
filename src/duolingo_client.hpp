#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace duolingo {

struct Course {
    std::string title;
    std::string learningLanguage;
    std::int64_t xp{};
};

struct Streak {
    std::optional<std::int64_t> length;
    std::optional<std::string> startDate;
};

struct UserProfile {
    std::int64_t id{};
    std::string username;
    std::int64_t streak{};
    Streak currentStreak;
    std::int64_t totalXp{};
    bool hasPlus{};
    bool hasRecentActivity15{};
    std::vector<Course> courses;
};

auto parseUserProfile(const nlohmann::json& json) -> UserProfile;

auto fetchPublicProfile(const std::string& username) -> UserProfile;

} // namespace duolingo
