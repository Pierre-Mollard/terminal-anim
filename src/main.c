#include "escape-sequences.h"
#include "platform.h"
#include "scene.h"
#include "terminal-anim.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
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
  char screen_buffer[MAX_BUFFER_WIDTH];
  char *cursor = screen_buffer;
  memset(screen_buffer, '\0', sizeof(screen_buffer));

  write_in_buffer_move(&cursor, 1, 1);

  write_in_buffer(&cursor, CLEAR_SCREEN);
  write_in_buffer_color(&cursor, 255, 0, 0);
  write_in_buffer_color(&cursor, 0, 255, 0);
  //  draw_square(&cursor, 10, 20, 5, 5);
  write_in_buffer_color(&cursor, 0, 0, 255);
  // draw_square(&cursor, 20, 30, 50, 7);
  write_in_buffer_color(&cursor, 120, 120, 120);
  // draw_square(&cursor, user_x, user_y, 5, 5);

  write_in_buffer_color(&cursor, 255, 255, 255);
  write_in_buffer_move(&cursor, 0, 0);
  write_in_buffer_f(&cursor, "ROWS: %d, COLS: %d", screen_max_rows,
                    screen_max_cols);

  write_in_buffer_move(&cursor, 0, 50);
  write_in_buffer_f(&cursor, "READ: %c", input_display);

  write_in_buffer_move(&cursor, 0, 70);
  write_in_buffer(&cursor, "CTRL-C to quit");

  write_in_buffer_move(&cursor, 2, 0);
  write_in_buffer(&cursor, "hjkl to move");
  write_in_buffer_move(&cursor, 2, 80);
  write_in_buffer(&cursor, "maj SHIFT move further");

  // disabled: scene_update(&cursor, 0, 0, screen_max_cols / 3, screen_max_rows
  // / 3);

  write_in_term(screen_buffer);
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
