/***************************************************************************
  This is a library for the SkyTraq NMEA from I2C slave

 ***************************************************************************/
#include <math.h>
#include <limits.h>
#include "SkyTraq_NMEA_I2C.h"

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C
*/
/**************************************************************************/
static void writeCommand(byte reg, byte value)
{
	Wire.beginTransmission((uint8_t)SKYTRAQ_I2C_SLAVE_ADDRESS);
	Wire.write((uint8_t)reg);
	Wire.write((uint8_t)value);
	Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
static void read8(byte reg, uint8_t *value)
{
	Wire.beginTransmission((uint8_t)SKYTRAQ_I2C_SLAVE_ADDRESS);
	Wire.write((uint8_t)reg);
	Wire.endTransmission(0);
	Wire.requestFrom((uint8_t)SKYTRAQ_I2C_SLAVE_ADDRESS, (byte)1);
	*value = Wire.read();
	Wire.endTransmission();
}

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
 
/**************************************************************************/
/*!
    @brief  Instantiates a new SkyTraq_NMEA_I2C class
*/
/**************************************************************************/
SkyTraq_NMEA_I2C::SkyTraq_NMEA_I2C() {

}


/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/
 
/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
bool SkyTraq_NMEA_I2C::begin()
{
	// Enable I2C
	Wire.begin();
    
	return true;
}

/**************************************************************************/
/*!
    @brief  get control status
*/
/**************************************************************************/
void SkyTraq_NMEA_I2C::getCtrlStatus(uint8_t *status)
{
	read8(SKYTRAQ_I2C_SLAVE_REGISTER_CTRL, status);
 
}

void SkyTraq_NMEA_I2C::getNMEASlaveTXSize(uint8_t *size)
{
	read8(SKYTRAQ_I2C_SLAVE_REGISTER_CTRL, size); 
	//bit 5:7 is tx size
	*size=(*size&0xe0)>>5;
 
}

uint8_t SkyTraq_NMEA_I2C::getNMEASlaveTXData(uint8_t *buf)
{
	uint8_t i;
	uint8_t size=0;
	uint8_t ctrl_status;
	uint8_t nmea_slave_clear_ctrl_bit_1=0;
	getCtrlStatus(&ctrl_status);
	if((ctrl_status&0x2)!=0)
	{
		getNMEASlaveTXSize(&size);
		if(size<=SKYTRAQ_SLAVE_TX_QUEUE_SIZE)
		{
			for(i=0;i<size;i++)
			{
				read8((byte)i, &(buf[i])); 
			}
			nmea_slave_clear_ctrl_bit_1=1;
		}
		else
		size=0;
		if(nmea_slave_clear_ctrl_bit_1==1)
		{
			if(nmea_slave_clear_ctrl_bit_1==1)
				ctrl_status=ctrl_status&0xd;
			writeCommand(SKYTRAQ_I2C_SLAVE_REGISTER_CTRL,ctrl_status);
		}
	}
	return size;
}


