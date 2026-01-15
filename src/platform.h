#pragma once
#include <stdbool.h>

typedef void (*pf_size_change_cb)(int new_rows, int new_cols);

void pf_get_size(int *rows, int *cols);

void pf_register_size_change_cb(pf_size_change_cb callback);

void pf_poll_events();
