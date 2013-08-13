ifneq ($(GLX),)
CFLAGS=-DGLX -lX11
else
CFLAGS=-DSDL `sdl2-config --cflags` `sdl2-config --libs`
endif
rb:rb.c
	gcc -std=gnu99 -Os -march=native -s -ffast-math -fwhole-program -ftree-vectorize -o ringbeam rb.c ${CFLAGS} -lGL