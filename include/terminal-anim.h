#ifndef TERMINAL_ANIM_PUB_LIB_H
#define TERMINAL_ANIM_PUB_LIB_H

#include <signal.h>
#include <stdint.h>

// NOTE: tau is prefix of this lib (for now...)

// Context descriptor
typedef struct tau_ctx tau_ctx;

tau_ctx *tau_create();
void tau_destroy(tau_ctx *ctx);
void tau_fill(tau_ctx *ctx, uint32_t symbol);
void tau_draw(tau_ctx *ctx);
void tau_present(tau_ctx *ctx);

void draw_square(char **buffer_cursor, int x, int y, unsigned int width,
                 unsigned int height);

void clear_screen(char **buffer_cursor);

extern volatile sig_atomic_t tau_g_is_running;

#endif
