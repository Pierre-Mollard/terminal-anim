#ifndef TERMINAL_ANIM_PUB_LIB_H
#define TERMINAL_ANIM_PUB_LIB_H

#include <signal.h>
#include <stddef.h>
#include <stdint.h>

// NOTE: tau is prefix of this lib (for now...)

// Context descriptor
typedef struct tau_ctx tau_ctx;
struct tau_style {
  uint8_t fg_r, fg_g, fg_b;
  uint8_t bg_r, bg_g, bg_b;
  uint8_t attrs;
};
typedef struct tau_style tau_style;

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
