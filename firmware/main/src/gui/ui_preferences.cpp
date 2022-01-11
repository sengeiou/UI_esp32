#include "ui_main.h"
#include "lvgl_port.h"
#include "variables.h"
#include "dbg_task.h"

#define BUTTON_X_SIZE     140
#define BUTTON_Y_SIZE     110

#define X_CENTER        160
#define Y_CENTER        65
#define Y_OFFSET        20

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_PREF"
#else
    #define LOG_TAG "UI_PREF"
#endif

/* UI function declaration */
ui_func_desc_t ui_preferences_func = {
    .name = "ui_preferences",
    .init = ui_preferences_init,
    .show = ui_preferences_show,
    .hide = ui_preferences_hide,
};

/* LVGL objects defination */
static lv_obj_t* obj_page_settings = NULL;
static lv_obj_t* obj_page_statistics = NULL;
static lv_obj_t* obj_page_factory_reset = NULL;
static lv_obj_t* obj_page_wifi_reset = NULL;
static lv_obj_t* obj_page_fast_cleaning = NULL;
static lv_obj_t* obj_page_full_cleaning = NULL;

/* Static function forward declaration */
static void btn_cb(lv_obj_t* obj, lv_event_t event);

bool isPreferencesPageActive = false;

static void wifi_reset_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        if(1 == lv_msgbox_get_active_btn(obj))
        {
            machineConnectivity.wifiEnabled = false;
            ui_status_bar_update_wifi_status(machineConnectivity.wifiEnabled);
        }

        ui_show(&ui_preferences_func, UI_SHOW_OVERRIDE);
        lv_obj_del(obj);
    }
}

void ui_preferences_init(void *data)
{
    (void)data;
    obj_page_settings = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_page_settings, BUTTON_X_SIZE, BUTTON_Y_SIZE);
    lv_obj_set_style_local_bg_color(obj_page_settings, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_page_settings, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_page_settings, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_page_settings, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_page_settings, NULL, LV_ALIGN_CENTER, -X_CENTER, -Y_CENTER +Y_OFFSET);

    obj_page_statistics = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_page_statistics, BUTTON_X_SIZE, BUTTON_Y_SIZE);
    lv_obj_set_style_local_bg_color(obj_page_statistics, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_page_statistics, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_page_statistics, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_page_statistics, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_page_statistics, NULL, LV_ALIGN_CENTER, -X_CENTER, Y_CENTER +Y_OFFSET);

    obj_page_factory_reset = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_page_factory_reset, BUTTON_X_SIZE, BUTTON_Y_SIZE);
    lv_obj_set_style_local_bg_color(obj_page_factory_reset, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_page_factory_reset, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_page_factory_reset, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_page_factory_reset, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_page_factory_reset, NULL, LV_ALIGN_CENTER, X_CENTER, -Y_CENTER +Y_OFFSET);

    obj_page_wifi_reset = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_page_wifi_reset, BUTTON_X_SIZE, BUTTON_Y_SIZE);
    lv_obj_set_style_local_bg_color(obj_page_wifi_reset, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_page_wifi_reset, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_page_wifi_reset, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_page_wifi_reset, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_page_wifi_reset, NULL, LV_ALIGN_CENTER, X_CENTER, Y_CENTER +Y_OFFSET);

    obj_page_fast_cleaning = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_page_fast_cleaning, BUTTON_X_SIZE, BUTTON_Y_SIZE);
    lv_obj_set_style_local_bg_color(obj_page_fast_cleaning, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_page_fast_cleaning, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_page_fast_cleaning, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_page_fast_cleaning, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_page_fast_cleaning, NULL, LV_ALIGN_CENTER, 0, -Y_CENTER +Y_OFFSET);

    obj_page_full_cleaning = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_page_full_cleaning, BUTTON_X_SIZE, BUTTON_Y_SIZE);
    lv_obj_set_style_local_bg_color(obj_page_full_cleaning, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_page_full_cleaning, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_page_full_cleaning, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_page_full_cleaning, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_page_full_cleaning, NULL, LV_ALIGN_CENTER, 0, +Y_CENTER +Y_OFFSET);
   
    lv_obj_set_style_local_value_str(obj_page_settings,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Settings");
    lv_obj_set_style_local_value_font(obj_page_settings, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_page_settings,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_page_settings,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_page_settings,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    
    lv_obj_set_style_local_value_str(obj_page_statistics,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Statistics");
    lv_obj_set_style_local_value_font(obj_page_statistics, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_page_statistics,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_page_statistics,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_page_statistics,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_obj_set_style_local_value_str(obj_page_factory_reset,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Factory\n Reset");
    lv_obj_set_style_local_value_font(obj_page_factory_reset, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_page_factory_reset,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_page_factory_reset,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_page_factory_reset,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_obj_set_style_local_value_str(obj_page_wifi_reset,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, " Wifi\nReset");
    lv_obj_set_style_local_value_font(obj_page_wifi_reset, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_page_wifi_reset,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_page_wifi_reset,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_page_wifi_reset,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_obj_set_style_local_value_str(obj_page_fast_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, " Auto\nClean");
    lv_obj_set_style_local_value_font(obj_page_fast_cleaning, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_page_fast_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_page_fast_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_page_fast_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_obj_set_style_local_value_str(obj_page_full_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Semi-Auto\n    Clean");
    lv_obj_set_style_local_value_font(obj_page_full_cleaning, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_page_full_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_page_full_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_page_full_cleaning,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_obj_set_event_cb(obj_page_settings, btn_cb);
    lv_obj_set_event_cb(obj_page_statistics, btn_cb);
    lv_obj_set_event_cb(obj_page_factory_reset, btn_cb);
    lv_obj_set_event_cb(obj_page_wifi_reset, btn_cb);
    lv_obj_set_event_cb(obj_page_fast_cleaning, btn_cb);
    lv_obj_set_event_cb(obj_page_full_cleaning, btn_cb);
}

void ui_preferences_show(void *data)
{
    if(NULL == obj_page_statistics)
    {
        ui_preferences_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_page_statistics, false);
        lv_obj_set_hidden(obj_page_settings, false);
        lv_obj_set_hidden(obj_page_factory_reset, false);
        lv_obj_set_hidden(obj_page_wifi_reset, false);
        lv_obj_set_hidden(obj_page_fast_cleaning, false);
        lv_obj_set_hidden(obj_page_full_cleaning, false);
    }
    isPreferencesPageActive = true;

    ui_status_bar_show(true);

    #if ENABLE_CAPS_RECOGNITION_MODULE == 1
    ui_status_bar_update_recognition_status();
    #endif
}

void ui_preferences_hide(void *data)
{
    (void)data;

    if(NULL != obj_page_statistics)
    {
        lv_obj_set_hidden(obj_page_settings, true);
        lv_obj_set_hidden(obj_page_statistics, true);
        lv_obj_set_hidden(obj_page_factory_reset, true);
        lv_obj_set_hidden(obj_page_wifi_reset, true);
        lv_obj_set_hidden(obj_page_fast_cleaning, true);
        lv_obj_set_hidden(obj_page_full_cleaning, true);
    }

    isPreferencesPageActive = false;
}

/* ******************************** Event Handler(s) ******************************** */
static void btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if (LV_EVENT_CLICKED == event)
    {
        if(obj == obj_page_settings)
        {
            ui_show(&ui_setting_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_page_statistics)
        {
            ui_show(&ui_statistics_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_page_factory_reset)
        {
            ESP_LOGI(LOG_TAG, "FACTORY RESET");
            static const char* btns[] = { "CANCEL", "CONFIRM", "" };
            lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
            lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
            lv_msgbox_set_text(msgbox, "Factory reset?");
            lv_msgbox_add_btns(msgbox, btns);
            lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
        }

        if(obj == obj_page_wifi_reset)
        {
            ESP_LOGI(LOG_TAG, "WIFI RESET");
            static const char* btns[] = { "CANCEL", "CONFIRM", "" };
            lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
            lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
            lv_msgbox_set_text(msgbox, "Wifi reset?");
            lv_msgbox_add_btns(msgbox, btns);
            lv_obj_set_event_cb(msgbox, wifi_reset_cb);
            lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
        }

        if(obj == obj_page_fast_cleaning)
        {
            if(true == guiInternalState.powerOn)
            {
                special_function(DBG_ON_OFF);
                vTaskDelay(pdMS_TO_TICKS(200));
            }

            special_function(DBG_FAST_CLEANING);
            vTaskDelay(pdMS_TO_TICKS(200));
            ui_show(&ui_fast_cleaning_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_page_full_cleaning)
        {
            if(true == guiInternalState.powerOn)
            {
                special_function(DBG_ON_OFF);
                vTaskDelay(pdMS_TO_TICKS(200));
            }

            special_function(DBG_FULL_CLEANING);
            vTaskDelay(pdMS_TO_TICKS(200));
            ui_show(&ui_full_cleaning_func, UI_SHOW_OVERRIDE);
        }
    }
}
