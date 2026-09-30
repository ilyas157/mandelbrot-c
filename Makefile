CC = gcc
CFLAGS = -O2 -Wall
LIBS = -lSDL2 -lm

mandelbrot: main.c matrix.c matrix.h
	$(CC) $(CFLAGS) main.c matrix.c -o mandelbrot $(LIBS)

clean:
	rm -f mandelbrot a.out
