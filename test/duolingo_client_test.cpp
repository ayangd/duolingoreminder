#include "duolingo_client.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

TEST(ParseUserProfile, FullProfile) {
    auto json = nlohmann::json::parse(R"({
        "users": [{
            "id": 623077017,
            "username": "LuisvonAhn",
            "streak": 42,
            "totalXp": 39407,
            "hasPlus": true,
            "hasRecentActivity15": false,
            "streakData": {
                "currentStreak": {
                    "length": 42,
                    "startDate": "2026-03-10",
                    "endDate": "2026-04-20"
                }
            },
            "courses": [
                {"title": "French", "learningLanguage": "fr", "xp": 8889},
                {"title": "German", "learningLanguage": "de", "xp": 7427}
            ]
        }]
    })");

    auto profile = duolingo::parseUserProfile(json);

    EXPECT_EQ(profile.id, 623077017);
    EXPECT_EQ(profile.username, "LuisvonAhn");
    EXPECT_EQ(profile.streak, 42);
    EXPECT_EQ(profile.totalXp, 39407);
    EXPECT_TRUE(profile.hasPlus);
    EXPECT_FALSE(profile.hasRecentActivity15);
    EXPECT_EQ(profile.currentStreak.length.value_or(0), 42);
    EXPECT_EQ(profile.currentStreak.startDate.value_or(""), "2026-03-10");
    EXPECT_EQ(profile.currentStreak.endDate.value_or(""), "2026-04-20");
    ASSERT_EQ(profile.courses.size(), 2);
    EXPECT_EQ(profile.courses[0].title, "French");
    EXPECT_EQ(profile.courses[0].learningLanguage, "fr");
    EXPECT_EQ(profile.courses[0].xp, 8889);
    EXPECT_EQ(profile.courses[1].title, "German");
}

TEST(ParseUserProfile, NullStreak) {
    auto json = nlohmann::json::parse(R"({
        "users": [{
            "id": 1,
            "username": "nostreak",
            "streak": 0,
            "totalXp": 100,
            "hasPlus": false,
            "hasRecentActivity15": false,
            "streakData": {
                "currentStreak": null
            },
            "courses": []
        }]
    })");

    auto profile = duolingo::parseUserProfile(json);

    EXPECT_EQ(profile.streak, 0);
    EXPECT_FALSE(profile.currentStreak.length.has_value());
    EXPECT_FALSE(profile.currentStreak.startDate.has_value());
    EXPECT_FALSE(profile.currentStreak.endDate.has_value());
    EXPECT_TRUE(profile.courses.empty());
}

TEST(ParseUserProfile, MissingFields) {
    auto json = nlohmann::json::parse(R"({
        "users": [{"id": 1}]
    })");

    auto profile = duolingo::parseUserProfile(json);

    EXPECT_EQ(profile.id, 1);
    EXPECT_EQ(profile.username, "");
    EXPECT_EQ(profile.streak, 0);
    EXPECT_EQ(profile.totalXp, 0);
    EXPECT_FALSE(profile.currentStreak.endDate.has_value());
    EXPECT_TRUE(profile.courses.empty());
}

TEST(ParseUserProfile, EndDateAbsent) {
    auto json = nlohmann::json::parse(R"({
        "users": [{
            "id": 1,
            "username": "test",
            "streak": 10,
            "totalXp": 500,
            "streakData": {
                "currentStreak": {
                    "length": 10,
                    "startDate": "2026-04-10"
                }
            },
            "courses": []
        }]
    })");

    auto profile = duolingo::parseUserProfile(json);

    ASSERT_TRUE(profile.currentStreak.startDate.has_value());
    EXPECT_FALSE(profile.currentStreak.endDate.has_value());
}

TEST(ParseUserProfile, EndDateNull) {
    auto json = nlohmann::json::parse(R"({
        "users": [{
            "id": 1,
            "username": "test",
            "streak": 10,
            "totalXp": 500,
            "streakData": {
                "currentStreak": {
                    "length": 10,
                    "startDate": "2026-04-10",
                    "endDate": null
                }
            },
            "courses": []
        }]
    })");

    auto profile = duolingo::parseUserProfile(json);

    EXPECT_FALSE(profile.currentStreak.endDate.has_value());
}
