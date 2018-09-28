/*
  NOTE: This demo uses the UART2 to send out control code and data to an external
        7-segment display to display the time (hh:mm) get from GNSS satellite.
*/

#include "sti_gnss_lib.h"
#include "GNSS.h"

/*
  NOTE:
*/
void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  Serial.begin(9600);
  Serial.write(0x76); // Clear display
}

void loop() {
  // put your main code here, to run repeatedly:

}

void task_called_after_GNSS_update() {
  static uint8_t toggle = 0;
  char hourDigit[4];
  char minDigit[4];

  gnssInfo.update();

  // set cursor to left-most
  Serial.write(0x79);
  Serial.write(0x00);

  if (gnssInfo.time.hour() == 0) {
    Serial.write('0');
    Serial.write('0');
  }
  else {
    // convert GNSS hours to 2 digits
    utoa(gnssInfo.time.hour(), hourDigit, 10);
    if (gnssInfo.time.hour() > 9) {
      Serial.write(hourDigit[0]);
      Serial.write(hourDigit[1]);
    }
    else {
      Serial.write('0');
      Serial.write(hourDigit[0]);
    }
  }

  // toggle the ':'
  toggle = (toggle == 1) ? 0 : 1;
  Serial.write(0x77);
  Serial.write(toggle<<4);

  // convert GNSS minutes to 2 digits
  if (gnssInfo.time.minute() == 0) {
    Serial.write('0');
    Serial.write('0');
  }
  else {
    utoa(gnssInfo.time.minute(), minDigit, 10);
    if (gnssInfo.time.minute() > 9) {
      Serial.write(minDigit[0]);
      Serial.write(minDigit[1]);
    }
    else {
      Serial.write('0');
      Serial.write(minDigit[0]);
    }
  }
}
