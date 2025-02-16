{ pkgs, ... }:

{
  packages = with pkgs; [
    llvmPackages_19.clang-tools
    cmake
    just

    gcc14
  ];
}
