#include "escape-sequences.h"
#include "terminal-anim.h"

#include "platform.h"

void draw_square(int x, int y, unsigned int width, unsigned int height) {

  unsigned int screen_width, screen_height;
  pf_get_size(&screen_height, &screen_width);

  if (x >= screen_width || y >= screen_height)
    return;
  int start_x = (x < 0) ? 0 : x;
  int start_y = (y < 0) ? 0 : y;

  int end_x = x + width;
  int end_y = y + height;

  if (end_x > screen_width)
    end_x = screen_width;
  if (end_y > screen_height)
    end_y = screen_height;

  int draw_w = end_x - start_x;
  int draw_h = end_y - start_y;

  if (draw_w <= 0 || draw_h <= 0)
    return;

  char buffer_row[4096];
  if (draw_w > sizeof(buffer_row))
    draw_w = sizeof(buffer_row) - 1;

  memset(buffer_row, '#', draw_w);
  buffer_row[draw_w] = '\0';

  // ANSI is 1-based so +1
  for (int r = 0; r < draw_h; r++) {
    term_move(start_y + r + 1, start_x + 1);
    term_write(buffer_row);
  }
}
