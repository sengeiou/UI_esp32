#include "ui_main.h"
#include "lv_qrcode.h"
#include "dbg_task.h"
#include "esp_wifi.h"
#include "variables.h"
#include "spiffs_utils.h"
#include "wifi_task.h"
#include "system_utils.h"
#include "string_it.h"

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

static ui_state_t ui_wifi_state = ui_state_dis;


/* LVGL objects defination */
static lv_obj_t* obj_container = NULL;

static lv_obj_t* obj_button_ok      = NULL;
static lv_obj_t* obj_button_cancel  = NULL;
static lv_obj_t* obj_button_scan    = NULL;
static lv_obj_t* obj_ddlist_ssid    = NULL;
static lv_obj_t* obj_password_area  = NULL;
static lv_obj_t* obj_keyboard       = NULL;

/* Static function forward declaration */
static void wifi_btn_cb(lv_obj_t *obj, lv_event_t event);
static void wifi_ssid_cb(lv_obj_t* obj, lv_event_t event);
static void keyboard_cb(lv_obj_t* keyboard, lv_event_t event);
static void pass_textarea_cb(lv_obj_t* obj, lv_event_t event);

#define DEFAULT_SCAN_LIST_SIZE 32
static char ssid[128];


static void configure_button(lv_obj_t* button, const char* text)
{
    lv_obj_set_size(button, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_RADIUS);
    lv_obj_set_style_local_border_width(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_BORDER);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_event_cb(button, wifi_btn_cb);

    lv_obj_t* label = lv_label_create(button, NULL);
    lv_label_set_recolor(label, true);
    lv_label_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}


void ui_wifi_init(void *data)
{
    (void)data;

    lvgl_sem_take();

    obj_container = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(obj_container, true);
    lv_obj_set_size(obj_container, MENU_CONT_WIDTH, MENU_CONT_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_container, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_button_ok = lv_btn_create(obj_container, NULL);
    configure_button(obj_button_ok, LV_SYMBOL_SAVE);
    lv_obj_align(obj_button_ok, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, -40);   

    obj_button_cancel = lv_btn_create(obj_container, NULL);
    configure_button(obj_button_cancel, LV_SYMBOL_CLOSE);
    lv_obj_align(obj_button_cancel, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 40);   

    obj_button_scan = lv_btn_create(obj_container, NULL);
    lv_obj_set_size(obj_button_scan, WIFI_SCAN_BUTTON_WIDTH, WIFI_SCAN_BUTTON_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_button_scan, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_button_scan, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_border_width(obj_button_scan, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_text_color(obj_button_scan, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_event_cb(obj_button_scan, wifi_btn_cb);
    lv_obj_align(obj_button_scan, NULL, LV_ALIGN_IN_TOP_MID, WIFI_SCAN_BUTTON_X_OFFSET, WIFI_SCAN_BUTTON_Y_OFFSET);   

    lv_obj_t* label = lv_label_create(obj_button_scan, NULL);
    lv_label_set_recolor(label, true);
    lv_label_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, LV_SYMBOL_REFRESH "  " "SCAN");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_ddlist_ssid = lv_dropdown_create(obj_container, NULL);
    lv_obj_set_size(obj_ddlist_ssid, WIFI_SCAN_LIST_WIDTH, WIFI_SCAN_LIST_HEIGHT);
    lv_dropdown_set_options(obj_ddlist_ssid, "Scan Wi-Fi network...");
    lv_obj_align(obj_ddlist_ssid, obj_button_scan, LV_ALIGN_OUT_BOTTOM_MID, 0, WIFI_SCAN_LIST_Y_OFFSET);
    lv_obj_set_event_cb(obj_ddlist_ssid, wifi_ssid_cb);

    obj_password_area = lv_textarea_create(obj_container, NULL);
    lv_obj_set_size(obj_password_area, WIFI_PASS_AREA_WIDTH, WIFI_PASS_AREA_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_password_area, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_password_area, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_password_area, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_password_area, obj_ddlist_ssid, LV_ALIGN_OUT_BOTTOM_MID, 0, WIFI_PASS_AREA_Y_OFFSET);
    lv_textarea_set_text(obj_password_area, "password");
    lv_obj_set_event_cb(obj_password_area, pass_textarea_cb);

    lvgl_sem_give();

    ui_wifi_state = ui_state_show;
}

void ui_wifi_show(void *data)
{
    if(ui_state_dis == ui_wifi_state)
    {
        ui_wifi_init(data);
    }
    else
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, false);
        }

        ui_wifi_state = ui_state_show;
    }

    ui_warning_bar_show(false);
    ui_menu_bar_show(false);

    ESP_LOGI(LOG_TAG, "Show");
}

void ui_wifi_hide(void *data)
{
    (void)data;

    if(ui_state_show == ui_wifi_state)
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, true);
        }

        ui_wifi_state = ui_state_hide;
    }

    ESP_LOGI(LOG_TAG, "Hide");
}


/* ******************************** Event Handler(s) ******************************** */
static void wifi_btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == obj_button_ok)
        {
            strcpy(machineConnectivity.ssid, ssid);
            strcpy(machineConnectivity.password, lv_textarea_get_text(obj_password_area));
            machineConnectivity.wifiEnabled = true;
            xEventGroupSetBits(xWifiEvents, WIFI_CONFIGURATION_BIT);

            ui_show(&ui_menu_func, UI_SHOW_OVERRIDE);
        }
        
        if(obj == obj_button_cancel)
        {
            esp_wifi_scan_stop();
            ui_show(&ui_menu_func, UI_SHOW_OVERRIDE);
        }
        
        if(obj == obj_button_scan)
        {
            printf("SCAN Button\n");
            lv_dropdown_clear_options(obj_ddlist_ssid);

            xEventGroupSetBits(xWifiEvents, WIFI_RESET_BIT);
            std::this_thread::sleep_for(std::chrono::seconds(3));

            if(ESP_OK == esp_wifi_scan_start(NULL, true))
            {
                uint16_t number = DEFAULT_SCAN_LIST_SIZE;
                wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
                uint16_t ap_count = 0;
                memset(ap_info, 0, sizeof(ap_info));

                esp_wifi_scan_get_ap_records(&number, ap_info);
                esp_wifi_scan_get_ap_num(&ap_count);
                ESP_LOGI(LOG_TAG, "Total APs scanned = %u", ap_count);
                for(int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
                {
                    ESP_LOGI(LOG_TAG, "SSID \t\t%s (RSSI: %d)", ap_info[i].ssid, ap_info[i].rssi);
                    lv_dropdown_add_option(obj_ddlist_ssid, (const char*)ap_info[i].ssid, i);
                }
            }
            else
            {
                printf("Failed to scan networks\n");
            }
        }
    }
}

static void wifi_ssid_cb(lv_obj_t* obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        if(0 == lv_dropdown_get_option_cnt(obj))
        {
            ESP_LOGW(LOG_TAG, "Not found Wi-Fi networks");
        }
        else
        {
            lv_dropdown_get_selected_str(obj, ssid, sizeof(ssid));
            ESP_LOGI(LOG_TAG, "Selected Wi-Fi network: %s", ssid);
        }
    }
}

static void pass_textarea_cb(lv_obj_t* obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED  && obj_keyboard == NULL)
    {
        obj_keyboard = lv_keyboard_create(lv_scr_act(), NULL);
        lv_keyboard_set_cursor_manage(obj_keyboard, true);
        lv_obj_set_event_cb(obj_keyboard, keyboard_cb);
        lv_keyboard_set_textarea(obj_keyboard, obj);
    }
}

static void keyboard_cb(lv_obj_t* keyboard, lv_event_t event)
{
    if(event == LV_EVENT_CANCEL || event == LV_EVENT_APPLY)
    {
        lv_keyboard_set_textarea(obj_keyboard, NULL);
        lv_obj_del(obj_keyboard);
        obj_keyboard = NULL;
    }
    else
    {
        lv_keyboard_def_event_cb(obj_keyboard, event);
    }
}