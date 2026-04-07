#ifndef TERMINAL_ANIM_LIB_H_INTERNAL
#define TERMINAL_ANIM_LIB_H_INTERNAL

#include "terminal-anim.h"
#include <stddef.h>
#include <stdint.h>
#include <termios.h>

#define BYTES_PER_PIXEL 32
#define TERM_ASPECT_RATIO 2.0
#define MAX_INPUT_NB 255
#define MAX_EVENT_NB 255

struct tau_cell {
  uint32_t symbol;
  struct tau_style style;
};

typedef struct {
  unsigned char data[MAX_INPUT_NB];
  size_t head;
  size_t tail;
} tau_input_fifo;

typedef struct {
  tau_event data[MAX_EVENT_NB];
  size_t head;
  size_t tail;
} tau_event_fifo;

typedef struct {
  int stdin_fd;
  int stdin_flags_init;
  tau_input_fifo in_fifo;
  tau_event_fifo evt_fifo;
} tau_input_state;

struct tau_ctx {
  int id;
  struct tau_cell *front_buffer;
  struct tau_cell *back_buffer;
  size_t nb_cols;
  size_t nb_rows;
  size_t nb_cells;
  size_t output_capacity;
  char *output_buffer;
  struct termios termios_conf_init;
  tau_input_state input_state;
};

static inline size_t fifo_next(size_t i, size_t cap) { return (i + 1) % cap; }

static inline int input_fifo_empty(const tau_input_fifo *q) {
  return q->head == q->tail;
}

static inline int input_fifo_full(const tau_input_fifo *q) {
  return fifo_next(q->tail, MAX_INPUT_NB) == q->head;
}

static inline int input_fifo_push(tau_input_fifo *q, unsigned char c) {
  if (input_fifo_full(q))
    return 0;
  q->data[q->tail] = c;
  q->tail = fifo_next(q->tail, MAX_INPUT_NB);
  return 1;
}

static inline int input_fifo_pop(tau_input_fifo *q, unsigned char *out) {
  if (input_fifo_empty(q))
    return 0;
  *out = q->data[q->head];
  q->head = fifo_next(q->head, MAX_INPUT_NB);
  return 1;
}

static inline size_t input_fifo_count(const tau_input_fifo *q) {
  if (q->tail >= q->head)
    return q->tail - q->head;

  return MAX_INPUT_NB - q->head + q->tail;
}

static inline size_t input_fifo_peek(const tau_input_fifo *q,
                                     unsigned char *out, size_t maxn) {
  size_t n = input_fifo_count(q);
  if (n > maxn)
    n = maxn;

  for (size_t i = 0; i < n; i++) {
    out[i] = q->data[(q->head + i) % MAX_INPUT_NB];
  }

  return n;
}

static inline void input_fifo_discard(tau_input_fifo *q, size_t n) {
  size_t avail = input_fifo_count(q);
  if (n > avail)
    n = avail;

  q->head = (q->head + n) % MAX_INPUT_NB;
}

static inline int event_fifo_empty(const tau_event_fifo *q) {
  return q->head == q->tail;
}

static inline int event_fifo_full(const tau_event_fifo *q) {
  return fifo_next(q->tail, MAX_EVENT_NB) == q->head;
}

static inline int event_fifo_push(tau_event_fifo *q, tau_event ev) {
  if (event_fifo_full(q))
    return 0;
  q->data[q->tail] = ev;
  q->tail = fifo_next(q->tail, MAX_EVENT_NB);
  return 1;
}

static inline int event_fifo_pop(tau_event_fifo *q, tau_event *out) {
  if (event_fifo_empty(q))
    return 0;
  *out = q->data[q->head];
  q->head = fifo_next(q->head, MAX_EVENT_NB);
  return 1;
}

#endif
