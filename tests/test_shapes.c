#include "terminal-anim.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Hello world from test clear\n");
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

  tau_style text3_style = text_style;
  text3_style.attrs = TAU_ATTR_ITALIC;

  tau_fill(ctx, '.', bg_style);
  tau_draw_full(ctx);

  tau_put_square(ctx, 50, 10, 10, 10, fg_style);

  tau_put_char(ctx, 'O', 10, 10, text_style);
  tau_put_char(ctx, 'O', 11, 11, text_style);
  tau_put_char(ctx, 'O', 9, 9, text_style);
  tau_put_char(ctx, 'O', 9, 11, text_style);

  char *text = "shape test";
  tau_put_str(ctx, text, strlen(text), 4, 4, text2_style);

  tau_put_char(ctx, 'X', 90, 30, text_style);
  tau_put_circle(ctx, 90, 30, 4, fg_style);
  tau_put_char(ctx, 'X', 90, 10, text_style);
  tau_put_circle(ctx, 90, 10, 9, fg_style);
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
