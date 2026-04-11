#include "terminal-anim.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

tau_ctx *ctx;
tau_style bg_style = {0};
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

  tau_fill(ctx, '.', bg_style);
  tau_draw_full(ctx);

  char *text = "box test";
  tau_put_str(ctx, text, strlen(text), 2, 1, text_style);

  int x1 = 4;
  int x2 = 50;
  int y1 = 3;
  int y2 = 12;

  tau_put_box(ctx, 10, 10, 10, 5, TAU_BOX_DOUBLE, fg_light_style);
  tau_put_box(ctx, 21, 10, 10, 5, TAU_BOX_LIGHT, fg_style);
  tau_put_box(ctx, 10, 21, 10, 5, TAU_BOX_HEAVY, hv_lines_style);

  tau_box box1 = {.x = 0,
                  .y = 2,
                  .width = 10,
                  .height = 5,
                  .box_style = TAU_BOX_LIGHT,
                  .style = fg_light_style};
  tau_box box2 = {.x = 10,
                  .y = 2,
                  .width = 10,
                  .height = 5,
                  .box_style = TAU_BOX_HEAVY,
                  .style = fg_light_style};

  tau_box box_list[2] = {box1, box2};

  tau_put_box_grid(ctx, box_list, 2);

  tau_draw_diff(ctx);
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
