#include <Arduino.h>
#include "Servo.h"

static servo_t servos[MAX_SERVOS];                          // static array of servo structures
uint8_t ServoCount = 0;                                     // the total number of attached servos

Servo::Servo()
{
  if( ServoCount < MAX_SERVOS) {
    servoIndex = ServoCount++;                    // assign a servo index to this instance
  }
  else  {
    servoIndex = INVALID_SERVO ;  // too many servos
  }
   minPulseWidth = 0;
   maxPulseWidth = 0;     
   tcxoFreq = 0;
   current = 0;
}

uint8_t Servo::attach(int pin)
{
  return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t Servo::attach(int pin, int min, int max)
{
  if(pin != GPIO20_PWM0 || servoIndex == INVALID_SERVO ) {
    return INVALID_SERVO;
  }

  pinMode(pin, OUTPUT) ;                                   // set servo pin to output
  analogPWMFreq(pin, 1000000 / REFRESH_INTERVAL);
  servos[this->servoIndex].Pin.nbr = pin;
  // todo min/max check: abs(min - MIN_PULSE_WIDTH) /4 < 128
  this->minPulseWidth = min;
  this->maxPulseWidth = max;
  servos[this->servoIndex].Pin.isActive = true;  // this must be set after the check for isTimerActive
  tcxoFreq = v822_pwm0_clk_hz();
  //totalPulse = (uint32_t)((double)tcxoFreq * (1000000 / REFRESH_INTERVAL) / 1000);
  totalPulse = (uint32_t)((double)tcxoFreq * REFRESH_INTERVAL / 1000000);

  Serial.print(tcxoFreq);
  Serial.print(" ");
  Serial.println(totalPulse);
  return this->servoIndex ;
}

void Servo::detach()
{
  servos[this->servoIndex].Pin.isActive = false;
  servoIndex= INVALID_SERVO;
  minPulseWidth = 0;
  maxPulseWidth = 0;     
}

void Servo::write(int value)
{
  if(value < 0) value = 0;
  if(value > 180) value = 180;
  Serial.print("write:");
  Serial.println(value);  
  value = map(value, 0, 180, minPulseWidth,  maxPulseWidth);
  this->writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value)
{
  if(this->servoIndex==INVALID_SERVO) return;
  
  if(value < minPulseWidth) value = minPulseWidth;
  if(value > maxPulseWidth) value = maxPulseWidth;
  Serial.print("writeMicroseconds:");
  Serial.println(value);  
  
  current = (uint32_t)((double)tcxoFreq * value / 1000000);
  venus8Pwm0Write(totalPulse - current, current);
}

int Servo::read() // return the value as degrees
{
  return  map(readMicroseconds() + 1, minPulseWidth, maxPulseWidth, 0, 180);
}

int Servo::readMicroseconds()
{
  if(servoIndex==INVALID_SERVO) return 0;
  return (int)((double)current * 1000000 / tcxoFreq + 0.5);
}

bool Servo::attached()
{
  return servos[this->servoIndex].Pin.isActive ;
}













