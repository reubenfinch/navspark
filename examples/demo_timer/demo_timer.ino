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

void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(115200);

  Serial.print("Go go go !!\r\n");

  Timer0.every(1000, timer0_test, 50); // setup timer 0 for alarm every 1 second with 50 rounds
  Timer1.every(2500, timer1_test, 20); // setup timer 1 for alarm every 2.5 seconds with 20 rounds
//Timer2.every(5000, timer2_test, 10); // setup timer 2 for alarm every 5 seconds with 10 rounds
  Timer2.every(5000, timer2_test); // setup timer 2 for alarm every 5 seconds, never stop
//Timer2.after(5000, timer2_test); // setup timer 2 for alarm every 5 seconds and execute once
}

void timer0_test(void)
{
  char buf[32];
  sprintf(buf, "Timer-0 active, still have %d times!\r\n", Timer0.remainTimes());
  Serial.print(buf);
}

void timer1_test(void)
{
  char buf[32];
  sprintf(buf, "Timer-1 active, still have %d times!\r\n", Timer1.remainTimes());
  Serial.print(buf);
}

void timer2_test(void)
{
  char buf[32];
  sprintf(buf, "Timer-2 active, still have %d times!\r\n", Timer2.remainTimes());
  Serial.print(buf);
}

void loop() {
  // put your main code here, to run repeatedly:

}

