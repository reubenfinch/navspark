/*
  TwoWire.h - Header file of library for 2-wire interface
  Copyright (c) 2013 NavSpark.  All right reserved.

	This library is free software; you can redistribute it under the terms
  of the GNU Lesser General Public License as published by the Free Software
  Foundation; either version 2.1 of the License, or (at your option) any
  later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this library; if not, write to the Free Software Foundation,
  Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	Created 25 December 2013 by Ming-Jen Chen

	$Id$
*/

#ifndef _2WIRE_H_
#define _2WIRE_H_

#include "Stream.h"

#define	TWOWIRE_BUFFER_SIZE	16

#define	TWOWIRE_MASTER	1
#define	TWOWIRE_SLAVE		0

#define	TWOWIRE_SLAVE_TX_FIFO_BASE	0
#define	TWOWIRE_SLAVE_TX_FIFO_SIZE	4
#define	TWOWIRE_SLAVE_RX_FIFO_BASE	4
#define	TWOWIRE_SLAVE_RX_FIFO_SIZE	4

typedef	struct _2wire_buffer
{
  uint8_t *buffer;
  volatile unsigned int wrPtr;
  volatile unsigned int numSpace;
  volatile unsigned int rdPtr;
  volatile unsigned int numData;
} s2WireBuffer, *p2WireBuffer;

class TwoWire_MasterSlave : public Stream
{
	private:
		bool	enabled;

		uint8_t	_mode;	// 1 for master, 0 for slave
		uint8_t _devAddr;	// only valid in slave mode
		uint8_t _txAddr;	// only valid in master mode
		uint8_t _rxAddr;	// only valid in master mode
		uint32_t _clkRate; // only valid in master mode

		s2WireBuffer	recv;
		p2WireBuffer	pRecv;
		s2WireBuffer	xmit;
		p2WireBuffer	pXmit;
		uint16_t recvBuffSize;
		uint16_t xmitBuffSize;

		bool	slvModeTxFilled;

		void (*user_onRequest)(void);
		void (*user_onReceive)(void);

	public:
		TwoWire_MasterSlave(uint8_t mode);
		TwoWire_MasterSlave(void);

		// Set the clock rate of 2-wire interface in master mode
		void config(uint32_t clkRate);
		uint32_t getClkRate(void);

		// Reconfigure the buffer sizes
		bool setBufferSize(uint16_t xmitSize, uint16_t recvSize);

		// Reset the buffer settings
		void reset(void);
		void resetTx(void);
		void resetRx(void);

		// Set the device address
		void begin(void);
		void begin(uint8_t addr);

		// Set the address of target device (master mode only)
		void setTransmitDeviceAddr(uint8_t addr);
		void setReceiveDeviceAddr(uint8_t addr);
    
		// Added by Andrew
		void beginTransmission(uint8_t address);
        void beginTransmission(int address);

		// Transfer data from buffer to target device
		uint16_t endTransmission(void);
		uint16_t endTransmission(bool xmitStop);

		// Read data from target device to buffer
		uint16_t readDevice(uint16_t quantity);
		uint16_t readDevice(uint16_t quantity, bool xmitStop);
		uint16_t readDeviceFromOffset(uint8_t devOffset, uint16_t quantity, bool xmitStop);
                
        // Added by Andrew
		uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop);
        uint8_t requestFrom(uint8_t address, uint8_t quantity);
		uint8_t requestFrom(int address, int quantity);
        uint8_t requestFrom(int address, int quantity, int sendStop);
    
		// Throw data to the Tx buffer only
		virtual size_t write(uint8_t data);
        virtual size_t write(uint8_t *data, size_t size);
		void send(uint8_t data);
		void send(uint8_t *data, size_t size);
		
		// Throw data to the Tx buffer and start transfer
        size_t writeAtOffset(uint8_t devOffset, uint8_t *data, size_t size);

		// Return how many bytes of ingress data received in buffer
		virtual size_t available(void);
		size_t recv_space(void);

		// Return how many bytes of egress data still in transmit buffer
		size_t remaining(void);
		size_t xmit_space(void);

		// Read out the data received in buffer
		virtual int read(void);

		// Move the data received in buffer to specified storage
		size_t blk_read(uint8_t *dst, size_t size);

		// Pick the data next to current data for peeking
		virtual int peek(void);

		// Not implemented yet
		virtual void flush(void);

		// user-defined ISR hooker
		void onReceive( void (*function)(void) );
		void onRequest( void (*function)(void) );

		// Handler to move data between ring buffers and H/W FIFOs
		void isr(void);

		//
		operator bool();

		inline size_t write(unsigned long n) { return write((uint8_t)n); }
		inline size_t write(long n) { return write((uint8_t)n); }
		inline size_t write(unsigned int n) { return write((uint8_t)n); }
		inline size_t write(int n) { return write((uint8_t)n); }
		
		using Print::write;
};

extern TwoWire_MasterSlave twMaster;
extern TwoWire_MasterSlave twSlave;

#endif // _2WIRE_H_
