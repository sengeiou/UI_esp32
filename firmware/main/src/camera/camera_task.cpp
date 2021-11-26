#include "camera_task.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "driver/uart.h"

#include "utils.h"
#include "types.h"
#include "variables.h"
#include "system_utils.h"
#include "spiffs_utils.h"

#include "camera_protocol.h"
#include "dbg_protocol.h"

#define RX_BUF_SIZE     512


#ifdef ADVANCED_DEBUG
    #define CAM_TAG LINE_STRING "|" "CAM_TASK"
    #define CAM_CMD_TAG LINE_STRING "|" "CAM_COMMAND_TASK"
#else
    #define CAM_TAG "CAM_TASK"
    #define CAM_CMD_TAG "CAM_COMMAND_TASK"
#endif

int send_uart2(const uint8_t* data)
{
    const int len = data[2] + 5;
    const int txBytes = uart_write_bytes(CONFIG_CAM_UART_INTERFACE_NUMBER, data, len);

    ESP_LOG_BUFFER_HEXDUMP("SEND MESSAGE", data, len, ESP_LOG_DEBUG);
    return txBytes;
}

void add_to_queue2(QueueHandle_t xQueue, const uint8_t* data)
{
    if((xQueue != nullptr) && (xQueueSend(xQueue, (void *)data, (TickType_t) 10) != pdPASS))
    {
        ESP_LOGW(CAM_TAG, "Unable to add to queue");
    }
    else
    {
        ESP_LOGD(CAM_TAG, "Command sent to queue");
    }
}

void camera_function(camera_func_code_t cmd)
{    
    printf("Camera function 0.%2X\n", cmd);
    switch(cmd)
    {
        case CAM_TAKE_PHOTO:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x04, 0xD6, 0x43};
            add_to_queue2(xQueueCamUartTx, data);
            break;
        }
        case CAM_GET_PHOTO:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x05, 0xC6, 0x62};
            add_to_queue2(xQueueCamUartTx, data);
            break;
        }
        case CAM_ENABLE_LED:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x06, 0xF6, 0x01};
            add_to_queue2(xQueueCamUartTx, data);
            break;
        }
        case CAM_DISABLE_LED:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x07, 0xE6, 0x20};
            add_to_queue2(xQueueCamUartTx, data);
            break;
        }
        case CAM_NONE:
        default:
        {
            //Do nothing
            break;
        }
    }
}


bool process_msg2(uint8_t* message, uint8_t size)
{
    bool retVal = false;
    dbgMsg_stc msg;
    uint8_t msgsIndex[2];
    uint8_t nMsg = lavazza::dbg::findMsgs(message, size, msgsIndex);
    
    for(uint8_t i = 0; i < nMsg; i++)
    {
        lavazza::dbg::checkMsg(&message[msgsIndex[i]], msg);
        // printf("Code %d | Len %d [", msg.cmdCode, msg.payloadLen);
        // for(uint8_t i = 0; i < msg.payloadLen; i++)
        //     printf("%d ", msg.payload[i]);
        // printf("]\n");

        retVal = true;
        switch(msg.cmdCode)
        {
            case DBG_CODE_ACK_NACK:
            {
                if(msg.payloadLen > 1)
                {
                    if(DBG_ACK_VALUE == msg.payload[1])
                        ESP_LOGI(CAM_CMD_TAG, "Received ack for cmd 0x%2X", msg.payload[0]);
                    else
                        ESP_LOGW(CAM_CMD_TAG, "Received nack for cmd 0x%2X", msg.payload[0]);
                }
                break;
            }
            case CAM_RECEIVED_PHOTO_CHUNK:
            {
                ESP_LOGD(CAM_CMD_TAG, "Received Photo Chunk");
                break;
            }
            case CAM_RECEIVED_QR_DATA:
            {
                ESP_LOGI(CAM_CMD_TAG, "Received QR data");
                break;
            }
            default:
            {
                /* Unknown message code: No response */
                retVal = false;
                break;
            }
        }
    }
    return retVal;
}

void cam_tx_task(void *arg)
{
    uint8_t message_to_send[CAM_MESSAGE_LENGTH];
    while(true) 
    {
        if(xQueueReceive(xQueueCamUartTx, (void*) &message_to_send, 10/portTICK_RATE_MS) > 0)
        {
            send_uart2(message_to_send);
            ESP_LOGD(CAM_CMD_TAG, "Message Sent - Queue contains %d remaining element", uxQueueMessagesWaiting(xQueueCamUartTx)); 
        }
        else
        {
            //No message to send   
        }
    }
}

void cam_rx_task(void* arg)
{
    uint8_t* data = (uint8_t*) malloc(CAM_MESSAGE_LENGTH);
    uint8_t* ack_data = (uint8_t*) malloc(8);
    while(true) 
    {
        memset(data, 0, CAM_MESSAGE_LENGTH);
        memset(ack_data, 0, 8);
        const int rxBytes = uart_read_bytes(CONFIG_CAM_UART_INTERFACE_NUMBER, data, CAM_MESSAGE_LENGTH, 10/portTICK_RATE_MS);
        if(rxBytes > 0) 
        {
            data[rxBytes] = 0;
            ESP_LOGD(CAM_CMD_TAG, "Received %d byte", rxBytes);
            ESP_LOG_BUFFER_HEXDUMP("DBG RX Task - MSG", data, rxBytes, ESP_LOG_DEBUG);

            if(true == process_msg2(data, rxBytes))
                ESP_LOGD(CAM_CMD_TAG, "Message correct");
            else
                ESP_LOGE(CAM_CMD_TAG, "Failed to process received message");
        }
        else
        {
            ESP_LOGD(CAM_CMD_TAG, "Nothing received");
        }
    }
    free(data);
}

static void cam_init_uart(void)
{
    const uart_config_t uart_config = {
        .baud_rate = CONFIG_CAM_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = (uart_parity_t)CONFIG_CAM_UART_PARITY_FLAG,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    ESP_ERROR_CHECK(uart_param_config(CONFIG_CAM_UART_INTERFACE_NUMBER, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(CONFIG_CAM_UART_INTERFACE_NUMBER, CONFIG_CAM_UART_TXD, CONFIG_CAM_UART_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(CONFIG_CAM_UART_INTERFACE_NUMBER, RX_BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_flush(CONFIG_CAM_UART_INTERFACE_NUMBER));
}

void cam_task(void *pvParameter)
{
    ESP_LOGI(CAM_TAG, "Cam task started");

    cam_init_uart();
    utils::system::start_thread_core(&cam_tx_task, &xHandleCamTx, "cam_tx_task", 1024*4, 4, 0);
    utils::system::start_thread_core(&cam_rx_task, &xHandleCamRx, "cam_rx_task", 1024*4, 4, 0);
    vTaskDelete(nullptr);
}