# nix-build default.nix && ./result/bin/yaml-parse

with import <nixpkgs> {};

let
  #debugFlags = "";
  debugFlags = "-g";
in
stdenv.mkDerivation {

  pname = "parse";
  version = "0.0.1";

  buildInputs = [
    glib
    libyaml
  ];

  makeFlags  = [
    "prefix=${placeholder "out"}"
    "sysconfdir=${placeholder "out"}/etc" # absolute path to /etc
  ];

  preBuild = ''
    makeFlagsArray+=(CFLAGS="${debugFlags} -I${glib.dev}/include/glib-2.0 -I${glib.out}/lib/glib-2.0/include -I${libyaml}/include") # glib.h + glibconfig.h
    makeFlagsArray+=(LDFLAGS="-L${glib.out}/lib -L${libyaml.out}/lib") # libglib-2.0.so
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

