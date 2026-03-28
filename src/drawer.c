#include "escape-sequences.h"
#include "platform.h"
#include "terminal-anim-internal.h"
#include "terminal-anim.h"

#include "platform.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

void tau_put_char(tau_ctx *ctx, uint32_t symbol, int x, int y,
                  tau_style style) {
  if (!ctx)
    return;
  if (x < 0 || y < 0)
    return;
  if (x >= (int)ctx->nb_cols || y >= (int)ctx->nb_rows)
    return;

  size_t pos = (size_t)y * ctx->nb_cols + (size_t)x;
  ctx->back_buffer[pos].symbol = symbol;
  ctx->back_buffer[pos].style = style;
}

void tau_put_filled_rectangle(tau_ctx *ctx, int x, int y, unsigned int width,
                              unsigned int height, uint32_t symbol,
                              tau_style style) {

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

  for (int i = start_x; i < end_x; i++) {
    for (int j = start_y; j < end_y; j++) {
      tau_put_char(ctx, symbol, i, j, style);
    }
  }
}

void tau_put_rectangle(tau_ctx *ctx, int x, int y, unsigned int width,
                       unsigned int height, uint32_t symbol, tau_style style) {

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

  for (int i = start_x; i < end_x; i++) {
    for (int j = start_y; j < end_y; j++) {
      if (i == start_x || i == end_x - 1 || j == start_y || j == end_y - 1) {
        tau_put_char(ctx, symbol, i, j, style);
      }
    }
  }
}

static void plot_ellipse_4(tau_ctx *ctx, int cx, int cy, int x, int y,
                           uint32_t symbol, tau_style style) {
  tau_put_char(ctx, symbol, cx + x, cy + y, style);
  tau_put_char(ctx, symbol, cx - x, cy + y, style);
  tau_put_char(ctx, symbol, cx + x, cy - y, style);
  tau_put_char(ctx, symbol, cx - x, cy - y, style);
}

static void fill_ellipse_4(tau_ctx *ctx, int cx, int cy, int x, int y,
                           uint32_t symbol, tau_style style) {
  tau_put_hline(ctx, cy + y, cx - x, cx + x, symbol, style);
  tau_put_hline(ctx, cy - y, cx - x, cx + x, symbol, style);
}

void tau_put_ellipse(tau_ctx *ctx, int cx, int cy, int rx, int ry, char symbol,
                     tau_style style) {
  if (!ctx || rx < 0 || ry < 0)
    return;

  long long rx2 = (long long)rx * rx;
  long long ry2 = (long long)ry * ry;
  long long two_rx2 = 2 * rx2;
  long long two_ry2 = 2 * ry2;

  long long x = 0;
  long long y = ry;

  long long px = 0;
  long long py = two_rx2 * y;

  long long p1 = ry2 - rx2 * ry + rx2 / 4;

  while (px < py) {
    plot_ellipse_4(ctx, cx, cy, (int)x, (int)y, symbol, style);

    x++;
    px += two_ry2;

    if (p1 < 0) {
      p1 += ry2 + px;
    } else {
      y--;
      py -= two_rx2;
      p1 += ry2 + px - py;
    }
  }

  double p2 = (double)ry2 * (x + 0.5) * (x + 0.5) +
              (double)rx2 * (y - 1) * (y - 1) - (double)rx2 * ry2;

  while (y >= 0) {
    plot_ellipse_4(ctx, cx, cy, (int)x, (int)y, symbol, style);

    y--;
    py -= two_rx2;

    if (p2 > 0) {
      p2 += rx2 - py;
    } else {
      x++;
      px += two_ry2;
      p2 += rx2 - py + px;
    }
  }
}

void tau_put_filled_ellipse(tau_ctx *ctx, int cx, int cy, int rx, int ry,
                            char symbol, tau_style style) {
  if (!ctx || rx < 0 || ry < 0)
    return;

  long long rx2 = (long long)rx * rx;
  long long ry2 = (long long)ry * ry;
  long long two_rx2 = 2 * rx2;
  long long two_ry2 = 2 * ry2;

  long long x = 0;
  long long y = ry;

  long long px = 0;
  long long py = two_rx2 * y;

  long long p1 = ry2 - rx2 * ry + rx2 / 4;

  while (px < py) {
    fill_ellipse_4(ctx, cx, cy, (int)x, (int)y, symbol, style);

    x++;
    px += two_ry2;

    if (p1 < 0) {
      p1 += ry2 + px;
    } else {
      y--;
      py -= two_rx2;
      p1 += ry2 + px - py;
    }
  }

  double p2 = (double)ry2 * (x + 0.5) * (x + 0.5) +
              (double)rx2 * (y - 1) * (y - 1) - (double)rx2 * ry2;

  while (y >= 0) {
    fill_ellipse_4(ctx, cx, cy, (int)x, (int)y, symbol, style);

    y--;
    py -= two_rx2;

    if (p2 > 0) {
      p2 += rx2 - py;
    } else {
      x++;
      px += two_ry2;
      p2 += rx2 - py + px;
    }
  }
}

void tau_put_circle(tau_ctx *ctx, int c_x, int c_y, unsigned int radius,
                    uint32_t symbol, tau_style style) {
  if (!ctx)
    return;

  tau_put_ellipse(ctx, c_x, c_y, 2 * radius, radius, symbol, style);
}

void tau_put_filled_circle(tau_ctx *ctx, int c_x, int c_y, unsigned int radius,
                           uint32_t symbol, tau_style style) {
  if (!ctx)
    return;

  tau_put_filled_ellipse(ctx, c_x, c_y, (int)((TERM_ASPECT_RATIO)*radius),
                         radius, symbol, style);
}

void tau_put_triangle(tau_ctx *ctx, int x0, int y0, int x1, int y1, int x2,
                      int y2, u_int32_t symbol, tau_style style) {
  if (!ctx)
    return;

  tau_put_line_aspect(ctx, x0, y0, x1, y1, 'L', style);
  tau_put_line_aspect(ctx, x0, y0, x2, y2, 'L', style);
  tau_put_line_aspect(ctx, x1, y1, x2, y2, 'L', style);
}

static void fill_flat_bottom_triangle(tau_ctx *ctx, int x0, int y0, int x1,
                                      int y1, int x2, int y2, uint32_t symbol,
                                      tau_style style) {
  double invslope1 = (double)(x1 - x0) / (double)(y1 - y0);
  double invslope2 = (double)(x2 - x0) / (double)(y2 - y0);

  double curx1 = x0;
  double curx2 = x0;

  for (int y = y0; y <= y1; y++) {
    tau_put_hline(ctx, y, (int)curx1, (int)curx2, symbol, style);
    curx1 += invslope1;
    curx2 += invslope2;
  }
}

static void fill_flat_top_triangle(tau_ctx *ctx, int x0, int y0, int x1, int y1,
                                   int x2, int y2, uint32_t symbol,
                                   tau_style style) {
  double invslope1 = (double)(x2 - x0) / (double)(y2 - y0);
  double invslope2 = (double)(x2 - x1) / (double)(y2 - y1);

  double curx1 = x2;
  double curx2 = x2;

  for (int y = y2; y >= y0; y--) {
    tau_put_hline(ctx, y, (int)curx1, (int)curx2, symbol, style);
    curx1 -= invslope1;
    curx2 -= invslope2;
  }
}

void tau_put_filled_triangle(tau_ctx *ctx, int x0, int y0, int x1, int y1,
                             int x2, int y2, u_int32_t symbol,
                             tau_style style) {
  if (!ctx)
    return;

  // sorted coords by y
  int sx0, sx1, sx2, sy0, sy1, sy2;
  sx0 = x0;
  sy0 = y0;
  sx1 = x1;
  sy1 = y1;
  sx2 = x2;
  sy2 = y2;

  if (sy0 > sy1) {
    int tmp = sy0;
    sy0 = sy1;
    sy1 = tmp;
    tmp = sx0;
    sx0 = sx1;
    sx1 = tmp;
  }
  if (sy1 > sy2) {
    int tmp = sy1;
    sy1 = sy2;
    sy2 = tmp;
    tmp = sx1;
    sx1 = sx2;
    sx2 = tmp;
  }

  if (sy0 > sy1) {
    int tmp = sy0;
    sy0 = sy1;
    sy1 = tmp;
    tmp = sx0;
    sx0 = sx1;
    sx1 = tmp;
  }

  if (sy1 == sy2) {
    fill_flat_bottom_triangle(ctx, sx0, sy0, sx1, sy1, sx2, sy2, symbol, style);
  } else if (sy0 == sy1) {
    fill_flat_top_triangle(ctx, sx0, sy0, sx1, sy1, sx2, sy2, symbol, style);
  } else {
    int sx3 =
        sx0 + (int)((double)(sy1 - sy0) / (double)(sy2 - sy0) * (sx2 - sx0));
    int sy3 = sy1;

    fill_flat_bottom_triangle(ctx, sx0, sy0, sx1, sy1, sx3, sy3, symbol, style);
    fill_flat_top_triangle(ctx, sx1, sy1, sx3, sy3, sx2, sy2, symbol, style);
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

void tau_fill(tau_ctx *ctx, uint32_t symbol, tau_style style) {
  for (size_t i = 0; i < ctx->nb_cells; i++) {
    ctx->back_buffer[i].symbol = symbol;
    ctx->back_buffer[i].style = style;
  }
}

void write_style_in_buffer(char **cursor, tau_style old, tau_style new) {

  bool font_style_differs = new.attrs != old.attrs;
  uint32_t added_styles = new.attrs & ~old.attrs;
  uint32_t removed_styles = old.attrs & ~new.attrs;

  // font styles
  if (font_style_differs) {
    if (removed_styles & TAU_ATTR_BOLD)
      write_in_buffer(cursor, SGR_BOLD_OFF);
    if (removed_styles & TAU_ATTR_FAINT)
      write_in_buffer(cursor, SGR_FAINT_OFF);
    if (removed_styles & TAU_ATTR_ITALIC)
      write_in_buffer(cursor, SGR_ITALIC_OFF);
    if (removed_styles & TAU_ATTR_UNDERLINE)
      write_in_buffer(cursor, SGR_UNDERLINE_OFF);
    if (removed_styles & TAU_ATTR_BLINK_S)
      write_in_buffer(cursor, SGR_BLINK_OFF);
    if (removed_styles & TAU_ATTR_BLINK_F)
      write_in_buffer(cursor, SGR_BLINK_OFF);
    if (removed_styles & TAU_ATTR_REVERSE)
      write_in_buffer(cursor, SGR_REVERSE_OFF);
    if (removed_styles & TAU_ATTR_CONCEAL)
      write_in_buffer(cursor, SGR_CONCEAL_OFF);
    if (removed_styles & TAU_ATTR_STRIKE)
      write_in_buffer(cursor, SGR_STRIKE_OFF);
    if (removed_styles & TAU_ATTR_DOUBLE_UNDERLINE)
      write_in_buffer(cursor, SGR_DOUBLE_UNDERLINE_OFF);
    if (removed_styles & TAU_ATTR_FRAME)
      write_in_buffer(cursor, SGR_FRAME_OFF);
    if (removed_styles & TAU_ATTR_ENCIRCLE)
      write_in_buffer(cursor, SGR_ENCIRCLE_OFF);
    if (removed_styles & TAU_ATTR_OVERLINE)
      write_in_buffer(cursor, SGR_OVERLINE_OFF);

    if (added_styles & TAU_ATTR_BOLD)
      write_in_buffer(cursor, SGR_BOLD);
    if (added_styles & TAU_ATTR_FAINT)
      write_in_buffer(cursor, SGR_FAINT);
    if (added_styles & TAU_ATTR_ITALIC)
      write_in_buffer(cursor, SGR_ITALIC);
    if (added_styles & TAU_ATTR_UNDERLINE)
      write_in_buffer(cursor, SGR_UNDERLINE);
    if (added_styles & TAU_ATTR_BLINK_S)
      write_in_buffer(cursor, SGR_BLINK_SLOW);
    if (added_styles & TAU_ATTR_BLINK_F)
      write_in_buffer(cursor, SGR_BLINK_RAPID);
    if (added_styles & TAU_ATTR_REVERSE)
      write_in_buffer(cursor, SGR_REVERSE);
    if (added_styles & TAU_ATTR_CONCEAL)
      write_in_buffer(cursor, SGR_CONCEAL);
    if (added_styles & TAU_ATTR_STRIKE)
      write_in_buffer(cursor, SGR_STRIKE);
    if (added_styles & TAU_ATTR_DOUBLE_UNDERLINE)
      write_in_buffer(cursor, SGR_DOUBLE_UNDERLINE);
    if (added_styles & TAU_ATTR_FRAME)
      write_in_buffer(cursor, SGR_FRAME);
    if (added_styles & TAU_ATTR_ENCIRCLE)
      write_in_buffer(cursor, SGR_ENCIRCLE);
    if (added_styles & TAU_ATTR_OVERLINE)
      write_in_buffer(cursor, SGR_OVERLINE);
  }

  // colors
  if (new.has_bg && new.has_fg) {
    write_in_buffer_colors(cursor, new.fg, new.bg);
  } else if (new.has_bg && !new.has_fg) {
    write_in_buffer_bg_color(cursor, new.bg);
    write_in_buffer(cursor, SGR_RESET_FG);
  } else if (new.has_fg && !new.has_bg) {
    write_in_buffer_fg_color(cursor, new.fg);
    write_in_buffer(cursor, SGR_RESET_BG);
  } else {
    write_in_buffer(cursor, SGR_RESET_COLORS);
  }
}

int styles_differ(struct tau_style a, struct tau_style b) {
  return a.attrs != b.attrs || a.bg.r != b.bg.r || a.bg.g != b.bg.g ||
         a.bg.b != b.bg.b || a.fg.r != b.fg.r || a.fg.g != b.fg.g ||
         a.fg.b != b.fg.b || a.has_fg != b.has_fg || a.has_bg != b.has_bg ||
         a.is_valid != b.is_valid;
}

int cells_differ(struct tau_cell a, struct tau_cell b) {
  return a.symbol != b.symbol || styles_differ(a.style, b.style);
}

void tau_draw_full(tau_ctx *ctx) {
  if (!ctx || !ctx->back_buffer || !ctx->output_buffer)
    return;

  char *cursor = ctx->output_buffer;

  // HOME cursor and clear
  write_in_buffer(&cursor, CURSOR_HOME);

  // COPY back buffer to output
  tau_style cached_style = TAU_STYLE_INVALID;
  for (size_t y = 0; y < ctx->nb_rows; y++) {
    for (size_t x = 0; x < ctx->nb_cols; x++) {
      size_t i = y * ctx->nb_cols + x;
      struct tau_cell cell = ctx->back_buffer[i];
      char c = (char)cell.symbol;

      if (styles_differ(cell.style, cached_style)) {
        write_style_in_buffer(&cursor, cached_style, cell.style);
        cached_style = cell.style;
      }

      if (c == '\0')
        c = ' ';
      *cursor++ = c;
    }

    if (y + 1 < ctx->nb_rows) {
      *cursor++ = '\r';
      *cursor++ = '\n';
    }
  }

  // DRAWS once ouput
  size_t len = (size_t)(cursor - ctx->output_buffer);
  write(STDOUT_FILENO, ctx->output_buffer, len);

  // UPDATES front buffer to be what is on screen
  memcpy(ctx->front_buffer, ctx->back_buffer,
         ctx->nb_cells * sizeof(*ctx->front_buffer));
}

// draws the difference between back buffer (what is new from the user)
// and front buffer (believed to be on screen)
// NOTE: current algo redraws in-between first and last diff in each row
void tau_draw_diff(tau_ctx *ctx) {
  if (!ctx || !ctx->front_buffer || !ctx->back_buffer || !ctx->output_buffer)
    return;

  char *cursor = ctx->output_buffer;

  for (size_t row = 0; row < ctx->nb_rows; row++) {
    int diff_in_line = 0;
    int first_index = 0;
    int last_index = 0;
    int first_diff_x = 0;

    for (size_t col = 0; col < ctx->nb_cols; col++) {
      size_t i = row * ctx->nb_cols + col;

      if (cells_differ(ctx->back_buffer[i], ctx->front_buffer[i])) {
        // first diff in line, get the x value
        if (!diff_in_line) {
          diff_in_line = 1;
          first_diff_x = col;
          first_index = i;
        }
        // last diff in line is always the latest
        last_index = i;
      }
    }

    if (diff_in_line) {
      write_in_buffer_move(&cursor, row + 1, first_diff_x + 1);

      tau_style cached_style = TAU_STYLE_INVALID;

      for (size_t i = first_index; i <= last_index; i++) {
        struct tau_cell cell = ctx->back_buffer[i];
        char c = (char)cell.symbol;

        if (styles_differ(cell.style, cached_style)) {
          write_style_in_buffer(&cursor, cached_style, cell.style);
          cached_style = cell.style;
        }

        if (c == '\0')
          c = ' ';
        *cursor++ = c;
      }

      // UPDATES front buffer to be what is on screen (with only the part that
      // changed)
      memcpy(&ctx->front_buffer[first_index], &ctx->back_buffer[first_index],
             (last_index - first_index + 1) * sizeof(*ctx->back_buffer));
    }
  }

  // DRAWS differences found
  size_t len = (size_t)(cursor - ctx->output_buffer);
  if (len > 0) {
    write(STDOUT_FILENO, ctx->output_buffer, len);
  }
}

void tau_put_line(tau_ctx *ctx, int x0, int y0, int x1, int y1, uint32_t symbol,
                  tau_style style) {
  if (!ctx)
    return;

  int dx = abs(x1 - x0);
  int dy = -abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (1) {
    tau_put_char(ctx, symbol, x0, y0, style);

    if (x0 == x1 && y0 == y1)
      break;

    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }

    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}
void tau_put_vline(tau_ctx *ctx, int x, int y0, int y1, uint32_t symbol,
                   tau_style style) {
  if (!ctx)
    return;

  if (x < 0 || x >= (int)ctx->nb_cols)
    return;

  if (y0 > y1) {
    int temp = y0;
    y0 = y1;
    y1 = temp;
  }

  if (y0 < 0)
    y0 = 0;

  if (y1 >= (int)ctx->nb_rows)
    y1 = (int)ctx->nb_rows - 1;

  for (int i = y0; i <= y1; i++) {
    tau_put_char(ctx, symbol, x, i, style);
  }
}

void tau_put_hline(tau_ctx *ctx, int y, int x0, int x1, uint32_t symbol,
                   tau_style style) {
  if (!ctx)
    return;

  if (y < 0 || y >= (int)ctx->nb_rows)
    return;

  if (x0 > x1) {
    int temp = x0;
    x0 = x1;
    x1 = temp;
  }

  if (x0 < 0)
    x0 = 0;

  if (x1 >= (int)ctx->nb_cols)
    x1 = (int)ctx->nb_cols - 1;

  for (int i = x0; i <= x1; i++) {
    tau_put_char(ctx, symbol, i, y, style);
  }
}

void tau_put_line_aspect(tau_ctx *ctx, int x0, int y0, int x1, int y1,
                         uint32_t symbol, tau_style style) {
  double aspect = 2.0; // cell height / cell width

  double px0 = x0;
  double py0 = y0 * aspect;
  double px1 = x1;
  double py1 = y1 * aspect;

  double dx = px1 - px0;
  double dy = py1 - py0;

  int steps = (int)(fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy));
  if (steps == 0) {
    tau_put_char(ctx, symbol, x0, y0, style);
    return;
  }

  for (int i = 0; i <= steps; i++) {
    double t = (double)i / (double)steps;

    double px = px0 + t * dx;
    double py = py0 + t * dy;

    int gx = (int)lround(px);
    int gy = (int)lround(py / aspect);

    tau_put_char(ctx, symbol, gx, gy, style);
  }
}
