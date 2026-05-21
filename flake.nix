{
  description = "duolingoreminder - C++23 Duolingo streak reminder";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    {
      homeManagerModules.default = import ./nix/hm-module.nix { inherit self; };
    } //
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        llvm = pkgs.llvmPackages_18;
      in {
        packages.default = llvm.stdenv.mkDerivation {
          pname = "duolingoreminder";
          version = "0.1.0";
          src = self;

          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
          ];

          buildInputs = [
            pkgs.nlohmann_json
            pkgs.curl
            pkgs.tomlplusplus
            pkgs.libnotify
          ];

          cmakeFlags = [
            "-DENABLE_CLANG_TIDY=OFF"
          ];
        };

        devShells.default = pkgs.mkShell.override { stdenv = llvm.stdenv; } {
          packages = [
            llvm.clang
            llvm.clang-tools
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
            pkgs.gtest
            pkgs.nlohmann_json
            pkgs.curl
            pkgs.tomlplusplus
            pkgs.libnotify
          ];

          shellHook = ''
            export CC=clang
            export CXX=clang++
          '';
        };
      });
}
