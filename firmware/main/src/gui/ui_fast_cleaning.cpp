#include "ui_main.h"
#include "lvgl_port.h"

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
static lv_obj_t* obj_button = NULL;

/* Static function forward declaration */
static void btn_cb(lv_obj_t *obj, lv_event_t event);

bool isCleaningPageActive = false;

static int progress = 0;

static void simulator_cleaning_task(void* data)
{
    (void)data;

    progress = 0;
    lv_obj_set_click(obj_button, false);
    lv_obj_set_style_local_value_str(obj_button,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "In progress...");
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_GRAY);
    lv_label_set_text(obj_label, "Fast Cleaning in progress...");

    while(progress <= 100)
    {
        progress++;
        lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);
        vTaskDelay(10);
    }

    lv_label_set_text(obj_label, "Fast Cleaning done");

    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_value_str(obj_button,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Done");
    lv_obj_set_click(obj_button, true);

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

    obj_button = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_button, 200, 100);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_GRAY);
    lv_obj_set_style_local_radius(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_button, obj_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_obj_set_click(obj_button, false);
    lv_obj_set_event_cb(obj_button, btn_cb);

    lv_obj_set_style_local_value_str(obj_button,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "In progress...");
    lv_obj_set_style_local_value_font(obj_button, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_button,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_button,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_button,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
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
        lv_obj_set_hidden(obj_button, false);
    }
    isCleaningPageActive = true;

    xTaskCreate(simulator_cleaning_task, "Fast Cleaning Simulator Task", 4*1024, NULL, 5, NULL);
}

void ui_fast_cleaning_hide(void *data)
{
    (void)data;

    if(NULL != obj_label)
    {
        lv_obj_set_hidden(obj_label, true);
        lv_obj_set_hidden(obj_bar, true);
        lv_obj_set_hidden(obj_button, true);
    }
    isCleaningPageActive = false;
}

/* ******************************** Event Handler(s) ******************************** */
static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);
    }
}
