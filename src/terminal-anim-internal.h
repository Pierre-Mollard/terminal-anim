#ifndef TERMINAL_ANIM_LIB_H_INTERNAL
#define TERMINAL_ANIM_LIB_H_INTERNAL

#include <stddef.h>
#include <stdint.h>
#include <termios.h>

#define BYTES_PER_PIXEL 32

struct tau_cell {
  uint32_t symbol;
  uint8_t fg_r, fg_g, fg_b;
  uint8_t bg_r, bg_g, bg_b;
  uint8_t attrs;
};

struct tau_ctx {
  int id;
  struct tau_cell *front_buffer;
  struct tau_cell *back_buffer;
  size_t nb_cols;
  size_t nb_rows;
  size_t nb_cells;
  size_t output_capacity;
  char *output_buffer;
  int cursor_x;
  int cursor_y;
  struct termios termios_conf_init;
};

#endif
