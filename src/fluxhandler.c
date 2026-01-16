#include "escape-sequences.h"
#include "terminal-anim.h"

void reset_screen() { term_write(CLEAR_ALL); }
