/*
  Timer.cpp - C++ file of library for TIMER
  Copyright (c) 2014 NavSpark.  All right reserved.

	This library is free software; you can redistribute it under the terms
  of the GNU Lesser General Public License as published by the Free Software
  Foundation; either version 2.1 of the License, or (at your option) any
  later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.

	Created 24 Feb. 2014 by Ming-Jen Chen

	$Id$
*/

#include "stdint.h"
#include "Arduino.h"
#include "Timer.h"

// **********************************************************************
// Description: declaration of functions provided in "lib_io.a"
// **********************************************************************
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

// **********************************************************************
// Description: declaration of functions exported in C naming convention
// **********************************************************************
#ifdef __cplusplus
extern "C" {
#endif
void isrTimerFunc(void);
#ifdef __cplusplus
}
#endif

// **********************************************************************
// Description: declaration of callback functions
// **********************************************************************


// **********************************************************************
// Description: Instances of TIMER objects
// **********************************************************************
TIMER Timer0(0);
TIMER Timer1(1);
TIMER Timer2(2);

// **********************************************************************
// Description:
// **********************************************************************
void isrTimerFunc(void)
{
	// check & process Timer0
	if (Timer0.expire()) {
		gnss_timer_clear_interrupt(0);
		Timer0.isr();
	}
	// check & process Timer1
	if (Timer1.expire()) {
		gnss_timer_clear_interrupt(1);
		Timer1.isr();
	}
	// check & process Timer0
	if (Timer2.expire()) {
		gnss_timer_clear_interrupt(2);
		Timer2.isr();
	}
}

// **********************************************************************
// Description: Constructor for class "TIMER"
// **********************************************************************
TIMER::TIMER(void)
{
	timerId = 0;
	count = 0;
	enabled = true;
	userFuncForTimer = NULL;
}

TIMER::TIMER(uint8_t tmrId)
{
	if (tmrId <= 2) {
		timerId = tmrId;
		count = 0;
		enabled = true;
		userFuncForTimer = NULL;
	}
	else enabled = false;
}

// **********************************************************************
// Description:
// **********************************************************************
bool TIMER::isIdle(void)
{
	if (count == 0) return true;
	return false;
}

// **********************************************************************
// Description:
// **********************************************************************
void TIMER::isr(void)
{
	if (count == 0) { stop(); }
	else if (count != 0xffff) {
		if (count) { count--; }
	}

	if (userFuncForTimer != NULL) {
		userFuncForTimer();
	}
}

// **********************************************************************
// Description:
// **********************************************************************
uint8_t TIMER::every(uint32_t period, void (*callback)(void))
{
	return everyNano(period * 1000000, callback, 0xffff); // 0xffff means forever
}

uint8_t TIMER::every(uint32_t period, void (*callback)(void), uint16_t repeatCount)
{
	return everyNano(period * 1000000, callback, repeatCount);
}

uint8_t TIMER::everyMicro(uint32_t period, void (*callback)(void))
{
	return everyNano(period * 1000, callback, 0xffff); // 0xffff means forever
}

uint8_t TIMER::everyMicro(uint32_t period, void (*callback)(void), uint16_t repeatCount)
{
	return everyNano(period * 1000, callback, repeatCount);
}

uint8_t TIMER::everyNano(uint32_t period, void (*callback)(void))
{
	return everyNano(period, callback, 0xffff); // 0xffff means forever
}

uint8_t TIMER::everyNano(uint32_t period, void (*callback)(void), uint16_t repeatCount)
{
	uint32_t prescaler = gnss_get_prescaler(); // in nano-second

	// return in case of not legal timer
	if (enabled == false) return 0;

	// setup the down counter
	count = repeatCount;

	// register callback function
	userFuncForTimer = callback;

	// note period is in unit of nano-second
	if (prescaler <= period) {
		gnss_timer_init(timerId, (period/prescaler), 1, 1);
	}
	else  {
		return 0;
	}

  // enable timer specified
	gnss_timer_enable(timerId);

  return 1;
}

uint8_t TIMER::after(uint32_t period, void (*callback)(void))
{
	return every(period, callback, 1);
}

bool TIMER::expire(void)
{
	if (gnss_timer_interrupt_pending_status(timerId) == 1) {
		return true;
	}
	return false;
}

void TIMER::stop(void)
{
	gnss_timer_disable(timerId);
	userFuncForTimer = NULL;
	count=0;
}

uint16_t TIMER::remainTimes(void)
{
	return count;
}
