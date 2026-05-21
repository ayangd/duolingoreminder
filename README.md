# duolingoreminder

C++23 Duolingo streak reminder. Checks your public profile and sends a desktop notification with the Duo owl icon and a ding if you haven't done your lesson today.

## Setup

### Install via Nix

```bash
nix build
nix profile install .
```

Or run directly:

```bash
nix run . -- --check
```

### Runtime dependencies

- `notify-send` (from `libnotify`) for desktop notifications
- `paplay` (from PulseAudio/PipeWire) for notification sound

On NixOS, add `libnotify` to your system or home-manager packages. `paplay` is typically available via PipeWire or PulseAudio.

### Configuration

Create `~/.config/duolingoreminder/config.toml`:

```toml
username = "your-duolingo-username"

# Optional: custom nudge messages (random selection). Default: "Time for Duolingo!"
messages = [
    "Time for Duolingo!",
    "Your owl is watching...",
    "Don't break the streak!",
]

# Optional: override the bundled notification sound
# sound = "/path/to/custom/sound.mp3"
```

The bundled Duolingo correct-answer ding plays by default. Set `sound` only to override it.

### Home Manager module

Add the flake input and import the module:

```nix
# flake.nix
inputs.duolingoreminder = {
  url = "github:ayangd/duolingoreminder";
  inputs.nixpkgs.follows = "nixpkgs";
};
```

```nix
# home.nix
{ inputs, pkgs, ... }:
{
  imports = [ inputs.duolingoreminder.homeManagerModules.default ];

  services.duolingoreminder = {
    enable = true;
    username = "your-duolingo-username";

    # Optional
    messages = [
      "Time for Duolingo!"
      "Your owl is watching..."
    ];
    schedule = "*-*-* 09:00,13:00,19:00:00"; # default
    # sound = /path/to/custom/sound.mp3;
    # package = inputs.duolingoreminder.packages.${pkgs.system}.default.override {
    #   stdenv = pkgs.llvmPackages_18.stdenv;  # use clang instead of gcc
    # };
  };
}
```

This generates the config file, systemd service + timer, and ensures `notify-send` and `paplay` are in the service PATH.

### Manual systemd timer

```ini
# ~/.config/systemd/user/duolingoreminder.service
[Unit]
Description=Duolingo streak reminder

[Service]
Type=oneshot
ExecStart=%h/.nix-profile/bin/duolingoreminder --check
```

```ini
# ~/.config/systemd/user/duolingoreminder.timer
[Unit]
Description=Duolingo reminder schedule

[Timer]
OnCalendar=*-*-* 09:00,13:00,19:00:00

[Install]
WantedBy=timers.target
```

```bash
systemctl --user enable --now duolingoreminder.timer
```

## Usage

### Print profile

```bash
duolingoreminder <username>
```

```
$ duolingoreminder LuisvonAhn
User: LuisvonAhn
Streak: 0
Total XP: 39407
Plus: yes
Courses (4):
  French (fr) — 8889 XP
  English (en) — 9868 XP
  German (de) — 7427 XP
  Spanish (es) — 13223 XP
```

### Check streak and notify

```bash
duolingoreminder --check
duolingoreminder --check --config /path/to/config.toml
```

Fetches streak, sends notification with icon and sound if lesson not done today. Retries failed API calls 3x with exponential backoff (30s, 60s, 120s). Exit 0 on success, 1 on error.

## Development

### Dev environment

```bash
nix develop
cmake -B build -G Ninja
cmake --build build
```

### Tests

```bash
ctest --test-dir build --output-on-failure
```

### Tooling

| Tool | Purpose |
|------|---------|
| Clang 18 | Compiler (C++23) |
| CMake | Build system |
| Ninja | Build backend |
| clang-tidy | Linter (runs during build) |
| clang-format | Formatter (LLVM-based) |
| GoogleTest | Testing |

Disable clang-tidy for fast iteration:

```bash
cmake -B build-fast -G Ninja -DENABLE_CLANG_TIDY=OFF
cmake --build build-fast
```

### Bundled assets

| File | Purpose |
|------|---------|
| `assets/duolingo.png` | Duo owl icon for notifications |
| `assets/notification.mp3` | Duolingo correct-answer ding |

Installed to `$prefix/share/duolingoreminder/` by CMake. The binary falls back gracefully if assets are missing (e.g., dev builds without `cmake --install`).

## API

No official Duolingo API. All endpoints reverse-engineered, can break anytime.

| Endpoint | Auth | Status |
|----------|------|--------|
| `GET /2017-06-30/users?username=X` | None | Works |

Public data available without auth: streak, totalXp, courses, hasPlus, recentActivity, `streakData.currentStreak.endDate`.

Streak logic: `endDate == today` means lesson done — no notification sent.

## Stack

- C++23 / Clang 18
- libcurl (RAII-wrapped, no raw pointers)
- nlohmann/json
- toml++ (config parsing)
- Nix flake (dev shell + package derivation)
