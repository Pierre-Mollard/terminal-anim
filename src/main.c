#include "escape-sequences.h"
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
volatile sig_atomic_t running = 1;

void restore_terminal() {
  int rc = 0;

  if (!has_init)
    rc = 1;

  if (rc == 0) {
    rc = tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_conf_init);
  }

  printf("Out of RESTORE TERMINAL (RC=%d)\n", rc);
}

static void handler_sigs_end(int sig) {
  (void)sig; // unused
  running = 0;
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
    rc = tcgetattr(STDIN_FILENO, &termios_conf_init);
  }

  if (rc == 0) {
    // copy existing
    termios_conf_new = termios_conf_init;

    // diable flow control with C-S/C-Q
    termios_conf_new.c_iflag &= ~ICRNL;

    // disable print user
    // wait for enter input (byte by byte instead)
    // Disable C-V (paste)
    termios_conf_new.c_lflag &= ~(ECHO | ICANON | IEXTEN);

    termios_conf_new.c_lflag |= ISIG;
  }

  if (rc == 0) {
    rc = tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_conf_new);
  }

  if (rc == 0) {
    atexit(restore_terminal);
    struct sigaction sa_int;
    sa_int.sa_handler = handler_sigs_end;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGTERM, &sa_int, NULL);
    sigaction(SIGINT, &sa_int, NULL);
  }

  if (rc == 0) {
    has_init = 1;
  }

  printf("Out of SETUP TERMINAL (RC=%d)", rc);
  return rc;
}

void resize_screen_callback(int new_width, int new_height) {
  reset_screen();
  term_color(255, 0, 0);
  printf("New Width: %d, New Height: %d\n", new_width, new_height);
  term_color(255, 255, 255);
}

int main(int argc, char *argv[]) {
  int rc = setup_terminal();
  if (rc != 0) {
    printf("Setup failed with rc=%d\n", rc);
    exit(1);
  }
  reset_screen();
  printf("Hello world from MAIN\n");

  int width;
  int height;
  pf_get_size(&width, &height);
  printf("Width: %d, Height: %d\n", width, height);

  int maxloopcounter = 0;
  pf_register_size_change_cb((pf_size_change_cb)&resize_screen_callback);
  while (running) {
    pf_poll_events();
    sleep(1); // interrupted by SIGWINCH handler
    maxloopcounter++;
    if (maxloopcounter > 10)
      running = 0;
  }

  restore_terminal();
  return 0;
}
