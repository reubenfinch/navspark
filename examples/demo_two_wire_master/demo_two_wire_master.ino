/*
  NOTE: This code configures NavSpark to work as a two-wire master and read/wrte
        data from/to remote two-wire slave using another NavSpark.
*/

#include "sti_gnss_lib.h"

void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(115200);

  twMaster.config(400000); // 400KHz
  twMaster.begin();
  twMaster.setTransmitDeviceAddr(0x57); // 0x57 is the device address
  twMaster.setReceiveDeviceAddr(0x57); // 0x57 is the device address
}

void loop() {
  // put your main code here, to run repeatedly:

}

void task_called_after_GNSS_update(void)
{
  static uint8_t txdata[] = {0xde, 0xad, 0xbe, 0xef, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa};
  static char buf[64];
  static uint16_t txPtr = 0;
  static uint16_t txSize = sizeof(txdata);
         uint16_t txNum;
         uint16_t rxSize;
         uint16_t rxNum;
         uint16_t k;
         uint8_t twSlvSts;
         uint8_t twSlvCmd;

  // poll the status of another V822 which is configured as slave
  twMaster.reset();
  twMaster.readDeviceFromOffset(0x08, 1, true);
  twSlvSts = twMaster.read();
  twSlvCmd = twSlvSts;

  if (twSlvSts & 0x02) { // data from slave is ready to read
    rxSize = (twSlvSts & 0xe0) >> 5;
    rxNum = (rxSize > TWOWIRE_BUFFER_SIZE) ? TWOWIRE_BUFFER_SIZE : rxSize;
    twMaster.resetRx();
    twMaster.readDeviceFromOffset(TWOWIRE_SLAVE_TX_FIFO_BASE, rxNum, true);
    rxNum = twMaster.available();
    if (rxNum) {
      for (k = 0; k < rxNum; k++) {
        sprintf(buf, "2-wire master recvs 0x%2x\r\n", twMaster.read());
        Serial.print(buf);
      }
      twSlvCmd &= ~(1 << 1); // clear bit 1
      twSlvCmd &= ~(7 << 5); // clear bit 7 ~ 5
    }
  }

  if ((twSlvSts & 0x01) == 0) { // xmit data to slave
    if (txSize) {
      txNum = (txSize > TWOWIRE_SLAVE_RX_FIFO_SIZE) ? TWOWIRE_SLAVE_RX_FIFO_SIZE : txSize;
      twMaster.resetTx();
      txNum = twMaster.writeAtOffset(TWOWIRE_SLAVE_RX_FIFO_BASE, &txdata[txPtr], txNum);
      txSize -= txNum;
      txPtr += txNum;
      twSlvCmd &= ~(7 << 2); // clear bit 4 ~ 2
      twSlvCmd |= ((txNum & 0x07) << 2);
      twSlvCmd |= 0x1; // set bit 0
    }
    else {
      txSize = sizeof(txdata);
      txPtr = 0;
    }
  }

  if (twSlvSts != twSlvCmd) {
    twMaster.writeAtOffset(0x08, &twSlvCmd, 1);
  }
}
