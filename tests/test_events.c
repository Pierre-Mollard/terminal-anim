#include "terminal-anim.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

char text_buffer[50];

int main(int argc, char *argv[]) {
  printf("Hello world from test event\n");
  tau_ctx *ctx;
  ctx = tau_create();
  if (ctx == NULL) {
    printf("Test ERROR! - ctx is NULL\n");
    return 1;
  }

  tau_style bg_style = {0};
  bg_style.fg.b = 255;
  bg_style.fg.g = 255;
  bg_style.has_fg = true;

  tau_style fg_style = {0};
  fg_style.fg.r = 255;
  fg_style.has_fg = true;

  tau_style text_style = {0};
  text_style.fg.r = 255;
  text_style.fg.g = 255;
  text_style.fg.b = 100;
  text_style.has_fg = true;

  tau_style text2_style = text_style;
  text2_style.attrs |= TAU_ATTR_BOLD;
  text2_style.attrs |= TAU_ATTR_DOUBLE_UNDERLINE;

  tau_style text3_style = text_style;
  text3_style.attrs = TAU_ATTR_ITALIC;

  struct timespec ts;
  ts.tv_nsec = 20000000; // 20ms

  tau_fill(ctx, '_', bg_style);
  tau_put_filled_rectangle(ctx, 10, 20, 10, 10, 'R', fg_style);
  char *text = "this is a event test";
  tau_put_str(ctx, text, strlen(text), 1, 1, text_style);
  tau_draw_diff(ctx);

  tau_event evt = {.type = TAU_EVT_NONE};
  int counter = 0;
  int evt_counter = 0;
  int mouse_move_counter = 0;
  int mouse_press_counter = 0;
  int mouse_release_counter = 0;
  int mouse_wheel_up_counter = 0;
  int mouse_wheel_down_counter = 0;
  int evt_resize_counter = 0;
  int evt_key_counter = 0;

  while (tau_g_is_running) {
    counter++;

    tau_update_input(ctx);

    bool has_event = tau_poll_event(ctx, &evt);
    if (has_event) {
      evt_counter++;
      if (evt.type == TAU_EVT_MOUSE_MOVE)
        mouse_move_counter++;
      if (evt.type == TAU_EVT_MOUSE_PRESS)
        mouse_press_counter++;
      if (evt.type == TAU_EVT_MOUSE_RELEASE)
        mouse_release_counter++;
      if (evt.type == TAU_EVT_MOUSE_WHEEL_UP)
        mouse_wheel_up_counter++;
      if (evt.type == TAU_EVT_MOUSE_WHEEL_DOWN)
        mouse_wheel_down_counter++;

      if (evt.type == TAU_EVT_RESIZE)
        evt_resize_counter++;
      if (evt.type == TAU_EVT_KEY)
        evt_key_counter++;
    }

    snprintf(text_buffer, sizeof(text_buffer), "[%d] any event counter : %d",
             counter, evt_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 3, fg_style);

    snprintf(text_buffer, sizeof(text_buffer), "Mouse Event MOVE       : %d",
             mouse_move_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 5, fg_style);
    snprintf(text_buffer, sizeof(text_buffer), "Mouse Event PRESS      : %d",
             mouse_press_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 6, fg_style);
    snprintf(text_buffer, sizeof(text_buffer), "Mouse Event RELEASE    : %d",
             mouse_release_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 7, fg_style);
    snprintf(text_buffer, sizeof(text_buffer), "Mouse Event MOUSE UP   : %d",
             mouse_wheel_up_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 8, fg_style);
    snprintf(text_buffer, sizeof(text_buffer), "Mouse Event MOUSE DOWN : %d",
             mouse_wheel_down_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 9, fg_style);

    snprintf(text_buffer, sizeof(text_buffer), "Mouse Event RESIZE     : %d",
             evt_resize_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 11, fg_style);
    snprintf(text_buffer, sizeof(text_buffer), "Mouse Event KEY        : %d",
             evt_key_counter);
    tau_put_str(ctx, text_buffer, 50, 1, 12, fg_style);

    tau_draw_diff(ctx);
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
      // retry with remaining time
    }
  }

  sleep(5);
  tau_destroy(ctx);
  ctx = NULL;
  if (ctx != NULL) {
    printf("Test ERROR! - ctx is not NULL after destroy\n");
    return 2;
  }

  printf("Test Finished!\n");
  return 0;
}
