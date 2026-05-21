{
  description = "duolingoreminder - C++23 Duolingo streak reminder";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        llvm = pkgs.llvmPackages_18;
      in {
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
          ];

          shellHook = ''
            export CC=clang
            export CXX=clang++
          '';
        };
      });
}
