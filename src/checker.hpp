#pragma once

#include "duolingo_client.hpp"

#include <chrono>
#include <cstdint>
#include <exception>
#include <random>
#include <string>
#include <vector>

namespace reminder {

enum class CheckResult : std::uint8_t { DONE, NOT_DONE, NO_STREAK };

struct NotificationData {
    std::string title;
    std::string body;
};

auto checkStreak(const duolingo::UserProfile& profile, const std::string& today) -> CheckResult;

auto formatNotification(const duolingo::UserProfile& profile,
                        const std::vector<std::string>& messages) -> NotificationData;

auto fetchWithRetry(const std::string& username, int maxRetries = 3,
                    std::chrono::seconds initialDelay = std::chrono::seconds{
                        30}) -> duolingo::UserProfile;

template <typename Fetcher, typename Sleeper>
auto retryCall(Fetcher&& fetch, int maxRetries, std::chrono::seconds initialDelay,
               Sleeper&& sleeper) -> decltype(fetch()) {
    std::exception_ptr lastException;
    auto delay = initialDelay;
    for (int attempt = 0; attempt <= maxRetries; ++attempt) {
        try {
            return fetch();
        } catch (...) {
            lastException = std::current_exception();
            if (attempt < maxRetries) {
                sleeper(delay);
                delay *= 2;
            }
        }
    }
    std::rethrow_exception(lastException);
}

} // namespace reminder
