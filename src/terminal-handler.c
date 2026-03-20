#include "escape-sequences.h"
#include "platform.h"
#include "terminal-anim-internal.h"
#include "terminal-anim.h"
#include <poll.h>
#include <signal.h>
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
}

void tau_destroy(tau_ctx *ctx) {
  if (!ctx)
    return;

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

void tau_fill(tau_ctx *ctx, uint32_t symbol, tau_style style) {
  for (size_t i = 0; i < ctx->nb_cells; i++) {
    ctx->back_buffer[i].symbol = symbol;
    ctx->back_buffer[i].style = style;
  }
}

void tau_draw(tau_ctx *ctx) {
  if (!ctx || !ctx->back_buffer || !ctx->output_buffer)
    return;

  char *cursor = ctx->output_buffer;

  // HOME cursor and clear
  write_in_buffer(&cursor, CURSOR_HOME);

  // COPY back buffer to output
  for (size_t y = 0; y < ctx->nb_rows; y++) {
    for (size_t x = 0; x < ctx->nb_cols; x++) {
      size_t i = y * ctx->nb_cols + x;
      char c = (char)ctx->back_buffer[i].symbol;
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

int compare_styles(struct tau_style a, struct tau_style b) {
  return a.attrs != b.attrs || a.bg_r != b.bg_r || a.bg_g != b.bg_g ||
         a.bg_b != b.bg_b || a.fg_r != b.fg_r || a.fg_g != b.fg_g ||
         a.fg_b != b.fg_b;
}

int compare_cells(struct tau_cell a, struct tau_cell b) {
  return a.symbol != b.symbol || compare_styles(a.style, b.style);
}

// draws the difference between back buffer (what is new from the user)
// and front buffer (believed to be on screen)
// NOTE: current algo redraws in-between first and last diff in each row
void tau_present(tau_ctx *ctx) {
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

      if (compare_cells(ctx->back_buffer[i], ctx->front_buffer[i])) {
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

      for (size_t i = first_index; i <= last_index; i++) {
        struct tau_cell cell = ctx->back_buffer[i];
        char c = (char)cell.symbol;
        write_in_buffer_color(&cursor, cell.style.fg_r, cell.style.fg_g,
                              cell.style.fg_b);
        if (c == '\0')
          c = ' ';
        *cursor++ = c;
      }

      // UPDATES front buffer to be what is on screen (with only the part that
      // changed)
      memcpy(&ctx->front_buffer[first_index], &ctx->back_buffer[first_index],
             (last_index - first_index - 1) * sizeof(*ctx->back_buffer));
    }
  }

  // DRAWS differences found
  size_t len = (size_t)(cursor - ctx->output_buffer);
  if (len > 0) {
    write(STDOUT_FILENO, ctx->output_buffer, len);
  }
}
