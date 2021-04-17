# bear: https://discourse.nixos.org/t/get-clangd-to-find-standard-headers-in-nix-shell/11268/4
# https://marketplace.visualstudio.com/items?itemName=arrterian.nix-env-selector
# https://discourse.nixos.org/t/c-ide-integration-autocomplete-etc/4021/5
# https://github.com/NixOS/nixpkgs/issues/92739

{ pkgs ? import <nixpkgs> { } }:
with pkgs;

let

  # copy paste from /etc/nixos/configuration.nix
  nur = import (builtins.fetchTarball {
    # NUR 2021-04-15
    url = "https://github.com/nix-community/NUR/archive/2ed3b8f5861313e9e8e8b39b1fb05f3a5a049325.tar.gz";
    sha256 = "1rpl2jpwvp05gj79xflw5ka6lv149rkikh6x7zhr3za36s27q5pz";
  }) { inherit pkgs; };

  libcyaml = nur.repos.suhr.libcyaml;

  debugFlags = "-g";

in

clangStdenv.mkDerivation rec {

  name = "dev-shell";

  src = null;

  nativeBuildInputs = [
    bear

    # c language servers
    clang-tools # clangd
    #ccls # not working. why?

    pkg-config # locate glib includes
  ];

  # copy paste from default.nix
  buildInputs = [
    glib # util fns, required by spotify anyway (zero cost)
    libyaml # low level yaml parser
    libcyaml # schema-based yaml parser
  ];

  #cmakeFlags = [ "-DCMAKE_EXPORT_COMPILE_COMMANDS=YES" ];

  # https://github.com/NixOS/nixpkgs/issues/92739
  shellHooks = ''
    export PS1='\n\[\033[1;32m\][${name}:\w]\$\[\033[0m\] '

    # copy paste from default.nix
    NIX_CFLAGS_COMPILE="${debugFlags} $(pkg-config --cflags glib-2.0) $NIX_CFLAGS_COMPILE"

    # Makefile -> compile_commands.json for clangd
    # without `make clean`, bear would produce an empty compile_commands.json
    make clean; bear -- make
    echo compile_commands.json:; cat compile_commands.json
  '';

}
