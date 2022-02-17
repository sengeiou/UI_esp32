/**
 * @file sys_check.c
 * @brief System requirements check.
 * @version 0.1
 * @date 2021-04-14
 * 
 * @copyright Copyright 2021 Espressif Systems (Shanghai) Co. Ltd.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "sys_check.h"
#include "lv_port_fs.h"
#include "esp_log.h"

static const char *TAG = "sys_check";

extern lv_font_t default_medium_font;

#define LOG_TRACE(...)  ESP_LOGI(TAG, ##__VA_ARGS__)
#define CHECK(a, str, ret_val) do { \
        if (!(a)) { \
            err_show(str); \
            ESP_LOGE(TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
            return (ret_val); \
        } else { \
            LOG_TRACE(str); \
        } \
    } while(0)

static lv_obj_t *msg_box = NULL;

/**
 * @brief Show error message on msgbox. Create one if not created.
 * 
 * @param text Text to show on msgbox. lv_msgbox will create a copy of text.
 */
static void err_show(const char *text)
{
    if (NULL == msg_box) {
        msg_box = lv_msgbox_create(lv_scr_act(), NULL);
        lv_obj_set_style_local_text_font(msg_box, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &default_medium_font);
        lv_obj_set_style_local_border_width(msg_box, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, 0);

    }
    lv_msgbox_set_text(msg_box, text);
    lv_obj_align(msg_box, NULL, LV_ALIGN_CENTER, 0, 30);
}

esp_err_t sys_check(void)
{
    CHECK(heap_caps_get_total_size(MALLOC_CAP_SPIRAM) > 1 * 1024 * 1024 + 512 * 1024,
        "Check for PSRAM avaliable size",
        ESP_FAIL);

    return ESP_OK;
}

esp_err_t img_file_check(const char *lv_img_path)
{
    lv_fs_file_t file;
    if (LV_FS_RES_OK == lv_fs_open(&file, lv_img_path, LV_FS_MODE_RD)) {
        lv_fs_close(&file);
        return ESP_OK;
    }

    err_show("Check for image resources!");
    
    return ESP_FAIL;
}
