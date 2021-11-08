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

#include "variables.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "GUI_TASK"
#else
    #define LOG_TAG "GUI_TASK"
#endif

#define LCD_SIZE_BUFF       (LV_HOR_RES_MAX * LV_VER_RES_MAX)
#define LV_TICK_PERIOD_MS   1

static void lv_tick_task(void* arg)
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

    EventBits_t bits;
    while(true)
    {
        bits = xEventGroupWaitBits(xGuiEvents, GUI_POWER_BIT | GUI_STATISTICS_DATA_BIT | GUI_SETTINGS_DATA_BIT | 
                                               GUI_NEW_EROGATION_DATA_BIT | GUI_STOP_EROGATION_BIT | GUI_ENABLE_CAPPUCCINO_BIT |
                                               GUI_WARNINGS_BIT | GUI_MACHINE_FAULT_BIT | GUI_CLOUD_REQUEST_BIT, 
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
