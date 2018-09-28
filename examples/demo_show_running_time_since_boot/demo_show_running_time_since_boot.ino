#include "sti_gnss_lib.h"

/*
  NOTE: To initialize the UART console port with baud rate 115,200
*/
void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
  NOTE: "task_called_after_GNSS_update()" will be called about every 1 second in
        case of update rate is 1Hz, so we display the running time here.
*/
void task_called_after_GNSS_update(void)
{
  char buf[64];
  unsigned long bootTimeInMilliSecond;
  unsigned long bootTimeHour;
  unsigned long bootTimeMin;
  unsigned long bootTimeSecond;

  // call built-in function to get the time since booting
  bootTimeInMilliSecond = millis();
  // process the time and format it to UART
  bootTimeSecond = bootTimeInMilliSecond / 1000;
  bootTimeInMilliSecond = bootTimeInMilliSecond % 1000;
  bootTimeMin = bootTimeSecond / 60;
  bootTimeSecond = bootTimeSecond % 60;
  bootTimeHour = bootTimeMin / 60;
  bootTimeMin = bootTimeMin % 60;

  sprintf(buf, "\r\n%02d(hours):%02d(mins):%02d.%03d(secs) after booting ...\r\n",
    bootTimeHour, bootTimeMin, bootTimeSecond, bootTimeInMilliSecond);
  Serial.print(buf);
}
