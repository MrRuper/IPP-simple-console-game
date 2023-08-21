// Ten plik włączamy na początku i dwa razy, aby sprawdzić, czy zawiera
// wszystko, co jest potrzebne.
#include "game.h"
#include "game.h"

// Używamy asercji.
#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// To jest makro obliczające liczbę elementów tablicy x.
#define SIZE(x) (sizeof x / sizeof x[0])

// Gdzieś musi być zdefiniowany magiczny napis służący do spawdzania, czy
// program w całości wykonał się poprawnie.
extern char quite_long_magic_string[];

/** FUNKCJE POMOCNE PRZY DEBUGOWANIU TESTÓW **/

#if 0

#include <stdio.h>
#include <inttypes.h>

static inline void print_board(game_t *g) {
  char *board = game_board(g);
  assert(board);
  printf(board);
  free(board);
}

static inline void print_players(game_t *g,
                                 uint32_t from_player, uint32_t to_player) {
  assert(from_player >= 1);
  for (uint32_t player = from_player - 1; player++ < to_player;)
    printf("player = %" PRIu32 ", busy = %" PRIu64 ", free = %" PRIu64 "\n",
           player,
           game_busy_fields(g, player),
           game_free_fields(g, player));
}

#endif

/** KONFIGUARACJA TESTÓW **/

// Zostało sformułowane wymaganie, aby ustawiać errno, więc to sprawdzamy.
static const bool test_errno = true;

// Możliwe wyniki testu
#define PASS 0
#define FAIL 1
#define WRONG_TEST 2

// Różne rozmiary planszy
#define SMALL_BOARD_SIZE    13
#define MIDDLE_BOARD_SIZE  128
#define BIG_BOARD_SIZE    1000

// Maksymalne liczby jednoczesnych gier i graczy
#define MANY_GAMES 42
#define MANY_PLAYERS 35

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t players;
  uint32_t areas;
} game_param_t;

/** WŁAŚCIWE TESTY **/

// Testuje opublikowany przykład użycia.
static int example(void) {
  static const char board[] =
  "1.........\n"
  "..........\n"
  "..........\n"
  "......2...\n"
  ".....1....\n"
  "..........\n"
  "..........\n"
  "1.........\n"
  "1222......\n"
  "1.........\n";

  game_t *g;

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
  assert(game_move(g, 2, 6, 6));
  assert(game_busy_fields(g, 1) == 5);
  assert(game_free_fields(g, 1) == 9);
  assert(game_busy_fields(g, 2) == 4);
  assert(game_free_fields(g, 2) == 91);

  char *p = game_board(g);
  assert(p);
  assert(strcmp(p, board) == 0);
  free(p);

  game_delete(g);
  return PASS;
}

// Testuje najmniejszą możliwą grę.
static int minimal(void) {
  game_t *g = game_new(1, 1, 1, 1);
  assert(g != NULL);
  assert(game_move(g, 1, 0, 0));
  assert(game_busy_fields(g, 1) == 1);
  assert(game_free_fields(g, 1) == 0);
  game_delete(g);
  return PASS;
}

// Testuje sprawdzanie poprawności parametrów poszczególnych funkcji.
static int params(void) {
  assert(game_new(0, 10, 2, 2) == NULL);
  assert(game_new(10, 0, 2, 2) == NULL);
  assert(game_new(10, 10, 0, 2) == NULL);
  assert(game_new(10, 10, 2, 0) == NULL);

  game_t *g = game_new(10, 10, 2, 2);

  assert(!game_move(NULL, 1, 5, 5));
  assert(!game_move(g, 0, 5, 5));
  assert(!game_move(g, 3, 5, 5));
  assert(!game_move(g, UINT32_MAX, 5, 5));
  assert(!game_move(g, 1, 10, 5));
  assert(!game_move(g, 1, UINT32_MAX, 5));
  assert(!game_move(g, 1, 5, 10));
  assert(!game_move(g, 1, 5, UINT32_MAX));

  assert(game_busy_fields(NULL, 1) == 0);
  assert(game_busy_fields(g, 0) == 0);
  assert(game_busy_fields(g, 3) == 0);
  assert(game_busy_fields(g, UINT32_MAX) == 0);

  assert(game_free_fields(NULL, 1) == 0);
  assert(game_free_fields(g, 0) == 0);
  assert(game_free_fields(g, 3) == 0);
  assert(game_free_fields(g, UINT32_MAX) == 0);

  assert(game_board(NULL) == NULL);

  game_delete(g);
  return PASS;
}

// Testuje, czy nie ma problemów przy wypisywaniu planszy w grze dużą liczbą
// graczy.
static int many_players(void) {
  game_t *g = game_new(MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, MANY_PLAYERS,
                       MIDDLE_BOARD_SIZE * MIDDLE_BOARD_SIZE / MANY_PLAYERS + 1);
  assert(g != NULL);

  for (uint32_t x = 0; x < MIDDLE_BOARD_SIZE; ++x)
    for (uint32_t y = 0; y < MIDDLE_BOARD_SIZE; ++y)
      assert(game_move(g, (x * MIDDLE_BOARD_SIZE + y) % MANY_PLAYERS + 1, x, y));

  char *board = game_board(g);
  assert(board != NULL);

  for (uint32_t x = 0; x < MIDDLE_BOARD_SIZE; ++x) {
    for (uint32_t y = 0; y < MIDDLE_BOARD_SIZE; ++y) {
      uint32_t player = (x * MIDDLE_BOARD_SIZE + y) % MANY_PLAYERS + 1;
      size_t position = (MIDDLE_BOARD_SIZE - 1 - (size_t)y) * (MIDDLE_BOARD_SIZE + 1) + (size_t)x;
      assert(board[position] == game_player(g, player));
    }
  }

  free(board);
  game_delete(g);
  return PASS;
}

// Testuje, czy można rozgrywać równocześnie więcej niż jedną grę.
static int many_games(void) {
  static const game_param_t game[] = {
    {7, 9, 2, 4},
    {11, 21, 2, 4},
    {47, 3, 2, 4},
    {2, 99, 2, 2},
  };
  static const uint64_t free1[SIZE(game)] = {8, 8, 6, 4};

  static game_t *g[MANY_GAMES][SIZE(game)];

  for (size_t i = 0; i < MANY_GAMES; ++i) {
    for (size_t j = 0; j < SIZE(game); ++j) {
      g[i][j] = game_new(game[j].width, game[j].height,
                         game[j].players, game[j].areas);
      assert(g[i][j]);
    }
  }

  for (size_t i = 0; i < MANY_GAMES; ++i) {
    for (size_t j = 0; j < SIZE(game); ++j) {
      assert(game_move(g[i][j], 1, 0, 0));
      assert(game_move(g[i][j], 1, 0, game[j].height - 1));
      assert(game_move(g[i][j], 1, game[j].width - 1, 0));
      assert(game_move(g[i][j], 1, game[j].width - 1, game[j].height - 1));
      assert(!game_move(g[i][j], 1, game[j].width, game[j].height - 1));
      assert(!game_move(g[i][j], 1, game[j].width - 1, game[j].height));
    }
  }

  for (size_t i = 0; i < MANY_GAMES; ++i) {
    for (size_t j = 0; j < SIZE(game); ++j) {
      assert(game_busy_fields(g[i][j], 1) == 4);
      assert(game_free_fields(g[i][j], 1) == free1[j]);
      assert(game_busy_fields(g[i][j], 2) == 0);
      uint64_t size = (uint64_t)game[j].width * (uint64_t)game[j].height;
      assert(game_free_fields(g[i][j], 2) == size - 4);
    }
  }

  for (size_t i = 0; i < MANY_GAMES; ++i) {
    for (size_t j = 0; j < SIZE(game); ++j) {
      assert(game_board_width(g[i][j]) == game[j].width);
      assert(game_board_height(g[i][j]) == game[j].height);
      assert(game_players(g[i][j]) == game[j].players);
      game_delete(g[i][j]);
    }
  }

  return PASS;
}

// Testuje, czy game_delete wywołane z parametrem NULL nic nie robi.
static int delete_null(void) {
  game_delete(NULL);
  return PASS;
}

// Uruchamia kilka krótkich testów poprawności wykonywania ruchów oraz
// obliczania liczby zajętych i wolnych pól po wykonaniu zwykłego ruchu.
static int moves(void) {
  game_t *g = game_new(2, 2, 2, 2);
  assert(g != NULL);

  assert(game_move(g, 1, 0, 0));
  assert(game_move(g, 2, 1, 0));
  assert(game_move(g, 1, 1, 1));
  assert(game_move(g, 2, 0, 1));

  assert(game_busy_fields(g, 1) == 2);
  assert(game_free_fields(g, 1) == 0);
  assert(game_busy_fields(g, 2) == 2);
  assert(game_free_fields(g, 2) == 0);

  game_delete(g);
  g = game_new(5, 5, 2, 4);
  assert(g != NULL);

  assert(game_move(g, 1, 2, 1));
  assert(game_move(g, 1, 2, 3));
  assert(game_move(g, 1, 1, 2));
  assert(game_move(g, 1, 3, 2));
  assert(game_move(g, 2, 2, 2));

  assert(game_busy_fields(g, 1) == 4);
  assert(game_free_fields(g, 1) == 8);
  assert(game_busy_fields(g, 2) == 1);
  assert(game_free_fields(g, 2) == 20);

  assert(game_move(g, 2, 1, 1));
  assert(game_move(g, 2, 3, 3));
  assert(game_move(g, 2, 1, 3));

  assert(game_busy_fields(g, 1) == 4);
  assert(game_free_fields(g, 1) == 5);
  assert(game_busy_fields(g, 2) == 4);
  assert(game_free_fields(g, 2) == 6);

  game_delete(g);
  g = game_new(5, 5, 5, 1);
  assert(g != NULL);

  assert(game_move(g, 2, 1, 2));
  assert(game_move(g, 4, 2, 1));
  assert(game_move(g, 5, 3, 2));
  assert(game_move(g, 3, 2, 2));
  assert(game_move(g, 1, 2, 3));

  assert(game_busy_fields(g, 1) == 1);
  assert(game_free_fields(g, 1) == 3);
  assert(game_busy_fields(g, 2) == 1);
  assert(game_free_fields(g, 2) == 3);
  assert(game_busy_fields(g, 3) == 1);
  assert(game_free_fields(g, 3) == 0);
  assert(game_busy_fields(g, 4) == 1);
  assert(game_free_fields(g, 4) == 3);
  assert(game_busy_fields(g, 5) == 1);
  assert(game_free_fields(g, 5) == 3);

  game_delete(g);
  return PASS;
}

// Testuje liczenie obszarów jednego gracza.
static int areas(void) {
  game_t *g = game_new(31, 37, 1, 42);
  assert(g != NULL);

  for (uint32_t i = 0; i < 21; ++i) {
    assert(game_move(g, 1, i, i));
    assert(game_move(g, 1, i + 2, i));
  }

  assert(!game_move(g, 1, 0, 2));
  assert(!game_move(g, 1, 0, 4));

  for (uint32_t i = 0; i < 9; ++i)
    assert(game_move(g, 1, i + 1, i));

  for (uint32_t i = 2; i <= 36; i += 2)
    assert(game_move(g, 1, 0, i));

  assert(!game_move(g, 1, 4, 0));
  assert(!game_move(g, 1, 6, 0));

  game_delete(g);
  return PASS;
}

// Testuje rozgałęzione obszary.
static int tree(void) {
  game_t *g = game_new(16, 15, 3, 2);
  assert(g != NULL);

  assert(game_move(g, 1, 1, 1));
  assert(game_move(g, 1, 12, 3));
  assert(game_move(g, 1, 2, 1));
  assert(game_move(g, 1, 12, 4));
  assert(game_move(g, 1, 1, 2));
  assert(game_move(g, 1, 12, 5));
  assert(game_move(g, 1, 3, 1));
  assert(game_move(g, 1, 12, 2));
  assert(game_move(g, 1, 1, 3));
  assert(game_move(g, 1, 12, 1));
  assert(game_move(g, 1, 4, 1));
  assert(game_move(g, 1, 11, 3));
  assert(game_move(g, 1, 2, 3));
  assert(game_move(g, 1, 10, 3));
  assert(game_move(g, 1, 5, 1));
  assert(game_move(g, 1, 13, 3));
  assert(game_move(g, 1, 3, 3));
  assert(game_move(g, 1, 14, 3));
  assert(game_move(g, 1, 6, 1));
  assert(game_move(g, 1, 10, 2));
  assert(game_move(g, 1, 4, 3));
  assert(game_move(g, 1, 10, 1));
  assert(game_move(g, 1, 7, 1));
  assert(game_move(g, 1, 11, 1));
  assert(game_move(g, 1, 5, 3));
  assert(game_move(g, 1, 13, 1));
  assert(game_move(g, 1, 8, 1));
  assert(game_move(g, 1, 14, 1));
  assert(game_move(g, 1, 6, 3));
  assert(game_move(g, 1, 14, 2));
  assert(game_move(g, 1, 6, 4));
  assert(game_move(g, 1, 14, 4));
  assert(game_move(g, 1, 7, 4));
  assert(game_move(g, 1, 14, 5));
  assert(game_move(g, 1, 7, 5));
  assert(game_move(g, 1, 13, 5));
  assert(game_move(g, 1, 11, 4));
  assert(game_move(g, 1, 11, 5));
  assert(game_move(g, 1, 10, 5));
  assert(game_move(g, 1, 13, 2));
  assert(game_move(g, 1, 13, 4));
  assert(game_move(g, 1, 9, 1));
  assert(game_move(g, 1, 4, 7));
  assert(game_move(g, 1, 4, 8));
  assert(game_move(g, 1, 4, 9));
  assert(game_move(g, 1, 4, 10));
  assert(game_move(g, 1, 4, 11));
  assert(game_move(g, 1, 4, 12));
  assert(game_move(g, 1, 4, 13));
  assert(game_move(g, 1, 3, 10));
  assert(game_move(g, 1, 2, 10));
  assert(game_move(g, 1, 1, 10));
  assert(game_move(g, 1, 5, 10));
  assert(game_move(g, 1, 6, 10));
  assert(game_move(g, 1, 7, 10));
  assert(game_move(g, 1, 8, 10));
  assert(game_move(g, 1, 3, 8));
  assert(game_move(g, 1, 5, 8));
  assert(game_move(g, 1, 2, 9));
  assert(game_move(g, 1, 2, 11));
  assert(game_move(g, 1, 3, 12));
  assert(game_move(g, 1, 5, 12));
  assert(game_move(g, 1, 7, 9));
  assert(game_move(g, 1, 7, 8));
  assert(game_move(g, 1, 7, 7));
  assert(game_move(g, 1, 7, 11));
  assert(game_move(g, 1, 7, 12));
  assert(game_move(g, 1, 8, 12));
  assert(game_move(g, 1, 8, 13));
  assert(game_move(g, 1, 9, 10));
  assert(game_move(g, 1, 10, 10));
  assert(game_move(g, 1, 11, 10));
  assert(game_move(g, 1, 11, 9));
  assert(game_move(g, 1, 11, 8));
  assert(game_move(g, 1, 12, 8));
  assert(game_move(g, 1, 12, 7));
  assert(game_move(g, 1, 11, 11));
  assert(game_move(g, 1, 11, 12));
  assert(game_move(g, 1, 10, 12));
  assert(game_move(g, 1, 12, 10));
  assert(game_move(g, 1, 13, 10));
  assert(game_move(g, 1, 13, 11));
  assert(game_move(g, 1, 14, 10));
  assert(game_move(g, 1, 13, 9));

  game_delete(g);
  return PASS;
}

// Testuje ruchy wykonywane wyłącznie na brzegu planszy.
static int border(void) {
  static const char board[] =
    "321432\n"
    "4....1\n"
    "4....4\n"
    "1....3\n"
    "123412\n";

  game_t *g = game_new(6, 5, 4, 4);
  assert(g != NULL);

  assert(game_move(g, 1, 0, 0));
  assert(game_move(g, 2, 1, 0));
  assert(game_move(g, 3, 2, 0));
  assert(game_move(g, 4, 3, 0));
  assert(game_move(g, 1, 4, 0));
  assert(game_move(g, 2, 5, 0));
  assert(game_move(g, 3, 5, 1));
  assert(game_move(g, 4, 5, 2));
  assert(game_move(g, 1, 5, 3));
  assert(game_move(g, 2, 5, 4));
  assert(game_move(g, 3, 4, 4));
  assert(game_move(g, 4, 3, 4));
  assert(game_move(g, 1, 2, 4));
  assert(game_move(g, 2, 1, 4));
  assert(game_move(g, 3, 0, 4));
  assert(game_move(g, 4, 0, 3));

  assert(!game_move(g, 1, 0, 2));
  assert(!game_move(g, 2, 0, 1));
  assert(!game_move(g, 3, 0, 2));
  assert(!game_move(g, 4, 0, 1));
  assert(game_move(g, 4, 0, 2));
  assert(game_move(g, 1, 0, 1));

  char *p = game_board(g);
  assert(p);
  assert(strcmp(p, board) == 0);
  free(p);

  assert(game_busy_fields(g, 1) == 5);
  assert(game_free_fields(g, 1) == 4);
  assert(game_busy_fields(g, 2) == 4);
  assert(game_free_fields(g, 2) == 2);
  assert(game_busy_fields(g, 3) == 4);
  assert(game_free_fields(g, 3) == 3);
  assert(game_busy_fields(g, 4) == 5);
  assert(game_free_fields(g, 4) == 5);

  game_delete(g);
  return PASS;
}

// Testuje odporność implementacji na duże wartości parametrów w game_new.
static int big_board(void) {
  static const game_param_t game[] = {
    {       UINT32_MAX,        UINT32_MAX, MANY_PLAYERS,        UINT32_MAX},
    {                2,        UINT32_MAX, MANY_PLAYERS,        UINT32_MAX},
    {       UINT32_MAX,                 2, MANY_PLAYERS,        UINT32_MAX},
    {       UINT32_MAX,        UINT32_MAX,            2,        UINT32_MAX},
    {       UINT32_MAX,        UINT32_MAX, MANY_PLAYERS,                 5},
    {       UINT32_MAX,                 2,            2,                 5},
    {                2,        UINT32_MAX,            2,                 5},
    {                2,                 2, MANY_PLAYERS,                 5},
    {                2,                 2,            2,        UINT32_MAX},
    {          1 << 16,           1 << 16,            2,                 5},
    {    (1 << 24) + 1,           1 <<  8,            2,                 5},
    {    (1 << 15) + 1,     (1 << 14) + 1,            2,                 5},
    {    (1 << 14) + 1,     (1 << 13) + 1,            2,                 5},
    {    (1 << 13) + 1,     (1 << 12) + 1,            2,                 5},
    {    (1 << 12) + 1,     (1 << 11) + 1,            2,                 5},
    {    (1 << 11) + 1,     (1 << 10) + 1,            2,                 5},
    {   BIG_BOARD_SIZE,    BIG_BOARD_SIZE, MANY_PLAYERS,    BIG_BOARD_SIZE},
    {MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, MANY_PLAYERS, MIDDLE_BOARD_SIZE},
    { SMALL_BOARD_SIZE,  SMALL_BOARD_SIZE, MANY_PLAYERS,  SMALL_BOARD_SIZE},
  };

  unsigned success = 0;
  for (size_t i = 0; i < SIZE(game); ++i) {
    errno = 0;
    game_t *g = game_new(game[i].width, game[i].height,
                         game[i].players, game[i].areas);
    if (g != NULL) {
      uint64_t size = (uint64_t)game[i].width * (uint64_t)game[i].height;
      assert(game_free_fields(g, game[i].players) == size);
      // Przynajmniej cztery ruchy powinny się udać.
      assert(game_move(g, game[i].players, 0, 0));
      assert(game_move(g, game[i].players, 0, game[i].height - 1));
      assert(game_move(g, game[i].players, game[i].width - 1, 0));
      assert(game_move(g, game[i].players, game[i].width - 1, game[i].height - 1));
      assert(game_free_fields(g, game[i].players) == size - 4);
      assert(game_busy_fields(g, game[i].players) == 4);
      game_delete(g);
      ++success;
    }
    else {
      assert(!test_errno || errno == ENOMEM);
    }
  }
  // Przynajmniej jedną z tych gier powinno się udać utworzyć.
  assert(success > 0);

  return PASS;
}

// Testuje ogranicznenia na rozmiar planszy w game_new.
static int middle_board(void) {
  unsigned success = 0;
  for (uint32_t size = 8000; size >= 125; size /= 2) {
    game_t *g1 = game_new(size, size, 2, 2);
    if (g1 != NULL) {
      game_delete(g1);
      // Jeśli udało się alokować planszę o rozmiarze size * size, to powinno
      // się też udać alokować inną planszę o nieco mniejszej powierzchni.
      game_t *g2;
      g2 = game_new((size - 1) * (size - 1), 1, 2, 2);
      assert(g2 != NULL);
      game_delete(g2);
      g2 = game_new(1, (size - 1) * (size - 1), 2, 2);
      assert(g2 != NULL);
      game_delete(g2);
      ++success;
    }
    else {
      assert(!test_errno || errno == ENOMEM);
    }
  }
  // Przynajmniej jedną z tych gier powinno się udać utworzyć.
  assert(success > 0);

  return PASS;
}

/** TESTY ALOKACJI PAMIĘCI
    Te testy muszą być linkowane z opcjami
    -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
    -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup
**/

// Przechwytujemy funkcje alokujące i zwalniające pamięć.
void * __real_malloc(size_t size) __attribute__((weak));
void * __real_calloc(size_t nmemb, size_t size)__attribute__((weak));
void * __real_realloc(void *ptr, size_t size)__attribute__((weak));
void * __real_reallocarray(void *ptr, size_t nmemb, size_t size)__attribute__((weak));
char * __real_strdup(const char *s)__attribute__((weak));
char * __real_strndup(const char *s, size_t size)__attribute__((weak));
void __real_free(void *ptr)__attribute__((weak));

// Trzymamy globalnie informacje o alokacjach i zwolnieniach pamięci i nie
// pozwalamy ich optymalizować.
static volatile unsigned call_total = 0;     // licznik wszystkich wywołań
static volatile unsigned call_counter = 0;   // licznik wywołań alokacji
static volatile unsigned fail_counter = 0;   // numer błędnej alokacji
static volatile unsigned alloc_counter = 0;  // liczba wykonanych alokacji
static volatile unsigned free_counter = 0;   // liczba wykonanych zwolnień
static volatile char *function_name = NULL;  // nazwa nieudanej funkcji
static volatile bool permanent_fail = false; // permanentny brak pamięci

// W zadanym momencie alokacja pamięci zawodzi.
static bool should_fail(void) {
  ++call_counter;
  return permanent_fail ? call_counter >= fail_counter :
                          call_counter == fail_counter;
}

// Realokacja musi się udać, jeśli nie zwiększamy rozmiaru alokowanej pamięci.
static bool can_fail(void const *old_ptr, size_t new_size) {
  if (old_ptr == NULL)
    return true;
  else
    return new_size > malloc_usable_size((void *)old_ptr);
}

// Symulujemy brak pamięci.
#define UNRELIABLE_ALLOC(ptr, size, fun, name)                           \
  do {                                                                   \
    call_total++;                                                        \
    if (ptr != NULL && size == 0) {                                      \
      /* Takie wywołanie realloc jest równoważne wywołaniu free(ptr). */ \
      free_counter++;                                                    \
      return fun;                                                        \
    }                                                                    \
    void *p = can_fail(ptr, size) && should_fail() ? NULL : (fun);       \
    if (p) {                                                             \
      alloc_counter += ptr != p;                                         \
      free_counter += ptr != p && ptr != NULL;                           \
    }                                                                    \
    else {                                                               \
      errno = ENOMEM;                                                    \
      function_name = name;                                              \
    }                                                                    \
    return p;                                                            \
  } while (0)

void *__wrap_malloc(size_t size) {
  UNRELIABLE_ALLOC(NULL, size, __real_malloc(size), "malloc");
}

void *__wrap_calloc(size_t nmemb, size_t size) {
  UNRELIABLE_ALLOC(NULL, nmemb * size, __real_calloc(nmemb, size), "calloc");
}

void *__wrap_realloc(void *ptr, size_t size) {
  UNRELIABLE_ALLOC(ptr, size, __real_realloc(ptr, size), "realloc");
}

void *__wrap_reallocarray(void *ptr, size_t nmemb, size_t size) {
  UNRELIABLE_ALLOC(ptr, nmemb * size, __real_reallocarray(ptr, nmemb, size), "reallocarray");
}

char *__wrap_strdup(const char *s) {
  UNRELIABLE_ALLOC(NULL, 0, __real_strdup(s), "strdup");
}

char *__wrap_strndup(const char *s, size_t size) {
  UNRELIABLE_ALLOC(NULL, 0, __real_strndup(s, size), "strndup");
}

// Zwalnianie pamięci zawsze się udaje. Odnotowujemy jedynie fakt zwolnienia.
void __wrap_free(void *ptr) {
  __real_free(ptr);
  if (ptr)
    ++free_counter;
}

#define V(code, where) (((unsigned long)code) << (3 * where))

// Testuje reakcję implementacji na niepowodzenie alokacji pamięci przy
// tworzeniu gry i wypisywaniu zawartości planszy.
static unsigned long alloc_fail_test_1(void) {
  unsigned long visited = 0;
  game_t *g;

  errno = 0;
  if ((g = game_new(4, 4, 2, 2)) != NULL) {
    visited |= V(1, 0);
  }
  else if (errno == ENOMEM && (g = game_new(4, 4, 2, 2)) != NULL) {
    visited |= V(2, 0);
  }
  else {
    visited |= V(4, 0);
    return visited;
  }

  // Nie jest tu istotne, czy wykonanie ruchu się udało.
  game_move(g, 1, 0, 0);
  game_move(g, 1, 0, 0);
  game_move(g, 2, 0, 0);
  game_move(g, 2, 1, 1);

  errno = 0;
  char *b = NULL;
  if ((b = game_board(g)) != NULL) {
    free(b);
    visited |= V(1, 1);
  }
  else if (errno == ENOMEM && (b = game_board(g)) != NULL) {
    free(b);
    visited |= V(2, 1);
  }
  else {
    visited |= V(4, 1);
    return visited;
  }

  game_delete(g);
  return visited;
}

// Testuje reakcję implementacji na permanentne niepowodzenie alokacji pamięci
// w trakcie gry.
static unsigned long alloc_fail_test_2(void) {
  #define PLAYERS 3
  #define AREAS 2
  #define TEST_COUNT 100

  srand(1);

  game_t *g = game_new(SMALL_BOARD_SIZE, SMALL_BOARD_SIZE, PLAYERS, AREAS);

  for (int i = 0; i < TEST_COUNT; ++i) {
    // Nie jest tu istotne, czy wykonanie ruchu się udało.
    game_move(g, rand() % PLAYERS + 1, rand() % SMALL_BOARD_SIZE, rand() % SMALL_BOARD_SIZE);
    game_busy_fields(g, rand() % PLAYERS + 1);
    game_free_fields(g, rand() % PLAYERS + 1);
  }

  game_delete(g);
  return 0;

  #undef PLAYESRS
  #undef AREAS
  #undef TEST_COUNT
}

// Sprawdza reakcję implementacji na niepowodzenie alokacji pamięci.
static int memory_test(unsigned long (* test_function)(void), bool permanent) {
  unsigned fail = 0, pass = 0;
  permanent_fail = permanent;
  for (fail_counter = 1; fail < 3 && pass < 3; ++fail_counter) {
    call_counter = 0;
    alloc_counter = 0;
    free_counter = 0;
    function_name = NULL;
    unsigned long visited_point = test_function();

    if (alloc_counter != free_counter || (visited_point & 0444444444444444444444UL) != 0) {
      fprintf(stderr,
              "fail_counter %u, alloc_counter %u, free_counter %u, "
              "function_name %s, visited_point %lo\n",
              fail_counter, alloc_counter, free_counter,
              function_name, visited_point);
      ++fail;
    }
    if (function_name == NULL)
      ++pass;
    else
      pass = 0;
  }

  return call_total > 0 && fail == 0 ? PASS : FAIL;
}

static int memory_one_fail(void) {
  return memory_test(alloc_fail_test_1, false);
}

static int memory_permanent_fail(void) {
  return memory_test(alloc_fail_test_2, true);
}

/** URUCHAMIANIE TESTÓW **/

typedef struct {
  char const *name;
  int (*function)(void);
} test_list_t;

#define TEST(t) {#t, t}

static const test_list_t test_list[] = {
  TEST(example),
  TEST(minimal),
  TEST(params),
  TEST(many_players),
  TEST(many_games),
  TEST(delete_null),
  TEST(moves),
  TEST(areas),
  TEST(tree),
  TEST(border),
  TEST(big_board),
  TEST(middle_board),
  TEST(memory_one_fail),
  TEST(memory_permanent_fail),
};

static int do_test(int (*function)(void)) {
  int result = function();
  puts(quite_long_magic_string);
  return result;
}

int main(int argc, char *argv[]) {
  if (argc == 2)
    for (size_t i = 0; i < SIZE(test_list); ++i)
      if (strcmp(argv[1], test_list[i].name) == 0)
        return do_test(test_list[i].function);

  fprintf(stderr, "Użycie:\n%s nazwa_testu\n", argv[0]);
  return WRONG_TEST;
}
