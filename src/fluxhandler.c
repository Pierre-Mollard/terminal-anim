#include "escape-sequences.h"
#include <stdio.h>

int reset_screen() {
  printf("%s", CLEAR_SCREEN_ANSI);
  return 0;
}
