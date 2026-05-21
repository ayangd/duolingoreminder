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
                    "startDate": "2026-03-10"
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
    ASSERT_TRUE(profile.currentStreak.length.has_value());
    EXPECT_EQ(profile.currentStreak.length.value(), 42);
    ASSERT_TRUE(profile.currentStreak.startDate.has_value());
    EXPECT_EQ(profile.currentStreak.startDate.value(), "2026-03-10");
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
    EXPECT_TRUE(profile.courses.empty());
}
