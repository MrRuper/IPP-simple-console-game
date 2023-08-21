CC 	 = gcc
CPPFLAGS =
CFLAGS   = -Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2
LDFLAGS  =

.PHONY: all clean

all: game

game: game_example.o game.o
game_example.o: game_example.c
game.o: game.h game.c

valgrind_test: 
	valgrind --leak-check=full -q --error-exitcode=1 --track-origins=yes ./game

clean:
	rm -f *.o game
 
