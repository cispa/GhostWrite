{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        pkgsCross = import nixpkgs { inherit system; crossSystem = { config = "riscv64-linux-gnu"; }; };
      in
      {
        devShells.default = pkgsCross.mkShell {
          buildInputs = [ pkgsCross.glibc.static ];
        };
      });
}
