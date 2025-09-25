#!/bin/bash
FLAGS="-Wall -Wextra -Werror"
RAYLIB="-I./raylib-5.5/include -L./raylib-5.5/lib -l:libraylib.a -lm"
FILES="src/main.c src/player.c"
gcc $FLAGS -o main $FILES $RAYLIB
