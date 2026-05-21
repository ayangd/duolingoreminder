#pragma once

#include "checker.hpp"

#include <optional>
#include <string>
#include <vector>

namespace reminder {

auto buildNotifyCommand(const NotificationData& data,
                        const std::optional<std::string>& iconPath = {}) -> std::vector<std::string>;
auto buildSoundCommand(const std::string& soundPath) -> std::vector<std::string>;
auto sendNotification(const NotificationData& data,
                      const std::optional<std::string>& soundPath,
                      const std::optional<std::string>& iconPath = {}) -> void;

} // namespace reminder
