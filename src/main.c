#include "box-drawing-chars.h"
#include "terminal-anim.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// variables for this file
volatile int user_x = 1;
volatile int user_y = 1;
static unsigned int screen_max_rows = 0, screen_max_cols = 0;
static char input_display = ' ';
static tau_style rainbow = {0};

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
  bg_style.fg.r = 0;
  bg_style.fg.g = 0;
  bg_style.fg.b = 0;
  tau_fill(ctx, ' ', bg_style);

  tau_style red = {0};
  red.fg.r = 255;
  red.has_fg = true;
  tau_style green = {0};
  green.fg.g = 255;
  green.has_fg = true;
  tau_style blue = {0};
  blue.fg.b = 255;
  blue.has_fg = true;
  tau_style gray = {0};
  gray.fg.r = 120;
  gray.fg.g = 120;
  gray.fg.b = 120;
  gray.has_fg = true;
  tau_style white = {0};
  white.fg.r = 255;
  white.fg.g = 255;
  white.fg.b = 255;
  white.has_fg = true;

  if (rainbow.fg.r < 255) {
    rainbow.fg.r++;
  } else {
    if (rainbow.fg.g < 255) {
      rainbow.fg.g++;
    } else {
      if (rainbow.fg.b < 255) {
        rainbow.fg.b++;
      } else {
        rainbow.fg.r = 0;
        rainbow.fg.g = 0;
        rainbow.fg.b = 0;
      }
    }
  }

  unsigned int nb_rows = 0, nb_cols = 0;
  tau_get_terminal_info(ctx, &nb_rows, &nb_cols);

  tau_put_hline(ctx, 0, 0, nb_cols - 1, TAU_BD_HEAVY_HORIZONTAL, gray);
  tau_put_hline(ctx, nb_rows - 1, 0, nb_cols - 1, TAU_BD_HEAVY_HORIZONTAL,
                gray);
  tau_put_vline(ctx, 0, 0, nb_rows - 1, TAU_BD_HEAVY_VERTICAL, gray);
  tau_put_vline(ctx, nb_cols - 1, 0, nb_rows - 1, TAU_BD_HEAVY_VERTICAL, gray);

  tau_put_char(ctx, TAU_BD_HEAVY_DOWN_AND_RIGHT, 0, 0, gray);
  tau_put_char(ctx, TAU_BD_HEAVY_DOWN_AND_LEFT, nb_cols - 1, 0, gray);
  tau_put_char(ctx, TAU_BD_HEAVY_UP_AND_RIGHT, 0, nb_rows - 1, gray);
  tau_put_char(ctx, TAU_BD_HEAVY_UP_AND_LEFT, nb_cols - 1, nb_rows - 1, gray);

  char buffer[256];
  snprintf(buffer, 256, "ROWS: %d, COLS: %d", nb_rows, nb_cols);
  tau_put_str(ctx, buffer, strlen(buffer), 1, 1, white);

  snprintf(buffer, 256, "READ: %c", input_display);
  tau_put_str(ctx, buffer, strlen(buffer), 1, 2, white);

  snprintf(buffer, 256, "CTRL-C to quit");
  tau_put_str(ctx, buffer, strlen(buffer), 1, nb_rows - 2, red);

  snprintf(buffer, 256, "hjkl to move");
  tau_put_str(ctx, buffer, strlen(buffer), nb_cols - 13, 1, white);
  snprintf(buffer, 256, "maj SHIFT move further");
  tau_put_str(ctx, buffer, strlen(buffer), nb_cols - 23, 2, white);

  // disabled: scene_update(&cursor, 0, 0, screen_max_cols / 3, screen_max_rows
  // / 3);

  tau_put_filled_rectangle(ctx, 10, 20, 5, 5, 'R', green);
  tau_put_filled_rectangle(ctx, nb_cols - 51, nb_rows - 8, 50, 7, 'R', blue);
  tau_put_filled_rectangle(ctx, user_x, user_y, 5, 5, '#', rainbow);
}

int main(int argc, char *argv[]) {
  tau_ctx *ctx = tau_create();
  if (ctx == NULL) {
    printf("Setup failed with to create ctx\n");
    exit(1);
  }

  struct timespec ts;
  ts.tv_nsec = 20000000; // 20ms

  tau_get_terminal_info(ctx, &screen_max_rows, &screen_max_cols);

  rainbow.has_fg = true;
  rainbow.fg.r = 0;
  rainbow.fg.g = 0;
  rainbow.fg.b = 0;

  tau_toggle_input_evt(ctx, true);
  tau_toggle_resize_evt(ctx, true);

  draw_screen(ctx);
  tau_draw_full(ctx);

  tau_event evt = {.type = TAU_EVT_NONE};
  bool size_changed = false;

  while (tau_g_is_running) {
    tau_update_input(ctx);
    tau_update_resize(ctx);

    bool has_event = tau_poll_event(ctx, &evt);
    if (has_event) {
      if (evt.type == TAU_EVT_KEY) {
        char key_pressed = evt.data.key.key;
        handle_user_input(key_pressed);
      }
      if (evt.type == TAU_EVT_RESIZE)
        size_changed = true;
    }

    draw_screen(ctx);
    if (size_changed)
      tau_draw_full(ctx);
    else
      tau_draw_diff(ctx);

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
      // retry with remaining time
    }
  }

  tau_destroy(ctx);
  return 0;
}
