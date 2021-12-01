#ifndef DBG_PROTOCOL_H_
#define DBG_PROTOCOL_H_

#include <stdint.h>
#include "esp_log.h"

#define HIHI_UINT32(a)  (uint8_t) (((a) >> 24u) & 0xFFu)
#define HILO_UINT32(a)  (uint8_t) (((a) >> 16u) & 0xFFu)
#define LOHI_UINT32(a)  (uint8_t) (((a) >> 8u) & 0xFFu)
#define LOLO_UINT32(a)  (uint8_t) ((a) & 0xFFu)
#define HI_UINT16(a)  (uint8_t) (((a) >> 8u) & 0xFFu)
#define LO_UINT16(a)  (uint8_t) ((a) & 0xFFu)
#define BUILD_UINT16(msb, lsb) ((((uint16_t) lsb) & 0x00FFu) | ((((uint16_t) msb) & 0x00FFu) << 8u))
#define BUILD_UINT32(mmsb, mlsb, lmsb, llsb) ((((uint32_t) llsb) & 0x000000FFu) | ((((uint32_t) lmsb) & 0x000000FFu) << 8u) | ((((uint32_t) mlsb) & 0x000000FFu) << 16u) | ((((uint32_t) mmsb) & 0x000000FFu) << 24u))


#define CMD_SOF                     (0xA5)      /** Plain text message Start Of Frame (SOF) */
#define ACK_SOF                     (0x5A)      /** ACK/NACK Start Of Frame */

#define DBG_PLAIN_HDR_SIZE          (3)         /** CMD_SOF  + CMD_CODE + PLD_LENGTH */

#define DBG_PAYLOAD_MAX_SIZE    	(256)
#define SETPAR_PAYLOAD_LENGTH   	  (9)   /* Length of payload for set parameter response */
#define DBG_HDR_LENGTH          	  (3)   /* Header Length = SOF + CMD_CODE + PAYLOAD_LENGTH */
#define DBG_CRC_LENGTH          	  (2)   /* Length of CRC (in bytes) */
#define DBG_MESSAGE_LENGTH      	(DBG_HDR_LENGTH + DBG_PAYLOAD_MAX_SIZE + DBG_CRC_LENGTH)
#define DBG_MAX_FAILURES        	  (5)   /* Max number of message retries */
#define DBG_ACK_VALUE           	  (0)   /* Ack message value to notify message acknowledge */
#define DBG_ACK_PAYLOAD_LEN     	  (2)   /* Length (in bytes) of Ack message payload */
#define DBG_NACK_VALUE          	  (1)   /* Ack message value to notify message un-acknowledge */
#define DBG_NACK_PAYLOAD_LEN    	  (2)   /* Length (in bytes) of Nack message payload */
#define DBG_LIVE_DATA_OFF         	  (0)   /* Value to disable live data */
#define DBG_LIVE_DATA_ON          	  (1)   /* Value to enable */
#define DBG_LIVE_DATA_PAYLOAD_LEN 	 (35)   /* Length (in bytes) of Livedata message payload */
#define LIVE_DATA_SEND_TIMEOUT  	(500)   /* Time interval to send Live Data in ms */

typedef enum
{
	DBG_CODE_NONE                   = 0x00,
	DBG_CODE_GET_LIVE_DATA          = 0x20,     /* Enable/Disable Live Data */
	DBG_CODE_ACK_NACK               = 0xFE,     /* ACK/NACK Command Code */
	DBG_CODE_GET_PAR                = 0x38,     /* Get parameter Command */
	DBG_CODE_SET_PAR                = 0x3C,     /* Set parameter Command */
	DBG_CODE_SPECIAL_FUNC           = 0x36,     /* Executes special functions */
	CAM_RECEIVED_PHOTO_CHUNK		= 0x98,
	CAM_RECEIVED_QR_DATA			= 0x99
} DBG_CODE_t;

typedef struct
{
	uint8_t temperature  		: 1;  /* LSBit */
	uint8_t heaterTimeout  		: 1;
	uint8_t notUsed1         	: 3;
	uint8_t ntc					: 1;
	uint8_t zc					: 1;
	uint8_t notUsed2			: 1;  /* MSBit */
} dbgErrorsBitField_bit_t;

typedef struct
{
	uint8_t shortCoffeeButtonStuck	: 1;  /* LSBit */
	uint8_t longCoffeeButtonStuck  	: 1;
	uint8_t pumpState         		: 1;
	uint8_t waterValveState   		: 1;
	uint8_t currentPreparation   	: 2;
	uint8_t firstInstallation    	: 1;
	uint8_t notUsed    				: 1;  /* MSBit */
} dbgLogicValuesBitField_bit_t;

typedef struct
{
	uint8_t podFull				: 1;  /* LSBit */
	uint8_t descalingProc		: 1;
	uint8_t emptyWater         	: 1;
	uint8_t notUsed   			: 5;  /* MSBit */
} dbgWarningStatusBitField_bit_t;

typedef union
{
	dbgErrorsBitField_bit_t bits;
	uint8_t  		  		bitFieldValue;
} dbgErrors_t;

typedef union
{
	dbgLogicValuesBitField_bit_t 	bits;
	uint8_t  		  				bitFieldValue;
} dbgLogicValues_t;

typedef union
{
	dbgWarningStatusBitField_bit_t 	bits;
	uint8_t  		  				bitFieldValue;
} dbgWarningStatus_t;

typedef struct
{
	uint8_t  startOfFrame;
	uint8_t  cmdCode;
	uint8_t  payloadLen;
	uint8_t  payload[DBG_PAYLOAD_MAX_SIZE];
} dbgMsg_stc;

typedef struct
{
	uint8_t  fsmState;
} dbgLivedata_t;



namespace lavazza
{
    namespace common
    {
        uint8_t* createMsg(uint8_t sof, DBG_CODE_t cmdCode, uint8_t payloadLen, const uint8_t *payload);
        uint8_t* createAck(const DBG_CODE_t cmdCode);
        uint8_t* createNack(const DBG_CODE_t cmdCode);
    }
    namespace dbg
    {
		uint8_t findMsgs(const uint8_t* message, uint8_t size, uint8_t* msgsIndex);
		bool parseGetParResponse(dbgMsg_stc& msg);
        bool checkMsg(const uint8_t* message, dbgMsg_stc &msg);
		void parseLivedata(dbgMsg_stc& msg);
    }
}
namespace crc
{
    inline uint16_t calculate(const uint8_t buf[], uint16_t len)
    {
        uint16_t crc = 0xFFFFU;     /* Initial value for continuous CRC calculation */
        for (uint16_t i = 0; i < len; i++)
        {
            crc ^= ((uint16_t) buf[i]) << 8u;

            for (uint8_t j = 0; j < 8u; j++)
            {
                if((crc & 0x8000u) == 0x8000u)
                    crc = (crc << 1) ^ 0x1021u;
                else
                    crc <<= 1;
            }
        }
        return crc;
    }

	inline bool verify(const uint8_t buf[], uint16_t len)
    {       
		uint16_t crc_calculated = crc::calculate(buf, buf[2] + DBG_HDR_LENGTH);
		uint16_t crc_received = BUILD_UINT16(buf[2] + DBG_HDR_LENGTH, buf[2] + DBG_HDR_LENGTH+1);
		
		bool retVal = (crc_calculated == crc_received);

		if(false == retVal)
			ESP_LOGE("CRC", "Failed to verify crc (received %u - calculated %u)", crc_received, crc_calculated);
		else
			ESP_LOGD("CRC", "Crc verified");

		return retVal;

    }
}
#endif // DBG_PROTOCOL_H_