#ifndef BUTTON_INPUT_H
#define BUTTON_INPUT_H

#include <stdint.h>

#define NUM_BUTTONS 9

typedef enum {ButtonNoop = 0, ButtonShortPress = 1, ButtonLongPress = 2} ButtonGesture;

typedef struct {
  uint16_t last_time[NUM_BUTTONS];
  uint16_t last_state;
  uint16_t made_longpress;
  ButtonGesture unacked_events[NUM_BUTTONS];
} ButtonInputState;

typedef struct {
  ButtonGesture gesture;
  uint8_t button_number;
} ButtonEvent;

void button_input_init(ButtonInputState *state);
char button_input_get_event(ButtonEvent *event);

void button_input_on_clock_tick_update();
void button_input_pcint0_isr();
void button_input_pcint2_isr();

#endif
