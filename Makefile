Source := SDLGameBoy.cpp Sound.cpp IOs.cpp Memory.cpp CPU.cpp Graphics.cpp TimeSync.cpp

make: 
	clang++ -o sdlboy $(addprefix SDLGameBoy/, $(Source)) `sdl2-config --libs --cflags` -ggdb3 -O0 -Wall -lm -lSDL2_image

clean: 
	rm -f sdlboy
