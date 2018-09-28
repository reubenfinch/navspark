/*
  NOTE: In this demo, the GPIO 3/6 are wired to GPIO 14/10, the program will assert
        the GPIO 3 and 6 to high level every 3 and 7 seconds, it will trig the ISRs
        registered by users and ISR will deassert the GPIO 3 and 6.
*/

#include "wiring_intr.h"
#include "sti_gnss_lib.h"

#define INTR1_OUT_PIN 3
#define INTR1_IN_PIN  14
#define INTR2_OUT_PIN 6
#define INTR2_IN_PIN  10

/*
  NOTE: 1. Set the modes of GPIO pins,
        2. deassert GPIO 3 and 6,
        3. register ISR functions.
*/
void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  pinMode(INTR1_OUT_PIN, OUTPUT);
  pinMode(INTR2_OUT_PIN, OUTPUT);
  pinMode(INTR1_IN_PIN, INPUT);
  pinMode(INTR2_IN_PIN, INPUT);
  digitalWrite(INTR1_OUT_PIN, LOW);
  digitalWrite(INTR2_OUT_PIN, LOW);
  attachInterrupt(INTR1_IN_PIN, gpio_intr1_task, RISING);
  attachInterrupt(INTR2_IN_PIN, gpio_intr2_task, RISING);

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
  NOTE: Task to assert GPIO 3 and 6 repeatedly.
*/
void task_called_after_GNSS_update(void)
{
  static uint16_t cntr = 0;

  Serial.print("task_called_after_GNSS_update() entered!!\r\n");
  cntr ++;
  if ((cntr%3)==0) { digitalWrite(INTR1_OUT_PIN, HIGH); }
  if ((cntr%7)==0) { digitalWrite(INTR2_OUT_PIN, HIGH); }
}

/*
  NOTE: ISRs defined by users
*/
void gpio_intr1_task() {
  digitalWrite(INTR1_OUT_PIN, LOW);
  Serial.print("GPIO ISR-1 called!!\r\n");
}

void gpio_intr2_task() {
  digitalWrite(INTR2_OUT_PIN, LOW);
  Serial.print("GPIO ISR-2 called!!\r\n");
}
