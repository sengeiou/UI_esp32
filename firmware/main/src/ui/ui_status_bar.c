#include "ui_main.h"
#include "lvgl_port.h"

/* LVGL objects defination */
static lv_obj_t *status_bar = NULL;
static lv_obj_t *btn_wifi = NULL;
static lv_obj_t *btn_setting = NULL;
// static lv_obj_t *btn_little_time = NULL;

extern bool isPreferencesPageActive;
extern bool isSettingsPageActive;
extern bool isStatisticsPageActive;
extern bool isErogationPageActive;
// extern bool isWifiPageActive;

/* Static function forward declaration */
static void btn_cb(lv_obj_t *obj, lv_event_t event);

void ui_status_bar_init(void)
{
    status_bar = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(status_bar, true);
    lv_obj_set_size(status_bar, 480, 40);
    lv_obj_set_style_local_bg_color(status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(status_bar, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    // btn_little_time = lv_btn_create(status_bar, NULL);
    // lv_obj_set_style_local_radius(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    // lv_obj_set_style_local_value_str(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "08:00");
    // lv_obj_set_style_local_bg_color(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    // lv_obj_set_style_local_border_color(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    // lv_obj_set_style_local_value_font(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &font_en_bold_16);
    // lv_obj_set_style_local_value_align(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    // lv_obj_set_style_local_value_color(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    // lv_obj_set_event_cb(btn_little_time, btn_cb);
    // lv_obj_align(btn_little_time, NULL, LV_ALIGN_CENTER, 0, 0);

    btn_setting = lv_btn_create(status_bar, NULL);
    lv_obj_set_style_local_radius(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_width(btn_setting, 38);
    lv_obj_set_style_local_bg_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_font(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &font_bar_symbol);
    lv_obj_set_style_local_value_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_value_str(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_local_value_str(btn_setting, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_SYMBOL_EXTRA_SETTING_SOLID);
    lv_obj_align(btn_setting, status_bar, LV_ALIGN_IN_RIGHT_MID, -15, 0);
    lv_obj_set_event_cb(btn_setting, btn_cb);

    btn_wifi = lv_btn_create(status_bar, NULL);
    lv_obj_set_width(btn_wifi, 38);
    // lv_obj_set_click(btn_wifi, false);
    lv_obj_set_style_local_radius(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_bg_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_font(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &font_bar_symbol);
    lv_obj_set_style_local_value_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_value_str(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_EXTRA_WIFI_SLASH);
    lv_obj_align(btn_wifi, status_bar, LV_ALIGN_IN_LEFT_MID, 15, 0);
    lv_obj_set_event_cb(btn_wifi, btn_cb);

    lv_obj_set_hidden(status_bar, false);
}

void ui_status_bar_time_show(bool show)
{
    // lv_obj_set_hidden(btn_little_time, !show);
}

/* ******************************** Event Handler ******************************** */
static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(btn_setting == obj)
        {
            if(true == isPreferencesPageActive)
                ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            else if(true == isSettingsPageActive || true == isStatisticsPageActive)
                ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);
            else if(true == isErogationPageActive)
            {
                //Do nothing;
            }
            else
                ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);

            return;
        }

        if(btn_wifi == obj) 
        {
            if( false == isPreferencesPageActive &&
                false == isSettingsPageActive && 
                false == isStatisticsPageActive &&
                false == isErogationPageActive)
            {
                static bool state = false;

                state = !state;
                ui_preparations_enable_cappuccino(state);
            }
            return;
        }
    }
}

void ui_status_bar_set_item_text(status_bar_item_t item, const char *text)
{
    if(NULL == text)
    {
        return;
    }

    switch(item)
    {
        case status_bar_item_wifi:
        {
            if(NULL != btn_wifi)
            {
                lv_obj_set_style_local_value_str(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, text);
            }
            break;
        }
        case status_bar_item_little_time:
        {
            // if(NULL != btn_little_time)
            // {
            //     lv_obj_set_style_local_value_str(btn_little_time, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, text);
            // }
            break;
        }
        default:
            break;
    }
}