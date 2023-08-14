#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>
#include "color.h"

#define INITIAL_FILE_STORAGE_SPACE 1024

struct file
{
    char filename[FILENAME_MAX];
    struct stat stat;
};

static struct file cwd;
static struct file *cwd_children = NULL;
static uint64_t cwd_children_count = 0;
static size_t cwd_children_size = 0;

int init_screen()
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(FALSE);

    if(!has_colors() || !can_change_color())
    {
        return 1;
    }

    start_color();
    initialize_colors();
    refresh();

    return 0;
}

void destroy_screen()
{
    endwin();
}

int navigate(const char *path)
{
    // Navigate to new cwd
    chdir(path);

    // Get name and stat
    (void)getcwd(cwd.filename, FILENAME_MAX);
    stat(cwd.filename, &cwd.stat);

    // Clear children
    memset(cwd_children, 0, cwd_children_size);

    // Populate children
    cwd_children_count = 0;
    DIR *dir = opendir(cwd.filename);
    struct dirent *next_entry = NULL;
    while((next_entry = readdir(dir)) != NULL)
    {
        struct file *child = &cwd_children[cwd_children_count];

        memcpy(child->filename, next_entry->d_name, next_entry->d_namlen);
        stat(child->filename, &child->stat);

        cwd_children_count++;
    }
    closedir(dir);

    return 0;
}

int main(int argc, const char *argv[])
{
    // Init Screen
    if(init_screen() != 0)
    {
        fprintf(stderr, "Failed to initialize ncurses!");
        return 1;
    }

    // Reserve space for children
    cwd_children_size = sizeof(struct file) * INITIAL_FILE_STORAGE_SPACE;
    cwd_children = malloc(cwd_children_size);

    // Do initial navigation
    if(argc > 1)
    {
        navigate(argv[1]);
    }
    else
    {
        navigate(".");
    }

    // Create windows
    WINDOW* wbrowser = newwin(LINES - 1, COLS / 2, 0, 0);
    WINDOW* winfo = newwin(LINES - 1, COLS / 2, 0, COLS / 2);
    WINDOW* wstatus = newwin(1, COLS, LINES - 1, 0);

    // Cursor
    uint64_t cursor_index = 0;

    // Event loop
    while(TRUE)
    {
        // Clear and rerender browser
        wclear(wbrowser);

        for(uint64_t i = 0; i < cwd_children_count; i++)
        {
            struct file *next = &cwd_children[i];

            if(next->stat.st_mode & S_IFDIR)
            {
                wattron(wbrowser, COLOR_PAIR(PAIR_DIRECTORY_NAME));
            }
            else
            {
                wattron(wbrowser, COLOR_PAIR(PAIR_GENERAL_FILE_NAME));
            }

            wprintw(wbrowser, "%s\n", next->filename);
        }

        mvwchgat(wbrowser, cursor_index, 0, -1, A_BOLD, PAIR_HIGHLIGHT_SELECTION, NULL);

        wrefresh(wbrowser);

        // Clear and rerender info
        wclear(winfo);

        box(winfo, '|', '-');

        struct file *selected_child = &cwd_children[cursor_index];
        mvwaddstr(winfo, 0, 1, selected_child->filename);
        mvwprintw(winfo, 1, 1, "File Size: %i bytes", selected_child->stat.st_size);

        wrefresh(winfo);

        // Clear and rerender status
        wclear(wstatus);

        wprintw(wstatus, "%s", cwd.filename);
        mvwchgat(wstatus, 0, 0, -1, A_NORMAL, PAIR_HIGHLIGHT_TEXT, NULL);
        wrefresh(wstatus);

        // Get input
        int key = getch();

        if(key == KEY_UP)
        {
            cursor_index--;
        }

        if(key == KEY_DOWN)
        {
            cursor_index++;
        }

        if(key == KEY_LEFT)
        {
            navigate("..");
        }

        if(key == '\n')
        {
            navigate(cwd_children[cursor_index].filename);
        }

        if(key == 'q') break;
    }

    delwin(wbrowser);
    delwin(winfo);
    delwin(wstatus);

    destroy_screen();
    free(cwd_children);
    return 0;
}
