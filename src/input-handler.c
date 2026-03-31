#include "escape-sequences.h"
#include "terminal-anim.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

static int parse_sgr_mouse(const char *buffer, size_t len,
                           tau_event *out_event) {

  if (!buffer || len < 6 || !out_event)
    return 0;

  if (buffer[0] != ESC_CHAR || buffer[1] != ESC_SEP_CHAR || buffer[2] != '<')
    return 0;

  char tmp[64];
  if (len >= sizeof(tmp))
    return 0;

  memcpy(tmp, buffer, len);
  tmp[len] = '\0';

  int pb = 0, px = 0, py = 0;
  char final = 0;

  if (sscanf(tmp + 3, "%d;%d;%d%c", &pb, &px, &py, &final) != 4)
    return 0;

  if (final != 'M' && final != 'm')
    return 0;
  if (px <= 0 || py <= 0)
    return 0;

  int base = pb & 3;
  int is_motion = (pb & 32) != 0;
  int is_wheel = (pb & 64) != 0;

  out_event->data.mouse.button = base;
  out_event->data.mouse.x = px - 1;
  out_event->data.mouse.y = py - 1;
  out_event->data.mouse.shift = (pb & 4) != 0;
  out_event->data.mouse.alt = (pb & 8) != 0;
  out_event->data.mouse.ctrl = (pb & 16) != 0;
  out_event->data.mouse.motion = is_motion;
  out_event->data.mouse.wheel = is_wheel;
  out_event->data.mouse.release = (final == 'm');

  if (is_wheel) {
    if (base == 0)
      out_event->type = TAU_EVT_MOUSE_WHEEL_UP;
    else if (base == 1)
      out_event->type = TAU_EVT_MOUSE_WHEEL_DOWN;
    else
      return 0;
  } else if (final == 'm') {
    out_event->type = TAU_EVT_MOUSE_RELEASE;
  } else if (is_motion) {
    out_event->type = TAU_EVT_MOUSE_MOVE;
  } else {
    out_event->type = TAU_EVT_MOUSE_PRESS;
  }

  return 1;
}

void tau_update_input(tau_ctx *ctx) {
  char read_buffer[64];
  tau_event read_event = {0};
  int len = read(stdin, read_buffer, sizeof(read_buffer));
  if (len > 0) {
    int parse_success = parse_sgr_mouse(read_buffer, len, &read_event);
    if (parse_success) {
      // actually read a mouse event
      // TODO: add holding ring buffer
    }
  }
}
