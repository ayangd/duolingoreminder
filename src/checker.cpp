#include "checker.hpp"

#include <format>
#include <thread>

namespace reminder {

auto checkStreak(const duolingo::UserProfile& profile, const std::string& today) -> CheckResult {
    if (profile.streak == 0 && !profile.currentStreak.length.has_value()) {
        return CheckResult::NO_STREAK;
    }
    if (profile.currentStreak.endDate.has_value() &&
        profile.currentStreak.endDate.value() == today) {
        return CheckResult::DONE;
    }
    return CheckResult::NOT_DONE;
}

auto formatNotification(const duolingo::UserProfile& profile,
                        const std::vector<std::string>& messages) -> NotificationData {
    auto rd = std::random_device{};
    auto gen = std::mt19937{rd()};
    auto dist = std::uniform_int_distribution<std::size_t>{0, messages.size() - 1};

    return NotificationData{
        .title = messages[dist(gen)],
        .body = std::format("Streak: {} days", profile.streak),
    };
}

auto fetchWithRetry(const std::string& username, int maxRetries,
                    std::chrono::seconds initialDelay) -> duolingo::UserProfile {
    return retryCall([&username]() { return duolingo::fetchPublicProfile(username); }, maxRetries,
                     initialDelay,
                     [](std::chrono::seconds delay) { std::this_thread::sleep_for(delay); });
}

} // namespace reminder
