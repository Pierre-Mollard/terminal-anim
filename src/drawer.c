#include "escape-sequences.h"
#include "platform.h"
#include "terminal-anim-internal.h"
#include "terminal-anim.h"

#include "platform.h"
#include <math.h>
#include <stdbool.h>
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

void tau_put_char(tau_ctx *ctx, char c, int x, int y, tau_style style) {
  if (!ctx)
    return;
  if (x < 0 || y < 0)
    return;
  if (x >= (int)ctx->nb_cols || y >= (int)ctx->nb_rows)
    return;

  size_t pos = (size_t)y * ctx->nb_cols + (size_t)x;
  ctx->back_buffer[pos].symbol = c;
  ctx->back_buffer[pos].style = style;
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

  for (int i = start_x; i < end_x; i++) {
    for (int j = start_y; j < end_y; j++) {
      int coords = j * ctx->nb_cols + i;
      ctx->back_buffer[coords].symbol = 'H';
      ctx->back_buffer[coords].style = style;
    }
  }
}

void tau_put_circle(tau_ctx *ctx, int c_x, int c_y, unsigned int radius,
                    tau_style style) {
  if (!ctx)
    return;

  int screen_width = (int)ctx->nb_cols;
  int screen_height = (int)ctx->nb_rows;

  for (int x = 0; x < screen_width; x++) {
    for (int y = 0; y < screen_height; y++) {
      int coords = y * ctx->nb_cols + x;

      double dx = (x - c_x) * 0.5;
      double dy = y - c_y;
      double dist = dx * dx + dy * dy;

      if (dist < radius * radius) {
        ctx->back_buffer[coords].symbol = 'H';
        ctx->back_buffer[coords].style = style;
      }
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
