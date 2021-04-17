# nix-build default.nix && ./result/bin/yaml-parse

with import <nixpkgs> {};

let
  #debugFlags = "";
  debugFlags = "-g";

  # https://github.com/nix-community/NUR#how-to-use
  # error: undefined variable 'nur' at /home/user/src/glib-config-parser/default.nix:11:14
  ####libcyaml = nur.repos.suhr.libcyaml;

in
stdenv.mkDerivation {

  pname = "parse";
  version = "0.0.1";

  buildInputs = [
    glib # util fns, required by spotify anyway (zero cost)
    libyaml # low level yaml parser
    ####libcyaml # schema-based yaml parser
  ];

  makeFlags  = [
    "prefix=${placeholder "out"}"
    "sysconfdir=${placeholder "out"}/etc" # absolute path to /etc
  ];

/* TODO restore with libcyaml
    makeFlagsArray+=(CFLAGS="${debugFlags} -I${glib.dev}/include/glib-2.0 -I${glib.out}/lib/glib-2.0/include -I${libyaml}/include -I${libcyaml}/include")
    makeFlagsArray+=(LDFLAGS="-L${glib.out}/lib -L${libyaml.out}/lib -L${libcyaml.out}/lib")
*/

  # TODO use pkg-config to generate paths to include and lib
  preBuild = ''
    makeFlagsArray+=(CFLAGS="${debugFlags} -I${glib.dev}/include/glib-2.0 -I${glib.out}/lib/glib-2.0/include -I${libyaml}/include")
    makeFlagsArray+=(LDFLAGS="-L${glib.out}/lib -L${libyaml.out}/lib")
  '';

  src = ./src;
  sourceRoot = "source"; # TODO why is this default?
  unpackPhase = ''
    runHook preUnpack

    mkdir -p source
    cp -r $src/* source/
    chown -R nixbld:nixbld source

    runHook postUnpack
  '';

}

