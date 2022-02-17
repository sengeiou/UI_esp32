#include "ui_main.h"
#include "sys_check.h"
#include "esp_log.h"
#include "dbg_task.h"
#include "lvgl_gui.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_MAIN"
#else
    #define LOG_TAG "UI_MAIN"
#endif

void* data_logo = NULL;

void* data_espresso_corto = NULL;
void* data_espresso = NULL;
void* data_espresso_lungo = NULL;
void* data_macchiato = NULL;
void* data_cappuccino = NULL;
void* data_latte_macchiato = NULL;
void* data_dose_libera = NULL;
void* data_caffe_americano = NULL;
void* data_acqua_calda = NULL;

void* data_power = NULL;
void* data_menu = NULL;

void* data_descaling_warning  = NULL;
void* data_water_warning  = NULL;
void* data_pod_warning  = NULL;
void* data_generic_warning  = NULL;

void* data_popup = NULL;
void* data_fault = NULL;

/* Resources loading list */
static ui_data_fetch_t img_fetch_list[] = {
    /* Preparation Page */    
    { "S:/icons/EspressoCorto.bin", &data_espresso_corto },
    { "S:/icons/Espresso.bin", &data_espresso },
    { "S:/icons/EspressoLungo.bin", &data_espresso_lungo },
    { "S:/icons/Macchiato.bin", &data_macchiato },
    { "S:/icons/Cappuccino.bin", &data_cappuccino },
    { "S:/icons/LatteMacchiato.bin", &data_latte_macchiato },
    { "S:/icons/DoseLibera.bin", &data_dose_libera },
    { "S:/icons/CaffeAmericano.bin", &data_caffe_americano },
    { "S:/icons/AcquaCalda.bin", &data_acqua_calda },
    /* Standby Page */
    { "S:/icons/Logo.bin", &data_logo },
    /* Fault Page */
    { "S:/icons/Fault.bin", &data_fault },  //TODO get image
    /* General */
    { "S:/icons/Popup.bin", &data_popup },
    /* Menu Bar */
    { "S:/icons/Menu.bin", &data_menu },     //TODO get image
    { "S:/icons/Power.bin", &data_power },   //TODO get image
    /* Warning Bar */
    { "S:/icons/DescalingWarning.bin", &data_descaling_warning },
    { "S:/icons/Pod.bin", &data_pod_warning },  //TODO get image
    { "S:/icons/WaterEmptyWarning.bin", &data_water_warning },  
    { "S:/icons/GenericWarning.bin", &data_generic_warning },

};

static void ui_init_internal(void);

static esp_err_t ui_call_stack_push(ui_func_desc_t *func);
static esp_err_t ui_call_stack_pop(ui_func_desc_t *func);
static esp_err_t ui_call_stack_peek(ui_func_desc_t *func);
static esp_err_t ui_call_stack_clear(void);

void ui_main(void)
{
    /* Init main screen */
    lvgl_sem_take();
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    /* Show logo */
    LV_IMG_DECLARE(lavazza_logo)
    lv_obj_t *img = lv_img_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(img, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_img_set_src(img, &lavazza_logo);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, -50);
    lvgl_sem_give();

    if(ESP_OK != sys_check())
    {
        while(1)
        {
            /* Stop at here if system check not pass */
            ESP_LOGE(LOG_TAG, "Failed to init system");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    /* Create a bar to update loading progress */
    lvgl_sem_take();
    lv_obj_t* bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_border_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, 2);
    lv_obj_set_size(bar, 400, 12);
    lv_obj_align(bar, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_bar_set_range(bar, 0, sizeof(img_fetch_list) / sizeof(ui_data_fetch_t));
    lv_bar_set_value(bar, 1, LV_ANIM_ON);

    lv_obj_t* label_loading_hint = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_static(label_loading_hint, " ");
    lv_obj_set_style_local_text_font(label_loading_hint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &default_small_font);
    lv_obj_set_style_local_text_color(label_loading_hint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
    lv_obj_align(label_loading_hint, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lvgl_sem_give();

    /* Load resource from SD card to PSRAM */
    TickType_t tick = xTaskGetTickCount();
    for(size_t i = 0; i < sizeof(img_fetch_list) / sizeof(ui_data_fetch_t); i++)
    {
        lvgl_sem_take();
        lv_bar_set_value(bar, i + 1, LV_ANIM_ON);
        lv_label_set_text_fmt(label_loading_hint, "Loading \"%s\"", img_fetch_list[i].path);
        lv_obj_align(label_loading_hint, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        lvgl_sem_give();

        if(ESP_OK != img_file_check(img_fetch_list[i].path))
        {
            /* If any of file was missing, block the task */
            while(1)
            {
                ESP_LOGE(LOG_TAG, "File missed");
                vTaskDelay(1000);
            }
        }
        ESP_LOGI(LOG_TAG, "Load image %s", img_fetch_list[i].path);

        ui_laod_resource(img_fetch_list[i].path, img_fetch_list[i].data);

        /* Yeah, it's only because that makes ui more flexible */
        vTaskDelayUntil(&tick, pdMS_TO_TICKS(50));
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    /* Remove image and bar, reset background color */
    lvgl_sem_take();
    lv_obj_del(img);
    lv_obj_del(bar);
    lv_obj_del(label_loading_hint);
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_BG);
    lvgl_sem_give();

    /* Init call queue and pre-init widgets */
    ui_init_internal();

    /* Entering main UI */
    ui_menu_bar_init();
    ui_warning_bar_init();

    /* Init most used pages */
    ui_preparations_init(NULL);
    ui_preparations_hide(NULL);
    ui_erogation_init(NULL);
    ui_erogation_hide(NULL);
    ui_menu_init(NULL);
    ui_menu_hide(NULL);
    ui_wifi_init(NULL);
    ui_wifi_hide(NULL);
    ui_settings_init(NULL);
    ui_settings_hide(NULL);
    ui_statistics_init(NULL);
    ui_statistics_hide(NULL);

    ui_standby_show(NULL);
    ui_call_stack_push(&ui_standby_func);
    
    enable_livedata();
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