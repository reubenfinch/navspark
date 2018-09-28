#include "Arduino.h"
/*
  NOTE: This example shows how to setup a timer and associated callback function.
*/
#ifdef __cplusplus
extern "C" {
#endif
extern void isrTimerFunc(void);
#ifdef __cplusplus
}
#endif
#define AVAILABLE_TONE_TIMER 3

volatile long timer0_toggle_count;
volatile long timer1_toggle_count;
volatile long timer2_toggle_count;
volatile uint8_t *timer2_pin_port;
volatile uint8_t timer2_pin_mask;

static int8_t tone_timer_used_pin[AVAILABLE_TONE_TIMER] = { -1,-1,-1};    


static void timer0_stop(void)
{
  
  	if (timer0_toggle_count != 0)
    {
      if(((timer0_toggle_count)%2)==0)
      digitalWrite(tone_timer_used_pin[0], 0);
      else
      digitalWrite(tone_timer_used_pin[0], 1);

      if (timer0_toggle_count > 0)
      timer0_toggle_count--;
    }
    else
    {
    	digitalWrite(tone_timer_used_pin[0], 0);
      Timer0.stop();
      tone_timer_used_pin[0]=-1;
    }

}

static void timer1_stop(void)
{
  
  	if (timer1_toggle_count != 0)
    {
      if(((timer1_toggle_count)%2)==0)
      digitalWrite(tone_timer_used_pin[1], 0);
      else
      digitalWrite(tone_timer_used_pin[1], 1);

      if (timer1_toggle_count > 0)
      timer1_toggle_count--;
    }
    else
    {
    	digitalWrite(tone_timer_used_pin[1], 0);
      Timer1.stop();
      tone_timer_used_pin[1]=-1;
    }

}
static void timer2_stop(void)
{
  	if (timer2_toggle_count != 0)
    {
      if(((timer2_toggle_count)%2)==0)
      digitalWrite(tone_timer_used_pin[2], 0);
      else
      digitalWrite(tone_timer_used_pin[2], 1);

      if (timer2_toggle_count > 0)
      timer2_toggle_count--;
    }
    else
    {
    	digitalWrite(tone_timer_used_pin[2], 0);
      Timer2.stop();
      tone_timer_used_pin[2]=-1;
    }

}


static int8_t toneTimerSearch(uint8_t _pin)//find free timer
{
	//if(tone_timer_used_pin[0]==-1)//timer may be used by other app not tone
	if(Timer0.isIdle()==true)//timer may be used by other app not tone
	{
		tone_timer_used_pin[0]=_pin;
		return 0;
	}
	else
	if(Timer1.isIdle()==true)//timer may be used by other app not tone
	//if(tone_timer_used_pin[1]==-1)
	{
		tone_timer_used_pin[1]=_pin;
		return 1;
	}
	else //timer2
	if(Timer2.isIdle()==true)//timer may be used by other app not tone
	{
		tone_timer_used_pin[2]=_pin;
		return 2;
	}
	return -1;
}

static int8_t toneFindTimer(uint8_t _pin)
{
  int8_t _timer = -1;
  int8_t i;


  for (i = 0; i < AVAILABLE_TONE_TIMER; i++) {
    if (tone_timer_used_pin[i] == _pin) {
      return i;
    }
  }
  
  // search for an unused timer.
  _timer=toneTimerSearch(_pin);
  
  if (_timer == -1)
  {

  }

  return _timer;
}

void noTone(uint8_t _pin)
{
	int8_t i;
  
  
  for (i = 0; i < AVAILABLE_TONE_TIMER; i++) {
    if (tone_timer_used_pin[i] == _pin) {
      break;
    }
  }
  if(i == AVAILABLE_TONE_TIMER)
  return;
  

  tone_timer_used_pin[i]=-1;
  switch (i)
  {
  	case 0:
  	  Timer0.stop();
  	  break;
  	  case 1:
  	  Timer1.stop();
  	  break;
  	  case 2:
  	  Timer2.stop();
  	  break;
  }
  digitalWrite(tone_timer_used_pin[i], 0);
}
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration)
{
	//char buf[100];
	// Set the pinMode as OUTPUT
	unsigned int period;
	long toggle_count = 0;
	int8_t _timer;
	if(frequency==0)
	return;
	
	_timer=toneFindTimer(_pin);
	
	if(_timer==-1)
	{
	  return;
  }
	
	
	
	if (duration > 0)
  {
  	period=(1000000000/frequency);
    pinMode(_pin, OUTPUT);
    toggle_count = 2 * frequency * duration / 1000;
    
    period=period/2;//to nano in timer.cpp

	  switch (_timer)
    {
  	  case 0:
  	  tone_timer_used_pin[0]=_pin;
  	  timer0_toggle_count=toggle_count;
  	  Timer0.everyNano(period, timer0_stop);
  	  break;
  	  case 1:
  	  tone_timer_used_pin[1]=_pin;
  	  timer1_toggle_count=toggle_count;
  	  Timer1.everyNano(period, timer1_stop);
  	  break;
  	  case 2:
  	  tone_timer_used_pin[2]=_pin;
  	  timer2_toggle_count=toggle_count;
  	  Timer2.everyNano(period, timer2_stop);
  	  break;
    }
  }

}
