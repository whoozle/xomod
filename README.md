# Fully Featured Modular CHIP8 (XO/SuperCHIP) emulator.

This is a effectively a clone of John Earnest work, who's running OCTOJAM contests (check it out if you love 8 bit programming). 
Original project along with development tools is available in Octo repository. 
https://github.com/JohnEarnest/Octo

A lot of games are available on http://www.awfuljams.com/

This emulator was designed to be modular and simple, suitable for running in embedded environments or in terminal, using pseudographics.
Minimalistic backend API is allowing easy porting to new platforms or ways to render graphics. 

## Notes on Specific Backends

### Linux Console

* No sound (yet), add alsa backend(!)
* No input (yet)

### SDL2

Key mapping is matching original octo project

```
1234
qwer
asdf
zxcv
```
