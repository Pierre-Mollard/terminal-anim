#ifndef TERMINAL_ANIM_LIB_H
#define TERMINAL_ANIM_LIB_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// The 'escape' of espace sequence, 04/08 octal since /e is not standard
#define ESC "\033"
#define CSI ESC "["

// Note: _F means formatted
#define CURSOR_CMD "H"
#define CURSOR_HOME CSI CURSOR_CMD
#define CURSOR_TO_F CSI "%d;%d" CURSOR_CMD
#define CLEAR_SCREEN CSI "2J"
#define CLEAR_ALL CLEAR_SCREEN CURSOR_HOME
#define COLOR_F CSI "38;2;%d;%d;%dm"

// Not in the ECMA spec
#define HIDE_CURSOR CSI "?25l"
#define SHOW_CURSOR CSI "?25h"

static inline void term_write(const char *seq) {
  write(STDOUT_FILENO, seq, strlen(seq));
}

static inline void term_write_f(const char *format, ...) {
  char formatted[32];

  va_list args;
  va_start(args, format);

  int len = vsnprintf(formatted, sizeof(formatted), format, args);

  va_end(args);

  if (len > 0) {
    if ((size_t)len >= sizeof(formatted)) {
      len = sizeof(formatted) - 1;
    }
    term_write(formatted);
  }
}

static inline void term_move(int row, int col) {
  term_write_f(CURSOR_TO_F, row, col);
}

static inline void term_color(int r, int g, int b) {
  term_write_f(COLOR_F, r, g, b);
}

#endif
