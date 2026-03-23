#include "escape-sequences.h"
#include "platform.h"
#include "terminal-anim-internal.h"
#include "terminal-anim.h"
#include <poll.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>

volatile int g_tau_ctx_nb_open = 0;
static tau_ctx *g_active_ctx = NULL; // only one at a time

volatile sig_atomic_t tau_g_is_running;
static unsigned int screen_max_rows, screen_max_cols = 0;

void resize_screen_callback(int new_rows, int new_cols) {
  screen_max_rows = new_rows;
  screen_max_cols = new_cols;

  if (g_active_ctx != NULL) {
    tau_resize_buffers(g_active_ctx, new_rows, new_cols);
  }
}

void tau_destroy(tau_ctx *ctx) {
  if (!ctx)
    return;

  write_in_term(SGR_RESET_BG);
  write_in_term(SGR_RESET_FG);
  write_in_term(SGR_RESET_COLORS);
  write_in_term(SHOW_CURSOR);
  write_in_term(ALTERNATIVE_BUFFER_OFF);
  write_in_term(CLEAR_ALL);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->termios_conf_init);

  if (g_active_ctx == ctx) {
    g_active_ctx = NULL;
  }
  g_tau_ctx_nb_open--;

  free(ctx->back_buffer);
  free(ctx->front_buffer);
  free(ctx->output_buffer);
  free(ctx);
}

void generic_destroy() {
  if (g_active_ctx != NULL) {
    tau_destroy(g_active_ctx);
  }
}

static void handler_sigs_end(int sig) {
  (void)sig; // unused
  tau_g_is_running = 0;
}

tau_ctx *tau_create() {

  if (g_active_ctx != NULL) {
    perror("already active");
    return NULL;
  }

  if (!isatty(STDIN_FILENO)) {
    perror("tty error");
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
  ctx->output_capacity = ctx->nb_cells * BYTES_PER_PIXEL;
  ctx->output_buffer =
      calloc(ctx->output_capacity, sizeof(*ctx->output_buffer));

  if (!ctx->back_buffer || !ctx->front_buffer || !ctx->output_buffer) {
    printf("capacity: %zu\n", ctx->output_capacity);
    free(ctx->back_buffer);
    free(ctx->front_buffer);
    free(ctx->output_buffer);
    free(ctx);
    perror("calloc error");
    return NULL;
  }

  struct termios termios_conf_old = {0};
  struct termios termios_conf_new = {0};

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

  write_in_term(HIDE_CURSOR);
  write_in_term(ALTERNATIVE_BUFFER_ON);
  write_in_term(CLEAR_ALL);
  return ctx;
}

void tau_resize_buffers(tau_ctx *ctx, unsigned int rows, unsigned int cols) {
  if (ctx == NULL)
    return;

  if (rows == 0 || cols == 0)
    return;

  if (rows > SIZE_MAX / cols)
    return;

  size_t new_nb_cells = rows * cols;

  if (new_nb_cells > SIZE_MAX / sizeof(*ctx->back_buffer))
    return;

  struct tau_cell *new_back = calloc(new_nb_cells, sizeof(*new_back));
  if (new_back == NULL)
    return;

  if (new_nb_cells > SIZE_MAX / sizeof(*ctx->front_buffer)) {
    free(new_back);
    return;
  }

  struct tau_cell *new_front = calloc(new_nb_cells, sizeof(*new_front));
  if (new_front == NULL) {
    free(new_back);
    return;
  }

  if (new_nb_cells > SIZE_MAX / BYTES_PER_PIXEL) {
    free(new_back);
    free(new_front);
    return;
  }
  size_t new_output_capacity = new_nb_cells * BYTES_PER_PIXEL;
  char *new_output = calloc(new_output_capacity, sizeof(*new_output));
  if (!new_output) {
    free(new_back);
    free(new_front);
    return;
  }

  size_t min_rows = ctx->nb_rows < rows ? ctx->nb_rows : rows;
  size_t min_cols = ctx->nb_cols < cols ? ctx->nb_cols : cols;

  for (size_t y = 0; y < min_rows; y++) {
    memcpy(&new_back[y * cols], &ctx->back_buffer[y * ctx->nb_cols],
           min_cols * sizeof(*new_back));
    memcpy(&new_front[y * cols], &ctx->front_buffer[y * ctx->nb_cols],
           min_cols * sizeof(*new_front));
  }

  free(ctx->back_buffer);
  free(ctx->front_buffer);
  free(ctx->output_buffer);

  ctx->nb_rows = rows;
  ctx->nb_cols = cols;
  ctx->nb_cells = new_nb_cells;
  ctx->output_capacity = new_output_capacity;
  ctx->back_buffer = new_back;
  ctx->front_buffer = new_front;
  ctx->output_buffer = new_output;
}
