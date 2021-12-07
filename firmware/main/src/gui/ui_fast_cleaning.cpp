#include "ui_main.h"
#include "lvgl_port.h"
#include "dbg_task.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_FAST_CLEAN"
#else
    #define LOG_TAG "UI_FAST_CLEAN"
#endif

/* UI function declaration */
ui_func_desc_t ui_fast_cleaning_func = {
    .name = "ui_fast_cleaning",
    .init = ui_fast_cleaning_init,
    .show = ui_fast_cleaning_show,
    .hide = ui_fast_cleaning_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_bar = NULL;
static lv_obj_t* obj_label = NULL;
static lv_obj_t* obj_status_btn = NULL;
static lv_obj_t* obj_stop_btn = NULL;

/* Static function forward declaration */
static void btn_cb(lv_obj_t *obj, lv_event_t event);

bool isCleaningPageActive = false;

static int progress = 0;
static int oldProgress = 0;

void ui_cleaning_fast_update(uint8_t current_step, uint8_t total_step)
{
    progress = 100*(current_step + 1)/total_step;
    ESP_LOGI(LOG_TAG, "UPDATE: %d/%d %d", current_step, total_step, progress);

    if(oldProgress != progress)
    {
        if(current_step == (total_step - 1))    //LAST STEP
        {
            lv_obj_set_style_local_bg_color(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
            lv_obj_set_style_local_bg_color(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
            lv_obj_set_style_local_value_str(obj_status_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "DONE");
            lv_obj_set_click(obj_status_btn, true);
            lv_obj_set_style_local_bg_color(obj_stop_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
            lv_obj_set_style_local_bg_color(obj_stop_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_GRAY);
            lv_obj_set_click(obj_stop_btn, false);
            lv_label_set_text(obj_label, "Fast Cleaning done");
        }
        else
        {
            lv_obj_set_style_local_value_str(obj_status_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "In progress...");
            lv_obj_set_style_local_bg_color(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
            lv_obj_set_style_local_bg_color(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_GRAY);
            lv_obj_set_click(obj_status_btn, false);
            lv_label_set_text(obj_label, "Fast Cleaning in progress...");
          
        }
        lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);

        oldProgress = progress;
    }
}


static void simulator_cleaning_task(void* data)
{
    (void)data;

    uint8_t currentStep = 0;
    uint8_t totalStep = 4;
    for(uint8_t i = 0; i < totalStep; i++)
    {
        ui_cleaning_fast_update(currentStep, totalStep);
        currentStep++;
        vTaskDelay(100);
    }

    vTaskDelete(NULL);
}

void ui_fast_cleaning_init(void *data)
{
    (void)data;

    obj_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_text_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_size(obj_label, 400, 100);
    lv_obj_set_style_local_value_font(obj_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_bold_48);
    lv_label_set_text(obj_label, "Fast Cleaning in progress...");
    lv_label_set_align(obj_label, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(obj_label, NULL, LV_ALIGN_CENTER, 0, -80);
    lv_obj_set_auto_realign(obj_label, true);

    obj_bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, 2);
    lv_obj_set_size(obj_bar, 400, 12);
    lv_bar_set_range(obj_bar, 0, 100);
    lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);
    lv_obj_align(obj_bar, obj_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);

    obj_status_btn = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_status_btn, 200, 100);
    lv_obj_set_style_local_bg_color(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_GRAY);
    lv_obj_set_style_local_radius(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_status_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_status_btn, obj_bar, LV_ALIGN_OUT_BOTTOM_LEFT, -10, 40);
    lv_obj_set_click(obj_status_btn, false);
    lv_obj_set_event_cb(obj_status_btn, btn_cb);

    lv_obj_set_style_local_value_str(obj_status_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "In progress...");
    lv_obj_set_style_local_value_font(obj_status_btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_status_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_status_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_status_btn,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);

    obj_stop_btn = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_stop_btn, 200, 100);
    lv_obj_set_style_local_bg_color(obj_stop_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_stop_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_stop_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_stop_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_stop_btn, obj_bar, LV_ALIGN_OUT_BOTTOM_RIGHT, 10, 40);
    lv_obj_set_event_cb(obj_stop_btn, btn_cb);

    lv_obj_set_style_local_value_str(obj_stop_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "STOP");
    lv_obj_set_style_local_value_font(obj_stop_btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_stop_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_stop_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_stop_btn,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK); 
}

void ui_fast_cleaning_show(void *data)
{
    if(NULL == obj_label)
    {
        ui_fast_cleaning_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_label, false);
        lv_obj_set_hidden(obj_bar, false);
        lv_obj_set_hidden(obj_status_btn, false);
        lv_obj_set_hidden(obj_stop_btn, false);
    }
    isCleaningPageActive = true;

    // xTaskCreate(simulator_cleaning_task, "Fast Cleaning Simulator Task", 4*1024, NULL, 5, NULL);
}

void ui_fast_cleaning_hide(void *data)
{
    (void)data;

    if(NULL != obj_label)
    {
        lv_obj_set_hidden(obj_label, true);
        lv_obj_set_hidden(obj_bar, true);
        lv_obj_set_hidden(obj_status_btn, true);
        lv_obj_set_hidden(obj_stop_btn, true);
    }
    isCleaningPageActive = false;
}

/* ******************************** Event Handler(s) ******************************** */
static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == obj_status_btn)
        {
            ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_stop_btn)
        {
            special_function(DBG_ON_OFF);
            ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);
        }
    }
}
