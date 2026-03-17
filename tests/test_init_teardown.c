#include "terminal-anim.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Hello world from test\n");
  tau_ctx *ctx;
  ctx = tau_create();
  if (ctx == NULL) {
    printf("Test ERROR! - ctx is NULL\n");
    return 1;
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
