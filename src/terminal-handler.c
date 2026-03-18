#include "escape-sequences.h"
#include "platform.h"
#include "terminal-anim.h"
#include <poll.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>

#define BYTES_PER_PIXEL 32

struct tau_cell {
  uint32_t symbol;
  uint8_t fg_r, fg_g, fg_b;
  uint8_t bg_r, bg_g, bg_b;
  uint8_t attrs;
};

struct tau_ctx {
  int id;
  struct tau_cell *front_buffer;
  struct tau_cell *back_buffer;
  size_t nb_cols;
  size_t nb_rows;
  size_t nb_cells;
  size_t output_capacity;
  char *output_buffer;
  int cursor_x;
  int cursor_y;
  struct termios termios_conf_init;
};

volatile int g_tau_ctx_nb_open = 0;
static tau_ctx *g_active_ctx = NULL; // only one at a time

volatile sig_atomic_t tau_g_is_running;
static unsigned int screen_max_rows, screen_max_cols = 0;

tau_ctx *g_ctx_destroy_fallback = NULL;

void resize_screen_callback(int new_rows, int new_cols) {
  screen_max_rows = new_rows;
  screen_max_cols = new_cols;
}

void tau_destroy(tau_ctx *ctx) {
  term_write_output(SHOW_CURSOR);
  term_write_output(ALTERNATIVE_BUFFER_OFF);
  term_write_output(CLEAR_ALL);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->termios_conf_init);

  free(ctx->back_buffer);
  free(ctx->front_buffer);
  free(ctx->output_buffer);
  free(ctx);

  g_tau_ctx_nb_open--;

  if (g_active_ctx == ctx) {
    g_active_ctx = NULL;
  }
}

void generic_destroy() {
  if (g_ctx_destroy_fallback != NULL) {
    tau_destroy(g_ctx_destroy_fallback);
  }
}

static void handler_sigs_end(int sig) {
  (void)sig; // unused
  tau_g_is_running = 0;
}

tau_ctx *tau_create() {

  if (g_active_ctx != NULL) {
    return NULL;
  }

  if (!isatty(STDIN_FILENO)) {
    return NULL;
  }

  tau_ctx *ctx = calloc(1, sizeof(*ctx));
  if (!ctx)
    return NULL;

  g_active_ctx = ctx;

  ctx->id = ++g_tau_ctx_nb_open;
  ctx->cursor_x = 0;
  ctx->cursor_y = 0;

  unsigned int rows, cols;
  pf_get_size(&rows, &cols);
  ctx->nb_rows = rows;
  ctx->nb_cols = cols;
  ctx->nb_cells = ctx->nb_rows * ctx->nb_cols;

  ctx->back_buffer = calloc(ctx->nb_cells, sizeof(*ctx->back_buffer));
  ctx->front_buffer = calloc(ctx->nb_cells, sizeof(*ctx->front_buffer));

  // output_buffer is BYTES_PER_PIXEL bytes per cell
  ctx->output_capacity =
      ctx->nb_cells * sizeof(*ctx->output_buffer) * BYTES_PER_PIXEL;
  ctx->output_buffer = calloc(ctx->output_capacity, ctx->output_capacity);

  if (!ctx->back_buffer || !ctx->front_buffer || !ctx->output_buffer) {
    free(ctx->back_buffer);
    free(ctx->front_buffer);
    free(ctx->output_buffer);
    free(ctx);
    return NULL;
  }

  struct termios termios_conf_old = {0};
  struct termios termios_conf_new = {0};

  g_ctx_destroy_fallback = ctx;

  tcgetattr(STDIN_FILENO, &termios_conf_old);
  ctx->termios_conf_init = termios_conf_old;

  // copy existing
  termios_conf_new = termios_conf_old;

  // diable flow control with C-S/C-Q
  termios_conf_new.c_iflag &= ~IXON;
  // disable disable carriage return to new line
  termios_conf_new.c_iflag &= ~ICRNL;
  // disable other post processing
  termios_conf_new.c_iflag &= ~OPOST;

  // disable print user
  // wait for enter input (byte by byte instead)
  // Disable C-V (paste)
  termios_conf_new.c_lflag &= ~(ECHO | ICANON | IEXTEN);

  termios_conf_new.c_lflag |= ISIG;

  // set new conf
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_conf_new);

  atexit(generic_destroy);
  struct sigaction sa_int;
  sa_int.sa_handler = handler_sigs_end;
  sa_int.sa_flags = 0;
  sigemptyset(&sa_int.sa_mask);
  sigaction(SIGTERM, &sa_int, NULL);
  sigaction(SIGINT, &sa_int, NULL);

  pf_register_size_change_cb((pf_size_change_cb)&resize_screen_callback);

  tau_g_is_running = 1;

  term_write_output(HIDE_CURSOR);
  term_write_output(ALTERNATIVE_BUFFER_ON);
  term_write_output(CLEAR_ALL);
  return ctx;
}

void tau_draw(tau_ctx *ctx) {
  if (!ctx || !ctx->front_buffer || !ctx->output_buffer)
    return;

  char *cursor = ctx->output_buffer;

  for (size_t y = 0; y < ctx->nb_rows; y++) {
    for (size_t x = 0; x < ctx->nb_cols; x++) {
      size_t i = y * ctx->nb_cols + x;
      char c = (char)ctx->front_buffer[i].symbol;
      if (c == '\0')
        c = ' ';
      *cursor++ = c;
    }

    if (y + 1 < ctx->nb_rows) {
      *cursor++ = '\r';
      *cursor++ = '\n';
    }
  }

  size_t len = (size_t)(cursor - ctx->output_buffer);
  write(STDOUT_FILENO, ctx->output_buffer, len);
}
