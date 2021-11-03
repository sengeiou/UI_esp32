#include "dbg_task.h"

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

#include "dbg_protocol.h"

#define RX_BUF_SIZE     512


#ifdef ADVANCED_DEBUG
    #define DBG_TAG LINE_STRING "|" "DBG_TASK"
    #define DBG_CMD_TAG LINE_STRING "|" "DBG_COMMAND_TASK"
#else
    #define DBG_TAG "DBG_TASK"
    #define DBG_CMD_TAG "DBG_CMD_TASK"
#endif

void add_to_queue(QueueHandle_t xQueue, const uint8_t* data);

static int send_uart(const uint8_t* data)
{
    const int len = data[2] + 5;
    const int txBytes = uart_write_bytes(CONFIG_DBG_UART_INTERFACE_NUMBER, data, len);

    ESP_LOG_BUFFER_HEXDUMP("SEND MESSAGE", data, len, ESP_LOG_DEBUG);
    return txBytes;
}


void enable_livedata(void)
{
    printf("Enable livedata\n");
    static const uint8_t data[] = {0xA5, 0x20, 0x01, 0x01, 0x77, 0x25};
    add_to_queue(xQueueDbgUartTx, data);
}

void disable_livedata(void)
{
    printf("Disable livedata\n");
    static const uint8_t data[] = {0xA5, 0x20, 0x01, 0x00, 0x67, 0x04};
    add_to_queue(xQueueDbgUartTx, data);
}

void special_function(dbg_special_func_code_t cmd)
{
    // uint8_t data[] = {0xA5, 0x36, 0x01, 0x00, 0x96, 0xC7}
    // uint8_t data[] = {0xA5, 0x36, 0x01, 0x01, 0x86, 0xE6}
    
    printf("Special function 0.%2X\n", cmd);
    switch(cmd)
    {
        case DBG_ON_OFF:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x04, 0xD6, 0x43};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_SHORT_COFFEE:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x05, 0xC6, 0x62};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_MEDIUM_COFFEE:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x06, 0xF6, 0x01};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_LONG_COFFEE:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x07, 0xE6, 0x20};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_FREE_COFFEE:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x08, 0x17, 0xCF};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_SHORT_CAPPUCCINO:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x11, 0x94, 0xD7};
            
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_MEDIUM_CAPPUCCINO:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x12, 0xA4, 0xB4};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_DOUBLE_CAPPUCCINO:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x13, 0xB4, 0x95};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_HOT_MILK:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x14, 0xC4, 0x72};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }
        case DBG_COLD_MILK:
        {
            uint8_t data[] = {0xA5, 0x36, 0x01, 0x15, 0xD4, 0x53};
            add_to_queue(xQueueDbgUartTx, data);
            break;
        }       
    }
}


void add_to_queue(QueueHandle_t xQueue, const uint8_t* data)
{
    if((xQueue != nullptr) && (xQueueSend(xQueue, (void *)data, (TickType_t) 10) != pdPASS))
    {
        ESP_LOGW(DBG_TAG, "Unable to add to queue");
    }
    else
    {
        ESP_LOGD(DBG_TAG, "Command sent to queue");
    }
}

bool process_msg(uint8_t* message)
{
    bool retVal = false;
    dbgMsg_stc msg;
    if(true == lavazza::dbg::checkMsg(message, msg))
    {
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
                        ESP_LOGI(DBG_CMD_TAG, "Rreceived ack for cmd 0x%2X", msg.payload[0]);
                    else
                        ESP_LOGW(DBG_CMD_TAG, "Received nack for cmd 0x%2X", msg.payload[0]);
                }
                break;
            }
            case DBG_CODE_GET_LIVE_DATA:
            {
                ESP_LOGD(DBG_CMD_TAG, "Received livedata");
                lavazza::dbg::parseLivedata(msg);
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
    else
    {
        /* Failed to check message */
        retVal = false;
    }
    return retVal;
}

void dbg_tx_task(void *arg)
{
    uint8_t message_to_send[DBG_MESSAGE_LENGTH];
    while(true) 
    {
        if(xQueueReceive(xQueueDbgUartTx, (void*) &message_to_send, 10/portTICK_RATE_MS) > 0)
        {
            send_uart(message_to_send);
            ESP_LOGD(DBG_CMD_TAG, "Message Sent - Queue contains %d remaining element", uxQueueMessagesWaiting(xQueueDbgUartTx)); 
        }
        else
        {
            //No message to send   
        }
    }
}

void dbg_rx_task(void* arg)
{
    uint8_t* data = (uint8_t*) malloc(DBG_MESSAGE_LENGTH);
    uint8_t* ack_data = (uint8_t*) malloc(8);
    while(true) 
    {
        memset(data, 0, DBG_MESSAGE_LENGTH);
        memset(ack_data, 0, 8);
        const int rxBytes = uart_read_bytes(CONFIG_DBG_UART_INTERFACE_NUMBER, data, DBG_MESSAGE_LENGTH, 10/portTICK_RATE_MS);
        if(rxBytes > 0) 
        {
            data[rxBytes] = 0;
            ESP_LOGD(DBG_CMD_TAG, "Received %d byte", rxBytes);
            ESP_LOG_BUFFER_HEXDUMP("DBG RX Task - MSG", data, rxBytes, ESP_LOG_DEBUG);

            if(true == process_msg(data))
                ESP_LOGD(DBG_CMD_TAG, "Message correct");
            else
                ESP_LOGE(DBG_CMD_TAG, "Failed to process received message");
        }
        else
        {
            ESP_LOGD(DBG_CMD_TAG, "Nothing received");
        }
    }
    free(data);
}

static void dbg_init_uart(void)
{
    const uart_config_t uart_config = {
        .baud_rate = CONFIG_DBG_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = (uart_parity_t)CONFIG_DBG_UART_PARITY_FLAG,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    ESP_ERROR_CHECK(uart_param_config(CONFIG_DBG_UART_INTERFACE_NUMBER, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(CONFIG_DBG_UART_INTERFACE_NUMBER, CONFIG_DBG_UART_TXD, CONFIG_DBG_UART_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(CONFIG_DBG_UART_INTERFACE_NUMBER, RX_BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_flush(CONFIG_DBG_UART_INTERFACE_NUMBER));
}

void dbg_task(void *pvParameter)
{
    ESP_LOGI(DBG_TAG, "DBG task started");

    dbg_init_uart();
    utils::system::start_thread_core(&dbg_tx_task, &xHandleDbgTx, "dbg_tx_task", 1024*4, 4, 0);
    disable_livedata();
    utils::system::start_thread_core(&dbg_rx_task, &xHandleDbgRx, "dbg_rx_task", 1024*14, 4, 0);
    vTaskDelete(nullptr);
}