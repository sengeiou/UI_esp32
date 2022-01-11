#include "ui_main.h"
#include "lvgl_port.h"
#include <time.h>
#include "dbg_task.h"

#define RADIUS          62
#define BUTTON_SIZE     62
#define WARN_SIZE       40

#define RAD_X_CENTER    125
#define RAD_Y_CENTER    0
#define ROUNDER         15

ui_preparation_t    preparation;

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_PREP"
#else
    #define LOG_TAG "UI_PREP"
#endif

/* UI function declaration */
ui_func_desc_t ui_preparations_func = {
    .name = "ui_preparations",
    .init = ui_preparations_init,
    .show = ui_preparations_show,
    .hide = ui_preparations_hide,
};

static ui_state_t ui_preparations_state = ui_state_dis;

/* LVGL objects defination */
static lv_obj_t* obj_coffee_short = NULL;
static lv_obj_t* obj_coffee_medium = NULL;
static lv_obj_t* obj_coffee_long = NULL;
static lv_obj_t* obj_coffee_free = NULL;
static lv_obj_t* obj_cappuccino_short = NULL;
static lv_obj_t* obj_cappuccino_medium = NULL;
static lv_obj_t* obj_cappuccino_double = NULL;
static lv_obj_t* obj_milk_hot = NULL;

static lv_obj_t* img_coffee_short = NULL;
static lv_obj_t* img_coffee_medium = NULL;
static lv_obj_t* img_coffee_long = NULL;
static lv_obj_t* img_coffee_free = NULL;
static lv_obj_t* img_cappuccino_short = NULL;
static lv_obj_t* img_cappuccino_medium = NULL;
static lv_obj_t* img_cappuccino_double = NULL;
static lv_obj_t* img_milk_hot = NULL;

static lv_obj_t* obj_tabview = NULL;
static lv_obj_t* obj_tabCoffee = NULL;
static lv_obj_t* obj_tabCappuccino = NULL;


static bool isCappuccinoEnable = false;
static bool isMachinePowerOn = false;
static bool isMachineFault = false;
static uint8_t lastTabActive = 0;

/* Extern image variable(s) */
extern void* data_short_coffee;
extern void* data_medium_coffee;
extern void* data_long_coffee;
extern void* data_free_coffee;
extern void* data_short_cappuccino;
extern void* data_medium_cappuccino;
extern void* data_double_cappuccino;
extern void* data_hot_milk;

/* Static function forward declaration */
static void btn_coffee_cb(lv_obj_t* obj, lv_event_t event);
static void btn_cappuccino_cb(lv_obj_t* obj, lv_event_t event);
static void tabview_cb(lv_obj_t* obj, lv_event_t event);


void configure_button_style(lv_obj_t* obj)
{
    lv_obj_set_size(obj, 180, 100);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, 4);
    lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
}

void configure_image_style(lv_obj_t* obj, const void* src)
{
    lv_img_set_src(obj, src);
    lv_img_set_zoom(obj, 170);
    lv_obj_set_style_local_image_recolor_opa(obj, LV_IMG_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_CHECKED | LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_HOVERED, LV_OPA_70);
    lv_obj_set_style_local_image_recolor(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_CHECKED | LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_HOVERED, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor_opa(obj, LV_IMG_PART_MAIN, LV_STATE_PRESSED, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
}

void ui_preparations_init(void *data)
{
    (void)data;

    /* Preparation page */
    obj_tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_tabview, 470, 250);
    lv_tabview_set_btns_pos(obj_tabview, LV_TABVIEW_TAB_POS_LEFT);
    lv_tabview_set_anim_time(obj_tabview, 0);
	lv_obj_set_style_local_pad_top(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_left(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_right(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, 10);
	lv_obj_set_style_local_pad_bottom(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, 10);

    lv_obj_set_style_local_bg_color(obj_tabview, LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    lv_obj_set_style_local_bg_color(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_border_color(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, isCappuccinoEnable ? LV_COLOR_GRAY : LV_COLOR_BLACK);

    lv_obj_set_style_local_bg_color(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_CHECKED, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_CHECKED, 2);
    lv_obj_set_style_local_border_color(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_CHECKED, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_CHECKED, LV_COLOR_WHITE);

    lv_obj_set_style_local_bg_color(obj_tabview, LV_TABVIEW_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(obj_tabview, LV_TABVIEW_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_tabview, LV_TABVIEW_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(obj_tabview, LV_TABVIEW_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_WHITE);

    lv_obj_set_style_local_bg_color(obj_tabview, LV_TABVIEW_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_tabview, LV_TABVIEW_PART_BG, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_color(obj_tabview, LV_TABVIEW_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    lv_obj_align(obj_tabview, NULL, LV_ALIGN_CENTER, 0, 20);

    obj_tabCoffee = lv_tabview_add_tab(obj_tabview, "C\nO\nF\nF\nE\nE");
    lv_page_set_scroll_propagation(obj_tabCoffee, false);
    obj_tabCappuccino = lv_tabview_add_tab(obj_tabview, "M\nI\nL\nK");
    lv_page_set_scroll_propagation(obj_tabCappuccino, false);

    obj_coffee_short = lv_obj_create(obj_tabCoffee, NULL);
    configure_button_style(obj_coffee_short);
    lv_obj_align(obj_coffee_short, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);

    img_coffee_short = lv_img_create(obj_coffee_short, NULL);
    configure_image_style(img_coffee_short, data_short_coffee);
    lv_obj_align(img_coffee_short, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_coffee_long = lv_obj_create(obj_tabCoffee, NULL);
    configure_button_style(obj_coffee_long);
    lv_obj_align(obj_coffee_long, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 20);

    img_coffee_long = lv_img_create(obj_coffee_long, NULL);
    configure_image_style(img_coffee_long, data_long_coffee);
    lv_obj_align(img_coffee_long, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_coffee_medium = lv_obj_create(obj_tabCoffee, NULL);
    configure_button_style(obj_coffee_medium);
    lv_obj_align(obj_coffee_medium, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -20);

    img_coffee_medium = lv_img_create(obj_coffee_medium, NULL);
    configure_image_style(img_coffee_medium, data_medium_coffee);
    lv_obj_align(img_coffee_medium, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_coffee_free = lv_obj_create(obj_tabCoffee, NULL);
    configure_button_style(obj_coffee_free);
    lv_obj_align(obj_coffee_free, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -20);

    img_coffee_free = lv_img_create(obj_coffee_free, NULL);
    configure_image_style(img_coffee_free, data_free_coffee);
    lv_obj_align(img_coffee_free, NULL, LV_ALIGN_CENTER, 0, 0);


    obj_cappuccino_short = lv_obj_create(obj_tabCappuccino, NULL);
    configure_button_style(obj_cappuccino_short);
    lv_obj_align(obj_cappuccino_short, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);

    img_cappuccino_short = lv_img_create(obj_cappuccino_short, NULL);
    configure_image_style(img_cappuccino_short, data_short_cappuccino);
    lv_obj_align(img_cappuccino_short, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_cappuccino_double = lv_obj_create(obj_tabCappuccino, NULL);
    configure_button_style(obj_cappuccino_double);
    lv_obj_align(obj_cappuccino_double, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 20);

    img_cappuccino_double = lv_img_create(obj_cappuccino_double, NULL);
    configure_image_style(img_cappuccino_double, data_double_cappuccino);
    lv_obj_align(img_cappuccino_double, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_cappuccino_medium = lv_obj_create(obj_tabCappuccino, NULL);
    configure_button_style(obj_cappuccino_medium);
    lv_obj_align(obj_cappuccino_medium, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -20);

    img_cappuccino_medium = lv_img_create(obj_cappuccino_medium, NULL);
    configure_image_style(img_cappuccino_medium, data_medium_cappuccino);
    lv_obj_align(img_cappuccino_medium, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_milk_hot = lv_obj_create(obj_tabCappuccino, NULL);
    configure_button_style(obj_milk_hot);
    lv_obj_align(obj_milk_hot, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -20);

    img_milk_hot = lv_img_create(obj_milk_hot, NULL);
    configure_image_style(img_milk_hot, data_hot_milk);
    lv_obj_align(img_milk_hot, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_event_cb(obj_coffee_short, btn_coffee_cb);
    lv_obj_set_event_cb(obj_coffee_medium, btn_coffee_cb);
    lv_obj_set_event_cb(obj_coffee_long, btn_coffee_cb);
    lv_obj_set_event_cb(obj_coffee_free, btn_coffee_cb);

    lv_obj_set_event_cb(obj_cappuccino_short, btn_cappuccino_cb);
    lv_obj_set_event_cb(obj_cappuccino_double, btn_cappuccino_cb);
    lv_obj_set_event_cb(obj_cappuccino_medium, btn_cappuccino_cb);
    lv_obj_set_event_cb(obj_milk_hot, btn_cappuccino_cb);

    lv_obj_set_event_cb(obj_tabview, tabview_cb);

    ui_preparations_state = ui_state_show;
}

void ui_preparations_show(void *data)
{
    if(ui_state_dis == ui_preparations_state)
    {
        ui_preparations_init(data);
    }
    else
    {
       if(NULL != obj_tabview)
        {
            lv_obj_set_hidden(obj_tabview, false);
            lv_obj_set_hidden(obj_tabCoffee, false);
            lv_obj_set_hidden(obj_tabCappuccino, false);
        }

        ui_preparations_state = ui_state_show;
    }

    lv_obj_set_style_local_image_recolor(img_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_image_recolor(img_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_tabview_set_tab_act(obj_tabview, lastTabActive, LV_ANIM_OFF);
    ui_status_bar_show(true);
}

void ui_preparations_hide(void *data)
{
    lastTabActive = lv_tabview_get_tab_act(obj_tabview);
    if(ui_state_show == ui_preparations_state)
    {
        if(NULL != obj_tabview)
        {
            lv_obj_set_hidden(obj_tabview, true);
            lv_obj_set_hidden(obj_tabCoffee, true);
            lv_obj_set_hidden(obj_tabCappuccino, true);
        }

        ui_preparations_state = ui_state_hide;
    }
}

void ui_preparations_set_desired(coffee_type_t prep)
{
    preparation.desired_prep = prep;
    preparation.isError = false;
    ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
}   

static void tabview_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        uint16_t id = lv_tabview_get_tab_act(obj_tabview);
        switch(id)
        {
            case 1:
            {
                if(false == isCappuccinoEnable)
                    lv_tabview_set_tab_act(obj_tabview, 0, LV_ANIM_OFF);
                break;
            }
            case 0:
            default:
            {
                //Not handled
                break;
            }
        }
    }
}

static void btn_coffee_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_FOCUSED == event)
    {
        if(obj_coffee_short == obj)
            lv_obj_set_style_local_image_recolor(img_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

        if(obj_coffee_medium == obj)
            lv_obj_set_style_local_image_recolor(img_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

        if(obj_coffee_long == obj)
            lv_obj_set_style_local_image_recolor(img_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

        if(obj_coffee_free == obj)
            lv_obj_set_style_local_image_recolor(img_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    }

    if(LV_EVENT_CLICKED == event)
    {
        if(obj_coffee_short == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE SHORT CLICK");
            preparation.desired_prep = COFFEE_SHORT;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_coffee_medium == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE MEDIUM CLICK");
            preparation.desired_prep = COFFEE_MEDIUM;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_coffee_long == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE LONG CLICK");
            preparation.desired_prep = COFFEE_LONG;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_coffee_free == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE FREE CLICK");
            preparation.desired_prep = COFFEE_FREE;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }
    }
}

static void btn_cappuccino_cb(lv_obj_t *obj, lv_event_t event)
{
    static bool hotWater = false;
    if(true == isCappuccinoEnable)
    {
        if(LV_EVENT_FOCUSED == event)
        {
            if(obj_cappuccino_short == obj)
                lv_obj_set_style_local_image_recolor(img_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

            if(obj_cappuccino_medium == obj)
                lv_obj_set_style_local_image_recolor(img_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

            if(obj_cappuccino_double == obj)
                lv_obj_set_style_local_image_recolor(img_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

            if(obj_milk_hot == obj)
                lv_obj_set_style_local_image_recolor(img_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        }

        if(LV_EVENT_LONG_PRESSED == event)
        {
            if(obj_milk_hot == obj)
            {
                lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_RED);
                lv_obj_set_style_local_image_recolor(img_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
                hotWater = true;
            }
        }

        if(LV_EVENT_CLICKED == event)
        {
            if(obj_cappuccino_short == obj)
            {
                ESP_LOGI(LOG_TAG, "CAPPUCCINO SHORT CLICK");
                preparation.desired_prep = CAPPUCCINO_SHORT;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }

            if(obj_cappuccino_medium == obj)
            {
                ESP_LOGI(LOG_TAG, "CAPPUCCINO MEDIUM CLICK");
                preparation.desired_prep = CAPPUCCINO_MEDIUM;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }

            if(obj_cappuccino_double == obj)
            {
                ESP_LOGI(LOG_TAG, "CAPPUCCINO DOUBLE CLICK");
                preparation.desired_prep = CAPPUCCINO_DOUBLE;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }

            if(obj_milk_hot == obj)
            {
                lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
                if(false == hotWater)
                {
                    ESP_LOGI(LOG_TAG, "MILK HOT CLICK");
                    preparation.desired_prep = HOT_MILK;
                }
                else
                {
                    ESP_LOGI(LOG_TAG, "HOT WATER CLICK");
                    preparation.desired_prep = HOT_WATER;
                }
                hotWater = false;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }
        }
    }
}

void ui_preparations_enable_cappuccino(bool enable)
{
    if(isCappuccinoEnable != enable)
    {
        isCappuccinoEnable = enable;
        
        if(NULL != obj_tabview)
        {
            lv_obj_set_style_local_text_color(obj_tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, isCappuccinoEnable ? LV_COLOR_GRAY : LV_COLOR_BLACK);
            if(false == isCappuccinoEnable)
                lv_tabview_set_tab_act(obj_tabview, 0, LV_ANIM_OFF);
        }
    }
}

void ui_preparations_set_power(bool on)
{
    if(isMachinePowerOn != on)
    {
        isMachinePowerOn = on;
        ui_status_bar_show(isMachinePowerOn);

        if(false == isMachinePowerOn)
            ui_show(&ui_standby_func, UI_SHOW_OVERRIDE);
        else
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
    }
}

void ui_preparations_set_fault(bool fault)
{
    if(isMachineFault != fault)
    {
        isMachineFault = fault;

        if(true == isMachineFault)
            ui_show(&ui_fault_func, UI_SHOW_OVERRIDE);
        else
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
    }
}