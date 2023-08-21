/** @file
 * Implementation of the interface game.h
 *
 * @author Bogdan Petraszczuk <bp372955@students.mimuw.edu.pl>
 *                            <bogdan.petraszczuk@gmail.com>
 * @copyright Uniwersytet Warszawski
 * @date 2023
 */

#include "game.h"

/** @brief An auxiliary structure which keeps the player number and
 * the "color" of the field in the game_board (i.e. the number
 * of calls of the game_move function).
 */
typedef struct Pair {
    uint64_t color;
    uint32_t player_number;
} pair_t;

/** @brief This structure represents the player information:
 * busy_fields     - the non negative number of occupied fields by
 *                   the player,
 * busy_areas      - the non negative number of occupied aries by
 *                   the player (the precise definition of area
 *                   could be found in game.h file),
 * boundary_length - the non negative number which represents
 *                   the length of the "boundary" of that player.
 *                   In other words this is the number of free
 *                   to take fields by the player if
 *                   Player.busy_areas = game.max_areas
 *                   (i.e. the player took already all possibles
 *                   areas but still can play putting figures on the
 *                   "boundary" of already existing connected fragment marked
 *                   by his number),
 * player_symbol   - symbol describing the player on the game board.
 */
typedef struct Player {
    uint64_t busy_fields;
    uint64_t boundary_length;
    uint32_t busy_areas;
    char player_symbol;
} player_t;

// Describes the maximum number of the potential
// neighbours for some field.
#define MAX_NEIGHBOURS 4

// Describes the first 9 players.
#define FIRST_NINE_PLAYERS 9

// Describes the first 35 players.
#define FIRST_THIRTY_FIVE_PLAYERS 35

// Describes the maximum possible number of players.
#define MAX_PLAYERS 61

/** @brief This structure represents the whole game.
 * width                 - non negative number describing the width
 *                         of the game board,
 * height                - non negative number describing the height
 *                         of the game board,
 * number_of_players     - non negative number representing the number of players,
 * max_areas             - non negative number representing the maximum
 *                         of free to take areas by each of the player,
 * game_board            - the matrix representing game status,
 * all_players           - the array of all players,
 * diff_pair_neighbour   - helper array holding for some coordinate (x,y) all
 *                         his different direct neighbours (neighbour_number, field_color),
 * diff_neighbour_number - helper array similar to diff_pair_neighbour but holding only
 *                         different neighhours player_numbers for some fixed (x,y) coordinate,
 * busy_neighbour_fields - number of direct neighbours for some (x,y) field,
 * fields_to_take        - non negative number of free fields in the game_board.
 */
struct game {
    pair_t diff_pair_neighbour[MAX_NEIGHBOURS];
    uint32_t diff_neighbour_number[MAX_NEIGHBOURS];
    uint64_t length_diff_neighbour_number; ///< Length of diff_neighbour_number.
    uint64_t busy_neighbour_fields;
    uint64_t fields_to_take;
    uint64_t potential_neighbour_number; ///< Number of all "valid" neighbour coordinates.
    uint32_t width;
    uint32_t height;
    uint32_t number_of_players;
    uint32_t max_areas;
    pair_t** game_board;
    player_t* all_players;
};

// This constant is for coloring the connected fragments of fields of
// the same figure number. In each iteration of game_move the
// global counter will be increasing by 1.
uint64_t GLOBAL_COUNTER = 1;


static uint64_t min(uint64_t const x, uint64_t const y) {
    return x <= y ? x : y;
}

// An auxilary function for correct delete
// malloced memory in game_new function.
static void remove_struct(game_t* g, player_t* all_players, pair_t** first_row,
                          pair_t* all_board) {
    free(all_players);
    free(all_board);
    free(first_row);
    free(g);
}

game_t* game_new(uint32_t width, uint32_t height, uint32_t players, uint32_t areas) {

    // Firstly check if the input is correct.
    if (width == 0 || height == 0 || players == 0 || areas == 0 || players > MAX_PLAYERS) {
        return NULL;
    }

    game_t* g = NULL;
    player_t* all_players = NULL;
    pair_t** game_board = NULL;
    pair_t** first_row = NULL;
    pair_t* all_board = NULL;

    g = calloc(1, sizeof(game_t));
    all_players = calloc(players, sizeof(player_t));
    first_row = (pair_t**)malloc(width * sizeof(pair_t*));
    all_board = (pair_t*)calloc((uint64_t)width * (uint64_t)height, sizeof(pair_t));

    if (!g || !all_players || !first_row || !all_board) {
        remove_struct(g, all_players, first_row, all_board);

        return NULL;
    }

    for (uint32_t i = 0; i < width; i++) {
        first_row[i] = &all_board[(uint64_t)i * (uint64_t)height];
    }

    game_board = first_row;

    // First 9 players will have 1,...,9 as a player symbol.
    // Next players are denoted alphabetically (using large
    // and small letters).
    for (uint32_t i = 0; i < players; i++) {
        if (i < FIRST_NINE_PLAYERS) {
            all_players[i].player_symbol = (char)('1' + i);
        }
        else if (i >= FIRST_NINE_PLAYERS && i < FIRST_THIRTY_FIVE_PLAYERS) {
            all_players[i].player_symbol = (char)('a' + (i - FIRST_NINE_PLAYERS));
        }
        else {
            all_players[i].player_symbol = (char)('A' + (i - FIRST_THIRTY_FIVE_PLAYERS));
        }
    }

    // The game creating.
    g->width = width;
    g->height = height;
    g->number_of_players = players;
    g->max_areas = areas;
    g->game_board = game_board;
    g->all_players = all_players;
    g->fields_to_take = (uint64_t)width * (uint64_t)height;

    // Always set GLOBAL_COUNTER to 1 when new_game is created.
    GLOBAL_COUNTER = 1;

    return g;
}

void game_delete(game_t* g) {
    if (g) {
        pair_t** first_row = g->game_board;
        pair_t* all_board = g->game_board[0];

        remove_struct(g, g->all_players, first_row, all_board);
    }
}

// Returns true if the player_number is correct and false otherwise.
static bool correct_player_number(game_t const* g, uint32_t const player_number) {
    return (!(player_number == 0 || player_number > g->number_of_players));
}

// Returns true if the player occupied all possible aries and false otherwise.
static bool player_occupied_all_areas(game_t const* g, uint32_t const player_number) {
    return (g->all_players[player_number - 1].busy_areas == g->max_areas);
}

// Returns true if the coordinate is valid and false otherwise.
static bool correct_coordinate(game_t const* g, uint32_t const x, uint32_t const y) {
    return (!(x >= g->width || y >= g->height));
}

// Returns true if the coordinate (x,y) is already occupied and false otherwise.
static bool empty_coordinate(game_t const* g, uint32_t const x, uint32_t const y) {
    return (g->game_board[x][y].player_number == 0);
}

// Helper function in update_structure procedure which is adding the new pair to array.
static void add_to_array(int* position, pair_t* neighbours, pair_t value_to_add,
                         uint64_t* length) {
    for (int i = 0; i < *position; i++) {
        if (neighbours[i].player_number == value_to_add.player_number &&
            neighbours[i].color == value_to_add.color) {
            return;
        }
    }

    neighbours[*position].player_number = value_to_add.player_number;
    neighbours[*position].color = value_to_add.color;
    (*position)++;
    (*length)++;
}

// Working with neighbours of (x,y) coordinate.
// Update all g members which depend on (x,y) coordinate in the
// definition.
static void update_structure(game_t* g, uint32_t x, uint32_t y) {
    uint64_t length_diff_pair_neighbour = 0;
    uint64_t length_diff_neighbour_number = 0;
    uint64_t busy_neighbour_fields = 0;
    uint64_t potential_neighbour_number = 0;
    bool valid_right = correct_coordinate(g, x + 1, y);
    bool valid_left = correct_coordinate(g, x - 1, y);
    bool valid_down = correct_coordinate(g, x, y + 1);
    bool valid_up = correct_coordinate(g, x, y - 1);
    int position = 0;

    // Update the array diff_pair_neighbour and busy_neighbour_fields,
    // Update the length_diff_pair_neighbour.
    if (valid_right) {
        potential_neighbour_number++;

        if (!empty_coordinate(g, x + 1, y)) {
            busy_neighbour_fields++;
            add_to_array(&position, g->diff_pair_neighbour,
                         g->game_board[x + 1][y], &length_diff_pair_neighbour);
        }
    }
    if (valid_left) {
        potential_neighbour_number++;

        if (!empty_coordinate(g, x - 1, y)) {
            busy_neighbour_fields++;
            add_to_array(&position, g->diff_pair_neighbour,
                         g->game_board[x - 1][y], &length_diff_pair_neighbour);
        }
    }
    if (valid_up) {
        potential_neighbour_number++;

        if (!empty_coordinate(g, x, y - 1)) {
            busy_neighbour_fields++;
            add_to_array(&position, g->diff_pair_neighbour,
                         g->game_board[x][y - 1], &length_diff_pair_neighbour);
        }
    }
    if (valid_down) {
        potential_neighbour_number++;

        if (!empty_coordinate(g, x, y + 1)) {
            busy_neighbour_fields++;
            add_to_array(&position, g->diff_pair_neighbour,
                         g->game_board[x][y + 1], &length_diff_pair_neighbour);
        }
    }

    // Update the g.diff_neighbour_numbers.
    bool copy;

    for (uint64_t i = 0; i < length_diff_pair_neighbour; i++) {
        copy = false;

        for (uint64_t z = 0; z < length_diff_neighbour_number; z++) {
            if (g->diff_pair_neighbour[i].player_number == g->diff_neighbour_number[z]) {
                copy = true;
                break;
            }
        }

        if (!copy) {
            g->diff_neighbour_number[length_diff_neighbour_number] =
                    g->diff_pair_neighbour[i].player_number;
            length_diff_neighbour_number++;
        }
    }

    g->length_diff_neighbour_number = length_diff_neighbour_number;
    g->busy_neighbour_fields = busy_neighbour_fields;
    g->potential_neighbour_number = potential_neighbour_number;
}

/** @brief Checks if adding new figure generates a new area for the player.
 * @param[in] neighbours      - is the array of diff_pair_neighbour of the player,
 * @param[in] player_number   - the player number.
 * @return true if adding creates the new area and false otherwise.
 */
static bool boundary_adding(pair_t const* neighbours, uint32_t const player_number) {
    int i = 0;

    while (neighbours[i].player_number != 0 && i < 4) {
        if (neighbours[i].player_number == player_number) {
            return true;
        }
        i++;
    }

    return false;
}

/** @brief An auxilary function which analyses free neighbour cell
 *  of the coordinate c := (x,y) and add +1 to the answer if that
 *  field has its own neighbour (different that c) with the
 *  same figure number as in c.
 * @param[in] g               - pointer to the game structure,
 * @param[in] x               - column number,
 * @param[in] y               - row number,
 * @param[in] player_number   - the number of the figure we put at (x,y) coordinate.
 * @return The number of empty diff_pair_neighbour of the (x,y) coordinate which has
 * in their own diff_pair_neighbour the player_number.
 */
static uint64_t check_non_direct_neighbours(game_t const* g, uint32_t x,
                                            uint32_t y, uint32_t player_number) {
    uint64_t answer = 0;
    bool valid_left = correct_coordinate(g, x - 1, y);
    bool valid_right = correct_coordinate(g, x + 1, y);
    bool valid_up = correct_coordinate(g, x, y - 1);
    bool valid_down = correct_coordinate(g, x, y + 1);

    // Define 3 bool variables for checking the validity of
    // non direct variables ("neighbours" of my neighbour).
    bool technical1;
    bool technical2;
    bool technical3;

    if (valid_left && empty_coordinate(g, x - 1, y)) {
        technical1 = correct_coordinate(g, x - 2, y);
        technical2 = correct_coordinate(g, x - 1, y + 1);
        technical3 = correct_coordinate(g, x - 1, y - 1);

        if ((technical1 && g->game_board[x - 2][y].player_number == player_number) ||
            (technical2 && g->game_board[x - 1][y + 1].player_number == player_number) ||
            (technical3 && g->game_board[x - 1][y - 1].player_number == player_number)) {
            answer++;
        }
    }
    if (valid_right && empty_coordinate(g, x + 1, y)) {
        technical1 = correct_coordinate(g, x + 2, y);
        technical2 = correct_coordinate(g, x + 1, y - 1);
        technical3 = correct_coordinate(g, x + 1, y + 1);

        if ((technical1 && g->game_board[x + 2][y].player_number == player_number) ||
            (technical2 && g->game_board[x + 1][y - 1].player_number == player_number) ||
            (technical3 && g->game_board[x + 1][y + 1].player_number == player_number)) {
            answer++;
        }
    }
    if (valid_up && empty_coordinate(g, x, y - 1)) {
        technical1 = correct_coordinate(g, x, y - 2);
        technical2 = correct_coordinate(g, x - 1, y - 1);
        technical3 = correct_coordinate(g, x + 1, y - 1);

        if ((technical1 && g->game_board[x][y - 2].player_number == player_number) ||
            (technical2 && g->game_board[x - 1][y - 1].player_number == player_number) ||
            (technical3 && g->game_board[x + 1][y - 1].player_number == player_number)) {
            answer++;
        }
    }
    if (valid_down && empty_coordinate(g, x, y + 1)) {
        technical1 = correct_coordinate(g, x, y + 2);
        technical2 = correct_coordinate(g, x - 1, y + 1);
        technical3 = correct_coordinate(g, x + 1, y + 1);

        if ((technical1 && g->game_board[x][y + 2].player_number == player_number) ||
            (technical2 && g->game_board[x - 1][y + 1].player_number == player_number) ||
            (technical3 && g->game_board[x + 1][y + 1].player_number == player_number)) {
            answer++;
        }
    }

    return answer;
}

// Reset all auxilary data in game structure to zero.
static void set_to_zero(game_t* g) {
    for (int i = 0; i < 4; i++) {
        g->diff_pair_neighbour[i].player_number = 0;
        g->diff_pair_neighbour[i].color = 0;
        g->diff_neighbour_number[i] = 0;
    }
}

// Looks at all direct busy neighbour fields having player_number figure
// on it and returns their minimum color.
static uint64_t find_min_color(game_t const* g, uint32_t player_number) {
    uint64_t answer = UINT64_MAX;

    for (int i = 0; i < 4; i++) {
        if (g->diff_pair_neighbour[i].player_number == player_number) {
            answer = min(answer, g->diff_pair_neighbour[i].color);
        }
    }

    return answer;
}

// Recursive function for recoloring different fragments of the same
// player. The parameter min_color describes the new color of that fragments.
static void BFS(game_t* g, uint32_t x, uint32_t y, uint64_t min_color,
                uint64_t player_number) {
    if (!correct_coordinate(g, x, y)) {
        return;
    }

    if (g->game_board[x][y].player_number == player_number &&
        g->game_board[x][y].color != min_color) {
        g->game_board[x][y].color = min_color;
        BFS(g, x + 1, y, min_color, player_number);
        BFS(g, x - 1, y, min_color, player_number);
        BFS(g, x, y - 1, min_color, player_number);
        BFS(g, x, y + 1, min_color, player_number);
    }
}

bool game_move(game_t* g, uint32_t player, uint32_t x, uint32_t y) {
    if (!g || !correct_player_number(g, player) || !correct_coordinate(g, x, y) ||
        g->game_board[x][y].player_number != 0) {
        return false;
    }

    /**
     * We split next part of that function on two cases:
     * (1) the move is "boundary" i.e. adding the figure
     * does not create new area,
     * (2) the move creates new area.
     */
    update_structure(g, x, y);

    if (!boundary_adding(g->diff_pair_neighbour, player)) {
        if (player_occupied_all_areas(g, player)) {
            return false;
        }

        // Update current player.
        g->all_players[player - 1].busy_areas++;
        g->all_players[player - 1].busy_fields++;
        g->all_players[player - 1].boundary_length += g->potential_neighbour_number -
                                                      g->busy_neighbour_fields -
                                                      check_non_direct_neighbours(g, x, y, player);

        // Update the game structure and the GLOBAL_COUNTER.
        g->game_board[x][y].player_number = player;
        g->game_board[x][y].color = GLOBAL_COUNTER;
        GLOBAL_COUNTER++;
        g->fields_to_take--;

        // Update all non empty diff_pair_neighbour.
        uint32_t helper_variable;

        for (uint64_t i = 0; i < g->length_diff_neighbour_number; i++) {
            helper_variable = g->diff_neighbour_number[i];
            g->all_players[helper_variable - 1].boundary_length--;
        }
    }
    else {
        uint64_t min_color = find_min_color(g, player);
        uint32_t fragments = 0;

        // Firstly find the number of neighbours with the same number.
        for (int i = 0; i < 4; i++) {
            if (g->diff_pair_neighbour[i].player_number == player) {
                fragments++;
            }
        }

        // Update me.
        g->all_players[player - 1].busy_areas -= fragments - 1;
        g->all_players[player - 1].busy_fields++;
        g->all_players[player - 1].boundary_length += g->potential_neighbour_number -
                                                      g->busy_neighbour_fields -
                                                      check_non_direct_neighbours(g, x, y, player);

        // Update the game structure and the GLOBAL_COUNTER.
        g->game_board[x][y].player_number = player;
        g->game_board[x][y].color = min_color;
        GLOBAL_COUNTER++;
        g->fields_to_take--;

        // Update all diff_pair_neighbour with different figures.
        int z = 0;
        uint32_t local_number; // Helper in next while loop.

        while (z < 4 && g->diff_neighbour_number[z] != 0) {
            local_number = g->diff_neighbour_number[z];
            g->all_players[local_number - 1].boundary_length--;

            z++;
        }

        // Update all diff_pair_neighbour with the same number by recoloring them.
        BFS(g, x + 1, y, min_color, player);
        BFS(g, x - 1, y, min_color, player);
        BFS(g, x, y - 1, min_color, player);
        BFS(g, x, y + 1, min_color, player);
    }

    set_to_zero(g);

    return true;
}

uint64_t game_busy_fields(game_t const* g, uint32_t player) {
    if (!g || !correct_player_number(g, player)) {
        return 0;
    }

    return g->all_players[player - 1].busy_fields;
}

uint64_t game_free_fields(game_t const* g, uint32_t player) {
    if (!g || !correct_player_number(g, player)) {
        return 0;
    }
    if (player_occupied_all_areas(g, player)) {
        return g->all_players[player - 1].boundary_length;
    }

    return g->fields_to_take;
}

uint64_t game_general_free_fields(game_t const* g) {
    return g->fields_to_take;
}

uint32_t game_board_width(game_t const *g) {
    if (!g) {
        return 0;
    }

    return g->width;
}

uint32_t game_board_height(game_t const *g) {
    if (!g) {
        return 0;
    }

    return g->height;
}

uint32_t game_areas(game_t const* g) {
    return g->max_areas;
}

uint32_t game_players(game_t const *g) {
    if (!g) {
        return 0;
    }

    return g->number_of_players;
}

char game_player(game_t const* g, uint32_t player) {
    if (!g || !correct_player_number(g, player)) {
        return '.';
    }

    return g->all_players[player - 1].player_symbol;
}

char* game_board(game_t const *g) {
    if (!g) {
        return NULL;
    }

    uint64_t size = ((uint64_t)g->width + 1) * (uint64_t )g->height + 1;
    char* board = (char*)malloc(size * sizeof(char));
    uint64_t local_index = 0;
    uint32_t player_number;

    if (!board) {
        return NULL;
    }

    for (uint32_t i = g->height; i-- > 0;) {
        for (uint32_t j = 0; j < g->width; j++) {
            player_number = g->game_board[j][i].player_number;

            if (player_number == 0) {
                board[local_index] = '.';
            }
            else {
                board[local_index] = g->all_players[player_number - 1].player_symbol;
            }

            local_index++;
        }

        board[local_index] = '\n';
        local_index++;
    }

    board[size - 1] = '\0';

    return board;
}
