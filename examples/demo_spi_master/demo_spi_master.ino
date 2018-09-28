/*
  NOTE: This code configures NavSpark to work as a SPI master and read/wrte
        data from/to remote SPI slave using another NavSpark.
*/

#include "sti_gnss_lib.h"

void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(115200);

#if 0 /* case-1: Only 1 external SPI device */
  spiMaster.config(0, 5000000, false, false); // mode 0, 5MHz, CS0 only
  spiMaster.begin();
  spiMaster.slaveSelect(0); // use GPIO28
  pinMode(GPIO22_SPISL_MCS1, OUTPUT); // GPIO22 can be normal GPIO in this case
  pinMode(GPIO6_WHEEL_TIC_MEAS, OUTPUT); // GPIO6 can be normal GPIO in this case
  digitalWrite(GPIO22_SPISL_MCS1, LOW);
  digitalWrite(GPIO6_WHEEL_TIC_MEAS, LOW);
#endif

#if 1 /* case-2: 2 external SPI devices */
  spiMaster.config(0, 5000000, true, false); // mode 0, 5MHz, CS0 and CS1
  spiMaster.begin();
//spiMaster.slaveSelect(0); // use GPIO28
  spiMaster.slaveSelect(1); // use GPIO22
  pinMode(GPIO6_WHEEL_TIC_MEAS, OUTPUT); // GPIO6 can be normal GPIO in this case
  digitalWrite(GPIO6_WHEEL_TIC_MEAS, LOW);
#endif

#if 0 /* case-3: 3 external SPI devices */
  spiMaster.config(0, 5000000, true, true); // mode 0, 5MHz, CS0, CS1 and CS2
  spiMaster.begin();
//spiMaster.slaveSelect(0); // use GPIO28
//spiMaster.slaveSelect(1); // use GPIO22
  spiMaster.slaveSelect(2); // use GPIO6
#endif
}

void loop() {
  // put your main code here, to run repeatedly:

}

#define	CMD_WR_REG0	0x80
#define	CMD_WR_REG1	0x81
#define	CMD_WR_REG2	0x82
#define	CMD_WR_REG3	0x83
#define	CMD_RD_REG0	0xC0
#define	CMD_RD_REG1	0xC1
#define	CMD_RD_REG2	0xC2
#define	CMD_RD_REG3	0xC3
#define	CMD_WR_BUFF	0x88
#define	CMD_RD_BUFF	0xC8

void task_called_after_GNSS_update(void)
{
  short len;
  char buf[64];
  volatile uint8_t reg0 = 0;
  volatile uint8_t reg2 = 0;
  uint8_t k;
  static uint8_t txd[7] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};

  spiMaster.resetTx();
  spiMaster.resetRx();
  spiMaster.write(CMD_RD_REG0);
  spiMaster.write(0x00); // null data
  spiMaster.transfer(2);
  reg0 = spiMaster.pick(1);

  if (reg0 & 0x2) { // buffer for master write is ready
    // write 0x7 to reg1 for master wants to write 7 bytes of data to buffer
    spiMaster.resetTx();
    spiMaster.resetRx();
    spiMaster.write(CMD_WR_REG1);
    spiMaster.write(0x7);
    spiMaster.transfer(2);
    // write the 7 bytes data to buffer
    spiMaster.resetTx();
    spiMaster.resetRx();
    spiMaster.write(CMD_WR_BUFF);
    spiMaster.write(txd[0]++);
    spiMaster.write(txd[1]++);
    spiMaster.write(txd[2]++);
    spiMaster.write(txd[3]++);
    spiMaster.write(txd[4]++);
    spiMaster.write(txd[5]++);
    spiMaster.write(txd[6]++);
    spiMaster.transfer(8);
  }

  if (reg0 & 0x4) { // data on slave's buffer for master read is valid
    // write 0x7 to reg2 for master wants to read 7 bytes of data from buffer
    spiMaster.resetTx();
    spiMaster.resetRx();
    spiMaster.write(CMD_RD_REG2);
    spiMaster.write(0x7);
    spiMaster.transfer(2);
    reg2 = spiMaster.pick(1);

    // identify how many bytes are stored in slave's buffer
    if (reg2 <= 7) {
      spiMaster.resetTx();
      spiMaster.resetRx();
      spiMaster.write(CMD_RD_BUFF);
      for (k = 0; k < reg2; k++) {
        spiMaster.write(0x00); // null data
      }
      spiMaster.transfer(); // CMD_RD_BUFF is included
      if (spiMaster.available()) {
        spiMaster.read(); // ignore 1st byte
        Serial.print("\r\nSPI-M recv:\r\n");
        while (spiMaster.available()) {
          if (spiMaster.available() == 1) {
            len = sprintf(buf, "0x%02x", spiMaster.read());
          }
          else {
            len = sprintf(buf, "0x%02x:", spiMaster.read());
          }
          Serial.print(buf);
        }
        Serial.print("\r\n");
      }
    }
  }
}
