#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "types.h"
#include "variables.h"
#include "utils.h"
#include "system_utils.h"

#include "sys/time.h"
#include "esp_sntp.h"
#include <cstring>
#include <thread>
#include <chrono>

#include "dbg_protocol.h"
#include "camera_protocol.h"
#include "board.h"
#include "sdkconfig.h"

#ifdef ADVANCED_DEBUG
    #define TAG_UTILS LINE_STRING "|" "UTILS"
#else
    #define TAG_UTILS "UTILS"
#endif


static void obtain_time(bool& sync)
{
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;

    while(timeinfo.tm_year < (2020 - 1900) && true == sync) 
    {
        ESP_LOGI(TAG_UTILS, "Waiting for system time to be set... tm_year:%d[times:%d]", timeinfo.tm_year, ++retry);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}

time_t sntp_get_current_time_seconds(bool& sync)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2020 - 1900))
    {
        ESP_LOGI(TAG_UTILS, "Time is not set yet. Connecting to network and getting time over NTP. timeinfo.tm_year:%d", timeinfo.tm_year);
        obtain_time(sync);
        // update 'now' variable with current time
        time(&now);
    }
    localtime_r(&now, &timeinfo);
    return now;
}

void get_time_from_sntp(bool& sync)
{
    ESP_LOGI(TAG_UTILS, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    
    sntp_init();
    uint32_t ts = 0;
    uint8_t retry = 0;

    while(ts == 0 && sync == true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        ts = sntp_get_current_time_seconds(sync);
        ESP_LOGI(TAG_UTILS, "Getting current time, retry %d, sync is %d", retry++, sync);
    }

    if(true == sync)
    {
        ESP_LOGI(TAG_UTILS, "ESP platform sntp inited!");

        time_t now = sntp_get_current_time_seconds(sync);

        char strftime_buf[64];
        struct tm timeinfo;

        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG_UTILS, "The current date/time is: %s", strftime_buf);
    }

    sntp_stop();
}

void firmware_restart(uint8_t delay_sec)
{
    ESP_LOGI(TAG_UTILS, "Reboting firmware");
    
    std::this_thread::sleep_for(std::chrono::seconds(2));  //Wait pending actions

    for(int i = delay_sec; i > 0; i--)
    {
        ESP_LOGI(TAG_UTILS, "Reboting in %d %s...", i, ((i==1) ? "second" : "seconds"));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ESP_LOGI(TAG_UTILS, "Reboot");
    esp_restart();
}

void init_queue(void)
{
    xQueueAzureTx = xQueueCreate(5, sizeof(azure_queue_message_t));
    xQueueDbgUartTx = xQueueCreate(5, DBG_MESSAGE_LENGTH*sizeof(uint8_t));
    xQueueCamUartTx = xQueueCreate(5, CAM_MESSAGE_LENGTH*sizeof(uint8_t));
}

void init_gpio(void)
{
    #if (CONFIG_BOARD_LAVAZZA_4_3 == 1)
    /* Touch controller (I2C) */
    gpio_reset_pin((gpio_num_t)BOARD_IO_I2C0_SCL);
    gpio_reset_pin((gpio_num_t)BOARD_IO_I2C0_SDA);
    gpio_reset_pin((gpio_num_t)BOARD_TOUCH_I2C_INT_PIN);

    /* TFT display (I2S) */
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_WR_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_RS_PIN;
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D0_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D1_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D2_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D3_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D4_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D5_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D6_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D7_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D8_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D9_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D10_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D11_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D12_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D13_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D14_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_I2S_D15_PIN);    
    #elif (CONFIG_BOARD_LAVAZZA_3_5 == 1)
    gpio_reset_pin((gpio_num_t)BOARD_IO_SPI2_MOSI);
    gpio_reset_pin((gpio_num_t)BOARD_IO_SPI2_MISO);
    gpio_reset_pin((gpio_num_t)BOARD_IO_SPI2_SCK);

    /* Touch controller (SPI) */
    gpio_reset_pin((gpio_num_t)BOARD_TOUCH_SPI_CS_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_TOUCH_IRQ_TOUCH_PIN);

    /* TFT display (SPI) */
    gpio_reset_pin((gpio_num_t)BOARD_LCD_SPI_CS_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_SPI_DC_PIN);
    gpio_reset_pin((gpio_num_t)BOARD_LCD_SPI_RESET_PIN);
    #else
    //TODO clean your used pins
    #endif

    /* DBG Uart */
    gpio_reset_pin((gpio_num_t)CONFIG_DBG_UART_RXD);
    gpio_reset_pin((gpio_num_t)CONFIG_DBG_UART_TXD);

    /* Camera Uart */
    // gpio_reset_pin((gpio_num_t)CONFIG_CAM_UART_RXD);
    // gpio_reset_pin((gpio_num_t)CONFIG_CAM_UART_TXD);
}
