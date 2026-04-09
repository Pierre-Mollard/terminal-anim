#ifndef TERMINAL_ANIM_PUB_LIB_H
#define TERMINAL_ANIM_PUB_LIB_H

#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// NOTE: tau is prefix of this lib (for now...)

// Context descriptor
typedef struct tau_ctx tau_ctx;

typedef struct tau_rgb {
  uint8_t r, g, b;
} tau_rgb;

// comment: use 1ULL if exceeding 31
enum tau_attr {
  TAU_ATTR_NONE = 0,
  TAU_ATTR_BOLD = 1u << 0,
  TAU_ATTR_FAINT = 1u << 1,
  TAU_ATTR_ITALIC = 1u << 2,
  TAU_ATTR_UNDERLINE = 1u << 3,
  TAU_ATTR_BLINK_S = 1u << 4,
  TAU_ATTR_BLINK_F = 1u << 5,
  TAU_ATTR_REVERSE = 1u << 6,
  TAU_ATTR_CONCEAL = 1u << 7,
  TAU_ATTR_STRIKE = 1u << 8,
  TAU_ATTR_DOUBLE_UNDERLINE = 1u << 9,
  TAU_ATTR_FRAME = 1u << 10,
  TAU_ATTR_ENCIRCLE = 1u << 11,
  TAU_ATTR_OVERLINE = 1u << 12,
};

typedef enum {
  TAU_EVT_NONE,
  TAU_EVT_RESIZE,
  TAU_EVT_KEY,
  TAU_EVT_MOUSE_PRESS,
  TAU_EVT_MOUSE_RELEASE,
  TAU_EVT_MOUSE_MOVE,
  TAU_EVT_MOUSE_WHEEL_UP,
  TAU_EVT_MOUSE_WHEEL_DOWN,
} tau_event_type;

typedef struct tau_style {
  tau_rgb fg;
  tau_rgb bg;
  bool has_fg;
  bool has_bg;
  uint32_t attrs;
  bool is_valid;
} tau_style;

typedef struct {
  tau_event_type type;
  union {
    struct {
      int x, y, button, wheel;
      int shift, alt, ctrl, motion, release;
    } mouse;
    struct {
      uint32_t codepoint, key, mods;
    } key;
    struct {
      unsigned int cols, rows;
    } resize;
  } data;
} tau_event;

#define TAU_STYLE_INVALID ((tau_style){.is_valid = false})
#define TAU_STYLE_DEFAULT                                                      \
  ((tau_style){.is_valid = true, .has_fg = false, .has_bg = false, .attrs = 0})

tau_ctx *tau_create();
void tau_destroy(tau_ctx *ctx);
void tau_resize_buffers(tau_ctx *ctx, unsigned int rows, unsigned int cols);
void tau_get_terminal_info(tau_ctx *ctx, unsigned int *rows,
                           unsigned int *cols);
void tau_update_input(tau_ctx *ctx);
bool tau_poll_event(tau_ctx *ctx, tau_event *evt);

void tau_toggle_input_evt(tau_ctx *ctx, bool enable);
void tau_toggle_motion_evt(tau_ctx *ctx, bool enable);

void tau_fill(tau_ctx *ctx, uint32_t symbol, tau_style style);
void tau_draw_full(tau_ctx *ctx);
void tau_draw_diff(tau_ctx *ctx);
void tau_clear(tau_ctx *ctx);

void tau_put_char(tau_ctx *ctx, uint32_t c, int x, int y, tau_style style);
void tau_put_str(tau_ctx *ctx, char *str, size_t size, int x, int y,
                 tau_style style);

void tau_put_line(tau_ctx *ctx, int x1, int y1, int x2, int y2, uint32_t symbol,
                  tau_style style);
void tau_put_vline(tau_ctx *ctx, int x, int y0, int y1, uint32_t symbol,
                   tau_style style);
void tau_put_hline(tau_ctx *ctx, int y, int x0, int x1, uint32_t symbol,
                   tau_style style);
void tau_put_line_aspect(tau_ctx *ctx, int x0, int y0, int x1, int y1,
                         uint32_t symbol, tau_style style);

void tau_put_rectangle(tau_ctx *ctx, int x, int y, unsigned int width,
                       unsigned int height, uint32_t symbol, tau_style style);
void tau_put_filled_rectangle(tau_ctx *ctx, int x, int y, unsigned int width,
                              unsigned int height, uint32_t symbol,
                              tau_style style);

void tau_put_ellipse(tau_ctx *ctx, int cx, int cy, int rx, int ry, char symbol,
                     tau_style style);
void tau_put_filled_ellipse(tau_ctx *ctx, int cx, int cy, int rx, int ry,
                            char symbol, tau_style style);

void tau_put_circle(tau_ctx *ctx, int x, int y, unsigned int radius,
                    uint32_t symbol, tau_style style);
void tau_put_filled_circle(tau_ctx *ctx, int x, int y, unsigned int radius,
                           uint32_t symbol, tau_style style);

void tau_put_filled_triangle(tau_ctx *ctx, int x0, int y0, int x1, int y1,
                             int x2, int y2, uint32_t symbol, tau_style style);
void tau_put_triangle(tau_ctx *ctx, int x0, int y0, int x1, int y1, int x2,
                      int y2, uint32_t symbol, tau_style style);

extern volatile sig_atomic_t tau_g_is_running;

#endif
