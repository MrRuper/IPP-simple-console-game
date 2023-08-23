#include "game.h"
#include <ncurses.h>

// This constant describes the ^D command.
#define GAME_BREAK 4

// Constant describing the space command.
#define SPACE 32

// LINES is the ncurses constant describing the height of the user
// screen.
#define SCREEN_HEIGHT LINES

// COLS is an another ncurses constant describing the width of the user
// screen. The screen is devided into "fields". The left up corner has
// the coordinate (0, 0) and the bottom right has (LINES - 1, COLS - 1).
#define SCREEN_WIDTH COLS

// KEY_LEFT - ncurses constant describing the left array.
#define MOVE_LEFT KEY_LEFT

// KEY_SLEFT - ncurses constant describing the shift + left arrow.
#define MOVE_SHIFT_LEFT KEY_SLEFT

// KEY_RIGHT - ncurses constant describing the right arrow.
#define MOVE_RIGHT KEY_RIGHT

// KEY_SRIGHT - ncurses constant describing the shift + right arrow.
#define MOVE_SHIFT_RIGHT KEY_SRIGHT

// KEY_UP - ncurses constant describing the upper arrow.
#define MOVE_UP KEY_UP

// KEY_SR - ncurses constant decribing the shift + uper arrow.
#define MOVE_SHIFT_UP KEY_SR

// KEY_DOWN - ncurses constant describing the bottom arrow.
#define MOVE_DOWN KEY_DOWN

// KEY_SF - ncurses constant describing the shift + bottom arrow.
#define MOVE_SHIFT_DOWN  KEY_SF

// The row of the upper left corner of the board.
#define FIRST_ROW 0

// The column of the upper left corner of the board.
#define FIRST_COLUMN 0

static void start_TUI_mode() {

    // Turn on the TUI mode.
    initscr();

    // Line buffering disabled.
    cbreak();

    // Do not display user input on screen.
    noecho();

    // Allows getch() function to catch arrows from a keyboard.
    keypad(stdscr, true);
}

static void end_TUI_mode() {

    // Ends the TUI mode.
    endwin();

    // Reset terminal settings. Ncurses is changing the
    // terminal settings so it is needed to reset them.
    reset_shell_mode();
}

// Checks the validity of game_in_TUI_mode parameters and updates the game parameters.
static void check_game_parameters(const int argc, const char **argv, uint32_t* width, uint32_t* height,
                           uint32_t* players, uint32_t* areas) {

    // The converted value is kept in converted_value.
    // A usage of strtoul convert function forces us to
    // take uint64_t.
    uint64_t converted_value;

    // This string is needed in checking of the
    // validity of user input.
    char* end_string;

    // Check if the number of input arguments is correct.
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <width> <height> <players> <areas>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    converted_value = strtoul(argv[1], &end_string, 10);

    // Read the game_in_TUI_mode parameters and check their validity.
    if (*end_string != '\0' || converted_value > UINT32_MAX) {
        fprintf(stderr, "Invalid width value: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    *width = (uint32_t)converted_value;
    converted_value = strtoul(argv[2], &end_string, 10);

    if (*end_string != '\0' || converted_value > UINT32_MAX) {
        fprintf(stderr, "Invalid height value: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    *height = (uint32_t)converted_value;
    converted_value = strtoul(argv[3], &end_string, 10);

    if (*end_string != '\0' || converted_value > UINT32_MAX) {
        fprintf(stderr, "Invalid players value: %s\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    *players = (uint32_t)converted_value;
    converted_value = strtoul(argv[4], &end_string, 10);

    if (*end_string != '\0' || converted_value > UINT32_MAX) {
        fprintf(stderr, "Invalid areas value: %s\n", argv[4]);
        exit(EXIT_FAILURE);
    }

    *areas = (uint32_t)converted_value;
}

// Checks the size of the game board and if it is bigger
// than size of user monitor returns false. Returns true otherwise.
static bool check_screen(const uint32_t width, const uint32_t height) {
    if (width > (uint32_t)(SCREEN_WIDTH - 1) || height > (uint32_t)(SCREEN_HEIGHT - 1)) {
        end_TUI_mode();
        fprintf(stderr, "Invalid game board size.\n");

        return false;
    }
    else if (height > (uint32_t)(SCREEN_HEIGHT - 7)) {
        printw("The game board is to big for that screen and some information about the game"
               " status may not be displayed.\n"
               "To continue press any button.");
        refresh();

        // Waits on user bottom for continue the game_in_TUI_mode.
        getch();

        // Clear the previous message.
        clear();
    }

    return true;
}

// Prints in TUI the empty game board.
static void print_empty_board(const uint32_t width, const uint32_t height) {
    for (uint32_t i = FIRST_ROW; i < height; i++) {
        for (uint32_t j = FIRST_COLUMN; j < width; j++) {
            mvprintw(i, j, ".");
        }
    }
}

static void go_left(const uint32_t* current_row, uint32_t* current_column) {
    if (*current_column == FIRST_COLUMN) {
        return;
    }

    (*current_column)--;
    move(*current_row, *current_column);
    refresh();
}

static void go_right(const uint32_t width, const uint32_t* current_row,
              uint32_t* current_column) {
    if (*current_column == width - 1) {
        return;
    }

    (*current_column)++;
    move(*current_row, *current_column);
    refresh();
}

static void go_up(uint32_t* current_row, const uint32_t* current_column) {
    if (*current_row == FIRST_ROW) {
        return;
    }

    (*current_row)--;
    move(*current_row, *current_column);
    refresh();
}

static void go_down(uint32_t height, uint32_t* current_row,
             const uint32_t* current_column) {
    if (*current_row == height - 1) {
        return;
    }

    (*current_row)++;
    move(*current_row, *current_column);
    refresh();
}

/** @brief Write a board state for a current player.
 * @param g                         - pointer on a game_in_TUI_mode structure,
 * @param current_player_number     - nonnegative number of current player,
 */
static void board_state(game_t const* g, uint32_t current_player_number) {
    mvprintw(game_board_height(g), FIRST_COLUMN, "Current player: %lu. \n"
                                   "Number of free fields: %lu. \n"
                                   "Number of occupied fields bu current player: %lu. \n"
                                   "Number of free fields on the game board: %lu. \n"
                                   "To make a move choose a free field on the game board and press SPACE. \n"
                                   "To resign from making a move press C and press CTRL + D to end the game.",
                                        current_player_number,
                                        game_free_fields(g, current_player_number),
                                        game_busy_fields(g, current_player_number),
                                        game_general_free_fields(g));
}

/** Deal with the interactive game mode, prints the game board state, players
 * information, at the end of the procedure deletes all malloced data.
 * @param g  - pointer on the game structure.
 */
static void game_in_TUI_mode(game_t* g) {
    uint32_t width, height;
    int user_input;
    char* result_board;

    width = game_board_width(g);
    height = game_board_height(g);

    // Define two local variables to keep the curent cursor localization.
    uint32_t current_row = FIRST_ROW;
    uint32_t current_column = FIRST_COLUMN;

    uint32_t current_player_number = 1;
    bool move_completed = false; // Keeps the result of game_move function.

    // This variable keeps the game status i.e.
    // is true if there exists players which could
    // put figures on the board.
    bool lets_play = true;

    // Move the cursor on the left upper corner.
    board_state(g, current_player_number);
    move(current_row, current_column);
    refresh();

    while (((user_input = getch()) != GAME_BREAK) && (lets_play == true)) {
        switch (user_input) {
            case MOVE_SHIFT_LEFT:
            case MOVE_LEFT:
                go_left(&current_row, &current_column);
                break;

            case MOVE_SHIFT_RIGHT:
            case MOVE_RIGHT:
                go_right(width, &current_row, &current_column);
                break;

            case MOVE_SHIFT_UP:
            case MOVE_UP:
                go_up(&current_row, &current_column);
                break;

            case MOVE_SHIFT_DOWN:
            case MOVE_DOWN:
                go_down(height, &current_row, &current_column);
                break;

            case SPACE:
                move_completed = game_move(g, current_player_number, current_column,
                                           height - 1 - (uint32_t)current_row);

                if (move_completed) {
                    mvprintw(current_row, current_column, "%c", game_player(g, current_player_number));

                    if (!find_next_player(g, &current_player_number)) {
                        lets_play = false;
                    }

                    board_state(g, current_player_number);
                    move(current_row, current_column);
                    refresh();
                }

                break;

            case 'c':
            case 'C':
                find_next_player(g, &current_player_number);
                board_state(g, current_player_number);
                move(current_row, current_column);
                refresh();
                break;

            default:
                break;
        }
    }

    end_TUI_mode();

    // Print the game board and the player scores.
    result_board = game_board(g);
    printf("%s", result_board);
    print_players_score(g);

    // Free all malloc data.
    free(result_board);
    game_delete(g);
}

int main(const int argc, const char* argv[]) {
    uint32_t width, height, players, areas;
    game_t* g;

    check_game_parameters(argc, argv, &width, &height, &players, &areas);
    g = game_new(width, height, players, areas);

    if (!g) {
        fprintf(stderr, "Invalid game parameters.");

        return 1;
    }

    // Start TUI mode also to check the screen size.
    start_TUI_mode();

    if (!check_screen(width, height)) {
        return 1;
    }

    print_empty_board(width, height);
    game_in_TUI_mode(g);

    return 0;
}
