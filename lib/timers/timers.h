#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>

void timer_init_0(void);
bool timer_overflow_event(void);
void timer_clear_overflow_flag(void);

#endif