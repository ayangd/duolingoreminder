#include "duolingo_client.hpp"

#include <format>
#include <memory>
#include <stdexcept>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace duolingo {

namespace {

auto writeCallback(char* data, size_t size, size_t nmemb, std::string* buffer) -> size_t {
    auto totalSize = size * nmemb;
    buffer->append(data, totalSize);
    return totalSize;
}

struct CurlDeleter {
    void operator()(CURL* handle) const noexcept { curl_easy_cleanup(handle); }
};

using CurlHandle = std::unique_ptr<CURL, CurlDeleter>;

auto httpGet(const std::string& url) -> std::string {
    CurlHandle handle(curl_easy_init());
    if (!handle) {
        throw std::runtime_error("Failed to initialize curl");
    }

    std::string response;

    curl_easy_setopt(handle.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle.get(), CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(handle.get(), CURLOPT_USERAGENT,
                     "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
    curl_easy_setopt(handle.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle.get(), CURLOPT_TIMEOUT, 30L);

    auto result = curl_easy_perform(handle.get());
    if (result != CURLE_OK) {
        throw std::runtime_error(
            std::format("HTTP request failed: {}", curl_easy_strerror(result)));
    }

    long httpCode = 0;
    curl_easy_getinfo(handle.get(), CURLINFO_RESPONSE_CODE, &httpCode);
    if (httpCode != 200) {
        throw std::runtime_error(std::format("HTTP {}: {}", httpCode, url));
    }

    return response;
}

} // namespace

auto parseUserProfile(const nlohmann::json& json) -> UserProfile {
    auto profile = UserProfile{};

    const auto& user = json.contains("users") ? json["users"][0] : json;

    profile.id = user.value("id", std::int64_t{0});
    profile.username = user.value("username", "");
    profile.streak = user.value("streak", std::int64_t{0});
    profile.totalXp = user.value("totalXp", std::int64_t{0});
    profile.hasPlus = user.value("hasPlus", false);
    profile.hasRecentActivity15 = user.value("hasRecentActivity15", false);

    if (user.contains("streakData") && user["streakData"].contains("currentStreak") &&
        !user["streakData"]["currentStreak"].is_null()) {
        const auto& cs = user["streakData"]["currentStreak"];
        profile.currentStreak.length = cs.value("length", std::int64_t{0});
        if (cs.contains("startDate") && !cs["startDate"].is_null()) {
            profile.currentStreak.startDate = cs.value("startDate", "");
        }
        if (cs.contains("endDate") && !cs["endDate"].is_null()) {
            profile.currentStreak.endDate = cs.value("endDate", "");
        }
    }

    if (user.contains("courses") && user["courses"].is_array()) {
        for (const auto& c : user["courses"]) {
            profile.courses.push_back(Course{
                .title = c.value("title", ""),
                .learningLanguage = c.value("learningLanguage", ""),
                .xp = c.value("xp", std::int64_t{0}),
            });
        }
    }

    return profile;
}

auto fetchPublicProfile(const std::string& username) -> UserProfile {
    auto url = std::format("https://www.duolingo.com/2017-06-30/users?username={}", username);
    auto response = httpGet(url);
    auto json = nlohmann::json::parse(response);
    return parseUserProfile(json);
}

} // namespace duolingo
