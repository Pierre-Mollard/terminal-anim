#include "escape-sequences.h"
#include "platform.h"
#include "terminal-anim.h"
#include <poll.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>

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
};

volatile int g_tau_ctx_nb_open = 0;

volatile sig_atomic_t tau_g_is_running;
static int has_init = 0;
static struct termios termios_conf_init;
static unsigned int screen_max_rows, screen_max_cols = 0;

tau_ctx *g_ctx_destroy_fallback = NULL;

void resize_screen_callback(int new_rows, int new_cols) {
  screen_max_rows = new_rows;
  screen_max_cols = new_cols;
}

void tau_destroy(tau_ctx *ctx) {
  int rc = 0;

  term_write_output(SHOW_CURSOR);
  term_write_output(ALTERNATIVE_BUFFER_OFF);
  term_write_output(CLEAR_ALL);

  if (!has_init)
    rc = 1;

  if (rc == 0) {
    rc = tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_conf_init);
  }

  g_tau_ctx_nb_open--;
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
  tau_ctx *ctx = calloc(1, sizeof(*ctx));
  if (!ctx)
    return NULL;

  ctx->id = ++g_tau_ctx_nb_open;
  ctx->cursor_x = 0;
  ctx->cursor_y = 0;

  // TODO: handle resize terminal that resize buffers
  ctx->nb_rows = 20;
  ctx->nb_cols = 20;
  ctx->nb_cells = ctx->nb_rows * ctx->nb_cols;

  ctx->back_buffer = calloc(ctx->nb_cells, sizeof(*ctx->back_buffer));
  ctx->front_buffer = calloc(ctx->nb_cells, sizeof(*ctx->front_buffer));

  // NOTE: output_buffer is 8bytes per cell
  ctx->output_capacity = ctx->nb_cells * sizeof(*ctx->output_buffer) * 8;
  ctx->output_buffer = calloc(ctx->output_capacity, ctx->output_capacity);

  if (!ctx->back_buffer || !ctx->front_buffer || !ctx->output_buffer) {
    free(ctx->back_buffer);
    free(ctx->front_buffer);
    free(ctx->output_buffer);
    free(ctx);
    return NULL;
  }

  struct termios termios_conf_new;
  int rc = 0;

  g_ctx_destroy_fallback = ctx;

  if (has_init)
    rc = 1;

  if (!isatty(STDIN_FILENO)) {
    rc = 2;
    ctx = NULL;
  }

  if (rc == 0) {
    rc = tcgetattr(STDIN_FILENO, &termios_conf_init);
  }

  if (rc == 0) {
    // copy existing
    termios_conf_new = termios_conf_init;

    // diable flow control with C-S/C-Q
    termios_conf_new.c_iflag &= ~ICRNL;

    // disable print user
    // wait for enter input (byte by byte instead)
    // Disable C-V (paste)
    termios_conf_new.c_lflag &= ~(ECHO | ICANON | IEXTEN);

    termios_conf_new.c_lflag |= ISIG;
  }

  if (rc == 0) {
    rc = tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_conf_new);
  }

  if (rc == 0) {
    atexit(generic_destroy);
    struct sigaction sa_int;
    sa_int.sa_handler = handler_sigs_end;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGTERM, &sa_int, NULL);
    sigaction(SIGINT, &sa_int, NULL);
  }

  if (rc == 0) {
    has_init = 1;
  } else {
    ctx = NULL;
  }

  pf_register_size_change_cb((pf_size_change_cb)&resize_screen_callback);

  tau_g_is_running = has_init;

  term_write_output(HIDE_CURSOR);
  term_write_output(ALTERNATIVE_BUFFER_ON);
  term_write_output(CLEAR_ALL);
  return ctx;
}
