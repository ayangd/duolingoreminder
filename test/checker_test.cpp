#include "checker.hpp"

#include <gtest/gtest.h>

namespace {

auto makeProfile(std::int64_t streak, std::optional<std::int64_t> streakLength,
                 std::optional<std::string> endDate,
                 std::int64_t totalXp = 1000) -> duolingo::UserProfile {
    auto profile = duolingo::UserProfile{};
    profile.username = "testuser";
    profile.streak = streak;
    profile.totalXp = totalXp;
    profile.currentStreak.length = streakLength;
    profile.currentStreak.endDate = std::move(endDate);
    return profile;
}

} // namespace

TEST(CheckStreak, DoneToday) {
    auto profile = makeProfile(42, 42, "2026-05-01");
    EXPECT_EQ(reminder::checkStreak(profile, "2026-05-01"), reminder::CheckResult::DONE);
}

TEST(CheckStreak, NotDoneToday) {
    auto profile = makeProfile(42, 42, "2026-04-30");
    EXPECT_EQ(reminder::checkStreak(profile, "2026-05-01"), reminder::CheckResult::NOT_DONE);
}

TEST(CheckStreak, NoEndDate) {
    auto profile = makeProfile(42, 42, std::nullopt);
    EXPECT_EQ(reminder::checkStreak(profile, "2026-05-01"), reminder::CheckResult::NOT_DONE);
}

TEST(CheckStreak, NoStreak) {
    auto profile = makeProfile(0, std::nullopt, std::nullopt);
    EXPECT_EQ(reminder::checkStreak(profile, "2026-05-01"), reminder::CheckResult::NO_STREAK);
}

TEST(FormatNotification, ContainsStreak) {
    auto profile = makeProfile(42, 42, "2026-04-30", 5000);
    auto result = reminder::formatNotification(profile, {"Do your lesson!"});

    EXPECT_EQ(result.title, "Do your lesson!");
    EXPECT_NE(result.body.find("42"), std::string::npos);
}

TEST(FormatNotification, SelectsFromMessages) {
    auto profile = makeProfile(10, 10, "2026-04-30");
    auto messages = std::vector<std::string>{"Msg A", "Msg B", "Msg C"};
    auto result = reminder::formatNotification(profile, messages);

    auto found = result.title == "Msg A" || result.title == "Msg B" || result.title == "Msg C";
    EXPECT_TRUE(found);
}

TEST(RetryCall, SucceedsFirstTry) {
    int calls = 0;
    auto result = reminder::retryCall(
        [&]() {
            ++calls;
            return 42;
        },
        3, std::chrono::seconds{1}, [](std::chrono::seconds) {});

    EXPECT_EQ(result, 42);
    EXPECT_EQ(calls, 1);
}

TEST(RetryCall, SucceedsAfterRetry) {
    int calls = 0;
    auto result = reminder::retryCall(
        [&]() -> int {
            ++calls;
            if (calls < 3) {
                throw std::runtime_error("fail");
            }
            return 42;
        },
        3, std::chrono::seconds{1}, [](std::chrono::seconds) {});

    EXPECT_EQ(result, 42);
    EXPECT_EQ(calls, 3);
}

TEST(RetryCall, ThrowsAfterAllRetries) {
    int calls = 0;
    EXPECT_THROW(reminder::retryCall(
                     [&]() -> int {
                         ++calls;
                         throw std::runtime_error("fail");
                     },
                     3, std::chrono::seconds{1}, [](std::chrono::seconds) {}),
                 std::runtime_error);

    EXPECT_EQ(calls, 4);
}

TEST(RetryCall, BackoffDoubles) {
    std::vector<std::chrono::seconds> delays;
    EXPECT_THROW(reminder::retryCall([&]() -> int { throw std::runtime_error("fail"); }, 3,
                                     std::chrono::seconds{30},
                                     [&](std::chrono::seconds d) { delays.push_back(d); }),
                 std::runtime_error);

    ASSERT_EQ(delays.size(), 3);
    EXPECT_EQ(delays[0].count(), 30);
    EXPECT_EQ(delays[1].count(), 60);
    EXPECT_EQ(delays[2].count(), 120);
}
