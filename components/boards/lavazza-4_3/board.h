#ifndef _IOT_BOARD_H_
#define _IOT_BOARD_H_

#include "esp_err.h"
#include "i2c_bus.h"
#include "spi_bus.h"

/*ENABLE Initialization Process in _board_init(void)*/
#define _ENABLE 1
#define _DISABLE 0
typedef void* board_res_handle_t;

/*Definations of Board*/
#define BOARD_NAME "Lavazza-Medium"
#define BOARD_VENDOR "Lavazza"
#define BOARD_URL "null"

#define CONFIG_BOARD_SPI2_INIT 0

/**
 * Resource ID on Board,
 * You can use iot_board_get_handle(ID,handle) to refrence Resource's Handle
 * */
typedef enum {
    NULL_RESOURCE,
    BOARD_I2C0_ID,
    BOARD_SPI2_ID,
}board_res_id_t;

#if CONFIG_IDF_TARGET_ESP32
#define CONFIG_BOARD_I2C0_INIT 0
#define BOARD_IO_I2C0_SCL 48
#define BOARD_IO_I2C0_SDA 45
#elif CONFIG_IDF_TARGET_ESP32S3
#define CONFIG_BOARD_I2C0_INIT 1
#define BOARD_IO_I2C0_SCL 4
#define BOARD_IO_I2C0_SDA 5
#endif

#define BOARD_LCD_WIDTH     480
#define BOARD_LCD_HEIGHT    800

/*Definations of Peripheral*/
#define BOARD_I2C0_MODE I2C_MODE_MASTER
#define BOARD_I2C0_SPEED (100000)
#define BOARD_I2C0_SCL_PULLUP_EN _ENABLE
#define BOARD_I2C0_SDA_PULLUP_EN _ENABLE

/**< Screen inrerface pins */
#define BOARD_LCD_I2S_BITWIDTH 16
#define BOARD_LCD_I2S_PORT_NUM 0
#define BOARD_LCD_I2S_BL_PIN -1
#define BOARD_LCD_I2S_RESET_PIN -1
#define BOARD_LCD_I2S_CS_PIN -1

#if CONFIG_IDF_TARGET_ESP32
#define BOARD_LCD_I2S_WR_PIN 3
#define BOARD_LCD_I2S_RS_PIN 5
#define BOARD_LCD_I2S_D0_PIN 19
#define BOARD_LCD_I2S_D1_PIN 21
#define BOARD_LCD_I2S_D2_PIN 0
#define BOARD_LCD_I2S_D3_PIN 22
#define BOARD_LCD_I2S_D4_PIN 23
#define BOARD_LCD_I2S_D5_PIN 33
#define BOARD_LCD_I2S_D6_PIN 32
#define BOARD_LCD_I2S_D7_PIN 27
#if (BOARD_LCD_I2S_BITWIDTH == 16)
#define BOARD_LCD_I2S_D8_PIN 25
#define BOARD_LCD_I2S_D9_PIN 26
#define BOARD_LCD_I2S_D10_PIN 12
#define BOARD_LCD_I2S_D11_PIN 13
#define BOARD_LCD_I2S_D12_PIN 14
#define BOARD_LCD_I2S_D13_PIN 15
#define BOARD_LCD_I2S_D14_PIN 2
#define BOARD_LCD_I2S_D15_PIN 4
#else
#define BOARD_LCD_I2S_D8_PIN -1
#define BOARD_LCD_I2S_D9_PIN -1
#define BOARD_LCD_I2S_D10_PIN -1
#define BOARD_LCD_I2S_D11_PIN -1
#define BOARD_LCD_I2S_D12_PIN -1
#define BOARD_LCD_I2S_D13_PIN -1
#define BOARD_LCD_I2S_D14_PIN -1
#define BOARD_LCD_I2S_D15_PIN -1
#endif
#elif CONFIG_IDF_TARGET_ESP32S3
#define BOARD_LCD_I2S_WR_PIN 3
#define BOARD_LCD_I2S_RS_PIN 8
#define BOARD_LCD_I2S_D0_PIN 46
#define BOARD_LCD_I2S_D1_PIN 10
#define BOARD_LCD_I2S_D2_PIN 15
#define BOARD_LCD_I2S_D3_PIN 13
#define BOARD_LCD_I2S_D4_PIN 14
#define BOARD_LCD_I2S_D5_PIN 0
#define BOARD_LCD_I2S_D6_PIN 45
#define BOARD_LCD_I2S_D7_PIN 17
#if (BOARD_LCD_I2S_BITWIDTH == 16)
#define BOARD_LCD_I2S_D8_PIN 18
#define BOARD_LCD_I2S_D9_PIN 48
#define BOARD_LCD_I2S_D10_PIN 39
#define BOARD_LCD_I2S_D11_PIN 41
#define BOARD_LCD_I2S_D12_PIN 38
#define BOARD_LCD_I2S_D13_PIN 6
#define BOARD_LCD_I2S_D14_PIN 7
#define BOARD_LCD_I2S_D15_PIN 16
#else
#define BOARD_LCD_I2S_D8_PIN -1
#define BOARD_LCD_I2S_D9_PIN -1
#define BOARD_LCD_I2S_D10_PIN -1
#define BOARD_LCD_I2S_D11_PIN -1
#define BOARD_LCD_I2S_D12_PIN -1
#define BOARD_LCD_I2S_D13_PIN -1
#define BOARD_LCD_I2S_D14_PIN -1
#define BOARD_LCD_I2S_D15_PIN -1
#endif
#endif

#define BOARD_LCD_I2C_PORT_NUM 0
#define BOARD_LCD_I2C_CLOCK_FREQ BOARD_I2C0_SPEED
#define BOARD_LCD_I2C_SCL_PIN BOARD_IO_I2C0_SCL
#define BOARD_LCD_I2C_SDA_PIN BOARD_IO_I2C0_SDA

/**< Touch panel interface pins */
/**
 * When both the screen and the touch panel are SPI interfaces, 
 * they can choose to share a SPI host. The board ESP32-LCDKit is this.
 */

#define BOARD_TOUCH_I2C_PORT_NUM 0
#define BOARD_TOUCH_I2C_SCL_PIN BOARD_IO_I2C0_SCL
#define BOARD_TOUCH_I2C_SDA_PIN BOARD_IO_I2C0_SDA
#if CONFIG_IDF_TARGET_ESP32
#define BOARD_TOUCH_I2C_INT_PIN 9
#elif CONFIG_IDF_TARGET_ESP32S3
#define BOARD_TOUCH_I2C_INT_PIN -1
#endif
#define BOARD_TOUCH_I2C_CLOCK_FREQ BOARD_I2C0_SPEED

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Board level init.
 *        Peripherals can be chosen through menuconfig, which will be initialized with default configurations during iot_board_init.
 *        After board init, initialized peripherals can be referenced by handles directly.
 * 
 * @return esp_err_t 
 */
esp_err_t iot_board_init(void);

/**
 * @brief Board level deinit.
 *        After board deinit, initialized peripherals will be deinit and related handles will be set to NULL.
 * 
 * @return esp_err_t 
 */
esp_err_t iot_board_deinit(void);

/**
 * @brief Check if board is initialized 
 * 
 * @return true if board is initialized
 * @return false if board is not initialized
 */
bool iot_board_is_init(void);

/**
 * @brief Using resource's ID declared in board_res_id_t to get board level resource's handle
 * 
 * @param id Resource's ID declared in board_res_id_t
 * @return board_res_handle_t Resource's handle
 * if no related handle,NULL will be returned
 */
board_res_handle_t iot_board_get_handle(board_res_id_t id);

/**
 * @brief Get board information
 * 
 * @return String include BOARD_NAME etc. 
 */
char* iot_board_get_info();

#ifdef __cplusplus
}
#endif

#endif /* _IOT_BOARD_H_ */
