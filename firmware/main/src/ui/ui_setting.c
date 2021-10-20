#include "ui_main.h"
#include "lvgl_port.h"

/* UI function declaration */
ui_func_desc_t ui_setting_func = {
    .name = "ui_setting",
    .init = ui_setting_init,
    .show = ui_setting_show,
    .hide = ui_setting_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_panel = NULL;
static lv_obj_t* obj_slider_dose_coffee_short = NULL;
static lv_obj_t* obj_slider_dose_coffee_medium = NULL;
static lv_obj_t* obj_slider_dose_coffee_long = NULL;
static lv_obj_t* obj_slider_dose_cappuccino = NULL;
static lv_obj_t* obj_label_dose_coffee_short = NULL;
static lv_obj_t* obj_label_dose_coffee_medium = NULL;
static lv_obj_t* obj_label_dose_coffee_long = NULL;
static lv_obj_t* obj_label_dose_cappuccino = NULL;


// /* Static function forward declaration */
static void slider_cb(lv_obj_t* obj, lv_event_t event);

bool isSettingsPageActive = false;


void ui_setting_init(void *data)
{
    (void)data;

    obj_panel = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_panel, 460, 250);
    lv_obj_set_style_local_bg_color(obj_panel, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_panel, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_panel, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_panel, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    // lv_page_set_scroll_dir(obj_panel, LV_DIR_TOP);
    // lv_page_set_scrollbar_mode(obj_panel, LV_SCROLLBAR_MODE_ON);
    lv_obj_align(obj_panel, NULL, LV_ALIGN_CENTER, 0, 20);

    obj_slider_dose_coffee_short = lv_slider_create(obj_panel, NULL);
    lv_label_set_text(obj_slider_dose_coffee_short, "Dose Short Coffee");
    lv_obj_set_width(obj_slider_dose_coffee_short, 370);
    lv_obj_set_height(obj_slider_dose_coffee_short, 10);
    lv_obj_align(obj_slider_dose_coffee_short, obj_panel, LV_ALIGN_IN_TOP_MID, 0, 20);
    lv_slider_set_range(obj_slider_dose_coffee_short, 80, 150);
    lv_slider_set_value(obj_slider_dose_coffee_short, 110, LV_ANIM_OFF);

    obj_label_dose_coffee_short = lv_label_create(obj_panel, NULL);
    lv_label_set_text(obj_label_dose_coffee_short, "Short Coffee Dose: 110");
    lv_obj_set_auto_realign(obj_label_dose_coffee_short, true);
    lv_obj_align(obj_label_dose_coffee_short, obj_slider_dose_coffee_short, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    obj_slider_dose_coffee_medium = lv_slider_create(obj_panel, NULL);
    lv_label_set_text(obj_slider_dose_coffee_medium, "Dose Medium Coffee");
    lv_obj_set_width(obj_slider_dose_coffee_medium, 370);
    lv_obj_set_height(obj_slider_dose_coffee_medium, 10);
    lv_obj_align(obj_slider_dose_coffee_medium, obj_panel, LV_ALIGN_IN_TOP_MID, 0, 80);
    lv_slider_set_range(obj_slider_dose_coffee_medium, 80, 200);
    lv_slider_set_value(obj_slider_dose_coffee_medium, 140, LV_ANIM_OFF);

    obj_label_dose_coffee_medium = lv_label_create(obj_panel, NULL);
    lv_label_set_text(obj_label_dose_coffee_medium, "Medium Coffee Dose: 140");
    lv_obj_set_auto_realign(obj_label_dose_coffee_medium, true);
    lv_obj_align(obj_label_dose_coffee_medium, obj_slider_dose_coffee_medium, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    obj_slider_dose_coffee_long = lv_slider_create(obj_panel, NULL);
    lv_label_set_text(obj_slider_dose_coffee_long, "Dose Medium Coffee");
    lv_obj_set_width(obj_slider_dose_coffee_long, 370);
    lv_obj_set_height(obj_slider_dose_coffee_long, 10);
    lv_obj_align(obj_slider_dose_coffee_long, obj_panel, LV_ALIGN_IN_TOP_MID, 0, 140);
    lv_slider_set_range(obj_slider_dose_coffee_long, 80, 300);
    lv_slider_set_value(obj_slider_dose_coffee_long, 190, LV_ANIM_OFF);

    obj_label_dose_coffee_long = lv_label_create(obj_panel, NULL);
    lv_label_set_text(obj_label_dose_coffee_long, "Long Coffee Dose: 190");
    lv_obj_set_auto_realign(obj_label_dose_coffee_long, true);
    lv_obj_align(obj_label_dose_coffee_long, obj_slider_dose_coffee_long, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    obj_slider_dose_cappuccino = lv_slider_create(obj_panel, NULL);
    lv_label_set_text(obj_slider_dose_cappuccino, "Dose Medium Coffee");
    lv_obj_set_width(obj_slider_dose_cappuccino, 370);
    lv_obj_set_height(obj_slider_dose_cappuccino, 10);
    lv_obj_align(obj_slider_dose_cappuccino, obj_panel, LV_ALIGN_IN_TOP_MID, 0, 200);
    lv_slider_set_range(obj_slider_dose_cappuccino, 50, 150);
    lv_slider_set_value(obj_slider_dose_cappuccino, 100, LV_ANIM_OFF);

    obj_label_dose_cappuccino = lv_label_create(obj_panel, NULL);
    lv_label_set_text(obj_label_dose_cappuccino, "Cappuccino Dose: 100");
    lv_obj_set_auto_realign(obj_label_dose_cappuccino, true);
    lv_obj_align(obj_label_dose_cappuccino, obj_slider_dose_cappuccino, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_set_event_cb(obj_slider_dose_coffee_short, slider_cb);
    lv_obj_set_event_cb(obj_slider_dose_coffee_medium, slider_cb);
    lv_obj_set_event_cb(obj_slider_dose_coffee_long, slider_cb);
    lv_obj_set_event_cb(obj_slider_dose_cappuccino, slider_cb);

}

void ui_setting_show(void *data)
{
    if(NULL == obj_panel)
    {
        ui_setting_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_panel, false);
        lv_obj_set_hidden(obj_slider_dose_coffee_short, false);
        lv_obj_set_hidden(obj_slider_dose_coffee_medium, false);
        lv_obj_set_hidden(obj_slider_dose_coffee_long, false);
        lv_obj_set_hidden(obj_slider_dose_cappuccino, false);
        lv_obj_set_hidden(obj_label_dose_coffee_short, false);
        lv_obj_set_hidden(obj_label_dose_coffee_medium, false);
        lv_obj_set_hidden(obj_label_dose_coffee_long, false);
        lv_obj_set_hidden(obj_label_dose_cappuccino, false);
    }
    isSettingsPageActive = true;
}

void ui_setting_hide(void *data)
{
    (void)data;

    if(NULL != obj_panel)
    {
        lv_obj_set_hidden(obj_panel, true);
        lv_obj_set_hidden(obj_slider_dose_coffee_short, true);
        lv_obj_set_hidden(obj_slider_dose_coffee_medium, true);
        lv_obj_set_hidden(obj_slider_dose_coffee_long, true);
        lv_obj_set_hidden(obj_slider_dose_cappuccino, true);
        lv_obj_set_hidden(obj_label_dose_coffee_short, true);
        lv_obj_set_hidden(obj_label_dose_coffee_medium, true);
        lv_obj_set_hidden(obj_label_dose_coffee_long, true);
        lv_obj_set_hidden(obj_label_dose_cappuccino, true);
    }
    isSettingsPageActive = false;
}

// /* ******************************** Event Handler(s) ******************************** */
static void slider_cb(lv_obj_t* obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        char buf[30];

        if(obj == obj_slider_dose_coffee_short)
        {
            int dose = lv_slider_get_value(obj_slider_dose_coffee_short);
            printf("DOSE COFFEE SHORT Value: %d\n", dose);
            sprintf(buf, "Short Coffee dose: %d", dose);
            lv_label_set_text(obj_label_dose_coffee_short, buf);
        }

        if(obj == obj_slider_dose_coffee_medium)
        {
            int dose = lv_slider_get_value(obj_slider_dose_coffee_medium);
            printf("DOSE COFFEE MEDIUM Value: %d\n", dose);
            sprintf(buf, "Medium Coffee dose: %d", dose);
            lv_label_set_text(obj_label_dose_coffee_medium, buf);
        }

        if(obj == obj_slider_dose_coffee_long)
        {
            int dose = lv_slider_get_value(obj_slider_dose_coffee_long);
            printf("DOSE COFFEE LONG Value: %d\n", dose);
            sprintf(buf, "Long Coffee dose: %d", dose);
            lv_label_set_text(obj_label_dose_coffee_long, buf);
        }
        
        if(obj == obj_slider_dose_cappuccino)
        {
            int dose = lv_slider_get_value(obj_slider_dose_cappuccino);
            printf("DOSE CAPPUCCINO Value: %d\n", dose);
            sprintf(buf, "Cappuccino dose: %d", dose);
            lv_label_set_text(obj_label_dose_cappuccino, buf);
        }
    }
}
