#ifndef FILE_EXPLORER_COLOR_H
#define FILE_EXPLORER_COLOR_H

enum color_pair
{
    // General
    PAIR_HIGHLIGHT_TEXT = 1,
    PAIR_HIGHLIGHT_SELECTION,

    // File colors
    PAIR_GENERAL_FILE_NAME,
    PAIR_DIRECTORY_NAME
};

enum color_custom
{
    COLOR_BRIGHT_WHITE = 15
};

int initialize_colors();

#endif //FILE_EXPLORER_COLOR_H
