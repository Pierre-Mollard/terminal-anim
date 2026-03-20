#include "terminal-anim.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUFFER_WIDTH 40000
static unsigned int max_rows = 0;
static unsigned int max_cols = 0;

static int g_no_lib_running = 0;

void local_get_size(unsigned int *rows, unsigned int *cols) {
  struct winsize ws;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == 0) {
    *rows = ws.ws_row;
    *cols = ws.ws_col;
  } else {
    *rows = 24;
    *cols = 80;
  }
}

void fill_buffer(char *buffer, int nbrows, int nbcols, char content) {
  for (int y = 0; y < nbrows; y++) {
    memset(buffer + y * (nbcols + 1), content, nbcols);
    buffer[y * (nbcols + 1) + nbcols] = '\n';
  }
  buffer[nbrows * (nbcols + 1)] = '\0';
}

static void no_lib_teardown(int sig) {
  (void)sig; // unused
  g_no_lib_running = 0;
}

int main(int argc, char *argv[]) {
  local_get_size(&max_rows, &max_cols);
  printf("FULLSCREEN TEST ROWS:%d/COLS:%d\n", max_rows, max_cols);
  printf("Show template mode (1) or test lib (2) ?\n");
  printf("Type then enter...\n");

  int template_mode = 0;
  char user_input = '0';
  while (user_input != '1' && user_input != '2') {
    fscanf(stdin, "%c", &user_input);
  }

  struct timespec ts;
  ts.tv_nsec = 20000000; // 20ms

  if (user_input == '1') {
    char screen_buffer[MAX_BUFFER_WIDTH];
    char *cursor = screen_buffer;
    memset(screen_buffer, '\0', sizeof(screen_buffer));

    // NOTE: No lib INIT
    cursor = screen_buffer;
    printf("%s", screen_buffer);
    printf("%s", screen_buffer);
    fflush(stdout);
    printf("%s", screen_buffer);
    fflush(stdout);
    // setup teardown
    g_no_lib_running = 1;
    struct sigaction sa_int = {0};
    sa_int.sa_handler = no_lib_teardown;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGTERM, &sa_int, NULL);
    sigaction(SIGINT, &sa_int, NULL);

    // REFRESH
    char used_char = '#';
    int counter = 0;
    while (g_no_lib_running) {
      counter++;
      used_char = '#';
      if (counter % 2 == 0) {
        used_char = '@';
      }
      memset(screen_buffer, '\0', sizeof(screen_buffer));
      fill_buffer(screen_buffer, max_rows, max_cols, used_char);
      write(STDOUT_FILENO, screen_buffer, (max_cols + 1) * max_rows * 2);
      // NOTE: wont work with some escpapes sequences making it bigger
      while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        // retry with remaining time
      }
    }

    // TEARDOWN
    cursor = screen_buffer;
    memset(screen_buffer, '\0', sizeof(screen_buffer));
    printf("%s", screen_buffer);
    fflush(stdout);
    printf("%s", screen_buffer);
    fflush(stdout);

  } else {

    // NOTE: Lib INIT
    tau_ctx *ctx = tau_create();
    if (ctx == NULL) {
      printf("Setup failed with ctx NULL\n");
    }

    // TODO: Lib refresh
    char used_char = 'I';
    int counter = 0;

    tau_style bg_style = {0};
    bg_style.fg_b = 255;
    bg_style.fg_g = 255;
    bg_style.fg_r = 255;

    tau_fill(ctx, used_char, bg_style);
    tau_draw(ctx);

    while (tau_g_is_running) {
      counter++;
      used_char = '#';
      if (counter % 2 == 0) {
        used_char = '@';
      }
      tau_fill(ctx, used_char, bg_style);
      tau_present(ctx);
      while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        // retry with remaining time
      }
    }

    // TEARDOWN
    tau_destroy(ctx);
  }

  return 0;
}
