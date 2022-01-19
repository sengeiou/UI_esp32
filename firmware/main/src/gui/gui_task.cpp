#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"

/* Littlevgl specific */
#include "board.h"
#include "ui_main.h"
#include "lv_port_fs.h"
#include "lvgl_gui.h"
#include "variables.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "GUI_TASK"
#else
    #define LOG_TAG "GUI_TASK"
#endif


static scr_driver_t g_lcd;
static touch_panel_driver_t g_touch;

#if (CONFIG_BOARD_LAVAZZA_4_3 == 1)
void init_board(void)
{
    esp_err_t ret = ESP_OK;
    
    i2s_lcd_config_t i2s_lcd_cfg = {
        .data_width  = BOARD_LCD_I2S_BITWIDTH,
        .pin_data_num = {
            BOARD_LCD_I2S_D0_PIN,
            BOARD_LCD_I2S_D1_PIN,
            BOARD_LCD_I2S_D2_PIN,
            BOARD_LCD_I2S_D3_PIN,
            BOARD_LCD_I2S_D4_PIN,
            BOARD_LCD_I2S_D5_PIN,
            BOARD_LCD_I2S_D6_PIN,
            BOARD_LCD_I2S_D7_PIN,
            BOARD_LCD_I2S_D8_PIN,
            BOARD_LCD_I2S_D9_PIN,
            BOARD_LCD_I2S_D10_PIN,
            BOARD_LCD_I2S_D11_PIN,
            BOARD_LCD_I2S_D12_PIN,
            BOARD_LCD_I2S_D13_PIN,
            BOARD_LCD_I2S_D14_PIN,
            BOARD_LCD_I2S_D15_PIN,
        },
        .pin_num_cs = BOARD_LCD_I2S_CS_PIN,
        .pin_num_wr = BOARD_LCD_I2S_WR_PIN,
        .pin_num_rs = BOARD_LCD_I2S_RS_PIN,
        .clk_freq = 20000000,
        .i2s_port = I2S_NUM_0,
        .swap_data = false,
        .buffer_size = 64000,
    };

    scr_interface_driver_t *iface_drv;
    scr_interface_create(SCREEN_IFACE_8080, &i2s_lcd_cfg, &iface_drv);

    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = BOARD_LCD_I2S_RESET_PIN,
        .pin_num_bckl = BOARD_LCD_I2S_BL_PIN,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .width = BOARD_LCD_WIDTH,
        .height = BOARD_LCD_HEIGHT,
        .offset_hor = 0,
        .offset_ver = 0,
        .rotate = SCR_DIR_TBRL,
    };

    ret = scr_find_driver(SCREEN_CONTROLLER_RM68120, &g_lcd);
    if(ESP_OK != ret)
    {
        return;
        ESP_LOGE(LOG_TAG, "screen find failed");
    }

    ret = g_lcd.init(&lcd_cfg);

    if(ESP_OK != ret)
    {
        return;
        ESP_LOGE(LOG_TAG, "screen initialize failed");
    }

    scr_info_t g_lcd_info;
    g_lcd.get_info(&g_lcd_info);
    ESP_LOGI(LOG_TAG, "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);

    #if TOUCH_INIT
    i2c_config_t i2c_conf = {
        .mode = BOARD_I2C0_MODE,
        .sda_io_num = BOARD_TOUCH_I2C_SDA_PIN,
        .sda_pullup_en = BOARD_I2C0_SDA_PULLUP_EN,
        .scl_io_num = BOARD_TOUCH_I2C_SCL_PIN,
        .scl_pullup_en = BOARD_I2C0_SCL_PULLUP_EN,
        .master.clk_speed = BOARD_I2C0_SPEED,
    };
    i2c_bus_handle_t i2c_bus = i2c_bus_create(I2C_NUM_0, &i2c_conf);

    touch_panel_config_t touch_cfg = {
        .interface_i2c = {
            .i2c_bus = i2c_bus,
            .clk_freq = BOARD_TOUCH_I2C_CLOCK_FREQ,
            .i2c_addr = 0x38,
        },
        .interface_type = TOUCH_PANEL_IFACE_I2C,
        .pin_num_int = -1,
        .direction = SCR_DIR_TBRL,
        .width = BOARD_LCD_WIDTH,
        .height = BOARD_LCD_HEIGHT,
    };

    /* Initialize touch panel controller FT5x06 */
    touch_panel_find_driver(TOUCH_PANEL_CONTROLLER_FT5X06, &g_touch);
    g_touch.init(&touch_cfg);

    /* start to run calibration */
    // g_touch.calibration_run(&g_lcd, false);
    #endif

    vTaskDelay(pdMS_TO_TICKS(100));
}
#elif (CONFIG_BOARD_LAVAZZA_3_5 == 1)
void init_board(void)
{
    esp_err_t ret = ESP_OK;
    
    iot_board_init();
    spi_bus_handle_t spi_bus = iot_board_get_handle(BOARD_SPI2_ID);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = BOARD_LCD_SPI_CS_PIN,
        .pin_num_dc = BOARD_LCD_SPI_DC_PIN,
        .clk_freq = BOARD_LCD_SPI_CLOCK_FREQ,
        .swap_data = true,
    };

    scr_interface_driver_t *iface_drv;
    scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv);
    ret = scr_find_driver(SCREEN_CONTROLLER_ILI9486, &g_lcd);
    if (ESP_OK != ret) {
        return;
        ESP_LOGE(TAG, "screen find failed");
    }

    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = BOARD_LCD_SPI_RESET_PIN,
        .pin_num_bckl = BOARD_LCD_SPI_BL_PIN,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .width = BOARD_LCD_WIDTH,
        .height = BOARD_LCD_HEIGHT,
        .offset_hor = 0,
        .offset_ver = 0,
        .rotate = SCR_DIR_TBRL,
    };
    ret = g_lcd.init(&lcd_cfg);

    if(ESP_OK != ret)
    {
        return;
        ESP_LOGE(TAG, "screen initialize failed");
    }

    scr_info_t g_lcd_info;
    g_lcd.get_info(&g_lcd_info);
    ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);

    touch_panel_config_t touch_cfg = {
        .interface_spi = {
            .spi_bus = spi_bus,
            .pin_num_cs = BOARD_TOUCH_SPI_CS_PIN,
            .clk_freq = BOARD_LCD_SPI_CLOCK_FREQ,
        },
        .interface_type = TOUCH_PANEL_IFACE_SPI,
        .pin_num_int = BOARD_TOUCH_IRQ_TOUCH_PIN,
        .direction = SCR_DIR_TBRL,
        .width = BOARD_LCD_WIDTH,
        .height = BOARD_LCD_HEIGHT,
    };

    /* Initialize touch panel controller XPT2046 */
    touch_panel_find_driver(TOUCH_PANEL_CONTROLLER_XPT2046, &g_touch);
    g_touch.init(&touch_cfg);

    /* start to run calibration */
    // g_touch.calibration_run(&g_lcd, false);
    
    vTaskDelay(pdMS_TO_TICKS(100));
}
#else   //Board custom

#endif

void gui_task(void* data)
{
    /* Initialize Board */
    init_board();

    /* Initialize LittlevGL GUI */
    lvgl_init(&g_lcd, nullptr);

    /* Init LVGL file system API */
    lv_port_fs_init();

    /* Start main UI */
    ui_main();

    EventBits_t bits;
    while(true)
    {
        bits = xEventGroupWaitBits(xGuiEvents, GUI_POWER_BIT | GUI_STATISTICS_DATA_BIT | GUI_SETTINGS_DATA_BIT | 
                                               GUI_NEW_EROGATION_DATA_BIT | GUI_STOP_EROGATION_BIT | GUI_ENABLE_CAPPUCCINO_BIT |
                                               GUI_WARNINGS_BIT | GUI_MACHINE_FAULT_BIT | GUI_CLOUD_REQUEST_BIT | GUI_NEW_CLEANING_DATA_BIT, 
                                               pdFALSE, pdFALSE, 2000/portTICK_PERIOD_MS);

        if(bits & GUI_POWER_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_POWER_BIT);
            ui_preparations_set_power(guiInternalState.powerOn);
        }

        if(bits & GUI_NEW_EROGATION_DATA_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_NEW_EROGATION_DATA_BIT);
            ui_erogation_update(guiInternalState.erogation.dose, guiInternalState.erogation.temperature, 5.0f);
        }

        if(bits & GUI_NEW_CLEANING_DATA_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_NEW_CLEANING_DATA_BIT);
            if(guiInternalState.cleaning.recipeId == 0)
            {
                ui_cleaning_fast_update(guiInternalState.cleaning.currentSteps, guiInternalState.cleaning.totalSteps);
            }
            else if(guiInternalState.cleaning.recipeId == 1)
            {
                ui_cleaning_full_update(guiInternalState.cleaning.currentSteps, guiInternalState.cleaning.totalSteps);
            }
            else
            {
                //Do nothing
            }

        }

        if(bits & GUI_STOP_EROGATION_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_STOP_EROGATION_BIT);
            ui_erogation_completed();
        }

        if(bits & GUI_ENABLE_CAPPUCCINO_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_ENABLE_CAPPUCCINO_BIT);
            ui_preparations_enable_cappuccino(guiInternalState.milkHeadPresence);
        }

        if(bits & GUI_WARNINGS_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_WARNINGS_BIT);
            ui_status_bar_set_descaling_warning(guiInternalState.warnings.descaling);
            ui_status_bar_set_water_empty_warning(guiInternalState.warnings.waterEmpty);
            ui_status_bar_set_pod_warning(guiInternalState.warnings.podFull);
        }

        if(bits & GUI_MACHINE_FAULT_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_MACHINE_FAULT_BIT);
            ui_preparations_set_fault(guiInternalState.isFault);
        }

        if(bits & GUI_CLOUD_REQUEST_BIT)
        {
            xEventGroupClearBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
            ui_preparations_set_desired(guiInternalState.cloudReq.coffeeType);
        }

        #if LOG_MEM_INFO
        static char buffer[128];    /* Make sure buffer is enough for `sprintf` */
        sprintf(buffer, "   Biggest /     Free /    Total\n"
            "\t DRAM : [%8d / %8d / %8d]\n"
            "\tPSRAM : [%8d / %8d / %8d]",
            heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL),
            heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
            heap_caps_get_total_size(MALLOC_CAP_INTERNAL),
            heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM),
            heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
            heap_caps_get_total_size(MALLOC_CAP_SPIRAM));
        ESP_LOGI(LOG_TAG, "%s", buffer);
        #endif
    }
    
    vTaskDelete(NULL);
}
