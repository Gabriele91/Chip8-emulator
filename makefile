UNAME := $(shell uname)
CC := gcc
LDLIBS := -lmingw32 -lgdi32 -lopenGL32 -lglu32 -lwinmm -lcomdlg32


all: release debug clean

release:  main.c chip8.o window.o render.o
	$(CC) main.c chip8.o window.o render.o -O2 -o chip8.exe $(LDLIBS)

debug:  main.c chip8_d.o window_d.o render_d.o
	$(CC) main.c chip8_d.o window_d.o render_d.o -g -o chip8_d.exe $(LDLIBS)

chip8.o: chip.h render.h window.h chip_8.c 
	$(CC) -c chip_8.c -O2 -o chip8.o 

chip8_d.o: chip.h render.h window.h chip_8.c 
	$(CC) -c chip_8.c -g -o chip8_d.o 

window.o: chip.h render.h window.h window.c 
	$(CC) -c window.c -O2 -o window.o
	
window_d.o: chip.h render.h window.h window.c 
	$(CC) -c window.c -g -o window_d.o 
	
render.o: chip.h render.h window.h render.c 
	$(CC) -c render.c -O2 -o render.o
	
render_d.o: chip.h render.h window.h render.c 
	$(CC) -c render.c -g -o render_d.o

clean:
	rm -rf *o