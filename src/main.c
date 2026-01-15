
// #include "terminal-anim.h"
#include "platform.h"
#include "terminal-anim.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// variables for this file
static int has_init = 0;
static struct termios termios_conf_init;

void restore_terminal() {
  int rc = 0;

  if (!has_init)
    rc = 1;

  if (rc == 0) {
    rc = tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_conf_init);
  }

  printf("Out of RESTORE TERMINAL (RC=%d)\n", rc);
}

static void signal_handler(int sig) {
  restore_terminal();
  exit(0);
}

int setup_terminal() {
  struct termios termios_conf_new;
  int rc = 0;

  if (has_init)
    rc = 1;

  if (!isatty(STDIN_FILENO)) {
    rc = 2;
  }

  if (rc == 0) {
    atexit(restore_terminal);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
  }

  if (rc == 0) {
    rc = tcgetattr(STDIN_FILENO, &termios_conf_init);
  }
  rc = tcgetattr(STDIN_FILENO, &termios_conf_init);

  if (rc == 0) {

    // diable flow control with C-S/C-Q
    termios_conf_new.c_iflag &= ~ICRNL;

    // disable print user
    // wait for enter input (byte by byte instead)
    // Disable C-V (paste)
    termios_conf_new.c_lflag &= ~(ECHO | ICANON | IEXTEN);
  }

  if (rc == 0) {
    rc = tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_conf_new);
  }

  if (rc == 0) {
    has_init = 1;
  }

  printf("Out of SETUP TERMINAL (RC=%d)", rc);
  return rc;
}

int main(int argc, char *argv[]) {
  setup_terminal();
  reset_screen();
  printf("Hello world from MAIN\n");

  int width;
  int height;
  pf_get_size(&width, &height);
  printf("Width: %d, Height: %d\n", width, height);

  restore_terminal();
  return 0;
}
