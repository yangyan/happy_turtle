main:main.c parse.c parse.h draw.c draw.h
	gcc -o $@ $^ -std=c99 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes \
	  -I/usr/include/SDL -lSDL -g
