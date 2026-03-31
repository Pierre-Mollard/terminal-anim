#include "escape-sequences.h"
#include "terminal-anim.h"
#include <stddef.h>
#include <stdio.h>

static int parse_sgr_mouse(const char *buffer, size_t len,
                           tau_event *out_event) {

  if (len < 6 || !out_event)
    return 0;

  if (buffer[0] != ESC || buffer[1] != ESC_SEP || buffer[2] != '<')
    return 0;

  int b, x, y = 0;
  char last = 0;

  if (sscanf(buffer + 3, "%d;%d;%d%c", &b, &x, &y, &last) != 4)
    return 0;

  if (last != 'M' && last != 'm')
    return 0;

  out_event->type = TAU_EVT_MOUSE_DOWN;
  out_event->data.mouse.x = x - 1;
  out_event->data.mouse.y = y - 1;

  return 1;
}
