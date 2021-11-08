#include "ui_main.h"
#include "lvgl_port.h"


#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_SETT"
#else
    #define LOG_TAG "UI_SETT"
#endif

/* UI function declaration */
ui_func_desc_t ui_setting_func = {
    .name = "ui_setting",
    .init = ui_setting_init,
    .show = ui_setting_show,
    .hide = ui_setting_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_container = NULL;
static lv_obj_t* obj_slider_dose_coffee_short = NULL;
static lv_obj_t* obj_slider_dose_coffee_medium = NULL;
static lv_obj_t* obj_slider_dose_coffee_long = NULL;
static lv_obj_t* obj_slider_dose_cappuccino = NULL;
static lv_obj_t* obj_slider_standby_time = NULL;
static lv_obj_t* obj_label_dose_coffee_short = NULL;
static lv_obj_t* obj_label_dose_coffee_medium = NULL;
static lv_obj_t* obj_label_dose_coffee_long = NULL;
static lv_obj_t* obj_label_dose_cappuccino = NULL;
static lv_obj_t* obj_label_standby_time = NULL;


// /* Static function forward declaration */
static void slider_cb(lv_obj_t* obj, lv_event_t event);

bool isSettingsPageActive = false;


void ui_setting_init(void *data)
{
    (void)data;

    obj_container = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_container, 460, 250);
    lv_obj_align(obj_container, NULL, LV_ALIGN_CENTER, 0, 20);
    lv_page_set_scrl_layout(obj_container, LV_LAYOUT_PRETTY_TOP);
    lv_page_set_scrl_height(obj_container, 250);
    lv_page_set_scrlbar_mode(obj_container, LV_SCRLBAR_MODE_AUTO);

    obj_slider_dose_coffee_short = lv_slider_create(obj_container, NULL);
    lv_obj_set_width(obj_slider_dose_coffee_short, 370);
    lv_obj_set_height(obj_slider_dose_coffee_short, 10);
    lv_obj_align(obj_slider_dose_coffee_short, obj_container, LV_ALIGN_IN_TOP_MID, 0, 20);
    lv_slider_set_range(obj_slider_dose_coffee_short, 80, 150);
    lv_slider_set_value(obj_slider_dose_coffee_short, 110, LV_ANIM_OFF);

    obj_label_dose_coffee_short = lv_label_create(obj_container, NULL);
    lv_label_set_text(obj_label_dose_coffee_short, "Short Coffee dose [pulses]: 110");
    lv_obj_set_auto_realign(obj_label_dose_coffee_short, true);
    lv_obj_align(obj_label_dose_coffee_short, obj_slider_dose_coffee_short, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    obj_slider_dose_coffee_medium = lv_slider_create(obj_container, NULL);
    lv_obj_set_width(obj_slider_dose_coffee_medium, 370);
    lv_obj_set_height(obj_slider_dose_coffee_medium, 10);
    lv_obj_align(obj_slider_dose_coffee_medium, obj_container, LV_ALIGN_IN_TOP_MID, 0, 80);
    lv_slider_set_range(obj_slider_dose_coffee_medium, 80, 200);
    lv_slider_set_value(obj_slider_dose_coffee_medium, 140, LV_ANIM_OFF);

    obj_label_dose_coffee_medium = lv_label_create(obj_container, NULL);
    lv_label_set_text(obj_label_dose_coffee_medium, "Medium Coffee dose [pulses]: 140");
    lv_obj_set_auto_realign(obj_label_dose_coffee_medium, true);
    lv_obj_align(obj_label_dose_coffee_medium, obj_slider_dose_coffee_medium, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    obj_slider_dose_coffee_long = lv_slider_create(obj_container, NULL);
    lv_obj_set_width(obj_slider_dose_coffee_long, 370);
    lv_obj_set_height(obj_slider_dose_coffee_long, 10);
    lv_obj_align(obj_slider_dose_coffee_long, obj_container, LV_ALIGN_IN_TOP_MID, 0, 140);
    lv_slider_set_range(obj_slider_dose_coffee_long, 80, 300);
    lv_slider_set_value(obj_slider_dose_coffee_long, 190, LV_ANIM_OFF);

    obj_label_dose_coffee_long = lv_label_create(obj_container, NULL);
    lv_label_set_text(obj_label_dose_coffee_long, "Long Coffee dose [pulses]: 190");
    lv_obj_set_auto_realign(obj_label_dose_coffee_long, true);
    lv_obj_align(obj_label_dose_coffee_long, obj_slider_dose_coffee_long, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    obj_slider_dose_cappuccino = lv_slider_create(obj_container, NULL);
    lv_obj_set_width(obj_slider_dose_cappuccino, 370);
    lv_obj_set_height(obj_slider_dose_cappuccino, 10);
    lv_obj_align(obj_slider_dose_cappuccino, obj_container, LV_ALIGN_IN_TOP_MID, 0, 200);
    lv_slider_set_range(obj_slider_dose_cappuccino, 50, 150);
    lv_slider_set_value(obj_slider_dose_cappuccino, 100, LV_ANIM_OFF);

    obj_label_dose_cappuccino = lv_label_create(obj_container, NULL);
    lv_label_set_text(obj_label_dose_cappuccino, "Cappuccino dose [pulses]: 100");
    lv_obj_set_auto_realign(obj_label_dose_cappuccino, true);
    lv_obj_align(obj_label_dose_cappuccino, obj_slider_dose_cappuccino, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    obj_slider_standby_time = lv_slider_create(obj_container, NULL);
    lv_obj_set_width(obj_slider_standby_time, 370);
    lv_obj_set_height(obj_slider_standby_time, 10);
    lv_obj_align(obj_slider_standby_time, obj_container, LV_ALIGN_IN_TOP_MID, 0, 260);
    lv_slider_set_range(obj_slider_standby_time, 0, 15);
    lv_slider_set_value(obj_slider_standby_time, 9, LV_ANIM_OFF);

    obj_label_standby_time = lv_label_create(obj_container, NULL);
    lv_label_set_text(obj_label_standby_time, "Standby time [min]: 9");
    lv_obj_set_auto_realign(obj_label_standby_time, true);
    lv_obj_align(obj_label_standby_time, obj_slider_standby_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_set_event_cb(obj_slider_dose_coffee_short, slider_cb);
    lv_obj_set_event_cb(obj_slider_dose_coffee_medium, slider_cb);
    lv_obj_set_event_cb(obj_slider_dose_coffee_long, slider_cb);
    lv_obj_set_event_cb(obj_slider_dose_cappuccino, slider_cb);
    lv_obj_set_event_cb(obj_slider_standby_time, slider_cb);
}

void ui_setting_show(void *data)
{
    if(NULL == obj_container)
    {
        ui_setting_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_container, false);
        lv_obj_set_hidden(obj_slider_dose_coffee_short, false);
        lv_obj_set_hidden(obj_slider_dose_coffee_medium, false);
        lv_obj_set_hidden(obj_slider_dose_coffee_long, false);
        lv_obj_set_hidden(obj_slider_dose_cappuccino, false);
        lv_obj_set_hidden(obj_slider_standby_time, false);
        lv_obj_set_hidden(obj_label_dose_coffee_short, false);
        lv_obj_set_hidden(obj_label_dose_coffee_medium, false);
        lv_obj_set_hidden(obj_label_dose_coffee_long, false);
        lv_obj_set_hidden(obj_label_dose_cappuccino, false);
        lv_obj_set_hidden(obj_label_standby_time, false);
    }
    isSettingsPageActive = true;
}

void ui_setting_hide(void *data)
{
    (void)data;

    if(NULL != obj_container)
    {
        lv_obj_set_hidden(obj_container, true);
        lv_obj_set_hidden(obj_slider_dose_coffee_short, true);
        lv_obj_set_hidden(obj_slider_dose_coffee_medium, true);
        lv_obj_set_hidden(obj_slider_dose_coffee_long, true);
        lv_obj_set_hidden(obj_slider_dose_cappuccino, true);
        lv_obj_set_hidden(obj_slider_standby_time, true);
        lv_obj_set_hidden(obj_label_dose_coffee_short, true);
        lv_obj_set_hidden(obj_label_dose_coffee_medium, true);
        lv_obj_set_hidden(obj_label_dose_coffee_long, true);
        lv_obj_set_hidden(obj_label_dose_cappuccino, true);
        lv_obj_set_hidden(obj_label_standby_time, true);
    }
    isSettingsPageActive = false;
}

// /* ******************************** Event Handler(s) ******************************** */
static void slider_cb(lv_obj_t* obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED || event == LV_EVENT_RELEASED)
    {
        char buf[64];

        if(obj == obj_slider_dose_coffee_short)
        {
            int dose = lv_slider_get_value(obj_slider_dose_coffee_short);
            sprintf(buf, "Short Coffee dose [pulses]: %d", dose);
            lv_label_set_text(obj_label_dose_coffee_short, buf);
            if(event == LV_EVENT_RELEASED)
                ESP_LOGI(LOG_TAG, "DOSE COFFEE SHORT Value: %d", dose);
        }

        if(obj == obj_slider_dose_coffee_medium)
        {
            int dose = lv_slider_get_value(obj_slider_dose_coffee_medium);
            sprintf(buf, "Medium Coffee dose [pulses]: %d", dose);
            lv_label_set_text(obj_label_dose_coffee_medium, buf);
            if(event == LV_EVENT_RELEASED)
                ESP_LOGI(LOG_TAG, "DOSE COFFEE MEDIUM Value: %d", dose);
        }

        if(obj == obj_slider_dose_coffee_long)
        {
            int dose = lv_slider_get_value(obj_slider_dose_coffee_long);
            sprintf(buf, "Long Coffee dose [pulses]: %d", dose);
            lv_label_set_text(obj_label_dose_coffee_long, buf);
            if(event == LV_EVENT_RELEASED)
                ESP_LOGI(LOG_TAG, "DOSE COFFEE LONG Value: %d", dose);
        }
        
        if(obj == obj_slider_dose_cappuccino)
        {
            int dose = lv_slider_get_value(obj_slider_dose_cappuccino);
            sprintf(buf, "Cappuccino dose [pulses]: %d", dose);
            lv_label_set_text(obj_label_dose_cappuccino, buf);
            if(event == LV_EVENT_RELEASED)
                ESP_LOGI(LOG_TAG, "DOSE CAPPUCCINO Value: %d", dose);
        }

        if(obj == obj_slider_standby_time)
        {
            int time = lv_slider_get_value(obj_slider_standby_time);
            sprintf(buf, "Standby time [min]: %d", time);
            lv_label_set_text(obj_label_standby_time, buf);
            if(event == LV_EVENT_RELEASED)
                ESP_LOGI(LOG_TAG, "STANDBY TIME Value: %d", time);
        }
    }
}
