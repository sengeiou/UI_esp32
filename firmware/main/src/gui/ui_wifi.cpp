#include "ui_main.h"
#include "lvgl_port.h"
#include "variables.h"
#include "spiffs_utils.h"
#include "wifi_task.h"
#include "system_utils.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_WIFI"
#else
    #define LOG_TAG "UI_WIFI"
#endif

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

static void basic_popup_cb(lv_obj_t* obj, lv_event_t event);

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
            if(false == machineConnectivity.wifiEnabled)
            {
                machineConnectivity.wifiEnabled = true;
                utils::system::start_thread_core(&wifi_task, &xHandleWiFi, "wifi_task", 1024*8, 4, 1);
                vTaskDelay(1000/portTICK_PERIOD_MS);
                strcpy(machineConnectivity.ssid, lv_textarea_get_text(obj_ssid_area));
                strcpy(machineConnectivity.password, lv_textarea_get_text(obj_password_area));
                machineConnectivity.wifiEnabled = true;
                xEventGroupSetBits(xWifiEvents, WIFI_CONFIGURATION_BIT);

                ui_status_bar_update_wifi_status(machineConnectivity.wifiEnabled);
                ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            }
            else
            {
                static const char* btns[] = { "OK", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
                lv_msgbox_set_text(msgbox, "Please reset wifi\n    from settings");
                lv_msgbox_add_btns(msgbox, btns);
                lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_event_cb(msgbox, basic_popup_cb);
            }
            
            
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

    ui_status_bar_update_wifi_status(machineConnectivity.wifiEnabled);
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


static void basic_popup_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        if(0 == lv_msgbox_get_active_btn(obj))
        {
            lv_obj_del(obj);
        }
    }
}