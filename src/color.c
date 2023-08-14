#include <ncurses.h>
#include "color.h"

int initialize_colors()
{
    init_color(COLOR_BRIGHT_WHITE, 1000, 1000, 1000);

    init_pair(PAIR_HIGHLIGHT_TEXT, COLOR_BRIGHT_WHITE, COLOR_CYAN);
    init_pair(PAIR_HIGHLIGHT_SELECTION, COLOR_BLACK, COLOR_BRIGHT_WHITE);

    init_pair(PAIR_GENERAL_FILE_NAME, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_DIRECTORY_NAME, COLOR_CYAN, COLOR_BLACK);

    return 0;
}
