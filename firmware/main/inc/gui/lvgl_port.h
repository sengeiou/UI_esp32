#pragma once

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"

/* Littlevgl specific */
#include "lvgl.h"
#include "lvgl_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_font_t font_clock_32;
extern lv_font_t font_clock_108;
extern lv_font_t font_en_18;
extern lv_font_t font_en_20;
extern lv_font_t font_en_24;
extern lv_font_t font_en_28;
extern lv_font_t font_en_32;
extern lv_font_t font_en_36;
extern lv_font_t font_en_40;
extern lv_font_t font_en_48;
extern lv_font_t font_en_bold_16;
extern lv_font_t font_en_bold_20;
extern lv_font_t font_en_bold_24;
extern lv_font_t font_en_bold_28;
extern lv_font_t font_en_bold_32;
extern lv_font_t font_en_bold_36;
extern lv_font_t font_en_bold_48;
extern lv_font_t font_en_bold_60;
extern lv_font_t font_en_bold_72;
extern lv_font_t font_en_bold_24;
extern lv_font_t font_en_bold_28;
extern lv_font_t font_en_bold_48;
extern lv_font_t font_en_20_compress;
extern lv_font_t font_kb_24;
extern lv_font_t font_symbol_28;
extern lv_font_t font_en_thin_20;
extern lv_font_t font_bar_symbol;
                                
#define LVGL_SCR_SIZE       (LV_HOR_RES_MAX * LV_VER_RES_MAX)
#define LVGL_BUFFER_SIZE    (sizeof(lv_color_t) * LVGL_SCR_SIZE)
#define LVGL_INIT_PIXCNT    (LV_HOR_RES_MAX * LV_VER_RES_MAX)

#define LV_BUF_ALLOC_INTERNAL   (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#define LV_BUF_ALLOC_EXTERNAL   (MALLOC_CAP_SPIRAM) 

typedef struct {
    size_t buffer_size;
    uint32_t buffer_alloc_caps;
} lvgl_port_init_config_t;

/**
 * @brief Initialize LVGL with config
 * 
 * @param cfg 
 * @return esp_err_t 
 */
esp_err_t lvgl_init(size_t buffer_pix_size, uint32_t buffer_caps);

/**
 * @brief Take LVGL widget update and flush semaphore
 * 
 * @return esp_err_t 
 */
esp_err_t lv_port_sem_take(void);

/**
 * @brief Give LVGL widget update and flush semaphore
 * 
 * @return esp_err_t 
 */
esp_err_t lv_port_sem_give(void);

/**
 * @brief Use default ported lvgl handler task or not.
 * 
 * @param en Enable or not.
 */
void lv_port_use_default_handler(bool en);

#ifdef __cplusplus
}
#endif