#include <stdio.h>

const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";

int reset_screen() {
  printf("%s", CLEAR_SCREEN_ANSI);
  return 0;
}
