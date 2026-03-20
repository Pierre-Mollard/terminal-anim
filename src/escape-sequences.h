#ifndef TERMINAL_ANIM_LIB_H
#define TERMINAL_ANIM_LIB_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_WIDTH 100000

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

// Not in the ECMA spec, Xterm control sequences
#define HIDE_CURSOR CSI "?25l"
#define SHOW_CURSOR CSI "?25h"
#define ALTERNATIVE_BUFFER_ON CSI "?1049h"
#define ALTERNATIVE_BUFFER_OFF CSI "?1049l"

static inline void write_in_term(const char *seq) {
  write(STDOUT_FILENO, seq, strlen(seq));
}

static inline void write_in_buffer_char(char **buffer_cursor, const char seq) {
  **buffer_cursor = seq;
  (*buffer_cursor)++;
  **buffer_cursor = '\0';
}

static inline void write_in_buffer(char **buffer_cursor, const char *seq) {
  size_t len = strlen(seq);
  memcpy(*buffer_cursor, seq, len);
  *buffer_cursor += len;
  **buffer_cursor = '\0';
}

static inline void write_in_buffer_f(char **buffer_cursor, const char *format,
                                     ...) {

  va_list args;
  va_start(args, format);

  int len = vsprintf(*buffer_cursor, format, args);

  va_end(args);

  *buffer_cursor += len;
}

static inline void write_in_buffer_move(char **buffer_cursor, int row,
                                        int col) {
  write_in_buffer_f(buffer_cursor, CURSOR_TO_F, row, col);
}

static inline void write_in_buffer_color(char **buffer_cursor, int r, int g,
                                         int b) {
  write_in_buffer_f(buffer_cursor, COLOR_F, r, g, b);
}

#endif
