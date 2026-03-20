#include "escape-sequences.h"
#include "platform.h"
#include "scene.h"
#include "terminal-anim.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>

// variables for this file
volatile int user_x = 0;
volatile int user_y = 0;
static unsigned int screen_max_rows, screen_max_cols = 0;
static char input_display = ' ';

void handle_user_input(char c) {
  if (c == 'j' && user_y < screen_max_rows)
    user_y++;
  if (c == 'k' && user_y > 0)
    user_y--;
  if (c == 'l' && user_x < screen_max_cols)
    user_x++;
  if (c == 'h' && user_x > 0)
    user_x--;

  if (c == 'J' && user_y < screen_max_rows - 4)
    user_y += 5;
  if (c == 'K' && user_y > 4)
    user_y -= 5;
  if (c == 'L' && user_x < screen_max_cols - 4)
    user_x += 5;
  if (c == 'H' && user_x > 4)
    user_x -= 5;

  input_display = c;
}

void draw_screen(tau_ctx *ctx) {

  tau_style bg_style = {0};
  bg_style.fg_r = 0;
  bg_style.fg_g = 0;
  bg_style.fg_b = 0;
  tau_fill(ctx, ' ', bg_style);

  tau_style red = {0};
  red.fg_r = 255;
  tau_style green = {0};
  green.fg_g = 255;
  tau_style blue = {0};
  blue.fg_b = 255;
  tau_style gray = {0};
  gray.fg_r = 120;
  gray.fg_g = 120;
  gray.fg_b = 120;
  tau_style white = {0};
  white.fg_r = 255;
  white.fg_g = 255;
  white.fg_b = 255;
  tau_put_square(ctx, 10, 20, 5, 5, green);
  tau_put_square(ctx, 20, 30, 50, 7, blue);
  tau_put_square(ctx, user_x, user_y, 5, 5, gray);

  char buffer[256];
  snprintf(buffer, 256, "ROWS: %d, COLS: %d", screen_max_rows, screen_max_cols);
  tau_put_str(ctx, buffer, strlen(buffer), 0, 0, white);

  snprintf(buffer, 256, "READ: %c", input_display);
  tau_put_str(ctx, buffer, strlen(buffer), 0, 2, white);

  snprintf(buffer, 256, "CTRL-C to quit");
  tau_put_str(ctx, buffer, strlen(buffer), 0, 30, red);

  snprintf(buffer, 256, "hjkl to move");
  tau_put_str(ctx, buffer, strlen(buffer), 2, 29, white);
  snprintf(buffer, 256, "maj SHIFT move further");
  tau_put_str(ctx, buffer, strlen(buffer), 50, 30, white);

  // disabled: scene_update(&cursor, 0, 0, screen_max_cols / 3, screen_max_rows
  // / 3);

  tau_present(ctx);
}

int main(int argc, char *argv[]) {
  tau_ctx *ctx = tau_create();
  if (ctx == NULL) {
    printf("Setup failed with to create ctx\n");
    exit(1);
  }

  pf_get_size(&screen_max_rows, &screen_max_cols);

  struct pollfd poll_fd[1];
  poll_fd[0].fd = STDIN_FILENO;
  poll_fd[0].events = POLLIN;
  poll_fd[0].revents = 0;

  scene_init();

  while (tau_g_is_running) {
    pf_poll_events();

    int ret_poll = poll(poll_fd, 1, 20); // ms
    if (ret_poll == -1) {
      if (errno == EINTR)
        continue; // interrupted by SIGWINCH handler
      perror("poll");
      break;
    }
    if (ret_poll > 0) {
      if (poll_fd[0].revents & POLLIN) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
          handle_user_input(c);
        }
      }
    }

    draw_screen(ctx);
  }

  tau_destroy(ctx);
  return 0;
}
