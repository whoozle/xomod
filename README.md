# Fully Featured Modular CHIP8 (XO/SuperCHIP) emulator.

This is a effectively a clone of John Earnest's work, who's running OCTOJAM contests (check it out if you love 8-bit programming). 

The original project, along with development tools, is available in his Octo repository:
https://github.com/JohnEarnest/Octo

A lot of games are available on http://www.awfuljams.com/

This emulator was designed to be modular and simple, suitable for running in embedded environments or in a terminal, using pseudographics.
The minimalistic backend API allows easy porting to new platforms or ways to render graphics. 

## How to build

```
mkdir build
cd build
cmake ..
make
cd ..
./build/xomod games/t8nks.ch8
```

## Instruction extension

5XYF used for dumping register range vX-vY without any side effects.

## Notes on Specific Backends

### SDL2

Key mapping is the same as in the original octo project

```
1234
qwer
asdf
zxcv
```

### Linux Console

* No sound (yet), add alsa backend(!)
* No input (yet)

# Well-known games gist ids

* Skyward, 8cf4cc3b15df836471d8a157eb7f5b4b
* Kesha was bird (the latest version with no keyboard warning), 75b2da4b0820c49b1412
* Kesha was biird, 3c3d86bfb79e84c66c969f505264485c

you can use ```tools/download-octo-gist``` tool to download source/convert option
