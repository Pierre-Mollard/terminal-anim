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

typedef struct tau_style {
  tau_rgb fg;
  tau_rgb bg;
  bool has_fg;
  bool has_bg;
  uint8_t attrs;
  bool is_valid;
} tau_style;

#define TAU_STYLE_INVALID ((tau_style){.is_valid = false})
#define TAU_STYLE_DEFAULT                                                      \
  ((tau_style){.is_valid = true, .has_fg = false, .has_bg = false})

tau_ctx *tau_create();
void tau_destroy(tau_ctx *ctx);
void tau_fill(tau_ctx *ctx, uint32_t symbol, tau_style style);
void tau_draw(tau_ctx *ctx);
void tau_present(tau_ctx *ctx);
void tau_clear(tau_ctx *ctx);
void tau_put_square(tau_ctx *ctx, int x, int y, unsigned int width,
                    unsigned int height, tau_style style);
void tau_put_str(tau_ctx *ctx, char *str, size_t size, int x, int y,
                 tau_style style);

extern volatile sig_atomic_t tau_g_is_running;

#endif
