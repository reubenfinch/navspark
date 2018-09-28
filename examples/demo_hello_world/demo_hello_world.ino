#include <Arduino.h>
#include "GNSS.h"
#include "sti_gnss_lib.h"
uint8_t led = 13;

void setup() {
  GnssConf.init();
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  Serial.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:

}

void task_called_after_GNSS_update(void)
{
  static uint8_t val = HIGH;
  char buf[32];

  if (val == HIGH) {
    digitalWrite(led, HIGH);
    sprintf(buf, "Hello world (GPIO%02d high)\r\n", led);
    Serial.print(buf);
    val = LOW;
  }
  else if (val == LOW) {
    digitalWrite(led, LOW);
    sprintf(buf, "Hello world (GPIO%02d low)\r\n", led);
    Serial.print(buf);
    val = HIGH;
  }
}
