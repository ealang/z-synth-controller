#include "button_input.h"
#include "time.h"

#include <avr/io.h>
#include <util/atomic.h>
#include <string.h>

#define PORTB_MASK   0x7
#define PORTB_OFFSET 0

#define PORTD_MASK      0b11111100
#define PORTD_OFFSET    2
#define PORTD_BIT_COUNT 6

#define DEBOUNCE_TIME_TICKS  MS_TO_TICKS(25)
#define SHORT_PRESS_TICKS    MS_TO_TICKS(1000)

static ButtonInputState *singleton = 0;

static uint16_t read_button_states() {
    uint8_t bvalues = (PINB & PORTB_MASK) >> PORTB_OFFSET;
    uint8_t dvalues = (PIND & PORTD_MASK) >> PORTD_OFFSET;
    return ~(dvalues | (bvalues << PORTD_BIT_COUNT));
}

void button_input_init(ButtonInputState *state) {
    memset(state, 0, sizeof(ButtonInputState));
    singleton = state;

    DDRB &= ~PORTB_MASK;  // input pins
    PORTB |= PORTB_MASK;   // pull-up resistors

    DDRD &= ~PORTD_MASK;  // input pins
    PORTD |= PORTD_MASK;  // pull-up resistors

    PCICR |= (1 << PCIE0) |  // enable interrupt port B
             (1 << PCIE2);   // enable interrupt port D

    PCMSK0 |= PORTB_MASK;  // enable interrupts on B pins
    PCMSK2 |= PORTD_MASK;  // enable interrupts on D pins
}

char button_input_get_event(ButtonEvent *event) {
    for (uint8_t button_num = 0; button_num < NUM_BUTTONS; ++button_num) {
        ButtonGesture gesture = singleton->unacked_events[button_num];
        if (gesture != ButtonNoop) {
            singleton->unacked_events[button_num] = ButtonNoop;
            event->button_number = button_num;
            event->gesture = gesture;
            return 1;
        }
    }
    return 0;
}

static void process_button_state_change() {
    uint16_t new_state = read_button_states();
    uint16_t change_mask = new_state ^ singleton->last_state;
    uint16_t cur_time = get_time_ticks();

    uint16_t button_mask = 1;
    for (uint8_t button_num = 0; button_num < NUM_BUTTONS; ++button_num) {
        if (change_mask & button_mask) {
            if (new_state & button_mask) {
                // button down
                singleton->last_time[button_num] = cur_time;
                singleton->made_longpress &= ~button_mask;
            } else {
                uint16_t already_handled = singleton->made_longpress & button_mask;
                uint16_t time_delta = cur_time - singleton->last_time[button_num];
                if (!already_handled && time_delta > DEBOUNCE_TIME_TICKS) {
                    if (time_delta <= SHORT_PRESS_TICKS) {
                        singleton->unacked_events[button_num] = ButtonShortPress;
                    } else {
                        singleton->unacked_events[button_num] = ButtonLongPress;
                    }
                }
            }
        }
        button_mask <<= 1;
    }

    singleton->last_state = new_state;
}

void button_input_timer_isr() {
    uint16_t cur_time = get_time_ticks();
    uint16_t button_states = singleton->last_state;

    // truncate long presses
    uint16_t button_mask = 1;
    for (uint8_t button_num = 0; button_num < NUM_BUTTONS; ++button_num) {
        if (button_states & button_mask) {
            uint16_t already_handled = singleton->made_longpress & button_mask;
            uint16_t time_delta = cur_time - singleton->last_time[button_num];
            if (!already_handled && time_delta > SHORT_PRESS_TICKS) {
                singleton->made_longpress |= button_mask;
                singleton->unacked_events[button_num] = ButtonLongPress;
            }
        }
        button_mask <<= 1;
    }
}

void button_input_pcint0_isr() {
    process_button_state_change();
}

void button_input_pcint2_isr() {
    process_button_state_change();
}

