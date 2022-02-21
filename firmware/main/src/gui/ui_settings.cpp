#include "ui_main.h"
#include "lv_qrcode.h"
#include "dbg_task.h"

#include "string_it.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_SETTINGS"
#else
    #define LOG_TAG "UI_SETTINGS"
#endif

typedef struct {
    uint16_t  parId;             /* Parameter ID */
    lv_obj_t* button;            /* Button */
} ui_settings_list_item_t;

/* UI function declaration */
ui_func_desc_t ui_settings_func = {
    .name = "ui_settings",
    .init = ui_settings_init,
    .show = ui_settings_show,
    .hide = ui_settings_hide,
};

static ui_state_t ui_settings_state = ui_state_dis;


/* LVGL objects defination */
static lv_obj_t* obj_container = NULL;

static lv_obj_t* obj_button_up      = NULL;
static lv_obj_t* obj_button_down    = NULL;
static lv_obj_t* obj_button_save    = NULL;
static lv_obj_t* obj_button_refresh = NULL;
static lv_obj_t* obj_button_back    = NULL;

static lv_obj_t* obj_list           = NULL;

static lv_obj_t* obj_list_par001    = NULL;
static lv_obj_t* obj_list_par002    = NULL;
static lv_obj_t* obj_list_par003    = NULL;
static lv_obj_t* obj_list_par004    = NULL;
static lv_obj_t* obj_list_par005    = NULL;
static lv_obj_t* obj_list_par006    = NULL;
static lv_obj_t* obj_list_par007    = NULL;
static lv_obj_t* obj_list_par008    = NULL;
static lv_obj_t* obj_list_par009    = NULL;
static lv_obj_t* obj_list_par010    = NULL;

/* Static function forward declaration */
static void menu_btn_cb(lv_obj_t *obj, lv_event_t event);
static void list_btn_cb(lv_obj_t *obj, lv_event_t event);

static void update_selected_item_list();

#define MAX_PARAMETERS_LIST     10


static uint16_t list_index;


static uint16_t get_parid_from_button(lv_obj_t* button)
{
    uint16_t parId = -1;
    if(button == obj_list_par001)
        parId = 1;
    else if(button == obj_list_par002)
        parId = 2;
    else if(button == obj_list_par003)
        parId = 3;
    else if(button == obj_list_par004)
        parId = 4;
    else if(button == obj_list_par005)
        parId = 5;
    else if(button == obj_list_par006)
        parId = 6;
    else if(button == obj_list_par007)
        parId = 7;
    else if(button == obj_list_par008)
        parId = 8;
    else if(button == obj_list_par009)
        parId = 9;
    else if(button == obj_list_par010)
        parId = 10;
    else 
        parId = -1;

    return parId;
}

static lv_obj_t* get_button_from_parid(uint16_t id)
{
    lv_obj_t* btn = NULL;
    
    if(id == 1)
        btn = obj_list_par001;
    else if(id == 2)
        btn = obj_list_par002;
    else if(id == 3)
        btn = obj_list_par003;
    else if(id == 4)
        btn = obj_list_par004;
    else if(id == 5)
        btn = obj_list_par005;
    else if(id == 6)
        btn = obj_list_par006;
    else if(id == 7)
        btn = obj_list_par007;
    else if(id == 8)
        btn = obj_list_par008;
    else if(id == 9)
        btn = obj_list_par009;
    else if(id == 10)
        btn = obj_list_par010;
    else
        btn = NULL;

    return btn;
}



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
    lv_obj_set_style_local_text_font(button, LV_OBJ_PART_MAIN, LV_STATE_ALL, &menubar_button_font);
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
    lv_obj_set_style_local_text_font(button, LV_OBJ_PART_MAIN, LV_STATE_ALL, &menubar_button_font);
    lv_obj_set_event_cb(button, menu_btn_cb);

    lv_obj_t* label = lv_label_create(button, NULL);
    lv_label_set_recolor(label, true);
    lv_label_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}


void ui_settings_init(void *data)
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

    obj_button_refresh = lv_btn_create(obj_container, NULL);
    configure_main_button(obj_button_refresh, LV_SYMBOL_REFRESH);
    lv_obj_align(obj_button_refresh, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 0);

    obj_button_save = lv_btn_create(obj_container, NULL);
    configure_main_button(obj_button_save, LV_SYMBOL_SAVE);
    lv_obj_align(obj_button_save, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, -70); 

    obj_button_back = lv_btn_create(obj_container, NULL);
    configure_main_button(obj_button_back, LV_SYMBOL_CLOSE);
    lv_obj_align(obj_button_back, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 70);   

    obj_button_up = lv_btn_create(obj_container, NULL);
    configure_arrow_button(obj_button_up, LV_SYMBOL_UP);
    lv_obj_align(obj_button_up, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, -150);   

    obj_button_down = lv_btn_create(obj_container, NULL);
    configure_arrow_button(obj_button_down, LV_SYMBOL_DOWN);
    lv_obj_align(obj_button_down, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 150);   

    obj_list = lv_list_create(obj_container, NULL);
    lv_obj_set_size(obj_list, MENU_LIST_WIDTH, MENU_LIST_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_list, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_list, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_list, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_list, NULL, LV_ALIGN_IN_LEFT_MID, MENU_LIST_X_OFFSET, 0);

    obj_list_par001 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 1");
    configure_list_button(obj_list_par001);

    obj_list_par002 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 2");
    configure_list_button(obj_list_par002);

    obj_list_par003 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 3");
    configure_list_button(obj_list_par003);

    obj_list_par004 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 4");
    configure_list_button(obj_list_par004);

    obj_list_par005 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 5");
    configure_list_button(obj_list_par005);

    obj_list_par006 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 6");
    configure_list_button(obj_list_par006);

    obj_list_par007 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 7");
    configure_list_button(obj_list_par007);

    obj_list_par008 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 8");
    configure_list_button(obj_list_par008);

    obj_list_par009 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 9");
    configure_list_button(obj_list_par009);

    obj_list_par010 = lv_list_add_btn(obj_list, NULL, "\t" "Parametro 10");
    configure_list_button(obj_list_par010);

    lvgl_sem_give();

    ui_settings_state = ui_state_show;
}

void ui_settings_show(void *data)
{
    list_index = 0;

    if(ui_state_dis == ui_settings_state)
    {
        ui_settings_init(data);
    }
    else
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, false);
        }

        ui_settings_state = ui_state_show;
    }

    update_selected_item_list();

    ui_warning_bar_show(false);
    ui_menu_bar_show(false);

    ESP_LOGI(LOG_TAG, "Show");
}

void ui_settings_hide(void *data)
{
    (void)data;

    if(ui_state_show == ui_settings_state)
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, true);
        }

        ui_settings_state = ui_state_hide;
    }

    ESP_LOGI(LOG_TAG, "Hide");
}


/* ******************************** Event Handler(s) ******************************** */
static void menu_btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == obj_button_refresh)
        {
            ESP_LOGI(LOG_TAG, "Refresh parameters");
        }

        if(obj == obj_button_save)
        {
            ESP_LOGI(LOG_TAG, "Save parameters");
        }
        
        if(obj == obj_button_back)
        {
            printf("BACK Button\n");
            ui_show(&ui_menu_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_button_up)
        {
            list_index = ((MAX_PARAMETERS_LIST + list_index - 1)%MAX_PARAMETERS_LIST);
            printf("UP Button (next item %d)\n", list_index);
            update_selected_item_list();
        }

        if(obj == obj_button_down)
        {
            list_index = ((MAX_PARAMETERS_LIST + list_index + 1)%MAX_PARAMETERS_LIST);
            printf("DOWN Button (next item %d)\n", list_index);
            update_selected_item_list();
        }

    }
}

static void list_btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        ESP_LOGI(LOG_TAG, "Show popup for parameter %d", get_parid_from_button(obj));
        list_index = get_parid_from_button(obj)-1;
    }
}

static void update_selected_item_list()
{
    lv_list_focus_btn(obj_list, get_button_from_parid(list_index+1));
}