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
  text2_style.attrs |= TAU_ATTR_DOUBLE_UNDERLINE;

  tau_style text3_style = text_style;
  text3_style.attrs = TAU_ATTR_ITALIC;

  tau_fill(ctx, '_', bg_style);
  tau_present(ctx);
  sleep(1);
  tau_put_square(ctx, 10, 10, 10, 10, fg_style);
  char *text = "this is a test";
  tau_put_str(ctx, text, strlen(text), 4, 4, text_style);
  tau_put_str(ctx, text, strlen(text), -2, 2, text2_style);
  tau_put_str(ctx, text, strlen(text), 180, 6, text3_style);
  tau_present(ctx);
  sleep(1);

  tau_destroy(ctx);
  ctx = NULL;
  if (ctx != NULL) {
    printf("Test ERROR! - ctx is not NULL after destroy\n");
    return 2;
  }

  printf("Test Finished!\n");
  return 0;
}
