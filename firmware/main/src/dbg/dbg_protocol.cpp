#include "dbg_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <esp_log.h>
#include "variables.h"
#include "utils.h"
#include "ui_main.h"

static uint8_t createErrorsBitField()
{
    dbgErrors_t error_union;
    // error_union.bits.temperature = machineInternalState.errors.temperature;
    // error_union.bits.heaterTimeout = machineInternalState.errors.heaterTimeout;
    // error_union.bits.notUsed1 = 0;
    // error_union.bits.ntc = machineInternalState.errors.ntc;
    // error_union.bits.zc = machineInternalState.errors.zc;
    // error_union.bits.notUsed2 = 0;
    return error_union.bitFieldValue;
}

static uint8_t createLogicValuesBitField()
{
    dbgLogicValues_t logicValue_union;
    // logicValue_union.bits.shortCoffeeButtonStuck = (uiStatus.buttonShort == STUCK_CLICK);
    // logicValue_union.bits.longCoffeeButtonStuck = (uiStatus.buttonLong == STUCK_CLICK);
    // logicValue_union.bits.pumpState = (machineInternalState.pumpActivationDuty != 0) ? 1 : 0;
    // logicValue_union.bits.waterValveState = (uint8_t)machineInternalState.waterValveState;
    // logicValue_union.bits.currentPreparation = (uint8_t)machineInternalState.desiredPreparation;
    // logicValue_union.bits.firstInstallation = 0;
    // logicValue_union.bits.notUsed = 0;
    return logicValue_union.bitFieldValue;
}

static uint8_t createWarningStatusBitField()
{
    dbgWarningStatus_t warningStatus_union;
    // warningStatus_union.bits.podFull = (machineRegisters.stats.podFullDeliveries >= CONFIG_MAX_POD_CONTAINER);
    // warningStatus_union.bits.descalingProc = (machineRegisters.stats.descaleState != DESCALING_NOT_NEEDED);
    // warningStatus_union.bits.emptyWater = (machineInternalState.waterTankState == WT_EMPTY);
    // warningStatus_union.bits.notUsed = 0;
    return warningStatus_union.bitFieldValue;
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
                    break;
                }
            }            
            printf("Received parameter %d, value %d\n", parId, parVal);
            ui_statistics_update_data(parId, parVal);
            return true;
        }

        /**
        * @brief Parse the received livedata message.
        */
        bool parseLivedata(dbgMsg_stc& msg)
        {
            static uint8_t fsmStatus, oldFsmStatus;
            static bool podWarning, waterWarning, descalingWarning;
            static bool milkPresence, oldMilkPresence;

            //Logic value [39], [40]
            //Warning value [44], [45]
            fsmStatus = msg.payload[0];
            podWarning = (msg.payload[45] & (1 << 2));
            waterWarning = (msg.payload[45] & (1 << 3));
            descalingWarning = (msg.payload[45] & (1 << 4));
            milkPresence = (msg.payload[40] & (1 << 7));

            if(fsmStatus == 0x01)
            {
                if(fsmStatus != oldFsmStatus)
                {
                    ui_preparations_set_power(false);
                }
                else
                {
                    //Do nothing
                }
            }
            else if(fsmStatus == 0x07)
            {
                //update data during brewing
                uint8_t temperature = msg.payload[7];    //18 per il latte
                uint16_t dose = BUILD_UINT16(msg.payload[1], msg.payload[2]);
                ui_erogation_update(dose, temperature, 5.0f);
            }
            else if(fsmStatus == 0x03)
            {
                if(oldFsmStatus == 0x07 || oldFsmStatus == 0x08)    //from Steaming (0x08) or Brewing (0x07)
                    ui_erogation_completed();
            }

            ui_status_bar_set_descaling_warning(descalingWarning);
            ui_status_bar_set_water_empty_warning(waterWarning);
            ui_status_bar_set_pod_warning(podWarning);

            if(oldMilkPresence != milkPresence)
            {
                ui_preparations_enable_cappuccino(milkPresence);
            }

            oldFsmStatus = fsmStatus;
            oldMilkPresence = milkPresence;
            return true;
        }
    }
}