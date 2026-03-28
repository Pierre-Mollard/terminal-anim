#ifndef TERMINAL_ANIM_LIB_H
#define TERMINAL_ANIM_LIB_H

#include "terminal-anim.h"
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

#define SYNC_BEGIN CSI "2026h"
#define SYNC_END CSI "2026l"

// Colors
// Note: _RGB means formatted
#define SGR_FG_RGB CSI "38;2;%d;%d;%dm"
#define SGR_BG_RGB CSI "48;2;%d;%d;%dm"
#define SGR_FG_BG_RGB CSI "38;2;%d;%d;%d;48;2;%d;%d;%dm"

#define SGR_RESET_COLORS CSI "39;49m"
#define SGR_RESET_FG CSI "39m"
#define SGR_RESET_BG CSI "49m"

// Styles
#define SGR_RESET CSI "0m"

#define SGR_BOLD CSI "1m"
#define SGR_FAINT CSI "2m"
#define SGR_ITALIC CSI "3m"
#define SGR_UNDERLINE CSI "4m"
#define SGR_BLINK_SLOW CSI "5m"
#define SGR_BLINK_RAPID CSI "6m"
#define SGR_REVERSE CSI "7m"
#define SGR_CONCEAL CSI "8m"
#define SGR_STRIKE CSI "9m"

#define SGR_BOLD_OFF CSI "22m"
#define SGR_FAINT_OFF CSI "22m"
#define SGR_ITALIC_OFF CSI "23m"
#define SGR_UNDERLINE_OFF CSI "24m"
#define SGR_BLINK_OFF CSI "25m"
#define SGR_REVERSE_OFF CSI "27m"
#define SGR_CONCEAL_OFF CSI "28m"
#define SGR_STRIKE_OFF CSI "29m"

// Less supported styles
#define SGR_DOUBLE_UNDERLINE CSI "21m"
#define SGR_FRAME CSI "51m"
#define SGR_ENCIRCLE CSI "52m"
#define SGR_OVERLINE CSI "53m"

#define SGR_DOUBLE_UNDERLINE_OFF CSI "24m"
#define SGR_FRAME_OFF CSI "54m"
#define SGR_ENCIRCLE_OFF CSI "54m"
#define SGR_OVERLINE_OFF CSI "55m"

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

static inline void write_in_buffer_fg_color(char **buffer_cursor,
                                            tau_rgb color) {
  write_in_buffer_f(buffer_cursor, SGR_FG_RGB, color.r, color.g, color.b);
}

static inline void write_in_buffer_bg_color(char **buffer_cursor,
                                            tau_rgb color) {
  write_in_buffer_f(buffer_cursor, SGR_BG_RGB, color.r, color.g, color.b);
}

static inline void write_in_buffer_colors(char **buffer_cursor,
                                          tau_rgb fg_color, tau_rgb bg_color) {
  write_in_buffer_f(buffer_cursor, SGR_FG_BG_RGB, fg_color.r, fg_color.g,
                    fg_color.b, bg_color.r, bg_color.g, bg_color.b);
}

#endif
