{
  description = "Shell completions generator.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};

      default = pkgs.stdenv.mkDerivation {
        pname = "compgen";
        version = "0.0.1";

        src = ./.;

        doCheck = true;
        nativeCheckInputs = [
          pkgs.clang-tools
        ];

        nativeBuildInputs = with pkgs; [
          gnumake
        ];

        installPhase = ''
          make install PREFIX=$out
        '';
      };

      devShell = pkgs.mkShell {
        packages = with pkgs; [
          gnumake
          clang-tools
          bash
          coreutils
        ];
      };
    in
    {
      packages.${system}.default = default;
      devShells.${system}.default = devShell;
    };
}
