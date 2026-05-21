#include "notifier.hpp"

#include <cstdlib>

#include <sys/wait.h>
#include <unistd.h>

namespace reminder {

auto buildNotifyCommand(const NotificationData& data,
                        const std::optional<std::string>& iconPath) -> std::vector<std::string> {
    auto cmd = std::vector<std::string>{"notify-send", "--app-name=Duolingo Reminder"};
    if (iconPath) {
        cmd.emplace_back("-i");
        cmd.emplace_back(*iconPath);
    }
    cmd.emplace_back(data.title);
    cmd.emplace_back(data.body);
    return cmd;
}

auto buildSoundCommand(const std::string& soundPath) -> std::vector<std::string> {
    return {"paplay", soundPath};
}

namespace {

auto execCommand(std::vector<std::string>& cmd, bool wait = false) -> void {
    auto pid = fork();
    if (pid == 0) {
        std::vector<char*> argv;
        argv.reserve(cmd.size() + 1);
        for (auto& arg : cmd) {
            argv.push_back(arg.data());
        }
        argv.push_back(nullptr);
        execvp(argv[0], argv.data());
        _exit(EXIT_FAILURE);
    }
    if (wait && pid > 0) {
        waitpid(pid, nullptr, 0);
    }
}

} // namespace

auto sendNotification(const NotificationData& data,
                      const std::optional<std::string>& soundPath, // NOLINT(bugprone-easily-swappable-parameters)
                      const std::optional<std::string>& iconPath) -> void {
    auto notifyCmd = buildNotifyCommand(data, iconPath);
    execCommand(notifyCmd);

    if (soundPath) {
        auto soundCmd = buildSoundCommand(*soundPath);
        execCommand(soundCmd, true);
    }
}

} // namespace reminder
