# Rock Paper Scissors (C, raylib + raygui)

A small Rock Paper Scissors game with a graphical interface. Play against the computer, watch a short
"Rock... Paper... Scissors!" countdown, and keep score. Hands are drawn in code, so there are no image files.

Built with [raylib](https://github.com/raysan5/raylib) and [raygui](https://github.com/raysan5/raygui).

## Just want to play? (Windows)
Download `rps-windows.zip` from the **Releases** page, unzip it, and run `rps.exe`.

## Build from source
You need a C compiler and raylib. `raygui.h` is included in this repo.

### Windows (MinGW / w64devkit)
1. Download the raylib `win64_mingw-w64` zip from https://github.com/raysan5/raylib/releases and extract it.
2. Get a compiler, e.g. [w64devkit](https://github.com/skeeto/w64devkit/releases).
3. From this folder (adjust the raylib paths to where you extracted it):

```
gcc main.c -o rps.exe -O1 -Wall -Wno-unused-function -std=c99 -I "C:/raylib/include" -I . -L "C:/raylib/lib" -lraylib -lopengl32 -lgdi32 -lwinmm
rps.exe
```

