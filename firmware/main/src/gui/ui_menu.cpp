#include "ui_main.h"
#include "lv_qrcode.h"
#include "dbg_task.h"

#include "string_it.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_MENU"
#else
    #define LOG_TAG "UI_MENU"
#endif

/* UI function declaration */
ui_func_desc_t ui_menu_func = {
    .name = "ui_menu",
    .init = ui_menu_init,
    .show = ui_menu_show,
    .hide = ui_menu_hide,
};

static ui_state_t ui_menu_state = ui_state_dis;


/* LVGL objects defination */
static lv_obj_t* obj_container = NULL;

static lv_obj_t* obj_button_up      = NULL;
static lv_obj_t* obj_button_down    = NULL;
static lv_obj_t* obj_button_ok      = NULL;
static lv_obj_t* obj_button_back  = NULL;

static lv_obj_t* obj_list           = NULL;

static lv_obj_t* obj_list_general       = NULL;
static lv_obj_t* obj_list_wifi          = NULL;
static lv_obj_t* obj_list_settings      = NULL;
static lv_obj_t* obj_list_statistics    = NULL;
static lv_obj_t* obj_list_cleaning_1    = NULL;
static lv_obj_t* obj_list_cleaning_2    = NULL;
static lv_obj_t* obj_list_descaling     = NULL;

/* Static function forward declaration */
static void menu_btn_cb(lv_obj_t *obj, lv_event_t event);
static void list_btn_cb(lv_obj_t *obj, lv_event_t event);

static void update_selected_item_list();

typedef enum
{
    GENERAL_LIST_ITEM = 0,
    WIFI_LIST_ITEM,
    SETTINGS_LIST_ITEM,
    STATISTICS_LIST_ITEM,
    CLEANING_1_LIST_ITEM,
    CLEANING_2_LIST_ITEM,
    DESCALING_LIST_ITEM,

    MAX_LIST_ITEM
} list_item_t;

static list_item_t list_index;

static void configure_list_button(lv_obj_t* button)
{
    lv_obj_set_event_cb(button, list_btn_cb);
}

static void configure_main_button(lv_obj_t* button, const char* text)
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
    lv_obj_set_event_cb(button, menu_btn_cb);

    lv_obj_t* label = lv_label_create(button, NULL);
    lv_label_set_recolor(label, true);
    lv_label_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

static void configure_arrow_button(lv_obj_t* button, const char* text)
{
    lv_obj_set_size(button, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_RADIUS);
    lv_obj_set_style_local_border_width(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_BORDER);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_event_cb(button, menu_btn_cb);

    lv_obj_t* label = lv_label_create(button, NULL);
    lv_label_set_recolor(label, true);
    lv_label_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}


void ui_menu_init(void *data)
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
    configure_main_button(obj_button_ok, LV_SYMBOL_OK);
    lv_obj_align(obj_button_ok, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, -40);   

    obj_button_back = lv_btn_create(obj_container, NULL);
    configure_main_button(obj_button_back, LV_SYMBOL_CLOSE);
    lv_obj_align(obj_button_back, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 40);   

    obj_button_up = lv_btn_create(obj_container, NULL);
    configure_arrow_button(obj_button_up, LV_SYMBOL_UP);
    lv_obj_align(obj_button_up, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, -120);   

    obj_button_down = lv_btn_create(obj_container, NULL);
    configure_arrow_button(obj_button_down, LV_SYMBOL_DOWN);
    lv_obj_align(obj_button_down, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 120);   

    obj_list = lv_list_create(obj_container, NULL);
    lv_obj_set_size(obj_list, MENU_LIST_WIDTH, MENU_LIST_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_list, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_list, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_list, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_list, NULL, LV_ALIGN_IN_LEFT_MID, MENU_LIST_X_OFFSET, 0);

    obj_list_general = lv_list_add_btn(obj_list, LV_SYMBOL_HOME, "\t" MENU_GENERAL_BUTTON);
    configure_list_button(obj_list_general);

    obj_list_wifi = lv_list_add_btn(obj_list, LV_SYMBOL_WIFI, "\t" MENU_WIFI_BUTTON);
    configure_list_button(obj_list_wifi);

    obj_list_settings = lv_list_add_btn(obj_list, LV_SYMBOL_SETTINGS, "\t" MENU_SETTINGS_BUTTON);
    configure_list_button(obj_list_settings);

    obj_list_statistics = lv_list_add_btn(obj_list, LV_SYMBOL_LIST, "\t" MENU_STATISTICS_BUTTON);
    configure_list_button(obj_list_statistics);

    obj_list_cleaning_1 = lv_list_add_btn(obj_list, LV_SYMBOL_REFRESH, "\t" MENU_CLEANING1_BUTTON);
    configure_list_button(obj_list_cleaning_1);

    obj_list_cleaning_2 = lv_list_add_btn(obj_list, LV_SYMBOL_REFRESH LV_SYMBOL_REFRESH, "\t" MENU_CLEANING2_BUTTON);
    configure_list_button(obj_list_cleaning_2);

    obj_list_descaling = lv_list_add_btn(obj_list, LV_SYMBOL_LOOP, "\t" MENU_DESCALING_BUTTON);
    configure_list_button(obj_list_descaling);

    lvgl_sem_give();

    ui_menu_state = ui_state_show;
}

void ui_menu_show(void *data)
{
    list_index = GENERAL_LIST_ITEM;

    if(ui_state_dis == ui_menu_state)
    {
        ui_menu_init(data);
    }
    else
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, false);
        }

        ui_menu_state = ui_state_show;
    }

    update_selected_item_list();

    ui_warning_bar_show(false);
    ui_menu_bar_show(false);

    ESP_LOGI(LOG_TAG, "Show");
}

void ui_menu_hide(void *data)
{
    (void)data;

    if(ui_state_show == ui_menu_state)
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, true);
        }

        ui_menu_state = ui_state_hide;
    }

    ESP_LOGI(LOG_TAG, "Hide");
}


/* ******************************** Event Handler(s) ******************************** */
static void menu_btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == obj_button_ok)
        {
            switch(list_index)
            {
                case GENERAL_LIST_ITEM:
                {
                    ESP_LOGI(LOG_TAG, "Open GENERAL page");
                    // ui_show(&ui_general_func, UI_SHOW_OVERRIDE);
                    break;
                }
                case WIFI_LIST_ITEM:
                {
                    ESP_LOGI(LOG_TAG, "Open WIFI page");
                    ui_show(&ui_wifi_func, UI_SHOW_OVERRIDE);
                    break;
                }
                case SETTINGS_LIST_ITEM:
                {
                    ESP_LOGI(LOG_TAG, "Open SETTINGS page");
                    ui_show(&ui_settings_func, UI_SHOW_OVERRIDE);
                    break;
                }
                case STATISTICS_LIST_ITEM:
                {
                    ESP_LOGI(LOG_TAG, "Open STATISTICS page");
                    // ui_show(&ui_statistics_func, UI_SHOW_OVERRIDE);
                    break;
                }
                case CLEANING_1_LIST_ITEM:
                {
                    ESP_LOGI(LOG_TAG, "Open CLEANING 1 page");
                    // ui_show(&ui_cleaning_auto_func, UI_SHOW_OVERRIDE);
                    break;
                }
                case CLEANING_2_LIST_ITEM:
                {
                    ESP_LOGI(LOG_TAG, "Open CLEANING 2 page");
                    // ui_show(&ui_cleaning_semiauto_func, UI_SHOW_OVERRIDE);
                    break;
                }
                case DESCALING_LIST_ITEM:
                {
                    ESP_LOGI(LOG_TAG, "Open DESCALING page");
                    // ui_show(&ui_descaling_func, UI_SHOW_OVERRIDE);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        
        if(obj == obj_button_back)
        {
            printf("BACK Button\n");
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_button_up)
        {
            list_index = (list_item_t)((MAX_LIST_ITEM + list_index - 1)%MAX_LIST_ITEM);
            printf("UP Button (next item %d)\n", list_index);
            update_selected_item_list();
        }

        if(obj == obj_button_down)
        {
            list_index = (list_item_t)((MAX_LIST_ITEM + list_index + 1)%MAX_LIST_ITEM);
            printf("DOWN Button (next item %d)\n", list_index);
            update_selected_item_list();
        }

    }
}

static void list_btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {

        if(obj == obj_list_general)
        {
            ESP_LOGI(LOG_TAG, "Open GENERAL page");
            // ui_show(&ui_general_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_list_wifi)
        {
            ESP_LOGI(LOG_TAG, "Open WIFI page");
            ui_show(&ui_wifi_func, UI_SHOW_OVERRIDE);
        }
        
        if(obj == obj_list_settings)
        {
            ESP_LOGI(LOG_TAG, "Open SETTINGS page");
            ui_show(&ui_settings_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_list_statistics)
        {
            ESP_LOGI(LOG_TAG, "Open STATISTICS page");
            // ui_show(&ui_statistics_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_list_cleaning_1)
        {
            ESP_LOGI(LOG_TAG, "Open CLEANING 1 page");
            // ui_show(&ui_cleaning_auto_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_list_cleaning_2)
        {
            ESP_LOGI(LOG_TAG, "Open CLEANING 2 page");
            // ui_show(&ui_cleaning_semiauto_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_list_descaling)
        {
            ESP_LOGI(LOG_TAG, "Open DESCALING page");
            // ui_show(&ui_descaling_func, UI_SHOW_OVERRIDE);
        }
    }
}

static void update_selected_item_list()
{
    switch(list_index)
    {
        case GENERAL_LIST_ITEM:
        {
            lv_list_focus_btn(obj_list, obj_list_general);
            break;
        }
        case WIFI_LIST_ITEM:
        {
            lv_list_focus_btn(obj_list, obj_list_wifi);
            break;
        }
        case SETTINGS_LIST_ITEM:
        {
            lv_list_focus_btn(obj_list, obj_list_settings);
            break;
        }
        case STATISTICS_LIST_ITEM:
        {
            lv_list_focus_btn(obj_list, obj_list_statistics);
            break;
        }
        case CLEANING_1_LIST_ITEM:
        {
            lv_list_focus_btn(obj_list, obj_list_cleaning_1);
            break;
        }
        case CLEANING_2_LIST_ITEM:
        {
            lv_list_focus_btn(obj_list, obj_list_cleaning_2);
            break;
        }
        case DESCALING_LIST_ITEM:
        {
            lv_list_focus_btn(obj_list, obj_list_descaling);
            break;
        }
        default:
        {
            break;
        }
    }
}