#include "platform.h"
#include <termio.h>
#include <unistd.h>

void pf_get_size(int *row, int *cols) {
  struct winsize ws;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == 0) {
    *row = ws.ws_row;
    *cols = ws.ws_col;
  } else {
    *row = 24;
    *cols = 80;
  }
}
