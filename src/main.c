#include "escape-sequences.h"
#include "platform.h"
#include "terminal-anim.h"
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// variables for this file
static int has_init = 0;
static struct termios termios_conf_init;
volatile sig_atomic_t running = 1;
volatile int user_x = 0;
volatile int user_y = 0;
static unsigned int screen_max_rows, screen_max_cols = 0;
static char input_display = ' ';

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

void resize_screen_callback(int new_rows, int new_cols) {
  screen_max_rows = new_rows;
  screen_max_cols = new_cols;

  if (user_x > screen_max_cols)
    user_x = screen_max_cols;
  if (user_y > screen_max_rows)
    user_y = screen_max_rows;
}

void handle_user_input(char c) {
  if (c == 'j' && user_y < screen_max_rows)
    user_y++;
  if (c == 'k' && user_y > 0)
    user_y--;
  if (c == 'l' && user_x < screen_max_cols)
    user_x++;
  if (c == 'h' && user_x > 0)
    user_x--;
  input_display = c;
}

void draw_screen() {
  reset_screen();
  term_color(255, 0, 0);
  term_color(0, 255, 0);
  draw_square(10, 20, 5, 5);
  term_color(0, 0, 255);
  draw_square(20, 30, 50, 7);
  term_color(120, 120, 120);
  draw_square(user_x, user_y, 5, 5);

  term_color(255, 255, 255);
  term_move(0, 0);
  term_write_f("ROWS: %d, COLS: %d", screen_max_rows, screen_max_cols);

  term_move(0, 50);
  term_write_f("READ: %c", input_display);
}

int main(int argc, char *argv[]) {
  int rc = setup_terminal();
  if (rc != 0) {
    term_write_f("Setup failed with rc=%d\n", rc);
    exit(1);
  }
  term_write(HIDE_CURSOR);

  pf_get_size(&screen_max_rows, &screen_max_cols);
  pf_register_size_change_cb((pf_size_change_cb)&resize_screen_callback);

  struct pollfd poll_fd[1];
  poll_fd[0].fd = STDIN_FILENO;
  poll_fd[0].events = POLLIN;
  poll_fd[0].revents = 0;

  while (running) {
    pf_poll_events();

    int ret_poll = poll(poll_fd, 1, 20); // ms
    if (ret_poll == -1) {
      if (errno == EINTR)
        continue; // interrupted by SIGWINCH handler
      perror("poll");
      break;
    }
    if (ret_poll > 0) {
      if (poll_fd[0].revents & POLLIN) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
          handle_user_input(c);
        }
      }
    }

    draw_screen();
  }

  term_write(SHOW_CURSOR);
  term_move(0, 0);
  restore_terminal();
  return 0;
}
