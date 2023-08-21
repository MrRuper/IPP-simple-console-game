CC          = gcc
CFLAGS      = -Wall -Wextra -Wno-implicit-fallthrough -O2 -std=c17 -g
LDFLAGS     = -lncurses

.PHONY: all clean

all: game

game: game.o game_main.o
	$(CC) game.o game_main.o -o game $(LDFLAGS)

game.o: game.h
game_main.o: game.h

clean:
	rm -f *.o game

valgrind_test:
	valgrind --error-exitcode=123 -q --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all ./game $(ARGS)
