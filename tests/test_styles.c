#include "terminal-anim.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Hello world from test styles\n");

  tau_ctx *ctx;
  ctx = tau_create();
  if (ctx == NULL) {
    printf("Test ERROR! - ctx is NULL\n");
    return 1;
  }

  uint32_t all_style_types[] = {TAU_ATTR_NONE,
                                TAU_ATTR_BOLD,
                                TAU_ATTR_FAINT,
                                TAU_ATTR_ITALIC,
                                TAU_ATTR_UNDERLINE,
                                TAU_ATTR_BLINK_S,
                                TAU_ATTR_BLINK_F,
                                TAU_ATTR_REVERSE,
                                TAU_ATTR_CONCEAL,
                                TAU_ATTR_STRIKE,
                                TAU_ATTR_DOUBLE_UNDERLINE,
                                TAU_ATTR_FRAME,
                                TAU_ATTR_ENCIRCLE,
                                TAU_ATTR_OVERLINE};

  tau_style bg_style = {0};
  bg_style.fg.b = 255;
  bg_style.fg.g = 255;
  bg_style.has_fg = true;

  tau_fill(ctx, '.', bg_style);
  tau_draw_init(ctx);

  char *text = "this is a test";
  for (size_t i = 0; i < sizeof(all_style_types) / sizeof(all_style_types[0]);
       i++) {
    uint32_t flag = all_style_types[i];
    tau_style dynamic_style = {0};
    dynamic_style.attrs = flag;
    dynamic_style.fg.r = 150 - 2 * i;
    dynamic_style.fg.g = 150 - i;
    dynamic_style.fg.b = 150 + i;
    dynamic_style.has_fg = true;
    tau_put_str(ctx, text, strlen(text), 4, 4 + i, dynamic_style);
  }

  tau_draw_update(ctx);
  sleep(3);

  tau_destroy(ctx);
  printf("Test Finished!\n");
  return 0;
}
