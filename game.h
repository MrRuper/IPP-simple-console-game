#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * To jest deklaracja struktury przechowującej stan gry.
 */
typedef struct game game_t;

/** @brief Tworzy strukturę przechowującą stan gry.
 * Alokuje pamięć na nową strukturę przechowującą stan gry.
 * Inicjuje tę strukturę, tak aby reprezentowała początkowy stan gry.
 * Gdy nie udało się alokować pamięci, ustawia @p errno na @p ENOMEM.
 * @param[in] width   – szerokość planszy, liczba dodatnia,
 * @param[in] height  – wysokość planszy, liczba dodatnia,
 * @param[in] players – liczba graczy, liczba dodatnia,
 * @param[in] areas   – maksymalna liczba obszarów, które może zająć jeden
 *                      gracz, liczba dodatnia.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się alokować
 * pamięci lub któryś z parametrów jest niepoprawny.
 */
game_t* game_new(uint32_t width, uint32_t height,
                 uint32_t players, uint32_t areas);

/** @brief Usuwa strukturę przechowującą stan gry.
 * Usuwa z pamięci strukturę wskazywaną przez @p g.
 * Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
 * @param[in] g       – wskaźnik na usuwaną strukturę.
 */
void game_delete(game_t* g);

/** @brief Wykonuje ruch.
 * Ustawia pionek gracza @p player na polu (@p x, @p y).
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref game_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref game_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref game_new.
 * @return Wartość @p true, jeśli ruch został wykonany, a @p false,
 * gdy ruch jest nielegalny, któryś z parametrów jest niepoprawny lub
 * wskaźnik @p g ma wartość NULL.
 */
bool game_move(game_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Podaje liczbę pól zajętych przez gracza.
 * Podaje liczbę pól zajętych przez gracza @p player.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref game_new.
 * @return Liczba pól zajętych przez gracza lub zero,
 * jeśli któryś z parametrów jest niepoprawny lub wskaźnik @p g ma wartość NULL.
 */
uint64_t game_busy_fields(game_t const *g, uint32_t player);

/** @brief Podaje liczbę pól, które jeszcze gracz może zająć.
 * Podaje liczbę wolnych pól, na których w danym stanie gry gracz @p player może
 * postawić swój pionek w następnym ruchu.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref game_new.
 * @return Liczba pól, jakie jeszcze może zająć gracz lub zero,
 * jeśli któryś z parametrów jest niepoprawny lub wskaźnik @p g ma wartość NULL.
 */
uint64_t game_free_fields(game_t const *g, uint32_t player);

/** @brief Podaje liczbę pól, które są wolne.
 * @param[in] g     - wskaźnik na strukturę przechowującą stan gry.
 * @return Liczbę wolnych pól na planszy.
 */
uint64_t game_general_free_fields(game_t const *g);

/** Podaje szerokość planszy.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry.
 * @return Szerokość planszy lub zero, gdy wskaźnik @p g ma wartość NULL.
 */
uint32_t game_board_width(game_t const *g);

/** Podaje wysokość planszy.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry.
 * @return Wysokość planszy lub zero, gdy wskaźnik @p g ma wartość NULL.
 */
uint32_t game_board_height(game_t const *g);

/** Podaje liczbę możliwych do zajęcia przez gracza obaszarów
 *  na planszy.
 * @param g     - wskaźnik na strukturę przechowującą stan gry.
 * @return  Maksymalna liczba możliwych do zajęcia obaszarów.
 */
uint32_t game_areas(game_t const* g);

/** Podaje liczbę graczy.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry.
 * @return Liczba graczy lub zero, gdy wskaźnik @p g ma wartość NULL.
 */
uint32_t game_players(game_t const *g);

/** Daje symbole wykorzystywane w funkcji @ref game_board.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref game_new.
 * @return Cyfra, litera lub inny jednoznakowy symbol gracza. Symbol oznaczający
 * puste pole, gdy numer gracza jest niepoprawny lub wskaźnik @p g ma wartość
 * NULL.
 */
char game_player(game_t const *g, uint32_t player);

/** @brief Daje napis opisujący stan planszy.
 * Alokuje w pamięci bufor, w którym umieszcza napis zawierający tekstowy
 * opis aktualnego stanu planszy. Przykład znajduje się w pliku game_example.c.
 * Gdy nie udało się alokować pamięci, ustawia @p errno na @p ENOMEM.
 * Funkcja wywołująca musi zwolnić ten bufor.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry.
 * @return Wskaźnik na alokowany bufor zawierający napis opisujący stan planszy
 * lub NULL, jeśli nie udało się alokować pamięci.
 */
char* game_board(game_t const *g);

/** @brief Znajduje kolejnego "wolnego" gracza dla wykonania ruchu i jego numer
 *  wpisuje do current_player_number.
 * @param g                       - wskaźnik na strukturę przechowująca stan gry.
 * @param current_player_number   - wskaźnik na numer aktualnego gracza.
 * @return true jeśli udało się znaleźć kolejnego gracza oraz false w przeciwnym
 * przypadku
 */
bool find_next_player(game_t* g, uint32_t* current_player_number);

/** @brief Wypisuje na standardowe wyjście ostateczną planszę gry wraz z
 * podsumowaniem, ile pól oraz obszarów zajął każdy z graczy.
 * @param g     - wskaźnik na strukturę przechowującą stan gry.
 */
void print_players_score(game_t* g);

#endif /* GAME_H */
