# nix-build default.nix && ./result/bin/yaml-parse
# nix-build default.nix && ./result/bin/yaml-parse-libcyaml

with import <nixpkgs> {};

let
  debugFlags = "-g";

  # copy paste from /etc/nixos/configuration.nix
  nur = import (builtins.fetchTarball {
    # NUR 2021-04-15
    url = "https://github.com/nix-community/NUR/archive/2ed3b8f5861313e9e8e8b39b1fb05f3a5a049325.tar.gz";
    sha256 = "1rpl2jpwvp05gj79xflw5ka6lv149rkikh6x7zhr3za36s27q5pz";
  }) { inherit pkgs; };

  libcyaml = nur.repos.suhr.libcyaml;

in
stdenv.mkDerivation {

  pname = "parse";
  version = "0.0.1";

  buildInputs = [
    glib # util fns, required by spotify anyway (zero cost)
    libyaml # low level yaml parser
    libcyaml # schema-based yaml parser
  ];

  nativeBuildInputs = [
    pkg-config
  ];

  makeFlags  = [
    "prefix=${placeholder "out"}"
    "sysconfdir=${placeholder "out"}/etc" # absolute path to /etc
  ];

  preBuild = ''
    # https://nixos.wiki/wiki/C
    NIX_CFLAGS_COMPILE="${debugFlags} $(pkg-config --cflags glib-2.0) $NIX_CFLAGS_COMPILE"

echo preBuild find:
find .

  '';

  src = ./.;
  sourceRoot = "source"; # TODO why is this default?
  unpackPhase = ''
    runHook preUnpack

    mkdir -p source
    cp -r $src/* source/
    chown -R nixbld:nixbld source

    runHook postUnpack
  '';

}

