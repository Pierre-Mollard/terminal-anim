#include "escape-sequences.h"
#include "terminal-anim-internal.h"
#include "terminal-anim.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <stdbool.h>
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

static int parse_input(tau_ctx *ctx, tau_event *out_evt) {
  unsigned char tmp[128];
  size_t n = input_fifo_peek(&ctx->input_state.in_fifo, tmp, sizeof(tmp));

  if (n == 0)
    return 0;

  if (tmp[0] != ESC_CHAR) {
    out_evt->type = TAU_EVT_KEY;
    out_evt->data.key.key = tmp[0];
    input_fifo_discard(&ctx->input_state.in_fifo, 1);
  }

  if (n >= 3 && tmp[0] == 0x1b && tmp[1] == '[' && tmp[2] == '<') {
    for (size_t i = 3; i < n; i++) {
      if (tmp[i] == 'M' || tmp[i] == 'm') {
        if (!parse_sgr_mouse((const char *)tmp, i + 1, out_evt))
          return 0;
        input_fifo_discard(&ctx->input_state.in_fifo, i + 1);
        return 1;
      }
    }
    return 0;
  }

  if (n == 1)
    return 0;

  out_evt->type = TAU_EVT_KEY;
  out_evt->data.key.codepoint = 0x1b;
  input_fifo_discard(&ctx->input_state.in_fifo, 1);
  return 1;
}

static void parse_pending_input(tau_ctx *ctx) {
  tau_event evt;

  while (parse_input(ctx, &evt)) {
    event_fifo_push(&ctx->input_state.evt_fifo, evt);
  }
}

void tau_update_input(tau_ctx *ctx) {
  if (!ctx)
    return;

  unsigned char read_buffer[256];

  // non blocking from the attributes of termios in ctx
  while (true) {
    ssize_t len =
        read(ctx->input_state.stdin_fd, read_buffer, sizeof(read_buffer));
    if (len > 0) {
      for (ssize_t i = 0; i < len; i++) {
        input_fifo_push(&ctx->input_state.in_fifo, read_buffer[i]);
      }
    } else if (len == 0) {
      break;
    } else {
      if (errno == EINTR) {
        continue;
      } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
        break;
      } else {
        break;
      }
    }
  }

  parse_pending_input(ctx);
}

bool tau_poll_event(tau_ctx *ctx, tau_event *evt) {
  if (!ctx || !evt)
    return false;

  return event_fifo_pop(&ctx->input_state.evt_fifo, evt);

  // TODO: keep only one way to handle resize data

  // resize event
  /*
  if (resize_pending == 1) {
    resize_pending = 0;
    evt->type = TAU_EVT_RESIZE;
    evt->data.resize.cols = screen_max_cols;
    evt->data.resize.rows = screen_max_rows;
    return;
  }
  */
}
