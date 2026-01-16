#include "escape-sequences.h"
#include "terminal-anim.h"

#include "platform.h"

void draw_square(int x, int y, int width, int height) {

  int screen_width, screen_height;
  pf_get_size(&screen_width, &screen_height);

  int square_x = 0;
  if (x >= 0 && x < screen_width)
    square_x = x;
  int square_y = 0;
  if (y >= 0 && y < screen_height)
    square_y = y;

  int square_w = 0;
  if (width > 0 && width <= (screen_width - x))
    square_w = width;
  if (width > (screen_width - x))
    square_w = width - x;

  int square_h = 0;
  if (height > 0 && height <= (screen_height - y))
    square_h = height;
  if (height > (screen_height - y))
    square_h = height - y;

  int counter_x = 0;
  int counter_y = 0;
  for (counter_x = square_x; counter_x <= square_x + square_w; counter_x++) {
    for (counter_y = square_y; counter_y <= square_y + square_h; counter_y++) {
      term_move(counter_y, counter_x);
      term_write("#");
    }
  }
}
