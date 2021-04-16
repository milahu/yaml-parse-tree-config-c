# yaml-parse-tree-config-c

how to parse a YAML config file in C code

using an [glib nary tree](https://developer.gimp.org/api/2.0/glib/glib-N-ary-Trees.html)
as intermediary data structure
as shown by [mk-fg @ stackoverflow.com](https://stackoverflow.com/a/621451/10440128)

## install

with the nix package manager, install is as simple as

```txt
git clone https://github.com/milahu/yaml-parse-tree-config-c.git
cd yaml-parse-tree-config-c
nix-build default.nix && ./result/bin/yaml-parse
```

## use case

https://github.com/abba23/spotify-adblock-linux/issues/119
