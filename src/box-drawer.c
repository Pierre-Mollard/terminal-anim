
#include "box-drawing-chars.h"
#include "terminal-anim.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static uint32_t resolve_border_style_from_grid(tau_box_style top,
                                               tau_box_style bottom,
                                               tau_box_style left,
                                               tau_box_style right) {
#define K(t, b, l, r) EDGE_KEY((t), (b), (l), (r))
#define RET(t, b, l, r, ch)                                                    \
  case K((t), (b), (l), (r)):                                                  \
    return (ch)
#define NONE TAU_BOX_NONE
#define LIGHT TAU_BOX_LIGHT
#define HEAVY TAU_BOX_HEAVY
#define DOUBLE TAU_BOX_DOUBLE

  switch (EDGE_KEY(top, bottom, left, right)) {
    RET(NONE, NONE, NONE, NONE, ' ');

    /* single segments */
    RET(LIGHT, NONE, NONE, NONE, TAU_BD_LIGHT_UP);
    RET(HEAVY, NONE, NONE, NONE, TAU_BD_HEAVY_UP);
    RET(NONE, LIGHT, NONE, NONE, TAU_BD_LIGHT_DOWN);
    RET(NONE, HEAVY, NONE, NONE, TAU_BD_HEAVY_DOWN);
    RET(NONE, NONE, LIGHT, NONE, TAU_BD_LIGHT_LEFT);
    RET(NONE, NONE, HEAVY, NONE, TAU_BD_HEAVY_LEFT);
    RET(NONE, NONE, NONE, LIGHT, TAU_BD_LIGHT_RIGHT);
    RET(NONE, NONE, NONE, HEAVY, TAU_BD_HEAVY_RIGHT);

    /* straight lines */
    RET(LIGHT, LIGHT, NONE, NONE, TAU_BD_LIGHT_VERTICAL);
    RET(HEAVY, HEAVY, NONE, NONE, TAU_BD_HEAVY_VERTICAL);
    RET(DOUBLE, DOUBLE, NONE, NONE, TAU_BD_DOUBLE_VERTICAL);

    RET(NONE, NONE, LIGHT, LIGHT, TAU_BD_LIGHT_HORIZONTAL);
    RET(NONE, NONE, HEAVY, HEAVY, TAU_BD_HEAVY_HORIZONTAL);
    RET(NONE, NONE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_HORIZONTAL);

    /* mixed half-lines */
    RET(LIGHT, HEAVY, NONE, NONE, TAU_BD_LIGHT_UP_AND_HEAVY_DOWN);
    RET(HEAVY, LIGHT, NONE, NONE, TAU_BD_HEAVY_UP_AND_LIGHT_DOWN);
    RET(NONE, NONE, LIGHT, HEAVY, TAU_BD_LIGHT_LEFT_AND_HEAVY_RIGHT);
    RET(NONE, NONE, HEAVY, LIGHT, TAU_BD_HEAVY_LEFT_AND_LIGHT_RIGHT);

    /* light corners */
    RET(NONE, LIGHT, NONE, LIGHT, TAU_BD_LIGHT_DOWN_AND_RIGHT);
    RET(NONE, LIGHT, LIGHT, NONE, TAU_BD_LIGHT_DOWN_AND_LEFT);
    RET(LIGHT, NONE, NONE, LIGHT, TAU_BD_LIGHT_UP_AND_RIGHT);
    RET(LIGHT, NONE, LIGHT, NONE, TAU_BD_LIGHT_UP_AND_LEFT);

    /* heavy corners */
    RET(NONE, HEAVY, NONE, HEAVY, TAU_BD_HEAVY_DOWN_AND_RIGHT);
    RET(NONE, HEAVY, HEAVY, NONE, TAU_BD_HEAVY_DOWN_AND_LEFT);
    RET(HEAVY, NONE, NONE, HEAVY, TAU_BD_HEAVY_UP_AND_RIGHT);
    RET(HEAVY, NONE, HEAVY, NONE, TAU_BD_HEAVY_UP_AND_LEFT);

    /* mixed light/heavy corners */
    RET(NONE, LIGHT, NONE, HEAVY, TAU_BD_DOWN_LIGHT_AND_RIGHT_HEAVY);
    RET(NONE, HEAVY, NONE, LIGHT, TAU_BD_DOWN_HEAVY_AND_RIGHT_LIGHT);
    RET(NONE, LIGHT, HEAVY, NONE, TAU_BD_DOWN_LIGHT_AND_LEFT_HEAVY);
    RET(NONE, HEAVY, LIGHT, NONE, TAU_BD_DOWN_HEAVY_AND_LEFT_LIGHT);
    RET(LIGHT, NONE, NONE, HEAVY, TAU_BD_UP_LIGHT_AND_RIGHT_HEAVY);
    RET(HEAVY, NONE, NONE, LIGHT, TAU_BD_UP_HEAVY_AND_RIGHT_LIGHT);
    RET(LIGHT, NONE, HEAVY, NONE, TAU_BD_UP_LIGHT_AND_LEFT_HEAVY);
    RET(HEAVY, NONE, LIGHT, NONE, TAU_BD_UP_HEAVY_AND_LEFT_LIGHT);

    /* double corners */
    RET(NONE, DOUBLE, NONE, DOUBLE, TAU_BD_DOUBLE_DOWN_AND_RIGHT);
    RET(NONE, DOUBLE, DOUBLE, NONE, TAU_BD_DOUBLE_DOWN_AND_LEFT);
    RET(DOUBLE, NONE, NONE, DOUBLE, TAU_BD_DOUBLE_UP_AND_RIGHT);
    RET(DOUBLE, NONE, DOUBLE, NONE, TAU_BD_DOUBLE_UP_AND_LEFT);

    /* mixed single/double corners */
    RET(NONE, LIGHT, NONE, DOUBLE, TAU_BD_DOWN_SINGLE_AND_RIGHT_DOUBLE);
    RET(NONE, DOUBLE, NONE, LIGHT, TAU_BD_DOWN_DOUBLE_AND_RIGHT_SINGLE);
    RET(NONE, LIGHT, DOUBLE, NONE, TAU_BD_DOWN_SINGLE_AND_LEFT_DOUBLE);
    RET(NONE, DOUBLE, LIGHT, NONE, TAU_BD_DOWN_DOUBLE_AND_LEFT_SINGLE);
    RET(LIGHT, NONE, NONE, DOUBLE, TAU_BD_UP_SINGLE_AND_RIGHT_DOUBLE);
    RET(DOUBLE, NONE, NONE, LIGHT, TAU_BD_UP_DOUBLE_AND_RIGHT_SINGLE);
    RET(LIGHT, NONE, DOUBLE, NONE, TAU_BD_UP_SINGLE_AND_LEFT_DOUBLE);
    RET(DOUBLE, NONE, LIGHT, NONE, TAU_BD_UP_DOUBLE_AND_LEFT_SINGLE);

    /* light tees */
    RET(LIGHT, LIGHT, NONE, LIGHT, TAU_BD_LIGHT_VERTICAL_AND_RIGHT);
    RET(LIGHT, LIGHT, LIGHT, NONE, TAU_BD_LIGHT_VERTICAL_AND_LEFT);
    RET(NONE, LIGHT, LIGHT, LIGHT, TAU_BD_LIGHT_DOWN_AND_HORIZONTAL);
    RET(LIGHT, NONE, LIGHT, LIGHT, TAU_BD_LIGHT_UP_AND_HORIZONTAL);

    /* heavy tees */
    RET(HEAVY, HEAVY, NONE, HEAVY, TAU_BD_HEAVY_VERTICAL_AND_RIGHT);
    RET(HEAVY, HEAVY, HEAVY, NONE, TAU_BD_HEAVY_VERTICAL_AND_LEFT);
    RET(NONE, HEAVY, HEAVY, HEAVY, TAU_BD_HEAVY_DOWN_AND_HORIZONTAL);
    RET(HEAVY, NONE, HEAVY, HEAVY, TAU_BD_HEAVY_UP_AND_HORIZONTAL);

    /* mixed light/heavy tees: right */
    RET(HEAVY, LIGHT, NONE, LIGHT, TAU_BD_UP_HEAVY_AND_RIGHT_DOWN_LIGHT);
    RET(LIGHT, HEAVY, NONE, LIGHT, TAU_BD_DOWN_HEAVY_AND_RIGHT_UP_LIGHT);
    RET(HEAVY, HEAVY, NONE, LIGHT, TAU_BD_VERTICAL_HEAVY_AND_RIGHT_LIGHT);
    RET(LIGHT, LIGHT, NONE, HEAVY, TAU_BD_VERTICAL_LIGHT_AND_RIGHT_HEAVY);
    RET(LIGHT, HEAVY, NONE, HEAVY, TAU_BD_DOWN_LIGHT_AND_RIGHT_UP_HEAVY);
    RET(HEAVY, LIGHT, NONE, HEAVY, TAU_BD_UP_LIGHT_AND_RIGHT_DOWN_HEAVY);

    /* mixed light/heavy tees: left */
    RET(LIGHT, LIGHT, HEAVY, NONE, TAU_BD_VERTICAL_LIGHT_AND_LEFT_HEAVY);
    RET(HEAVY, LIGHT, LIGHT, NONE, TAU_BD_UP_HEAVY_AND_LEFT_DOWN_LIGHT);
    RET(LIGHT, HEAVY, LIGHT, NONE, TAU_BD_DOWN_HEAVY_AND_LEFT_UP_LIGHT);
    RET(HEAVY, HEAVY, LIGHT, NONE, TAU_BD_VERTICAL_HEAVY_AND_LEFT_LIGHT);
    RET(LIGHT, HEAVY, HEAVY, NONE, TAU_BD_DOWN_LIGHT_AND_LEFT_UP_HEAVY);
    RET(HEAVY, LIGHT, HEAVY, NONE, TAU_BD_UP_LIGHT_AND_LEFT_DOWN_HEAVY);

    /* mixed light/heavy tees: top */
    RET(NONE, LIGHT, HEAVY, LIGHT, TAU_BD_LEFT_HEAVY_AND_RIGHT_DOWN_LIGHT);
    RET(NONE, LIGHT, LIGHT, HEAVY, TAU_BD_RIGHT_HEAVY_AND_LEFT_DOWN_LIGHT);
    RET(NONE, LIGHT, HEAVY, HEAVY, TAU_BD_DOWN_LIGHT_AND_HORIZONTAL_HEAVY);
    RET(NONE, HEAVY, LIGHT, LIGHT, TAU_BD_DOWN_HEAVY_AND_HORIZONTAL_LIGHT);
    RET(NONE, HEAVY, LIGHT, HEAVY, TAU_BD_RIGHT_LIGHT_AND_LEFT_DOWN_HEAVY);
    RET(NONE, HEAVY, HEAVY, LIGHT, TAU_BD_LEFT_LIGHT_AND_RIGHT_DOWN_HEAVY);

    /* mixed light/heavy tees: bottom */
    RET(LIGHT, NONE, HEAVY, LIGHT, TAU_BD_LEFT_HEAVY_AND_RIGHT_UP_LIGHT);
    RET(LIGHT, NONE, LIGHT, HEAVY, TAU_BD_RIGHT_HEAVY_AND_LEFT_UP_LIGHT);
    RET(LIGHT, NONE, HEAVY, HEAVY, TAU_BD_UP_LIGHT_AND_HORIZONTAL_HEAVY);
    RET(HEAVY, NONE, LIGHT, LIGHT, TAU_BD_UP_HEAVY_AND_HORIZONTAL_LIGHT);
    RET(HEAVY, NONE, LIGHT, HEAVY, TAU_BD_RIGHT_LIGHT_AND_LEFT_UP_HEAVY);
    RET(HEAVY, NONE, HEAVY, LIGHT, TAU_BD_LEFT_LIGHT_AND_RIGHT_UP_HEAVY);

    /* double tees */
    RET(DOUBLE, DOUBLE, NONE, DOUBLE, TAU_BD_DOUBLE_VERTICAL_AND_RIGHT);
    RET(DOUBLE, DOUBLE, DOUBLE, NONE, TAU_BD_DOUBLE_VERTICAL_AND_LEFT);
    RET(NONE, DOUBLE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_DOWN_AND_HORIZONTAL);
    RET(DOUBLE, NONE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_UP_AND_HORIZONTAL);

    /* mixed single/double tees */
    RET(LIGHT, LIGHT, NONE, DOUBLE, TAU_BD_VERTICAL_SINGLE_AND_RIGHT_DOUBLE);
    RET(DOUBLE, DOUBLE, NONE, LIGHT, TAU_BD_VERTICAL_DOUBLE_AND_RIGHT_SINGLE);
    RET(LIGHT, LIGHT, DOUBLE, NONE, TAU_BD_VERTICAL_SINGLE_AND_LEFT_DOUBLE);
    RET(DOUBLE, DOUBLE, LIGHT, NONE, TAU_BD_VERTICAL_DOUBLE_AND_LEFT_SINGLE);
    RET(NONE, LIGHT, LIGHT, DOUBLE, TAU_BD_DOWN_SINGLE_AND_HORIZONTAL_DOUBLE);
    RET(NONE, DOUBLE, LIGHT, LIGHT, TAU_BD_DOWN_DOUBLE_AND_HORIZONTAL_SINGLE);
    RET(LIGHT, NONE, LIGHT, DOUBLE, TAU_BD_UP_SINGLE_AND_HORIZONTAL_DOUBLE);
    RET(DOUBLE, NONE, LIGHT, LIGHT, TAU_BD_UP_DOUBLE_AND_HORIZONTAL_SINGLE);
    RET(LIGHT, LIGHT, LIGHT, DOUBLE,
        TAU_BD_VERTICAL_SINGLE_AND_HORIZONTAL_DOUBLE);
    RET(DOUBLE, DOUBLE, LIGHT, LIGHT,
        TAU_BD_VERTICAL_DOUBLE_AND_HORIZONTAL_SINGLE);

    /* pure crosses */
    RET(LIGHT, LIGHT, LIGHT, LIGHT, TAU_BD_LIGHT_VERTICAL_AND_HORIZONTAL);
    RET(HEAVY, HEAVY, HEAVY, HEAVY, TAU_BD_HEAVY_VERTICAL_AND_HORIZONTAL);
    RET(DOUBLE, DOUBLE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_VERTICAL_AND_HORIZONTAL);

    /* mixed light/heavy crosses */
    RET(LIGHT, LIGHT, HEAVY, LIGHT, TAU_BD_LEFT_HEAVY_AND_RIGHT_VERTICAL_LIGHT);
    RET(LIGHT, LIGHT, LIGHT, HEAVY, TAU_BD_RIGHT_HEAVY_AND_LEFT_VERTICAL_LIGHT);
    RET(LIGHT, LIGHT, HEAVY, HEAVY, TAU_BD_VERTICAL_LIGHT_AND_HORIZONTAL_HEAVY);
    RET(HEAVY, LIGHT, LIGHT, LIGHT, TAU_BD_UP_HEAVY_AND_DOWN_HORIZONTAL_LIGHT);
    RET(LIGHT, HEAVY, LIGHT, LIGHT, TAU_BD_DOWN_HEAVY_AND_UP_HORIZONTAL_LIGHT);
    RET(HEAVY, HEAVY, LIGHT, LIGHT, TAU_BD_VERTICAL_HEAVY_AND_HORIZONTAL_LIGHT);
    RET(HEAVY, LIGHT, HEAVY, LIGHT, TAU_BD_LEFT_UP_HEAVY_AND_RIGHT_DOWN_LIGHT);
    RET(HEAVY, LIGHT, LIGHT, HEAVY, TAU_BD_RIGHT_UP_HEAVY_AND_LEFT_DOWN_LIGHT);
    RET(LIGHT, HEAVY, HEAVY, LIGHT, TAU_BD_LEFT_DOWN_HEAVY_AND_RIGHT_UP_LIGHT);
    RET(LIGHT, HEAVY, LIGHT, HEAVY, TAU_BD_RIGHT_DOWN_HEAVY_AND_LEFT_UP_LIGHT);
    RET(LIGHT, HEAVY, HEAVY, HEAVY, TAU_BD_DOWN_LIGHT_AND_UP_HORIZONTAL_HEAVY);
    RET(HEAVY, LIGHT, HEAVY, HEAVY, TAU_BD_UP_LIGHT_AND_DOWN_HORIZONTAL_HEAVY);
    RET(HEAVY, HEAVY, LIGHT, HEAVY, TAU_BD_RIGHT_LIGHT_AND_LEFT_VERTICAL_HEAVY);
    RET(HEAVY, HEAVY, HEAVY, LIGHT, TAU_BD_LEFT_LIGHT_AND_RIGHT_VERTICAL_HEAVY);
  }

  if (top == DOUBLE || bottom == DOUBLE || left == DOUBLE || right == DOUBLE) {
    top = top ? DOUBLE : NONE;
    bottom = bottom ? DOUBLE : NONE;
    left = left ? DOUBLE : NONE;
    right = right ? DOUBLE : NONE;
  } else if (top == HEAVY || bottom == HEAVY || left == HEAVY ||
             right == HEAVY) {
    top = top ? HEAVY : NONE;
    bottom = bottom ? HEAVY : NONE;
    left = left ? HEAVY : NONE;
    right = right ? HEAVY : NONE;
  } else {
    top = top ? LIGHT : NONE;
    bottom = bottom ? LIGHT : NONE;
    left = left ? LIGHT : NONE;
    right = right ? LIGHT : NONE;
  }

  switch (EDGE_KEY(top, bottom, left, right)) {
    RET(NONE, NONE, NONE, NONE, ' ');
    RET(LIGHT, NONE, NONE, NONE, TAU_BD_LIGHT_UP);
    RET(HEAVY, NONE, NONE, NONE, TAU_BD_HEAVY_UP);
    RET(NONE, LIGHT, NONE, NONE, TAU_BD_LIGHT_DOWN);
    RET(NONE, HEAVY, NONE, NONE, TAU_BD_HEAVY_DOWN);
    RET(NONE, NONE, LIGHT, NONE, TAU_BD_LIGHT_LEFT);
    RET(NONE, NONE, HEAVY, NONE, TAU_BD_HEAVY_LEFT);
    RET(NONE, NONE, NONE, LIGHT, TAU_BD_LIGHT_RIGHT);
    RET(NONE, NONE, NONE, HEAVY, TAU_BD_HEAVY_RIGHT);

    RET(LIGHT, LIGHT, NONE, NONE, TAU_BD_LIGHT_VERTICAL);
    RET(HEAVY, HEAVY, NONE, NONE, TAU_BD_HEAVY_VERTICAL);
    RET(DOUBLE, DOUBLE, NONE, NONE, TAU_BD_DOUBLE_VERTICAL);

    RET(NONE, NONE, LIGHT, LIGHT, TAU_BD_LIGHT_HORIZONTAL);
    RET(NONE, NONE, HEAVY, HEAVY, TAU_BD_HEAVY_HORIZONTAL);
    RET(NONE, NONE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_HORIZONTAL);

    RET(NONE, LIGHT, NONE, LIGHT, TAU_BD_LIGHT_DOWN_AND_RIGHT);
    RET(NONE, LIGHT, LIGHT, NONE, TAU_BD_LIGHT_DOWN_AND_LEFT);
    RET(LIGHT, NONE, NONE, LIGHT, TAU_BD_LIGHT_UP_AND_RIGHT);
    RET(LIGHT, NONE, LIGHT, NONE, TAU_BD_LIGHT_UP_AND_LEFT);

    RET(NONE, HEAVY, NONE, HEAVY, TAU_BD_HEAVY_DOWN_AND_RIGHT);
    RET(NONE, HEAVY, HEAVY, NONE, TAU_BD_HEAVY_DOWN_AND_LEFT);
    RET(HEAVY, NONE, NONE, HEAVY, TAU_BD_HEAVY_UP_AND_RIGHT);
    RET(HEAVY, NONE, HEAVY, NONE, TAU_BD_HEAVY_UP_AND_LEFT);

    RET(NONE, DOUBLE, NONE, DOUBLE, TAU_BD_DOUBLE_DOWN_AND_RIGHT);
    RET(NONE, DOUBLE, DOUBLE, NONE, TAU_BD_DOUBLE_DOWN_AND_LEFT);
    RET(DOUBLE, NONE, NONE, DOUBLE, TAU_BD_DOUBLE_UP_AND_RIGHT);
    RET(DOUBLE, NONE, DOUBLE, NONE, TAU_BD_DOUBLE_UP_AND_LEFT);

    RET(LIGHT, LIGHT, NONE, LIGHT, TAU_BD_LIGHT_VERTICAL_AND_RIGHT);
    RET(LIGHT, LIGHT, LIGHT, NONE, TAU_BD_LIGHT_VERTICAL_AND_LEFT);
    RET(NONE, LIGHT, LIGHT, LIGHT, TAU_BD_LIGHT_DOWN_AND_HORIZONTAL);
    RET(LIGHT, NONE, LIGHT, LIGHT, TAU_BD_LIGHT_UP_AND_HORIZONTAL);

    RET(HEAVY, HEAVY, NONE, HEAVY, TAU_BD_HEAVY_VERTICAL_AND_RIGHT);
    RET(HEAVY, HEAVY, HEAVY, NONE, TAU_BD_HEAVY_VERTICAL_AND_LEFT);
    RET(NONE, HEAVY, HEAVY, HEAVY, TAU_BD_HEAVY_DOWN_AND_HORIZONTAL);
    RET(HEAVY, NONE, HEAVY, HEAVY, TAU_BD_HEAVY_UP_AND_HORIZONTAL);

    RET(DOUBLE, DOUBLE, NONE, DOUBLE, TAU_BD_DOUBLE_VERTICAL_AND_RIGHT);
    RET(DOUBLE, DOUBLE, DOUBLE, NONE, TAU_BD_DOUBLE_VERTICAL_AND_LEFT);
    RET(NONE, DOUBLE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_DOWN_AND_HORIZONTAL);
    RET(DOUBLE, NONE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_UP_AND_HORIZONTAL);

    RET(LIGHT, LIGHT, LIGHT, LIGHT, TAU_BD_LIGHT_VERTICAL_AND_HORIZONTAL);
    RET(HEAVY, HEAVY, HEAVY, HEAVY, TAU_BD_HEAVY_VERTICAL_AND_HORIZONTAL);
    RET(DOUBLE, DOUBLE, DOUBLE, DOUBLE, TAU_BD_DOUBLE_VERTICAL_AND_HORIZONTAL);
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

static inline size_t grid_index(int x, int y, int w) {
  return (size_t)y * (size_t)w + (size_t)x;
}

void tau_put_box_grid(tau_ctx *ctx, tau_box *boxes, size_t amount) {

  // building fast representation of box borders
  int min_x = 0, min_y = 0;
  int max_x = 0, max_y = 0;
  for (int i = 0; i < amount; i++) {
    tau_box current_box = boxes[i];
    if (current_box.x <= min_x)
      min_x = current_box.x;
    if (current_box.y <= min_y)
      min_y = current_box.y;
    if (current_box.x + current_box.width > max_x)
      max_x = current_box.x + current_box.width;
    if (current_box.y + current_box.height > max_y)
      max_y = current_box.y + current_box.height;
  }
  int effective_width = max_x - min_x;
  int effective_height = max_y - min_y;
  size_t total_size = effective_width * effective_height;

  struct box_content {
    tau_box_style box_style;
    tau_style style;
  };
  struct box_content *grid = calloc(total_size, sizeof(*grid));

  for (int i = 0; i < total_size; i++) {
    grid[i].box_style = TAU_BOX_NONE;
  }

  for (int i = 0; i < amount; i++) {
    tau_box current_box = boxes[i];

    int x1 = current_box.x - min_x;
    int y1 = current_box.y - min_y;
    int x2 = current_box.x + current_box.width - 1 - min_x;
    int y2 = current_box.y + current_box.height - 1 - min_y;

    for (int x = x1; x <= x2; x++) {
      grid[grid_index(x, y1, effective_width)].box_style =
          current_box.box_style;
      grid[grid_index(x, y2, effective_width)].box_style =
          current_box.box_style;
      grid[grid_index(x, y1, effective_width)].style = current_box.style;
      grid[grid_index(x, y2, effective_width)].style = current_box.style;
    }
    for (int y = y1; y <= y2; y++) {
      grid[grid_index(x1, y, effective_width)].box_style =
          current_box.box_style;
      grid[grid_index(x2, y, effective_width)].box_style =
          current_box.box_style;
      grid[grid_index(x1, y, effective_width)].style = current_box.style;
      grid[grid_index(x2, y, effective_width)].style = current_box.style;
    }
  }

  for (int y = 0; y < effective_height; y++) {
    for (int x = 0; x < effective_width; x++) {
      struct box_content center = grid[grid_index(x, y, effective_width)];
      if (center.box_style == TAU_BOX_NONE)
        continue;

      tau_box_style left =
          (x > 0) ? grid[grid_index(x - 1, y, effective_width)].box_style
                  : TAU_BOX_NONE;
      tau_box_style right =
          (x + 1 < effective_width)
              ? grid[grid_index(x + 1, y, effective_width)].box_style
              : TAU_BOX_NONE;
      tau_box_style top =
          (y > 0) ? grid[grid_index(x, y - 1, effective_width)].box_style
                  : TAU_BOX_NONE;
      tau_box_style bottom =
          (y + 1 < effective_height)
              ? grid[grid_index(x, y + 1, effective_width)].box_style
              : TAU_BOX_NONE;

      uint32_t ch = resolve_border_style_from_grid(top, bottom, left, right);
      if (ch != ' ')
        tau_put_char(ctx, ch, x + min_x, y + min_y, center.style);
    }
  }

  free(grid);
}
