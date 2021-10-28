#include "ui_main.h"
#include "lvgl_port.h"
#include "variables.h"



/* UI function declaration */
ui_func_desc_t ui_wifi_func = {
    .name = "ui_wifi",
    .init = ui_wifi_init,
    .show = ui_wifi_show,
    .hide = ui_wifi_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_ssid_area = NULL;
static lv_obj_t* obj_password_area = NULL;
static lv_obj_t* obj_keyboard = NULL;
static lv_obj_t* obj_btn_save = NULL;

bool isWifiPageActive = false;


static void kb_event_cb(lv_obj_t* keyboard, lv_event_t event)
{
    lv_keyboard_def_event_cb(obj_keyboard, event);
    if(event == LV_EVENT_CANCEL)
    {
        lv_keyboard_set_textarea(obj_keyboard, NULL);
        lv_obj_del(obj_keyboard);
        obj_keyboard = NULL;
    }
}

static void kb_area(lv_obj_t* obj)
{
    obj_keyboard = lv_keyboard_create(lv_scr_act(), NULL);
    lv_keyboard_set_cursor_manage(obj_keyboard, true);
    lv_obj_set_event_cb(obj_keyboard, kb_event_cb);
    lv_keyboard_set_textarea(obj_keyboard, obj);
}

static void ta_event_cb(lv_obj_t* obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        if(obj_keyboard != NULL)
        {
            lv_keyboard_set_textarea(obj_keyboard, obj);
        }
        else
        {
            kb_area(obj);
        }
    }
}

static void btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj_btn_save == obj)
        {
            strcpy(machineConnectivity.ssid, lv_textarea_get_text(obj_ssid_area));
            strcpy(machineConnectivity.password, lv_textarea_get_text(obj_password_area));
            machineConnectivity.wifiEnabled = true;
            machineConnectivity.status = WIFI_CONFIGURED;
            
            ui_status_bar_update_wifi_status(true);
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            return;
        }
    }
}

void ui_wifi_init(void *data)
{
    (void)data;

    obj_ssid_area = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_ssid_area, 220, 70);
    lv_obj_set_style_local_bg_color(obj_ssid_area, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_ssid_area, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_ssid_area, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_align(obj_ssid_area, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 80);
    lv_textarea_set_text(obj_ssid_area, "ssid");

    obj_password_area = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_password_area, 220, 70);
    lv_obj_set_style_local_bg_color(obj_password_area, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_password_area, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_password_area, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_align(obj_password_area, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 80);
    lv_textarea_set_text(obj_password_area, "password");

    obj_btn_save = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_radius(obj_btn_save, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_size(obj_btn_save, 120, 80);
    lv_obj_set_style_local_bg_color(obj_btn_save, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_save, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_value_font(obj_btn_save, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32);
    lv_obj_set_style_local_value_color(obj_btn_save, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_value_str(obj_btn_save, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_SAVE);
    lv_obj_set_style_local_value_str(obj_btn_save, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_SYMBOL_OK);
    lv_obj_align(obj_btn_save, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -50);
    lv_obj_set_event_cb(obj_btn_save, btn_cb);

    lv_obj_set_event_cb(obj_ssid_area, ta_event_cb);
    lv_obj_set_event_cb(obj_password_area, ta_event_cb);
    lv_obj_set_event_cb(obj_btn_save, btn_cb);
}

void ui_wifi_show(void *data)
{
    if(NULL == obj_ssid_area)
    {
        ui_wifi_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_ssid_area, false);
        lv_obj_set_hidden(obj_password_area, false);
        lv_obj_set_hidden(obj_btn_save, false);
    }

    if(machineConnectivity.status >= WIFI_CONFIGURED)
    {
        lv_textarea_set_text(obj_ssid_area, machineConnectivity.ssid);
        lv_textarea_set_text(obj_password_area, machineConnectivity.password);
    }
    else
    {
        lv_textarea_set_text(obj_ssid_area, "ssid");
        lv_textarea_set_text(obj_password_area, "password");
    }
    isWifiPageActive = true;
}

void ui_wifi_hide(void *data)
{
    (void)data;

    if(NULL != obj_ssid_area)
    {
        lv_obj_set_hidden(obj_ssid_area, true);
        lv_obj_set_hidden(obj_password_area, true);
        lv_obj_set_hidden(obj_btn_save, true);

        if(NULL != obj_keyboard)
        {
            lv_keyboard_set_textarea(obj_keyboard, NULL);
            lv_obj_del(obj_keyboard);
            obj_keyboard = NULL;
        }
    }
    isWifiPageActive = false;
}
