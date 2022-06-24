#!/bin/sh

clang -o focus focus.c -O3
strip ./focus

# dev
# export ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer)
# export ASAN_OPTIONS=symbolize=1
# clang -g -o focus focus.c -Wall -fsanitize=address -O3

cp ./focus /usr/local/bin/
