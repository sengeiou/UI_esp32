#include "ui_main.h"
#include "variables.h"
#include "dbg_task.h"

#include "string_it.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_WARN_BAR"
#else
    #define LOG_TAG "UI_WARN_BAR"
#endif

/* LVGL objects defination */
static lv_obj_t* obj_warning_bar = NULL;
static lv_obj_t* obj_btn_pod = NULL;
static lv_obj_t* obj_btn_descaling = NULL;
static lv_obj_t* obj_btn_water = NULL;
static lv_obj_t* obj_btn_generic = NULL;
static lv_obj_t* obj_btn_wifi = NULL;

static lv_obj_t* img_descaling_warn = NULL;
static lv_obj_t* img_pod_warn = NULL;
static lv_obj_t* img_water_warn = NULL;
static lv_obj_t* img_generic_warn = NULL;
static lv_obj_t* obj_label_wifi = NULL;

static bool isWarningDescaling = false;
static bool isWarningPod = false;
static bool isWarningWater = false;
static bool isWarningGeneric = false;
static bool isPopupOpened = false;

static wifi_status_item_t wifiStatus = wifi_status_item_off;

/* Extern image variable(s) */
extern void* data_descaling_warning;
extern void* data_water_warning;
extern void* data_pod_warning;
extern void* data_generic_warning;

#if ENABLE_CAPS_RECOGNITION_MODULE == 1
static lv_obj_t* obj_btn_recognition = NULL;
static lv_obj_t* img_recognition = NULL;
extern void* data_recognition;
#endif


/* Static function forward declaration */
static void obj_btn_cb(lv_obj_t *obj, lv_event_t event);
static void obj_btn_warning_cb(lv_obj_t *obj, lv_event_t event);
static void descaling_popup_cb(lv_obj_t *obj, lv_event_t event);
static void basic_popup_cb(lv_obj_t* obj, lv_event_t event);

/* Blink led variables and functions */
esp_timer_handle_t blink_led_timer;
static void blink_led_cb(void* arg);


extern ui_preparation_t preparation;

void ui_warning_bar_set_descaling_warning(bool warning)
{
    if(isWarningDescaling != warning)
    {
        ESP_LOGI(LOG_TAG, "Descaling Warning UPDATE");
        isWarningDescaling = warning;
        lv_obj_set_click(obj_btn_descaling, isWarningDescaling);
        lv_obj_set_style_local_image_recolor(img_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningDescaling ? LV_COLOR_ORANGE : LV_COLOR_GRAY);

        preparation.warnings.descaling = isWarningDescaling;
    }
}

void ui_warning_bar_set_pod_warning(bool warning)
{
    if(isWarningPod != warning)
    {
        ESP_LOGI(LOG_TAG, "Pod Full Warning UPDATE");
        isWarningPod = warning;
        lv_obj_set_click(obj_btn_pod, isWarningPod);
        lv_obj_set_style_local_image_recolor(img_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningPod ? LV_COLOR_ORANGE : LV_COLOR_GRAY);

        preparation.warnings.pod_full = isWarningPod;
    }
}

void ui_warning_bar_set_water_empty_warning(bool warning)
{
    if(isWarningWater != warning)
    {
        ESP_LOGI(LOG_TAG, "Water Empty Warning UPDATE");
        isWarningWater = warning;
        lv_obj_set_click(obj_btn_water, isWarningWater);
        lv_obj_set_style_local_image_recolor(img_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningWater ? LV_COLOR_ORANGE : LV_COLOR_GRAY);

        preparation.warnings.water_empty = isWarningWater;
    }
}

void ui_warning_bar_set_generic_warning(bool warning)
{
    if(isWarningGeneric != warning)
    {
        ESP_LOGI(LOG_TAG, "Generic Warning UPDATE");
        isWarningGeneric = warning;
        lv_obj_set_click(obj_btn_generic, isWarningGeneric);
        lv_obj_set_style_local_image_recolor(img_generic_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningGeneric ? LV_COLOR_ORANGE : LV_COLOR_GRAY);

        preparation.warnings.generic = isWarningGeneric;
    }
}

#if ENABLE_CAPS_RECOGNITION_MODULE == 1
void ui_warning_bar_update_recognition_status(void)
{
    //TODO
}
#endif

void ui_warning_bar_init(void)
{
    lvgl_sem_take();

    obj_warning_bar = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(obj_warning_bar, true);
    lv_obj_set_size(obj_warning_bar, WARNBAR_WIDTH, WARNBAR_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_warning_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj_warning_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(obj_warning_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_warning_bar, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);

    obj_btn_wifi = lv_obj_create(obj_warning_bar, NULL);
    lv_obj_set_size(obj_btn_wifi, MENUBAR_BUTTON_WIDTH, MENUBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_BORDER);
    lv_obj_set_style_local_text_color(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(obj_btn_wifi, obj_warning_bar, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_click(obj_btn_wifi, false);

    obj_label_wifi = lv_label_create(obj_btn_wifi, NULL);
    lv_label_set_text(obj_label_wifi, LV_SYMBOL_WIFI);
    lv_label_set_align(obj_label_wifi, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(obj_label_wifi, obj_btn_wifi, LV_ALIGN_CENTER, 0, 0);

    obj_btn_descaling = lv_obj_create(obj_warning_bar, NULL);
    lv_obj_set_size(obj_btn_descaling, WARNBAR_BUTTON_WIDTH, WARNBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_descaling, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_BORDER);
    lv_obj_align(obj_btn_descaling, NULL, LV_ALIGN_CENTER, 0, -1.5*MENUBAR_BUTTON_OFFSET);
    lv_obj_set_event_cb(obj_btn_descaling, obj_btn_warning_cb);

    img_descaling_warn = lv_img_create(obj_btn_descaling, NULL);
    lv_img_set_src(img_descaling_warn, data_descaling_warning);
    lv_img_set_zoom(img_descaling_warn, 140);
    lv_obj_set_style_local_image_recolor_opa(img_descaling_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(img_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningDescaling ? LV_COLOR_ORANGE : LV_COLOR_GRAY);
    lv_obj_align(img_descaling_warn, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_btn_pod = lv_obj_create(obj_warning_bar, NULL);
    lv_obj_set_size(obj_btn_pod, WARNBAR_BUTTON_WIDTH, WARNBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_pod, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_BORDER);
    lv_obj_align(obj_btn_pod, NULL, LV_ALIGN_CENTER, 0, -0.5*MENUBAR_BUTTON_OFFSET);
    lv_obj_set_event_cb(obj_btn_pod, obj_btn_warning_cb);
    
    img_pod_warn = lv_img_create(obj_btn_pod, NULL);
    lv_img_set_src(img_pod_warn, data_pod_warning);
    lv_img_set_zoom(img_pod_warn, 400);
    lv_obj_set_style_local_image_recolor_opa(img_pod_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(img_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningPod ? LV_COLOR_ORANGE : LV_COLOR_GRAY);
    lv_obj_align(img_pod_warn, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_btn_water = lv_obj_create(obj_warning_bar, NULL);
    lv_obj_set_size(obj_btn_water, WARNBAR_BUTTON_WIDTH, WARNBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_water, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_BORDER);
    lv_obj_align(obj_btn_water, NULL, LV_ALIGN_CENTER, 0, 0.5*MENUBAR_BUTTON_OFFSET);
    lv_obj_set_event_cb(obj_btn_water, obj_btn_warning_cb);

    img_water_warn = lv_img_create(obj_btn_water, NULL);
    lv_img_set_src(img_water_warn, data_water_warning);
    lv_img_set_zoom(img_water_warn, 140);
    lv_obj_set_style_local_image_recolor_opa(img_water_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(img_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningWater ? LV_COLOR_ORANGE : LV_COLOR_GRAY);
    lv_obj_align(img_water_warn, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_btn_generic = lv_obj_create(obj_warning_bar, NULL);
    lv_obj_set_size(obj_btn_generic, WARNBAR_BUTTON_WIDTH, WARNBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_generic, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_generic, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_generic, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_generic, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_BORDER);
    lv_obj_align(obj_btn_generic, NULL, LV_ALIGN_CENTER, 0, 1.5*MENUBAR_BUTTON_OFFSET);
    lv_obj_set_event_cb(obj_btn_generic, obj_btn_warning_cb);

    img_generic_warn = lv_img_create(obj_btn_generic, NULL);
    lv_img_set_src(img_generic_warn, data_generic_warning);
    lv_img_set_zoom(img_generic_warn, 110);
    lv_obj_set_style_local_image_recolor_opa(img_generic_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(img_generic_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, isWarningGeneric ? LV_COLOR_ORANGE : LV_COLOR_GRAY);
    lv_obj_align(img_generic_warn, NULL, LV_ALIGN_CENTER, 0, 0);

    #if CONFIG_ENABLE_CAPS_RECOGNITION_MODULE == 1
    obj_btn_recognition = lv_obj_create(obj_warning_bar, NULL);
    lv_obj_set_size(obj_btn_recognition, WARNBAR_BUTTON_WIDTH, WARNBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, WARNBAR_BUTTON_BORDER);
    lv_obj_set_style_local_value_font(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &stsbar_button_font);
    lv_obj_set_style_local_value_color(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_value_str(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_EYE_OPEN);
    lv_obj_align(obj_btn_recognition, obj_warning_bar, LV_ALIGN_IN_BOTTOM_MID, 0, -MENUBAR_BUTTON_OFFSET);
    lv_obj_set_event_cb(obj_btn_recognition, obj_btn_cb);
    #endif

    lv_obj_set_click(obj_btn_descaling, isWarningDescaling);
    lv_obj_set_click(obj_btn_water, isWarningWater);
    lv_obj_set_click(obj_btn_pod, isWarningPod);
    lv_obj_set_click(obj_btn_generic, isWarningGeneric);

    lvgl_sem_give();

    const esp_timer_create_args_t blink_led_timer_args = {
            .callback = &blink_led_cb,
            /* name is optional, but may help identify the timer when debugging */
            .name = "blink_led"
    };

    esp_timer_create(&blink_led_timer_args, &blink_led_timer);
}

void ui_warning_bar_show(bool show)
{
    if(NULL != obj_warning_bar)
    {
        lv_obj_set_hidden(obj_warning_bar, !show);

        ui_wifi_update_status(wifiStatus);

        ui_warning_bar_set_descaling_warning(isWarningDescaling);
        ui_warning_bar_set_pod_warning(isWarningPod);
        ui_warning_bar_set_water_empty_warning(isWarningWater);
        ui_warning_bar_set_generic_warning(isWarningGeneric);

        ESP_LOGI(LOG_TAG, "%s", show ? "Show" : "Hide");
    }
}

void ui_wifi_update_status(wifi_status_item_t status)
{
    wifiStatus = status;
    if(NULL != obj_btn_wifi)
    {
        switch(status)
        {
            case wifi_status_item_off:
            {
                lv_obj_set_style_local_text_color(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
                ESP_LOGI(LOG_TAG, "WiFi Status OFF.");
                break;
            }
            case wifi_status_item_connecting:
            {
                esp_timer_start_periodic(blink_led_timer, 500*1000);
                ESP_LOGI(LOG_TAG, "WiFi Status ON. Connecting...");
                break;
            }
            case wifi_status_item_connecting_cloud:
            {
                esp_timer_start_periodic(blink_led_timer, 100*1000);
                ESP_LOGI(LOG_TAG, "WiFi Status ON. Connecting...");
                break;
            }
            case wifi_status_item_connected:
            {
                esp_timer_stop(blink_led_timer);
                lv_obj_set_style_local_text_color(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
                ESP_LOGI(LOG_TAG, "WiFi Status ON. Connection established");
                break;
            }
            default:
            {
                //Do nothing
            }
        }
    }
}
/* ******************************** Event Handler ******************************** */
static void blink_led_cb(void* arg)
{
    static bool on = false;

    lv_obj_set_style_local_text_color(obj_btn_wifi, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, on ? LV_COLOR_BLUE : LV_COLOR_GRAY);
    on = !on;
}

static void obj_btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        #if ENABLE_CAPS_RECOGNITION_MODULE == 1
        if(obj_btn_recognition == obj) 
        {
            // if( false == isPreferencesPageActive &&
            //     false == isSettingsPageActive && 
            //     false == isStatisticsPageActive &&
            //     false == isErogationPageActive)
            // {
            //     if(true == isRecognitionPageActive)
            //     {
            //         lv_obj_set_style_local_value_color(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
            //         ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            //     }
            //     else
            //     {
            //         lv_obj_set_style_local_value_color(obj_btn_recognition, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
            //         ui_show(&ui_recognition_func, UI_SHOW_OVERRIDE);
            //     }
            // }
            // return;
        }
        #endif
    }
}

static void obj_btn_warning_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(false == isPopupOpened)
        {
            if(obj_btn_descaling == obj)
            {
                static const char* btns[] = { "OK", "START", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &warning_popup_font);
                lv_msgbox_set_text(msgbox, WARNBAR_DESCALING_NEEDED_MESSAGE);
                lv_msgbox_add_btns(msgbox, btns);
                lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_event_cb(msgbox, descaling_popup_cb);
                isPopupOpened = true;
            }

            if(obj_btn_water == obj)
            {
                static const char* btns[] = { "OK", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &warning_popup_font);
                lv_msgbox_set_text(msgbox, WARNBAR_WATER_EMPTY_MESSAGE);
                lv_msgbox_add_btns(msgbox, btns);
                lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_event_cb(msgbox, basic_popup_cb);
                isPopupOpened = true;
            }

            if(obj_btn_pod == obj)
            {
                static const char* btns[] = { "OK", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &warning_popup_font);
                lv_msgbox_set_text(msgbox, WARNBAR_POD_GENERIC_MESSAGE);
                lv_msgbox_add_btns(msgbox, btns);
                lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_event_cb(msgbox, basic_popup_cb);
                isPopupOpened = true;
            }

            if(obj_btn_generic == obj)
            {
                static const char* btns[] = { "OK", "" };
                lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
                lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &warning_popup_font);
                lv_msgbox_set_text(msgbox, WARNBAR_GENERIC_WARNING_MESSAGE);
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
            // ui_show(&ui_descaling_func, UI_SHOW_OVERRIDE);
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
