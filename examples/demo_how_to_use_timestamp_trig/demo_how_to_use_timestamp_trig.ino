#include "sti_gnss_lib.h"
#include "GNSS.h"

/*
  NOTE: To initialize the UART console port with baud rate 19,200
*/

void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(19200);

  GnssInfo.timestamp.setTrigCapture(TS_TRIG_ON, TS_TRIG_RISING, (void*)userFuncforTimeStamp);
}

void loop() {
  char buf[64];

  // for demo purpose, we just print out those timestamps on console
  while (GnssInfo.timestamp.numRecord() != 0) {
    if (GnssInfo.timestamp.pop() == true) {
      GnssInfo.timestamp.convertTimeStampToUTC();
      GnssInfo.timestamp.formatGPSString(buf);
      Serial.print(buf);
      Serial.print("; ");
      GnssInfo.timestamp.formatUTCString(buf);
      Serial.print(buf);
      Serial.print("\r\n");
    }
  }
}

/* NOTE: Since this callback function will occupy the time slot for GNSS interrupt,
*        user should NOT do complex jobs here or otherwise the NavSpark may fail to
*        lock GNSS.
*/
void userFuncforTimeStamp(TIME_STAMPING_STATUS_T ts) {
  if (ts.time_is_valid) {
    if (GnssInfo.timestamp.push(ts) == true) {
      // add necessary code for successful data push here, but note the more
      // code added, the higher possibility to loss GNSS lock
    }
  }
}
