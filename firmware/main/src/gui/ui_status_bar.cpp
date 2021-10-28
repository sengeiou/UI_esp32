#include "ui_main.h"
#include "lvgl_port.h"

#define LOG_TAG "UI_STATUS"

/* LVGL objects defination */
static lv_obj_t* status_bar = NULL;
static lv_obj_t* btn_wifi = NULL;
static lv_obj_t* btn_setting = NULL;
static lv_obj_t* btn_pod = NULL;
static lv_obj_t* btn_descaling = NULL;
static lv_obj_t* btn_water = NULL;

static lv_obj_t* img_descaling_warn = NULL;
static lv_obj_t* img_pod_warn = NULL;
static lv_obj_t* img_water_warn = NULL;


extern bool isPreferencesPageActive;
extern bool isSettingsPageActive;
extern bool isStatisticsPageActive;
extern bool isErogationPageActive;
extern bool isWifiPageActive;
extern bool isDescalingPageActive;
extern bool isCleaningPageActive;

static bool isWifiEnabled = false;

static bool isWarningDescaling = false;
static bool isWarningPod = false;
static bool isWarningWater = false;
static bool isPopupOpened = false;

/* Extern image variable(s) */
extern void* data_descaling_warning;
extern void* data_water_warning;
extern void* data_pod_warning;

/* Static function forward declaration */
static void btn_cb(lv_obj_t *obj, lv_event_t event);
static void btn_warning_cb(lv_obj_t *obj, lv_event_t event);
static void descaling_popup_cb(lv_obj_t *obj, lv_event_t event);
static void basic_popup_cb(lv_obj_t* obj, lv_event_t event);

void ui_status_bar_set_warning(bool descaling, bool pod_full, bool water_empty)
{
    ESP_LOGI(LOG_TAG, "Warning UPDATE");

    bool isToUpdate = false;
    if(isWarningDescaling != descaling)
    {
        isToUpdate |= true;
        isWarningDescaling = descaling;
    }
    if(isWarningPod != pod_full)
    {
        isToUpdate |= true;
        isWarningPod = pod_full;
    }
    if(isWarningWater != water_empty)
    {
        isToUpdate |= true;
        isWarningWater = water_empty;
    }

    if(true == isToUpdate)
    {
        ESP_LOGI(LOG_TAG, "Warning UPDATE");
        lv_obj_set_click(btn_descaling, isWarningDescaling);
        lv_obj_set_click(btn_water, isWarningWater);
        lv_obj_set_click(btn_pod, isWarningPod);

        lv_obj_set_style_local_image_recolor(img_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningDescaling ? LV_COLOR_ORANGE : LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningPod ? LV_COLOR_ORANGE : LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningWater ? LV_COLOR_ORANGE : LV_COLOR_GRAY);
    }
}

void ui_status_bar_update_wifi_status(bool active)
{
    isWifiEnabled = active;

    if(isWifiEnabled)
        lv_obj_set_style_local_value_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    else
        lv_obj_set_style_local_value_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
}

void ui_status_bar_init(void)
{
    status_bar = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(status_bar, true);
    lv_obj_set_size(status_bar, 480, 60);
    lv_obj_set_style_local_bg_color(status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(status_bar, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    btn_setting = lv_obj_create(status_bar, NULL);
    lv_obj_set_style_local_radius(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_width(btn_setting, 58);
    lv_obj_set_style_local_bg_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_value_font(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32);
    lv_obj_set_style_local_value_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_value_str(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_local_value_str(btn_setting, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_SYMBOL_SETTINGS);
    lv_obj_align(btn_setting, status_bar, LV_ALIGN_IN_RIGHT_MID, -15, 0);
    lv_obj_set_event_cb(btn_setting, btn_cb);

    btn_wifi = lv_obj_create(status_bar, NULL);
    lv_obj_set_width(btn_wifi, 58);
    lv_obj_set_style_local_radius(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_bg_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_value_font(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32);
    lv_obj_set_style_local_value_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_value_str(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_WIFI);
    lv_obj_align(btn_wifi, status_bar, LV_ALIGN_IN_LEFT_MID, 15, 0);
    lv_obj_set_event_cb(btn_wifi, btn_cb);

    btn_descaling = lv_obj_create(status_bar, NULL);
    lv_obj_set_width(btn_descaling, 58);
    lv_obj_set_style_local_radius(btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_bg_color(btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(btn_descaling, status_bar, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btn_descaling, btn_warning_cb);

    img_descaling_warn = lv_img_create(btn_descaling, NULL);
    lv_img_set_src(img_descaling_warn, data_descaling_warning);
    lv_img_set_zoom(img_descaling_warn, 128);
    lv_obj_set_style_local_image_recolor_opa(img_descaling_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_descaling_warn, NULL, LV_ALIGN_CENTER, 0, 0);

    btn_pod = lv_obj_create(status_bar, NULL);
    lv_obj_set_width(btn_pod, 58);
    lv_obj_set_style_local_radius(btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_bg_color(btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(btn_pod, status_bar, LV_ALIGN_CENTER, -65, 0);
    lv_obj_set_event_cb(btn_pod, btn_warning_cb);
    
    img_pod_warn = lv_img_create(btn_pod, NULL);
    lv_img_set_src(img_pod_warn, data_pod_warning);
    lv_img_set_zoom(img_pod_warn, 128);
    lv_obj_set_style_local_image_recolor_opa(img_pod_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_pod_warn, NULL, LV_ALIGN_CENTER, 0, 0);

    btn_water = lv_obj_create(status_bar, NULL);
    lv_obj_set_width(btn_water, 58);
    lv_obj_set_style_local_radius(btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_bg_color(btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(btn_water, status_bar, LV_ALIGN_CENTER, 65, 0);
    lv_obj_set_event_cb(btn_water, btn_warning_cb);

    img_water_warn = lv_img_create(btn_water, NULL);
    lv_img_set_src(img_water_warn, data_water_warning);
    lv_img_set_zoom(img_water_warn, 128);
    lv_obj_set_style_local_image_recolor_opa(img_water_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_water_warn, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_local_image_recolor(img_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);

    ui_status_bar_show(false);
}

void ui_status_bar_show(bool show)
{
    if(NULL != status_bar)
    {
        lv_obj_set_hidden(status_bar, !show);
    }
}

/* ******************************** Event Handler ******************************** */
static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(btn_setting == obj)
        {
            if(true == isPreferencesPageActive)
            {
                lv_obj_set_style_local_value_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
                ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            }
            else if(true == isSettingsPageActive || true == isStatisticsPageActive)
            {
                lv_obj_set_style_local_value_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
                ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);
            }
            else if(true == isErogationPageActive || true == isDescalingPageActive || true == isCleaningPageActive)
            {
                lv_obj_set_style_local_value_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
            }
            else
            {
                lv_obj_set_style_local_value_color(btn_setting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
                ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);
            }

            return;
        }

        if(btn_wifi == obj) 
        {
            if( false == isPreferencesPageActive &&
                false == isSettingsPageActive && 
                false == isStatisticsPageActive &&
                false == isErogationPageActive)
            {
                if(true == isWifiPageActive)
                {
                    ui_status_bar_update_wifi_status(isWifiEnabled);   
                    ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
                }
                else
                {
                    lv_obj_set_style_local_value_color(btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
                    ui_show(&ui_wifi_func, UI_SHOW_OVERRIDE);
                }
            }
            return;
        }
    }
}


static void btn_warning_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(false == isPopupOpened)
        {
            if(btn_descaling == obj)
            {
                static const char* btns[] = { "OK", "START", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
                lv_msgbox_set_text(msgbox, "A desclaing cycle is needed");
                lv_msgbox_add_btns(msgbox, btns);
                lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_event_cb(msgbox, descaling_popup_cb);
                isPopupOpened = true;
            }

            if(btn_water == obj)
            {
                static const char* btns[] = { "OK", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
                lv_msgbox_set_text(msgbox, "Water is empty");
                lv_msgbox_add_btns(msgbox, btns);
                lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_event_cb(msgbox, basic_popup_cb);
                isPopupOpened = true;
            }

            if(btn_pod == obj)
            {
                static const char* btns[] = { "OK", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
                lv_msgbox_set_text(msgbox, "Pod container is full");
                lv_msgbox_add_btns(msgbox, btns);
                lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_event_cb(msgbox, basic_popup_cb);
                isPopupOpened = true;
            }
        }
    }
}

static void descaling_popup_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        if(0 == lv_msgbox_get_active_btn(obj))
        {
            ESP_LOGI(LOG_TAG, "Descaling abort");
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            lv_obj_del(obj);
            isPopupOpened = false;
        }

        if(1 == lv_msgbox_get_active_btn(obj))
        {
            ESP_LOGI(LOG_TAG, "Descaling start");
            ui_show(&ui_descaling_func, UI_SHOW_OVERRIDE);
            lv_obj_del(obj);
            isPopupOpened = false;
        }
    }
}

static void basic_popup_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        if(0 == lv_msgbox_get_active_btn(obj))
        {
            ESP_LOGI(LOG_TAG, "Popup closed");
            lv_obj_del(obj);
            isPopupOpened = false;
        }
    }
}
