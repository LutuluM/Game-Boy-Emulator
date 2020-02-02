# Game-Boy-Emulator
A simple Game Boy emulator written in c/c++ using SDL2.0 for graphics, audio, and buttons.

This emulator should support most roms that have either no memory controller, mbc1 or mbc3 style memory controllers for the Game Boy. Doesn't support serial communications.
Sound has been added at a basic level. No white noise channel and only frequency is modified for each channel. i.e. no frequency shifting, setting play lengths have basic implimentations, ect. 

Requires the Game Boy bootrom* to run.

Confirmed working roms.
Pokemon Red, Pokemon Blue, Tetris

## TODO
Final Implimentations of Sound
Change Audio from reading from registers to variables, register writes to will update vairables of audio channels 
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
