#ifndef _IOT_BOARD_H_
#define _IOT_BOARD_H_

#include "esp_err.h"
#include "i2c_bus.h"
#include "spi_bus.h"

/*ENABLE Initialization Process in _board_init(void)*/
#define _ENABLE 1
#define _DISABLE 0
#define _UNDEFINE
typedef void* board_res_handle_t;

/*Definations of Board*/
#define BOARD_NAME "Lavazza-Small"
#define BOARD_VENDOR "Lavazza"
#define BOARD_URL "null"

#define CONFIG_BOARD_SPI2_INIT 1
#define CONFIG_BOARD_I2C0_INIT 0
/**
 * Resource ID on Board,
 * You can use iot_board_get_handle(ID,handle) to refrence Resource's Handle
 * */
typedef enum {
    NULL_RESOURCE,
    BOARD_I2C0_ID,
    BOARD_SPI2_ID,
}board_res_id_t;

#define BOARD_IO_SPI2_SCK 22
#define BOARD_IO_SPI2_MOSI 27
#define BOARD_IO_SPI2_MISO 33


/**< Screen interface pins */
#define BOARD_LCD_SPI_HOST 1
#define BOARD_LCD_SPI_CLOCK_FREQ 20000000
#define BOARD_LCD_SPI_MISO_PIN BOARD_IO_SPI2_MISO
#define BOARD_LCD_SPI_MOSI_PIN BOARD_IO_SPI2_MOSI
#define BOARD_LCD_SPI_CLK_PIN BOARD_IO_SPI2_SCK
#define BOARD_LCD_SPI_CS_PIN 23
#define BOARD_LCD_SPI_DC_PIN 25
#define BOARD_LCD_SPI_RESET_PIN 32
#define BOARD_LCD_SPI_BL_PIN -1


/**< Touch panel interface pins */
/**
 * When both the screen and the touch panel are SPI interfaces, 
 * they can choose to share a SPI host.
 */
#define BOARD_TOUCH_SPI_CS_PIN 0
#define BOARD_TOUCH_IRQ_TOUCH_PIN 21

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
