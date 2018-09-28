/*
  NOTE: This code configures NavSpark to work as a two-wire slave and receive/transmit
        data from/to remote two-wire master using another NavSpark.
*/

#include "sti_gnss_lib.h"

uint8_t txdata[] = {0x19, 0x68, 0x05, 0x31, 0x19, 0x70, 0x03, 0x12, 0x19, 0x71, 0x07, 0x14};

void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(115200);

  twSlave.begin(0x57); // 0x57 is my device address
  twSlave.reset();

  twSlave.write(txdata, 12);
}

void loop() {
  // put your main code here, to run repeatedly:
  static char buf[64];
  static uint16_t i;
  uint8_t k;

  // print out the data received from master
  k = twSlave.available();
  if (k) {
    for (i = 0; i < k; i++) {
      sprintf(buf, "2-wirev slave recvs 0x%2x\r\n", twSlave.read());
      Serial.print(buf);
    }
  }

  // fill the data to be sent
  if (twSlave.remaining() == 0) {
    twSlave.resetTx();
    twSlave.write(txdata, 12);
  }
}

