#include "notifier.hpp"

#include <gtest/gtest.h>

TEST(Notifier, BuildNotifyCommand) {
    auto data = reminder::NotificationData{.title = "Do your lesson!", .body = "Streak: 42"};
    auto cmd = reminder::buildNotifyCommand(data);

    ASSERT_EQ(cmd.size(), 4);
    EXPECT_EQ(cmd[0], "notify-send");
    EXPECT_EQ(cmd[1], "--app-name=Duolingo Reminder");
    EXPECT_EQ(cmd[2], "Do your lesson!");
    EXPECT_EQ(cmd[3], "Streak: 42");
}

TEST(Notifier, BuildNotifyCommandWithIcon) {
    auto data = reminder::NotificationData{.title = "Do your lesson!", .body = "Streak: 42"};
    auto cmd = reminder::buildNotifyCommand(data, "/path/to/icon.png");

    ASSERT_EQ(cmd.size(), 6);
    EXPECT_EQ(cmd[0], "notify-send");
    EXPECT_EQ(cmd[1], "--app-name=Duolingo Reminder");
    EXPECT_EQ(cmd[2], "-i");
    EXPECT_EQ(cmd[3], "/path/to/icon.png");
    EXPECT_EQ(cmd[4], "Do your lesson!");
    EXPECT_EQ(cmd[5], "Streak: 42");
}

TEST(Notifier, BuildSoundCommand) {
    auto cmd = reminder::buildSoundCommand("/usr/share/sounds/bell.wav");

    ASSERT_EQ(cmd.size(), 2);
    EXPECT_EQ(cmd[0], "paplay");
    EXPECT_EQ(cmd[1], "/usr/share/sounds/bell.wav");
}
