#include "terminal-anim.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

tau_ctx *ctx;
tau_style bg_style = {0};
tau_style fg_red = {0};
tau_style fg_style = {0};
tau_style fg_light_style = {0};
tau_style text_style = {0};
tau_style hv_lines_style = {0};

int main(int argc, char *argv[]) {
  printf("Hello world from test box\n");
  ctx = tau_create();
  if (ctx == NULL) {
    printf("Test ERROR! - ctx is NULL\n");
    return 1;
  }

  bg_style.fg.b = 255;
  bg_style.fg.g = 255;
  bg_style.fg.r = 255;
  bg_style.has_fg = true;

  fg_style.fg.r = 255;
  fg_style.has_fg = true;

  fg_red.fg.r = 136;
  fg_red.fg.g = 8;
  fg_red.fg.b = 68;
  fg_red.has_fg = true;

  fg_light_style.fg.b = 255;
  fg_light_style.has_fg = true;

  text_style.fg.r = 255;
  text_style.fg.g = 255;
  text_style.fg.b = 100;
  text_style.has_fg = true;

  hv_lines_style.fg.r = 200;
  hv_lines_style.fg.b = 70;
  hv_lines_style.fg.g = 70;
  hv_lines_style.has_fg = true;

  tau_draw_full(ctx);

  char *text = "box test";

  int x1 = 4;
  int x2 = 50;
  int y1 = 3;
  int y2 = 12;

  tau_box box1 = {.x = 0,
                  .y = 2,
                  .width = 10,
                  .height = 5,
                  .box_style = TAU_BOX_LIGHT,
                  .style = fg_red};
  tau_box box2 = {.x = 9,
                  .y = 2,
                  .width = 10,
                  .height = 5,
                  .box_style = TAU_BOX_HEAVY,
                  .style = fg_light_style};
  tau_box box3 = {.x = 20,
                  .y = 0,
                  .width = 10,
                  .height = 10,
                  .box_style = TAU_BOX_DOUBLE,
                  .style = fg_light_style};

  tau_box box_list[3] = {box1, box2, box3};

  tau_toggle_input_evt(ctx, true);

  int counter = 0;
  int evt_counter = 0;
  int mouse_press_counter = 0;
  int mouse_press_x = 0, mouse_press_y = 0;
  char text_buffer[100];
  tau_event evt = {0};
  while (tau_g_is_running) {
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 20000000; // 20ms
    struct timespec rem = {0};
    counter++;

    tau_update_input(ctx);
    tau_update_resize(ctx);

    bool has_event = tau_poll_event(ctx, &evt);
    if (has_event) {
      evt_counter++;
      if (evt.type == TAU_EVT_MOUSE_PRESS) {
        mouse_press_counter++;
        mouse_press_x = evt.data.mouse.x;
        mouse_press_y = evt.data.mouse.y;
      }
    }

    box_list[0].x = mouse_press_x;
    box_list[0].y = mouse_press_y;

    tau_clear(ctx);

    tau_put_str(ctx, text, strlen(text), 2, 1, text_style);
    tau_put_box_grid(ctx, box_list, 3);

    snprintf(text_buffer, sizeof(text_buffer), "[%d] any event counter : %d",
             counter, evt_counter);
    tau_put_str(ctx, text_buffer, 50, 100, 3, fg_style);

    snprintf(text_buffer, sizeof(text_buffer),
             "Mouse Event PRESS      : %d (%3d, %3d)", mouse_press_counter,
             mouse_press_x, mouse_press_y);
    tau_put_str(ctx, text_buffer, 50, 100, 6, fg_style);

    tau_put_box(ctx, 10, 10 + 20, 10, 5, TAU_BOX_DOUBLE, fg_light_style);
    tau_put_box(ctx, 21, 10 + 20, 10, 5, TAU_BOX_LIGHT, fg_style);
    tau_put_box(ctx, 10, 12 + 20, 10, 5, TAU_BOX_HEAVY, hv_lines_style);
    tau_draw_diff(ctx);
    while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
      // retry with remaining time
      req = rem;
    }
  }

  tau_destroy(ctx);
  ctx = NULL;
  if (ctx != NULL) {
    printf("Test ERROR! - ctx is not NULL after destroy\n");
    return 2;
  }

  printf("Test Finished!\n");
  return 0;
}
