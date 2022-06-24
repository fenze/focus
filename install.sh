#!/bin/sh

clang -o focus focus.c -O3
strip ./focus

mkdir -p ~/.local/share/focus

cp ./focus /usr/local/bin/
cp ./.zsh /usr/share/zsh/functions/Completion/Unix/_focus
