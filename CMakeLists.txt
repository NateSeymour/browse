cmake_minimum_required(VERSION 3.26)
project(browse C)

set(CMAKE_C_STANDARD 23)

add_executable(browse src/main.c
        src/color.h
        src/color.c
        src/view/view.c
        src/view/view.h)
target_link_libraries(browse PRIVATE ncurses)