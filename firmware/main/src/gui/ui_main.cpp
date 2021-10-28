#ifdef __cplusplus
extern "C" {
#endif

#include "ui_main.h"
#include "sys_check.h"
#include "lvgl_port.h"
#include "esp_log.h"

#define LOG_TAG "UI_MAIN"


void* data_on_off = NULL;
void* data_short_coffee = NULL;
void* data_medium_coffee = NULL;
void* data_long_coffee = NULL;
void* data_free_coffee = NULL;
void* data_short_cappuccino = NULL;
void* data_medium_cappuccino = NULL;
void* data_double_cappuccino = NULL;
void* data_hot_milk = NULL;
void* data_descaling_warning  = NULL;
void* data_water_warning  = NULL;
void* data_pod_warning  = NULL;
void* data_foam_boost  = NULL;
void* data_temp_boost  = NULL;

void* data_icon_app = NULL;
void* data_icon_about = NULL;

/* Resources loading list */
static ui_data_fetch_t img_fetch_list[] = {
    /* Preparation Page */
    { "S:/coffee/short_coffee.bin", &data_short_coffee },
    { "S:/coffee/medium_coffee.bin", &data_medium_coffee },
    { "S:/coffee/long_coffee.bin", &data_long_coffee },
    { "S:/coffee/free_coffee.bin", &data_free_coffee },
    { "S:/coffee/short_cappuccino.bin", &data_short_cappuccino },
    { "S:/coffee/medium_cappuccino.bin", &data_medium_cappuccino },
    { "S:/coffee/double_cappuccino.bin", &data_double_cappuccino },
    { "S:/coffee/hot_milk.bin", &data_hot_milk },
    { "S:/coffee/descaling.bin", &data_descaling_warning },
    { "S:/coffee/pod.bin", &data_pod_warning },
    { "S:/coffee/water.bin", &data_water_warning },
};

static void ui_init_internal(void);

static esp_err_t ui_call_stack_push(ui_func_desc_t *func);
static esp_err_t ui_call_stack_pop(ui_func_desc_t *func);
static esp_err_t ui_call_stack_peek(ui_func_desc_t *func);
static esp_err_t ui_call_stack_clear(void);

void ui_main(void)
{
    /* Init main screen */
    lv_port_sem_take();
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    /* Show logo */
    LV_IMG_DECLARE(lavazza_logo)
    lv_obj_t *img = lv_img_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(img, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_img_set_src(img, &lavazza_logo);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, -50);
    lv_port_sem_give();

    if(ESP_OK != sys_check())
    {
        while(1)
        {
            /* Stop at here if system check not pass */
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    lv_port_sem_take();
    /* Create a bar to update loading progress */
    lv_obj_t *bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_border_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, 2);
    lv_obj_set_size(bar, 400, 12);
    lv_obj_align(bar, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_bar_set_range(bar, 0, sizeof(img_fetch_list) / sizeof(ui_data_fetch_t));
    lv_bar_set_value(bar, 1, LV_ANIM_ON);

    lv_obj_t *label_loading_hint = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_static(label_loading_hint, " ");
    lv_obj_set_style_local_text_font(label_loading_hint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_text_color(label_loading_hint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
    lv_obj_align(label_loading_hint, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_port_sem_give();

    /* Load resource from SD crad to PSARM */
    TickType_t tick = xTaskGetTickCount();
    for(size_t i = 0; i < sizeof(img_fetch_list) / sizeof(ui_data_fetch_t); i++)
    {
        lv_port_sem_take();
        lv_bar_set_value(bar, i + 1, LV_ANIM_ON);
        lv_label_set_text_fmt(label_loading_hint, "Loading \"%s\"", img_fetch_list[i].path);
        lv_obj_align(label_loading_hint, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        lv_port_sem_give();

        if(ESP_OK != img_file_check(img_fetch_list[i].path))
        {
            /* If any of file was missing, block the task */
            while(1)
            {
                vTaskDelay(1000);
            }
        }

        ui_laod_resource(img_fetch_list[i].path, img_fetch_list[i].data);

        /* Yeah, it's only because that makes ui more flexible */
        vTaskDelayUntil(&tick, pdMS_TO_TICKS(50));
    }

    vTaskDelay(pdMS_TO_TICKS(500));

    /* Remove image and bar, reset background color */
    lv_port_sem_take();
    lv_obj_del(img);
    lv_obj_del(bar);
    lv_obj_del(label_loading_hint);
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_BG);
    lv_port_sem_give();

    /* Init call queue and pre-init widgets */
    ui_init_internal();

    /* Entering main UI */
    lv_port_sem_take();
    ui_status_bar_init();
    ui_standby_show(NULL);
    ui_call_stack_push(&ui_standby_func);
    lv_port_sem_give();

    ui_status_bar_set_warning(true, true, true);
}

void ui_laod_resource(const char *path, void **dst)
{
    void *_data = NULL;
    lv_fs_file_t file_img;
    size_t file_size = 0;
    size_t real_size = 0;

    /* Try to open file */
    if (LV_FS_RES_OK != lv_fs_open(&file_img, path, LV_FS_MODE_RD))
    {
        ESP_LOGE(LOG_TAG, "Can't open file : %s", path);
        return;
    }

    /* Get size of file */
    lv_fs_size(&file_img, &file_size);
    file_size -= 1;
    _data = heap_caps_malloc(file_size + 8, MALLOC_CAP_SPIRAM);
    lv_fs_read(&file_img, _data + 8, file_size, &real_size);
    lv_fs_close(&file_img);

    /* Change lv_img_dsc_t's data pointer */
    memmove(_data, _data + 8, 4);

    /* Set image information */
    *((uint32_t *)(_data + 4)) = file_size - 4;
    *((uint32_t *)(_data + 8)) = (uint32_t)(_data + 12);
    *dst = _data;
}

/* ******************************** CALL STACK ******************************** */
/**
 * @brief About call stack:
 *   - As for pages without back:
 *      ui_show(&ui_func_desc, UI_SHOW_OVERRIDE);
 *   - As for page with call queue:
 *      ui_show(&ui_func_desc, UI_SHOW_PEDDING);
 *   - As for back button:
 *      ui_show(&ui_func_desc, UI_SHOW_BACKPORT);
 */

static const int call_stack_size = 8;
typedef ui_func_desc_t call_stack_type_t;
static call_stack_type_t *call_stack = NULL;
static volatile size_t call_stack_index = 0;

/**
 * @brief An initialize task before UI start. Basicly create a call stack.
 * 
 */
static void ui_init_internal(void)
{
    assert(call_stack_size);
    call_stack = (ui_func_desc_t*)heap_caps_malloc(call_stack_size * sizeof(ui_func_desc_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
}

/**
 * @brief Push operator for call stack.
 * 
 * @param func pointer to `ui_func_desc_t`.
 * @return esp_err_t Push result.
 */
static esp_err_t ui_call_stack_push(ui_func_desc_t *func)
{
    if(call_stack_index <= call_stack_size - 1)
    {
        memcpy(&call_stack[call_stack_index], func, sizeof(call_stack_type_t));
        call_stack_index++;
        ESP_LOGD(LOG_TAG, "Send : %s", func->name);
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Call stack full");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

/**
 * @brief Pop operator for call stack.
 * 
 * @param func pointer to `ui_func_desc_t`.
 * @return esp_err_t Pop result.
 */
static esp_err_t ui_call_stack_pop(ui_func_desc_t *func)
{
    if(0 != call_stack_index)
    {
        call_stack_index--;
        memcpy(func, &call_stack[call_stack_index], sizeof(call_stack_type_t));
        ESP_LOGD(LOG_TAG, "Recieve : %s", func->name);
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Call queue empty");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

/**
 * @brief Peek top element from call stack. Keep data in call stack.
 * 
 * @param func pointer to `ui_func_desc_t`.
 * @return esp_err_t Peek result.
 */
static esp_err_t ui_call_stack_peek(ui_func_desc_t *func)
{
    if(0 != call_stack_index)
    {
        memcpy(func, &call_stack[call_stack_index - 1], sizeof(call_stack_type_t));
        ESP_LOGD(LOG_TAG, "Peek : %s", func->name);
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Call queue empty");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Clear all data in call stack.
 * 
 * @return esp_err_t Always return `ESP_OK`.
 */
static esp_err_t ui_call_stack_clear(void)
{
    ui_func_desc_t func;

    while (call_stack_index)
    {
        call_stack_index--;
        memcpy(&func, &call_stack[call_stack_index], sizeof(call_stack_type_t));
        ESP_LOGD(LOG_TAG, "Clear : %s", func.name);
    }

    call_stack_index = 0;

    return ESP_OK;
}

void ui_show(ui_func_desc_t *ui, ui_show_mode_t mode)
{
    static ui_func_desc_t ui_now;
    switch (mode)
    {
        case UI_SHOW_OVERRIDE:
        {
            ui_call_stack_pop(&ui_now);
            ui_now.hide(NULL);
            ui->show(NULL);
            ui_call_stack_clear();
            ui_call_stack_push(ui);
            break;
        }
        case UI_SHOW_PEDDING:
        {
            ui_call_stack_peek(&ui_now);
            ui_now.hide(NULL);
            ui->show(NULL);
            ui_call_stack_push(ui);
            break;
        }
        case UI_SHOW_BACKPORT:
        {
            ui_call_stack_pop(&ui_now);
            ui_now.hide(NULL);
            ui_call_stack_peek(&ui_now);
            ui_now.show(NULL);
            break;
        }
    }
}

#ifdef __cplusplus
} // closing brace for extern "C"
#endif