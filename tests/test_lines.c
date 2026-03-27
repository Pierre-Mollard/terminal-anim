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

void test_line(int x1, int y1, int x2, int y2) {
  if (ctx == NULL) {
    printf("Wait, error, dont forget to init lib first, var are global in test "
           "for test_line()\n");
    return;
  }

  tau_put_line(ctx, x1, y1, x2, y2, '#', fg_light_style);
  tau_put_line_aspect(ctx, x1, y1, x2, y2, '#', fg_style);
  tau_put_char(ctx, 'O', x1, y1, text_style);
  tau_put_char(ctx, 'O', x2, y2, text_style);
}

int main(int argc, char *argv[]) {
  printf("Hello world from test clear\n");
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

  char *text = "line test";
  tau_put_str(ctx, text, strlen(text), 2, 1, text_style);

  int x1 = 4;
  int x2 = 50;
  int y1 = 3;
  int y2 = 12;

  test_line(x1, y1, x2, y2);
  test_line(x1, y1 * 3, x2, y2 * 3);
  test_line(30, y1 * 3, 2, y2 * 3);

  char *texthv = "line test hv optimized";
  tau_put_str(ctx, text, strlen(text), 60, 1, text_style);
  tau_put_hline(ctx, 3, 62, 70, '-', hv_lines_style);
  tau_put_hline(ctx, 10, 100, 80, '-', hv_lines_style);
  tau_put_hline(ctx, 20, 70, 205, '-', hv_lines_style);
  tau_put_vline(ctx, 81, 5, 15, '|', hv_lines_style);
  tau_put_vline(ctx, 70, 25, 10, '|', hv_lines_style);
  tau_put_vline(ctx, 90, 20, 85, '|', hv_lines_style);

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
