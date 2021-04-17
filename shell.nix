# https://marketplace.visualstudio.com/items?itemName=arrterian.nix-env-selector

{ pkgs ? import <nixpkgs> { } }:
with pkgs;

let
  # https://github.com/nix-community/NUR#how-to-use
  # error: undefined variable 'nur' at /home/user/src/glib-config-parser/default.nix:11:14
  ####libcyaml = nur.repos.suhr.libcyaml;
in
mkShell {
  # buildInputs from default.nix - do we need shell.nix??
  buildInputs = [
    glib # util fns, required by spotify anyway (zero cost)
    libyaml # low level yaml parser
    ####libcyaml # schema-based yaml parser
  ];
}
