#include "config.hpp"

#include <gtest/gtest.h>

TEST(Config, ParseFullConfig) {
    auto config = reminder::parseConfig(R"(
        username = "ayangd"
        messages = ["Time for Duolingo!", "Your owl is watching..."]
        sound = "/usr/share/sounds/notification.wav"
    )");

    EXPECT_EQ(config.username, "ayangd");
    ASSERT_EQ(config.messages.size(), 2);
    EXPECT_EQ(config.messages[0], "Time for Duolingo!");
    EXPECT_EQ(config.messages[1], "Your owl is watching...");
    EXPECT_EQ(config.sound.value_or(""), "/usr/share/sounds/notification.wav");
}

TEST(Config, ParseMinimalConfig) {
    auto config = reminder::parseConfig(R"(username = "ayangd")");

    EXPECT_EQ(config.username, "ayangd");
    ASSERT_EQ(config.messages.size(), 1);
    EXPECT_EQ(config.messages[0], "Time for Duolingo!");
    EXPECT_FALSE(config.sound.has_value());
}

TEST(Config, DefaultMessageWhenAbsent) {
    auto config = reminder::parseConfig(R"(username = "test")");

    ASSERT_EQ(config.messages.size(), 1);
    EXPECT_EQ(config.messages[0], "Time for Duolingo!");
}

TEST(Config, EmptyUsernameThrows) {
    EXPECT_THROW(reminder::parseConfig(R"(username = "")"), std::runtime_error);
}

TEST(Config, MissingUsernameThrows) {
    EXPECT_THROW(reminder::parseConfig(R"(sound = "test.wav")"), std::runtime_error);
}

TEST(Config, InvalidTomlThrows) {
    EXPECT_THROW(reminder::parseConfig("invalid [[ toml"), std::runtime_error);
}

TEST(Config, MissingFileThrows) {
    EXPECT_THROW(reminder::loadConfig("/nonexistent/path/config.toml"), std::runtime_error);
}
