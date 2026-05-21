{ self }:

{ config, lib, pkgs, ... }:

let
  cfg = config.services.duolingoreminder;

  configFile = pkgs.writeText "duolingoreminder-config.toml" ''
    username = "${cfg.username}"

    messages = [
    ${lib.concatMapStringsSep "\n" (msg: "    \"${msg}\",") cfg.messages}
    ]

    ${lib.optionalString (cfg.sound != null) "sound = \"${cfg.sound}\""}
  '';
in
{
  options.services.duolingoreminder = {
    enable = lib.mkEnableOption "Duolingo streak reminder";

    package = lib.mkOption {
      type = lib.types.package;
      default = self.packages.${pkgs.system}.default;
      description = "The duolingoreminder package to use.";
    };

    username = lib.mkOption {
      type = lib.types.str;
      description = "Duolingo username to check.";
    };

    messages = lib.mkOption {
      type = with lib.types; listOf str;
      default = [
        "Time for Duolingo!"
        "Your owl is watching..."
        "Don't break the streak!"
      ];
      description = "Notification messages (one picked at random).";
    };

    sound = lib.mkOption {
      type = with lib.types; nullOr path;
      default = null;
      description = "Custom sound file path. Uses bundled Duolingo ding when null.";
    };

    schedule = lib.mkOption {
      type = with lib.types; either str (listOf str);
      default = "*-*-* 09:00,13:00,19:00:00";
      description = "systemd OnCalendar schedule. String or list of strings for multiple entries.";
    };
  };

  config = lib.mkIf cfg.enable {
    systemd.user.services.duolingoreminder = {
      Unit.Description = "Duolingo streak reminder";

      Service = {
        Type = "oneshot";
        ExecStart = "${cfg.package}/bin/duolingoreminder --check --config ${configFile}";
        Environment = [
          "PATH=${lib.makeBinPath [ pkgs.libnotify pkgs.pulseaudio ]}:$PATH"
        ];
      };
    };

    systemd.user.timers.duolingoreminder = {
      Unit.Description = "Duolingo reminder schedule";

      Timer.OnCalendar = cfg.schedule;

      Install.WantedBy = [ "timers.target" ];
    };
  };
}
