#ifndef SHIFT_CTRL_H
#define SHIFT_CTRL_H

#include <stdint.h>

void shift_ctrl_init();
void shift_ctrl_clear();

// Shift a single bit
void shift_ctrl_shift(uint8_t bitval);

// Present value on output
void shift_ctrl_display();

#endif
