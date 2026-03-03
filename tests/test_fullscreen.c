#include "terminal-anim.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUFFER_WIDTH 40000
static unsigned int max_rows = 0;
static unsigned int max_cols = 0;

void pf_get_size(unsigned int *rows, unsigned int *cols) {
  struct winsize ws;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == 0) {
    *rows = ws.ws_row;
    *cols = ws.ws_col;
  } else {
    *rows = 24;
    *cols = 80;
  }
}

void fill_buffer(char *buffer, int nbrows, int nbcols, char content) {
  for (int y = 0; y < nbrows; y++) {
    memset(buffer + y * (nbcols + 1), content, nbcols);
    buffer[y * (nbcols + 1) + nbcols] = '\n';
  }
  buffer[nbrows * (nbcols + 1)] = '\0';
}

int main(int argc, char *argv[]) {
  pf_get_size(&max_rows, &max_cols);
  printf("FULLSCREEN TEST ROWS:%d/COLS:%d\n", max_rows, max_cols);
  printf("Show template mode (1) or test lib (2) ?\n");
  printf("Type then enter...\n");

  int template_mode = 0;
  char user_input = '0';
  while (user_input != '1' && user_input != '2') {
    fscanf(stdin, "%c", &user_input);
  }

  char screen_buffer[MAX_BUFFER_WIDTH];
  char *cursor = screen_buffer;
  memset(screen_buffer, '\0', sizeof(screen_buffer));

  clear_screen(&cursor);
  printf("%s", screen_buffer);

  struct timespec ts;
  ts.tv_nsec = 20000000; // 20ms

  // TODO: see why the template version lags already (not even using custom lib)
  if (user_input == '1') {
    for (int i = 0; i < 1000; i++) {
      char used_char = '#';
      if (i % 2 == 0) {
        used_char = '@';
      }
      clear_screen(&cursor);
      printf("%s", screen_buffer);
      memset(screen_buffer, '\0', sizeof(screen_buffer));
      fill_buffer(screen_buffer, max_rows, max_cols, used_char);
      printf("%s", screen_buffer);
      while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        // retry with remaining time
      }
    }
  }

  return 0;
}
