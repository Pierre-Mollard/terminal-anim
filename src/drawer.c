#include "terminal-anim-internal.h"
#include "terminal-anim.h"

#include "platform.h"

void tau_clear(tau_ctx *ctx) {
  for (size_t i = 0; i < ctx->nb_cells; i++) {
    ctx->back_buffer[i].symbol = ' ';
    ctx->back_buffer[i].fg_r = 255;
    ctx->back_buffer[i].fg_g = 255;
    ctx->back_buffer[i].fg_b = 255;
    ctx->back_buffer[i].bg_r = 0;
    ctx->back_buffer[i].bg_g = 0;
    ctx->back_buffer[i].bg_b = 0;
    ctx->back_buffer[i].attrs = 0;
  }
}

void tau_put_square(tau_ctx *ctx, int x, int y, unsigned int width,
                    unsigned int height) {

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

  for (int i = start_x; i <= end_x; i++) {
    for (int j = start_y; j <= end_y; j++) {
      int coords = j * ctx->nb_cols + i;
      ctx->back_buffer[coords].symbol = 'H';
    }
  }
}
