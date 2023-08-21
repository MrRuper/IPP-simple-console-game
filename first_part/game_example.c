/**
* @author Marcin Peczarski <marpe@mimuw.edu.pl>
* @copyright Uniwersytet Warszawski
* @date 2023
*/

/**
 * W tym pliku nawet w wersji release chcemy korzystac z asercji.
 */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Tak ma wygladac plansza po wykonaniu caleego ponizszego przykladu:
 */
static const char board[] =
        "1.........\n"
        "..........\n"
        "..........\n"
        "..........\n"
        ".....1....\n"
        "..........\n"
        "...2......\n"
        "1..22.....\n"
        "1222......\n"
        "1.........\n";

static const char test[] =
        "121...1\n"
        "111....\n"
        "..11...\n"
        "..15...\n"
        "1111111\n";

/** @brief Testuje silnik gry.
 * Przeprowadza przykladowe testy silnika gry.
 * @return Zero, gdy wszystkie testy przebiegly poprawnie,
 * a w przeciwnym przypadku kod bledu.
 */
int main() {
    game_t *g;

    g = game_new(100,100,10000,100000);
    game_delete(g);
    
    g = game_new(7, 5, 12, 4);
    assert(g != NULL);

    for (uint32_t i = 1; i < 12; i++) {
        assert(game_busy_fields(g, i) == 0);
        assert(game_free_fields(g, i) == 35);
    }

    assert(game_board_width(g) == 7);
    assert(game_board_height(g) == 5);
    assert(game_players(g) == 12);

    assert(game_move(g, 1, 0, 0));
    assert(game_move(g, 1, 6, 0));
    assert(game_busy_fields(g, 1) == 2);
    assert(game_free_fields(g, 1) == 33);
    assert(game_move(g, 1, 6, 4));
    assert(game_move(g, 1, 0, 4));

    assert(game_free_fields(g, 1) == 8);
    assert(!game_move(g, 1, 3, 3));
    assert(game_move(g, 2, 1, 4));
    assert(game_free_fields(g, 1) == 7);
    assert(game_free_fields(g, 2) == 30);

    for (uint32_t i = 1; i < 6; i++) {
        if (i == 6) {
            assert(game_free_fields(g, 1) == 10) ;
        }
        assert(game_move(g, 1, i, 0));
    }

    for (uint32_t i = 1; i < 12; i++) {
        assert(!game_move(g, i, 0, 6));
    }

    assert(game_free_fields(g, 1) == 25);
/*
    char *q = game_board(g);
    assert(q);
    assert(strcmp(q, test) == 0);
    free(q);
*/
    assert(game_move(g, 1, 3, 2));
    assert(!game_move(g, 1, 0, 2));
    assert(game_busy_fields(g, 1) == 10);
    assert(game_free_fields(g, 1) == 13);

    for (uint32_t i = 2; i <= 12; i++) {
        assert(game_free_fields(g, i) == 24);
    }

    assert(game_move(g, 5, 3, 1));
    assert(game_free_fields(g, 1) == 12);
    assert(game_move(g, 1, 2, 1));
    assert(game_move(g, 1, 2, 2));
    assert(game_free_fields(g, 1) == 21);

    assert(game_move(g, 1, 2, 4));
    assert(game_move(g, 1, 0, 3));
    assert(!game_move(g, 1, 6, 2));
    assert(game_free_fields(g, 1) == 14);
    assert(game_move(g, 1, 1, 3));
    assert(game_free_fields(g, 1) == 13);
    assert(game_move(g, 1, 2, 3));
    assert(game_free_fields(g, 1) == 17);

    char *q = game_board(g);
    assert(q);
    assert(strcmp(q, test) == 0);
    free(q);
    game_delete(g);

    g = game_new(0, 0, 0, 0);
    assert(g == NULL);
    g = game_new(10, 10, 2, 3);
    assert(g != NULL);
    assert(game_move(g, 1, 0, 0));
    assert(game_busy_fields(g, 1) == 1);
    assert(game_busy_fields(g, 2) == 0);
    assert(game_free_fields(g, 1) == 99);
    assert(game_free_fields(g, 2) == 99);
    assert(game_move(g, 2, 3, 1));
    assert(game_busy_fields(g, 1) == 1);
    assert(game_busy_fields(g, 2) == 1);
    assert(game_free_fields(g, 1) == 98);
    assert(game_free_fields(g, 2) == 98);
    assert(game_move(g, 1, 0, 2));
    assert(game_move(g, 1, 0, 9));
    assert(!game_move(g, 1, 5, 5));
    assert(game_free_fields(g, 1) == 6);
    assert(game_move(g, 1, 0, 1));
    assert(game_free_fields(g, 1) == 95);
    assert(game_move(g, 1, 5, 5));
    assert(!game_move(g, 1, 6, 6));
    assert(game_busy_fields(g, 1) == 5);
    assert(game_free_fields(g, 1) == 10);
    assert(game_move(g, 2, 2, 1));
    assert(game_move(g, 2, 1, 1));
    assert(game_free_fields(g, 1) == 9);
    assert(game_free_fields(g, 2) == 92);
    assert(!game_move(g, 2, 0, 1));
    assert(game_move(g, 2, 4, 2));
    assert(game_move(g, 2, 3, 3));
    assert(game_free_fields(g, 2) == 11);


    assert(game_busy_fields(g, 1) == 5);
    assert(game_free_fields(g, 1) == 9);
    assert(game_busy_fields(g, 2) == 5);
    assert(game_move(g, 2, 3, 2));
    assert(game_free_fields(g, 2) == 89);

    char *p = game_board(g);
    assert(p);
    printf(p);
    printf("\n");
    assert(strcmp(p, board) == 0);

    free(p);

    game_delete(g);
    printf("wszystko ok\n");


    g = game_new(1, 11, UINT32_MAX, 0);
    assert(g == NULL);
    assert(game_board_width(g) == 0);
   /* g = game_new(SIZE_MAX, SIZE_MAX, UINT32_MAX, UINT32_MAX);
    assert(g == NULL);
*/
    g = game_new(3, 3, 2, 4);
    assert(g != NULL);
    assert(game_board_height(g) == 3);
    assert(game_board_width(g) == 3);
    assert(game_players(g) == 2);
    assert(game_move(g, 1, 0, 0));
    assert(!game_move(g, 2, 0, 0));
    assert(!game_move(g, 3, 1, 1));
    assert(!game_move(g, 1, 0, 0));
    assert(game_busy_fields(g, 1) == 1);
    assert(game_busy_fields(g, 2) == 0);
    assert(game_free_fields(g, 1) == 8);
    assert(game_free_fields(g, 2) == 8);
    assert(game_move(g, 1, 2, 0));
    assert(game_busy_fields(g, 1) == 2);
    assert(game_busy_fields(g, 2) == 0);
    assert(game_free_fields(g, 1) == 7);
    assert(game_free_fields(g, 2) == 7);
    assert(game_free_fields(g, 12) == 0);
    assert(game_move(g, 1, 0, 2));
    assert(game_move(g, 1, 2, 2));
    assert(game_busy_fields(g, 1) == 4);
    assert(game_free_fields(g, 1) == 4);
    assert(!game_move(g, 1, 1, 1));
    assert(game_move(g, 2, 1, 0));
    assert(game_free_fields(g, 1) == 3);
    assert(game_free_fields(g, 2) == 4);
    assert(!game_move(g, 2, 0, 0));
    assert(game_move(g, 2, 0, 1));
    assert(game_move(g, 2, 1, 2));
    assert(game_free_fields(g, 1) == 1);
    assert(game_free_fields(g, 2) == 2);
    assert(!game_move(g, 1, 1, 1));
    assert(game_move(g, 2, 2, 1));
    assert(game_free_fields(g, 1) == 0);
    assert(game_free_fields(g, 2) == 1);
    assert(game_busy_fields(g, 2) == 4);
    assert(game_move(g, 2, 1, 1));
    assert(game_free_fields(g, 2) == 0);

    game_delete(g);


    return 0;
}
