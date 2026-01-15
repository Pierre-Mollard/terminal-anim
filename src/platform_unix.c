#include "platform.h"
#include <signal.h>
#include <termio.h>
#include <unistd.h>

void pf_get_size(int *rows, int *cols) {
  struct winsize ws;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == 0) {
    *rows = ws.ws_row;
    *cols = ws.ws_col;
  } else {
    *rows = 24;
    *cols = 80;
  }
}

static volatile sig_atomic_t resize_pending = 0;

static pf_size_change_cb resize_callback = NULL;

static void handler_winch(int sig) {
  (void)sig; // unused
  resize_pending = 1;
}

void pf_register_size_change_cb(pf_size_change_cb callback) {
  resize_callback = callback;

  struct sigaction sig_action;
  sig_action.sa_handler = handler_winch;
  sig_action.sa_flags = 0;
  sigemptyset(&sig_action.sa_mask);
  sigaction(SIGWINCH, &sig_action, NULL);
}

void pf_poll_events() {
  if (resize_pending) {
    resize_pending = 0;
    if (resize_callback) {
      int r, c;
      pf_get_size(&r, &c);
      resize_callback(r, c);
    }
  }
}
