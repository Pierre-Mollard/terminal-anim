
#include "box-drawing-chars.h"
#include "terminal-anim.h"
#include <stdint.h>

static uint32_t resolve_border_style_from_grid(tau_box_style top,
                                               tau_box_style bottom,
                                               tau_box_style left,
                                               tau_box_style right) {
#define K(t, b, l, r) EDGE_KEY((t), (b), (l), (r))

  switch (EDGE_KEY(top, bottom, left, right)) {
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE):
    return ' ';

  /* single segments */
  case K(TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2575u; /* ╵ */
  case K(TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2579u; /* ╹ */
  case K(TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2577u; /* ╷ */
  case K(TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x257Bu; /* ╻ */
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_NONE):
    return 0x2574u; /* ╴ */
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_NONE):
    return 0x2578u; /* ╸ */
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_LIGHT):
    return 0x2576u; /* ╶ */
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_HEAVY):
    return 0x257Au; /* ╺ */

  /* straight lines */
  case K(TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2502u; /* │ */
  case K(TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2503u; /* ┃ */
  case K(TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2551u; /* ║ */

  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_LIGHT):
    return 0x2500u; /* ─ */
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_HEAVY):
    return 0x2501u; /* ━ */
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE):
    return 0x2550u; /* ═ */

  /* light corners */
  case K(TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_LIGHT):
    return 0x250Cu; /* ┌ */
  case K(TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_NONE):
    return 0x2510u; /* ┐ */
  case K(TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_LIGHT):
    return 0x2514u; /* └ */
  case K(TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_NONE):
    return 0x2518u; /* ┘ */

  /* heavy corners */
  case K(TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_HEAVY):
    return 0x250Fu; /* ┏ */
  case K(TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_NONE):
    return 0x2513u; /* ┓ */
  case K(TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_HEAVY):
    return 0x2517u; /* ┗ */
  case K(TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_NONE):
    return 0x251Bu; /* ┛ */

  /* double corners */
  case K(TAU_BOX_NONE, TAU_BOX_DOUBLE, TAU_BOX_NONE, TAU_BOX_DOUBLE):
    return 0x2554u; /* ╔ */
  case K(TAU_BOX_NONE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_NONE):
    return 0x2557u; /* ╗ */
  case K(TAU_BOX_DOUBLE, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_DOUBLE):
    return 0x255Au; /* ╚ */
  case K(TAU_BOX_DOUBLE, TAU_BOX_NONE, TAU_BOX_DOUBLE, TAU_BOX_NONE):
    return 0x255Du; /* ╝ */

  /* light tees */
  case K(TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_LIGHT):
    return 0x251Cu; /* ├ */
  case K(TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_NONE):
    return 0x2524u; /* ┤ */
  case K(TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_LIGHT):
    return 0x252Cu; /* ┬ */
  case K(TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_LIGHT):
    return 0x2534u; /* ┴ */

  /* heavy tees */
  case K(TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_HEAVY):
    return 0x2523u; /* ┣ */
  case K(TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_NONE):
    return 0x252Bu; /* ┫ */
  case K(TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_HEAVY):
    return 0x2533u; /* ┳ */
  case K(TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_HEAVY):
    return 0x253Bu; /* ┻ */

  /* double tees */
  case K(TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_NONE, TAU_BOX_DOUBLE):
    return 0x2560u; /* ╠ */
  case K(TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_NONE):
    return 0x2563u; /* ╣ */
  case K(TAU_BOX_NONE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE):
    return 0x2566u; /* ╦ */
  case K(TAU_BOX_DOUBLE, TAU_BOX_NONE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE):
    return 0x2569u; /* ╩ */

  /* crosses */
  case K(TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_LIGHT):
    return 0x253Cu; /* ┼ */
  case K(TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_HEAVY):
    return 0x254Bu; /* ╋ */
  case K(TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE):
    return 0x256Cu; /* ╬ */
  }

  /* fallback */
  if (top == TAU_BOX_DOUBLE || bottom == TAU_BOX_DOUBLE ||
      left == TAU_BOX_DOUBLE || right == TAU_BOX_DOUBLE) {
    top = top ? TAU_BOX_DOUBLE : TAU_BOX_NONE;
    bottom = bottom ? TAU_BOX_DOUBLE : TAU_BOX_NONE;
    left = left ? TAU_BOX_DOUBLE : TAU_BOX_NONE;
    right = right ? TAU_BOX_DOUBLE : TAU_BOX_NONE;
  } else if (top == TAU_BOX_HEAVY || bottom == TAU_BOX_HEAVY ||
             left == TAU_BOX_HEAVY || right == TAU_BOX_HEAVY) {
    top = top ? TAU_BOX_HEAVY : TAU_BOX_NONE;
    bottom = bottom ? TAU_BOX_HEAVY : TAU_BOX_NONE;
    left = left ? TAU_BOX_HEAVY : TAU_BOX_NONE;
    right = right ? TAU_BOX_HEAVY : TAU_BOX_NONE;
  } else {
    top = top ? TAU_BOX_LIGHT : TAU_BOX_NONE;
    bottom = bottom ? TAU_BOX_LIGHT : TAU_BOX_NONE;
    left = left ? TAU_BOX_LIGHT : TAU_BOX_NONE;
    right = right ? TAU_BOX_LIGHT : TAU_BOX_NONE;
  }

  switch (EDGE_KEY(top, bottom, left, right)) {
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_NONE):
    return ' ';
  case K(TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2502u;
  case K(TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2503u;
  case K(TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_NONE, TAU_BOX_NONE):
    return 0x2551u;
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_LIGHT, TAU_BOX_LIGHT):
    return 0x2500u;
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_HEAVY, TAU_BOX_HEAVY):
    return 0x2501u;
  case K(TAU_BOX_NONE, TAU_BOX_NONE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE):
    return 0x2550u;
  case K(TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_LIGHT, TAU_BOX_LIGHT):
    return 0x253Cu;
  case K(TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_HEAVY, TAU_BOX_HEAVY):
    return 0x254Bu;
  case K(TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE, TAU_BOX_DOUBLE):
    return 0x256Cu;
  }

  return ' ';
}

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
