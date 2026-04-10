
#include "box-drawing-chars.h"
#include "terminal-anim.h"
#include <stdint.h>

void tau_put_box(tau_ctx *ctx, int x, int y, unsigned int width,
                 unsigned int height, tau_box_style box_style,
                 tau_style style) {
  uint32_t horizontal_char = ' ';
  uint32_t vertical_char = ' ';

  switch (box_style) {
  case TAU_BOX_LIGHT:
    horizontal_char = TAU_BD_LIGHT_HORIZONTAL;
    break;
  case TAU_BOX_HEAVY:
    horizontal_char = TAU_BD_HEAVY_HORIZONTAL;
    break;
  case TAU_BOX_DOUBLE:
    horizontal_char = TAU_BD_DOUBLE_HORIZONTAL;
    break;
  case TAU_BOX_NONE:
    break;
  }

  switch (box_style) {
  case TAU_BOX_LIGHT:
    vertical_char = TAU_BD_LIGHT_VERTICAL;
    break;
  case TAU_BOX_HEAVY:
    vertical_char = TAU_BD_HEAVY_VERTICAL;
    break;
  case TAU_BOX_DOUBLE:
    vertical_char = TAU_BD_DOUBLE_VERTICAL;
    break;
  case TAU_BOX_NONE:
    break;
  }

  uint32_t top_left_char = ' ';
  switch (box_style) {
  case TAU_BOX_LIGHT:
    top_left_char = TAU_BD_LIGHT_DOWN_AND_RIGHT;
    break;
  case TAU_BOX_HEAVY:
    top_left_char = TAU_BD_HEAVY_DOWN_AND_RIGHT;
    break;
  case TAU_BOX_DOUBLE:
    top_left_char = TAU_BD_DOUBLE_DOWN_AND_RIGHT;
    break;
  case TAU_BOX_NONE:
    break;
  }

  uint32_t top_right_char = ' ';
  switch (box_style) {
  case TAU_BOX_LIGHT:
    top_right_char = TAU_BD_LIGHT_DOWN_AND_LEFT;
    break;
  case TAU_BOX_HEAVY:
    top_right_char = TAU_BD_HEAVY_DOWN_AND_LEFT;
    break;
  case TAU_BOX_DOUBLE:
    top_right_char = TAU_BD_DOUBLE_DOWN_AND_LEFT;
    break;
  case TAU_BOX_NONE:
    break;
  }

  uint32_t bottom_left_char = ' ';
  switch (box_style) {
  case TAU_BOX_LIGHT:
    bottom_left_char = TAU_BD_LIGHT_UP_AND_RIGHT;
    break;
  case TAU_BOX_HEAVY:
    bottom_left_char = TAU_BD_HEAVY_UP_AND_RIGHT;
    break;
  case TAU_BOX_DOUBLE:
    bottom_left_char = TAU_BD_DOUBLE_UP_AND_RIGHT;
    break;
  case TAU_BOX_NONE:
    break;
  }

  uint32_t bottom_right_char = ' ';
  switch (box_style) {
  case TAU_BOX_LIGHT:
    bottom_right_char = TAU_BD_LIGHT_UP_AND_LEFT;
    break;
  case TAU_BOX_HEAVY:
    bottom_right_char = TAU_BD_HEAVY_UP_AND_LEFT;
    break;
  case TAU_BOX_DOUBLE:
    bottom_right_char = TAU_BD_DOUBLE_UP_AND_LEFT;
    break;
  case TAU_BOX_NONE:
    break;
  }

  tau_put_hline(ctx, y, x + 1, x + width - 1, horizontal_char, style);
  tau_put_hline(ctx, y + height, x + 1, x + width - 1, horizontal_char, style);
  tau_put_vline(ctx, x, y + 1, y + height - 1, vertical_char, style);
  tau_put_vline(ctx, x + width, y + 1, y + height - 1, vertical_char, style);

  tau_put_char(ctx, top_left_char, x, y, style);
  tau_put_char(ctx, top_right_char, x + width, y, style);
  tau_put_char(ctx, bottom_left_char, x, y + height, style);
  tau_put_char(ctx, bottom_right_char, x + width, y + height, style);
}
