#include "terminal-anim.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Hello world from test\n");
  setup_terminal();

  restore_terminal();
  printf("Test Finished!\n");
  return 0;
}
