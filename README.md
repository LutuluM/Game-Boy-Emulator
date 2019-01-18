# Game-Boy-Emulator
A simple Game Boy emulator written in c/c++ using SDL2.0 for graphics

This emulator should support most roms that have either no memory controller, mbc1 or mbc3 style memory controllers for the Game Boy. Doesn't support sound or serial communications.

Requires the Game Boy bootrom* to run.

## TODO  
~~Fix Event Polling lag  ~~
Cap frame Rate  
Implement Sound  
Double Sprite Rendering  
Properly timed Timer  
Proper RealtimeClock  
Fix DMA Transfer  
mbc2 style memory controller  
optimization  

## Example with Tetris  
![intro](https://github.com/LutuluM/Game-Boy-Emulator/blob/master/screenshots/intro.PNG)
![LoadScreen](https://github.com/LutuluM/Game-Boy-Emulator/blob/master/screenshots/load%20screen.PNG)
![Game](https://github.com/LutuluM/Game-Boy-Emulator/blob/master/screenshots/game.PNG)

Print out of Vram  
![Vram](https://github.com/LutuluM/Game-Boy-Emulator/blob/master/screenshots/Memory.PNG)

*Bootloader and Roms NOT INCLUDED  
Game Boy is a register trademark of Nintendo.
