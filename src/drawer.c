#include "terminal-anim-internal.h"
#include "terminal-anim.h"

#include "platform.h"
#include <stdbool.h>

void tau_clear(tau_ctx *ctx) {
  for (size_t i = 0; i < ctx->nb_cells; i++) {
    ctx->back_buffer[i].symbol = ' ';
    ctx->back_buffer[i].style.fg.r = 255;
    ctx->back_buffer[i].style.fg.g = 255;
    ctx->back_buffer[i].style.fg.b = 255;
    ctx->back_buffer[i].style.has_fg = false;
    ctx->back_buffer[i].style.bg.r = 0;
    ctx->back_buffer[i].style.bg.g = 0;
    ctx->back_buffer[i].style.bg.b = 0;
    ctx->back_buffer[i].style.has_bg = false;
    ctx->back_buffer[i].style.attrs = 0;
  }
}

void tau_put_square(tau_ctx *ctx, int x, int y, unsigned int width,
                    unsigned int height, tau_style style) {

  unsigned int screen_width, screen_height;
  pf_get_size(&screen_height, &screen_width);

  if (x >= screen_width || y >= screen_height)
    return;
  int start_x = (x < 0) ? 0 : x;
  int start_y = (y < 0) ? 0 : y;

  int end_x = start_x + width;
  int end_y = start_y + height;

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
      ctx->back_buffer[coords].style = style;
    }
  }
}

void tau_put_str(tau_ctx *ctx, char *str, size_t size, int x, int y,
                 tau_style style) {
  if (!ctx || !str)
    return;

  int screen_width = (int)ctx->nb_cols;
  int screen_height = (int)ctx->nb_rows;

  if (y < 0 || y >= screen_height)
    return;

  if (x >= screen_width)
    return;

  int start_x = (x < 0) ? 0 : x;
  if (x < 0)
    start_x = 0;

  for (size_t i = 0; i < size; i++) {
    if (start_x + (int)i >= screen_width)
      break;
    if (str[i] == '\0')
      break;
    size_t pos = (ctx->nb_cols * (size_t)y) + (size_t)start_x + i;
    ctx->back_buffer[pos].style = style;
    ctx->back_buffer[pos].symbol = str[i];
  }
}
