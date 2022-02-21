#include "dbg_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <esp_log.h>
#include "variables.h"
#include "utils.h"
#include "ui_main.h"

#define BUILD_FOR_NROCS 1
#define BUILD_FOR_ELOGY_MILK !BUILD_FOR_NROCS

#if BUILD_FOR_ELOGY_MILK
#include "ElogyMilk.h"
#elif BUILD_FOR_NROCS
#include "NR_OCS.h"
#endif

static void printUint16(uint8_t msb, uint8_t lsb)
{
    unsigned char byte;
    uint16_t logicValues = BUILD_UINT16(msb, lsb);
    printf("LogicValues: %d | 0x%02X | ", logicValues, logicValues);
    for(size_t i = 0; i < 16; i++)
    {
        byte = (logicValues << i) & 1;
        printf("%u", byte);
    }
    printf("\n");

    // printf("LSB[0-7]: %u | %u | %u | %u | %u | %u | %u | %u\n", (logicValues & (1 << 0)), (logicValues & (1 << 1)), (logicValues & (1 << 2)), (logicValues & (1 << 3)), (logicValues & (1 << 4)), (logicValues & (1 << 5)), (logicValues & (1 << 6)), (logicValues & (1 << 7)));
    // printf("MSB[8-15]: %u | %u | %u | %u | %u | %u | %u | %u\n", (logicValues & (1 << 8)), (logicValues & (1 << 9)), (logicValues & (1 << 10)), (logicValues & (1 << 11)), (logicValues & (1 << 12)), (logicValues & (1 << 13)), (logicValues & (1 << 14)), (logicValues & (1 << 15)));

    printf("lsb[0-7]: %u | %u | %u | %u | %u | %u | %u | %u\n", ((logicValues >> 0) & 1), ((logicValues >> 1) & 1), ((logicValues >> 2) & 1), ((logicValues >> 3) & 1), ((logicValues >> 4) & 1), ((logicValues >> 5) & 1), ((logicValues >> 6) & 1), ((logicValues >> 7) & 1));
    printf("msb[8-15]: %u | %u | %u | %u | %u | %u | %u | %u\n", ((logicValues >> 8) & 1), ((logicValues >> 9) & 1), ((logicValues >> 10) & 1), ((logicValues >> 11) & 1), ((logicValues >> 12) & 1), ((logicValues >> 13) & 1), ((logicValues >> 14) & 1), ((logicValues >> 15) & 1));

}

namespace lavazza
{      
    namespace common
    {
        /**
        * @brief  Create message
        *
        * @param  [in] <sof>        : Start of Frame
        * @param  [in] <cmdCode>    : Command Code
        * @param  [in] <payloadLen> : Payload Length in bytes
        * @param  [in] <payload>    : Pointer to payload data buffer
        *
        * @retval The created message
        */
        uint8_t* createMsg(uint8_t sof, DBG_CODE_t cmdCode, uint8_t payloadLen, const uint8_t* payload)
        {
            static uint8_t dbgBuffTx[DBG_MESSAGE_LENGTH]; // HDR + PAYLOAD + CRC
            
            dbgBuffTx[0] = sof;
            dbgBuffTx[1] = (uint8_t) cmdCode;
            dbgBuffTx[2] = payloadLen;
            memcpy(&dbgBuffTx[3], payload, payloadLen);
            const uint16_t crc = crc::calculate(dbgBuffTx, payloadLen+3);
            dbgBuffTx[payloadLen+3] = HI_UINT16(crc);
            dbgBuffTx[payloadLen+4] = LO_UINT16(crc);

            return dbgBuffTx;
        }

        uint8_t* createAck(const DBG_CODE_t cmdCode)
        {
            static uint8_t ackNackPayload[DBG_ACK_PAYLOAD_LEN] = {0};
            ackNackPayload[0] = (uint8_t) cmdCode;
            ackNackPayload[1] = DBG_ACK_VALUE;

            return createMsg(ACK_SOF, DBG_CODE_ACK_NACK, sizeof(ackNackPayload), ackNackPayload);
        }

        uint8_t* createNack(const DBG_CODE_t cmdCode)
        {
            static uint8_t ackNackPayload[DBG_ACK_PAYLOAD_LEN] = {0};
            ackNackPayload[0] = (uint8_t) cmdCode;
            ackNackPayload[1] = DBG_NACK_VALUE;

            return createMsg(ACK_SOF, DBG_CODE_ACK_NACK, sizeof(ackNackPayload), ackNackPayload);
        }
    }
    namespace dbg
    {
        uint8_t findMsgs(const uint8_t* message, uint8_t size, uint8_t* msgsIndex)
        {
            uint8_t retVal = 0;
            uint8_t firstLen = message[2];
            // printf("First msg len = %d\n", firstLen);
            msgsIndex[retVal] = 0;
            retVal++;
            if((firstLen+5) != size)
            {
                uint8_t secondLen = message[7+firstLen];
                // printf("Second msg len = %d\n", secondLen);
                msgsIndex[retVal] = 5+firstLen;
                retVal++;
            }

            return retVal;
        }

        /**
        * @brief Check the received message.
        */
        bool checkMsg(const uint8_t* message, dbgMsg_stc& msg)
        {
            msg.startOfFrame = message[0];
            msg.cmdCode    = message[1];
            msg.payloadLen = message[2];

            (void) memcpy(msg.payload, &message[3], msg.payloadLen);

            return true;
        }

         /**
        * @brief Parse the received GetPar message response.
        */
        bool parseGetParResponse(dbgMsg_stc& msg)
        {
            // printf("Cmd 0x%.2X | PldLen %d -> ", msg.cmdCode, msg.payloadLen);
            // for(uint8_t i = 0; i < msg.payloadLen; i++)
            // {   
            //     printf("0x%.2X ", msg.payload[i]);
            // }
            // printf("\n");

            uint16_t parId = BUILD_UINT16(msg.payload[0], msg.payload[1]);
            uint32_t parVal;

            uint8_t size = msg.payloadLen -2;

            switch(size)
            {
                case 1:
                {
                    parVal = msg.payload[2];
                    break;
                }
                case 2:
                {
                    parVal = BUILD_UINT16(msg.payload[3], msg.payload[2]);
                    break;
                }
                case 4:
                {
                    parVal = BUILD_UINT32(msg.payload[5], msg.payload[4], msg.payload[3], msg.payload[2]);
                    break;
                }
                default:
                {
                    parVal = 0;
                    break;
                }
            }

            if(parId >= 300)    //Statistics
            {
                printf("Received statistic parameter %d, value %d\n", parId, parVal);
                ui_statistics_update_data(parId, parVal);
            }
            else if(parId >= 110 && parId <= 115)   //Doses 
            {
                printf("Received dose parameter %d, value %d\n", parId, parVal);
                ui_erogation_set_target_dose(parVal);
            }
            else
            {
                printf("Received parameter %d, value %d\n", parId, parVal);
            }
            return true;
        }

        /**
        * @brief Parse the received livedata message.
        */
        void parseLivedata(dbgMsg_stc& msg)
        {
            // DbgLivedata_t livedata = parseNrOcsLivedata(msg.payload);       
            static uint8_t fsmStatus;
            static uint8_t oldFsmStatus = FSM_STATE_STANDBY;
            static bool podFull, podOverflow, podRemoved;
            static bool podWarning, waterWarning, descalingWarning, milkPresence;
            static uint8_t recipeId;
            uint16_t logicValues;
            uint16_t dose;
            uint8_t  temperature;

            #if BUILD_FOR_NROCS
            logicValues = BUILD_UINT16(msg.payload[22], msg.payload[21]);
            fsmStatus = msg.payload[0];
            milkPresence = ((logicValues >> 0) & 1);
            podRemoved = ((logicValues >> 1) & 0);  //TODO fix value
            podFull = ((logicValues >> 2) & 1);
            podOverflow = ((logicValues >> 3) & 1);
            podWarning = (podFull | podOverflow | podRemoved);
            waterWarning = ((logicValues >> 4) & 1);
            recipeId = static_cast<uint8_t>(((logicValues >> 12) & 1));
            descalingWarning = ((logicValues >> 15) & 1);   //not implemented on NR_OCS
            dose = BUILD_UINT16(msg.payload[1], msg.payload[2]);
            temperature = msg.payload[7];    //10 per il latte
            #endif

            #if BUILD_FOR_ELOGY_MILK
            static uint16_t uiAlarm = BUILD_UINT16(msg.payload[22], msg.payload[21]);
            logicValues = msg.payload[13];
            fsmStatus = msg.payload[0];
            milkPresence = ((logicValues >> 0) & 1);
            podRemoved = ((uiAlarm >> 1) & 1);
            podFull = ((uiAlarm >> 2) & 1);
            podWarning = (podFull | podRemoved);
            waterWarning = ((uiAlarm >> 3) & 1);
            descalingWarning = ((uiAlarm >> 4) & 1);
            dose = BUILD_UINT16(msg.payload[2], msg.payload[3]);
            temperature = msg.payload[1];
            #endif

            // printUint16(msg.payload[22], msg.payload[21]);
            //printf("STS %d -> LOGIC VALUES %d (%d | %d | %d | %d | %d | %d)\n", fsmStatus, logicValues, milkPresence, podRemoved, podFull, podOverflow, waterWarning, descalingWarning, recipeId);
            if(fsmStatus == FSM_STATE_STANDBY)   //standby
            {
                if(oldFsmStatus != fsmStatus)
                {
                    xEventGroupSetBits(xGuiEvents, GUI_MACHINE_OFF);
                }
            }
            else if(fsmStatus == FSM_STATE_FAULT)  //fault
            {
                if((true != guiInternalState.isFault) || (oldFsmStatus != fsmStatus))
                {
                    guiInternalState.isFault = (fsmStatus == FSM_STATE_FAULT);
                    xEventGroupSetBits(xGuiEvents, GUI_MACHINE_FAULT_BIT);
                }
            }
            else if(fsmStatus == FSM_STATE_CLEANING)  //cleaning
            {
                if(2 == recipeId)
                {
                    printf("CLEANING FAST recipe %d\n", recipeId);
                }
                else
                {
                    guiInternalState.cleaning.recipeId = recipeId;
                    guiInternalState.cleaning.totalSteps = msg.payload[48];
                    guiInternalState.cleaning.currentSteps = msg.payload[47];
                    printf("CLEANING %d (%d / %d)\n", guiInternalState.cleaning.recipeId, guiInternalState.cleaning.currentSteps , guiInternalState.cleaning.totalSteps);
                    xEventGroupSetBits(xGuiEvents, GUI_NEW_CLEANING_DATA_BIT);
                }
            }
            else if(fsmStatus == FSM_STATE_BREWING)  //erogation
            {
                guiInternalState.erogation.dose = BUILD_UINT16(msg.payload[1], msg.payload[2]);
                guiInternalState.erogation.temperature = msg.payload[7];    //10 per il latte
                xEventGroupSetBits(xGuiEvents, GUI_NEW_EROGATION_DATA_BIT);
            }
            else if(fsmStatus == FSM_STATE_STEAMING)  //erogation
            {
                guiInternalState.steaming.percent = msg.payload[51];
                xEventGroupSetBits(xGuiEvents, GUI_NEW_MILK_EROGATION_DATA_BIT);
            }
            else if(fsmStatus == FSM_STATE_READY_TO_BREW)  //Ready to brew
            {
                if(oldFsmStatus == FSM_STATE_BREWING)
                {
                    xEventGroupSetBits(xGuiEvents, GUI_STOP_EROGATION_BIT);
                }

                if(oldFsmStatus == FSM_STATE_STEAMING)
                {
                    xEventGroupSetBits(xGuiEvents, GUI_STOP_MILK_EROGATION_BIT);
                }

                if(oldFsmStatus == FSM_STATE_STANDBY)
                {
                    xEventGroupSetBits(xGuiEvents, GUI_MACHINE_ON);
                }
            }
            else
            {                
                if(oldFsmStatus == FSM_STATE_STANDBY)
                {
                    xEventGroupSetBits(xGuiEvents, GUI_MACHINE_ON);
                }

                if(false != guiInternalState.isFault)
                {
                    guiInternalState.isFault = false;
                    xEventGroupSetBits(xGuiEvents, GUI_MACHINE_FAULT_BIT);
                }
            }

            if( guiInternalState.warnings.descaling != descalingWarning || 
                guiInternalState.warnings.waterEmpty != waterWarning ||
                guiInternalState.warnings.podFull != podWarning)
            {
                guiInternalState.warnings.descaling = descalingWarning;
                guiInternalState.warnings.waterEmpty = waterWarning;
                guiInternalState.warnings.podFull = podWarning;
                xEventGroupSetBits(xGuiEvents, GUI_WARNINGS_BIT);
            }

            if(guiInternalState.milkHeadPresence != milkPresence)
            {
                guiInternalState.milkHeadPresence = milkPresence;
                xEventGroupSetBits(xGuiEvents, GUI_ENABLE_MILK_BIT);
            }

            if(oldFsmStatus != fsmStatus)
            {
                oldFsmStatus = fsmStatus;
            }
        }
    }
}