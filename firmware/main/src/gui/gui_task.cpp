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
#include "ui_main.h"
#include "lvgl_port.h"
#include "lv_port_fs.h"

#define LCD_SIZE_BUFF (LV_HOR_RES_MAX * LV_VER_RES_MAX)

#define LV_TICK_PERIOD_MS 1
static void lv_tick_task(void *arg)
{
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void gui_task(void* data)
{
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Init LVGL, allocating buffer and create tasks for tick and handler */
    ESP_ERROR_CHECK(lvgl_init(DISP_BUF_SIZE, MALLOC_CAP_DMA));


    /* Init LVGL file system API */
    lv_port_fs_init();

    /* Start main UI */
    ui_main();

    #if LOG_MEM_INFO
    static char buffer[128];    /* Make sure buffer is enough for `sprintf` */
    while (1)
    {
        sprintf(buffer, "   Biggest /     Free /    Total\n"
            "\t DRAM : [%8d / %8d / %8d]\n"
            "\tPSRAM : [%8d / %8d / %8d]",
            heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL),
            heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
            heap_caps_get_total_size(MALLOC_CAP_INTERNAL),
            heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM),
            heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
            heap_caps_get_total_size(MALLOC_CAP_SPIRAM));
        ESP_LOGI("MEM", "%s", buffer);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    #endif
    
    vTaskDelete(NULL);
}
