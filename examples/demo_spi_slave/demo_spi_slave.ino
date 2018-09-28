/*
  NOTE: This code configures NavSpark to work as a SPI slave and receive/transmit
        data from/to remote SPI master using another NavSpark.
*/

#include "sti_gnss_lib.h"

// NOTE: The NavSpark only supports SPI mode 0 when operation in slave mode !!

void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(115200);

  spiSlave.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void task_called_after_GNSS_update(void)
{
  short len;
  char buf[64];
  uint8_t k;
  static uint8_t cnt = 0;
  static uint8_t txd[7] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};

  // in case of data from host was received
  if (spiSlave.available()) {
    Serial.print("\r\nSPI-S recv:\r\n");

    while (spiSlave.available()) {
      if (spiSlave.available() == 1) {
        len = sprintf(buf, "0x%02x", spiSlave.read());
      }
      else {
        len = sprintf(buf, "0x%02x:", spiSlave.read());
      }
      Serial.print(buf);
      cnt ++;
    }
    Serial.print("\r\n");
    if (cnt == 7) {
      cnt = 0;
      spiSlave.enableBufferForHostWrite();
    }
  }

  // in case of data has been transmitted
  if (spiSlave.validBufferForHostRead()) {
    spiSlave.resetTx();
    for (k = 0; k < 7; k++) {
      spiSlave.write(txd[k]++);
    }
    if (spiSlave.copyDataToBufferForHostRead()) {
      spiSlave.enableBufferForHostRead();
    }
  }
}
