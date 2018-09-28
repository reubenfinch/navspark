#ifndef _SKYTRA_NMEA_I2C_H_
#define _SKYTRA_NMEA_I2C_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>


/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
		#define SKYTRAQ_I2C_SLAVE_ADDRESS                (0x3C) //currently for accessing standard GPS I2C NMEA not arduino navspark NMEA
/*=========================================================================*/
		#define SKYTRAQ_SLAVE_TX_QUEUE_SIZE                   5
		#define SKYTRAQ_SLAVE_RX_QUEUE_SIZE                   3  
/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
		enum
		{
			SKYTRAQ_I2C_SLAVE_REGISTER_GP0            = 0x0,  // general purpose register 0 8 bits
			SKYTRAQ_I2C_SLAVE_REGISTER_GP1            = 0x1,  // general purpose register 1 8 bits 
			SKYTRAQ_I2C_SLAVE_REGISTER_GP2            = 0x2,  // general purpose register 2 8 bits
			SKYTRAQ_I2C_SLAVE_REGISTER_GP3            = 0x3,  // general purpose register 3 8 bits
			SKYTRAQ_I2C_SLAVE_REGISTER_GP4            = 0x4,  // general purpose register 4 8 bits 
			SKYTRAQ_I2C_SLAVE_REGISTER_GP5            = 0x5,  // general purpose register 5 8 bits
			SKYTRAQ_I2C_SLAVE_REGISTER_GP6            = 0x6,  // general purpose register 6 8 bits
			SKYTRAQ_I2C_SLAVE_REGISTER_GP7            = 0x7,  // general purpose register 7 8 bits
			SKYTRAQ_I2C_SLAVE_REGISTER_CTRL           = 0x8   // control register
		};


/*=========================================================================*/

class SkyTraq_NMEA_I2C
{
	public:
		SkyTraq_NMEA_I2C(void);
  
		bool  begin();
		void getCtrlStatus(uint8_t *status);
		void getNMEASlaveTXSize(uint8_t *size);
		uint8_t getNMEASlaveTXData(uint8_t *buf);
	private:
    

};

#endif
