# IPP
This repository contains my solution to the Individual Programming Project course offered by the Facultaty of Mathematics, Informatics and Mechanics 
at the University of Warsaw (further referred to as "MIM UW") in 2022/2023 summer semester. 

# Brief description
This project is the implementation of simple board / console game. At the beginning user gives the number of players, sizes of a board (width, height) and a number of "areas" that could be occupied by each player (area is the set of neighbours fields on the board, where two fields are called a neighbour fields if and only if they have a common edge). The program will create that board in terminal and user can simple walk through it putting on free fields some player numbers. The game is over when either there is no free fields or all players occupied add possible arreas.

# Full description

You can find the original task description(s) in Polish here:
- [part 1](https://github.com/MrRuper/IPP-simple-console-game/blob/main/full_description_part1.md)
- [part 2](https://github.com/MrRuper/IPP-simple-console-game/blob/main/full_description_part2.md)

# Note

This code uses lncurses library.

# How to use this program?

- Step 1: copy game.c, game.h, game_main.c, makefile to some_folder on Your local computer. 
- Step 2: go the some_folder and type:

```
make all
```

- Step 3: Type **ls** to make sure that **game** file was created.
- Step 4: Type 

``` 
./game width height number_of_players number_of_free_areas
```

where **width**, **height**, **number_of_players**, **number_of_free_areas** are some positive numbers. For example the command:
```
./game 4 3 5 2 
```

will create board 4x3 with 5 players and each player can occupy at most 2 areas on that board.

- Step 5: It is not needed but if You want to delete all files created in Step 2 type:

```
make clean
```

---

Copyright of the task's description and resources: MIM UW.
