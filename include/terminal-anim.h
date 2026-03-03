#ifndef TERMINAL_ANIM_PUB_LIB_H
#define TERMINAL_ANIM_PUB_LIB_H

#include <signal.h>

void draw_square(char **buffer_cursor, int x, int y, unsigned int width,
                 unsigned int height);

int setup_terminal();

void restore_terminal();

extern volatile sig_atomic_t g_is_running;

#endif
