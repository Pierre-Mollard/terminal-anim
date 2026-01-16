#include "escape-sequences.h"

void reset_screen() { term_write(CLEAR_ALL); }
