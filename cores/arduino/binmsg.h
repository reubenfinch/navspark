#ifndef _BINMSG_HEADER_
#define _BINMSG_HEADER_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

typedef const unsigned char *LPCSTR;

typedef enum BinaryType
{
  MSG_UNKNOWN = 0,  //No complete line
  MSG_OK,
  MSG_ERROR,  //Format error
  MSG_FULL,   //buffer full, it will discard old data.
} BinType;

#define InBufferSize    2 * 1024
#define LineBufferSize  256

#define SOS1 0xA0
#define SOS2 0xA1
#define EOS1 0x0D
#define EOS2 0x0A
//message ID
#define SET_QUERY_SW_VERSION   0x02
#define SET_QUERY_SW_CRC       0x03
#define SET_IMAGE_DOWNLOAD     0x0B
#define REPLY_SW_VERSION       0x80
#define REPLY_SW_CRC           0x81
#define REPLY_ACK              0x83
#define REPLY_NACK             0x84

#define FLASH_CHIP_SIZE 0x80000


class BinaryParser
{
	public:
		BinaryParser(void);
		uint8_t getBinaryParserStatus(uint8_t *buf, uint8_t *repbuf);
		BinType BinaryDataIn(U08* buf, int size);
	private:
		U08 inBuffer[InBufferSize];
		int inBufferLen;
		int inBufferPtr;
		U08 lineBuffer[LineBufferSize];
		int lineLen;
		
		void ClearInBuffer();
		void ClearLineBuffer();
		void OrganizeInBuffer();
		int ReadOneLine();
		BinType MessageType(LPCSTR pt, int len);
		int VarifyBinaryChecksum(LPCSTR pt, int len);
		void BMSG_CmdReply( U08 msgID, U08 reqID);
		void BMSG_MsgReply(U08 msgID, U08 *buf, U08 len);
		U08 BMSG_reply_sw_crc(U08 *buf, U08 type);
		U08 BMSG_cfg_query_sw_crc(const unsigned char *buf, U08 *repbuf);
		U08 BMSG_cal_checksum(U08 *buf, U16 len);
		U16 BMSG_PackMsg( U08 *ibuf, U08 *obuf, U16 len );
		U08 BMSG_new_image_download(const unsigned char *buf, U08 id);
    

};
#endif

