#include "ui_main.h"
#include "variables.h"
#include "dbg_task.h"

#include "string_it.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_MENU_BAR"
#else
    #define LOG_TAG "UI_MENU_BAR"
#endif

/* LVGL objects defination */
static lv_obj_t* obj_menu_bar = NULL;
static lv_obj_t* obj_btn_power = NULL;
static lv_obj_t* obj_btn_menu = NULL;

static lv_obj_t* img_power = NULL;
static lv_obj_t* img_menu = NULL;


/* Extern image variable(s) */
extern void* data_power;
extern void* data_menu;


/* Static function forward declaration */
static void menubar_btn_cb(lv_obj_t *obj, lv_event_t event);

void ui_menu_bar_init(void)
{
    lvgl_sem_take();
    
    obj_menu_bar = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_menu_bar, MENUBAR_WIDTH, MENUBAR_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_menu_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj_menu_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(obj_menu_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_menu_bar, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

    obj_btn_power = lv_obj_create(obj_menu_bar, NULL);
    lv_obj_set_size(obj_btn_power, MENUBAR_BUTTON_WIDTH, MENUBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_power, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, MENUBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_power, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_power, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_power, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, MENUBAR_BUTTON_BORDER);
    lv_obj_align(obj_btn_power, obj_menu_bar, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_event_cb(obj_btn_power, menubar_btn_cb);

    img_power = lv_img_create(obj_btn_power, NULL);
    lv_img_set_src(img_power, data_power);
    lv_img_set_zoom(img_power, 200);
    lv_obj_set_style_local_image_recolor_opa(img_power, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(img_power, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(img_power, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_btn_menu = lv_obj_create(obj_menu_bar, NULL);
    lv_obj_set_size(obj_btn_menu, MENUBAR_BUTTON_WIDTH, MENUBAR_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_menu, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, MENUBAR_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_menu, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_btn_menu, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_btn_menu, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, MENUBAR_BUTTON_BORDER);
    lv_obj_align(obj_btn_menu, obj_menu_bar, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_event_cb(obj_btn_menu, menubar_btn_cb);
    
    img_menu = lv_img_create(obj_btn_menu, NULL);
    lv_img_set_src(img_menu, data_menu);
    lv_img_set_zoom(img_menu, 256);
    lv_obj_set_style_local_image_recolor_opa(img_menu, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(img_menu, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(img_menu, NULL, LV_ALIGN_CENTER, 0, 0);

    lvgl_sem_give();
}

void ui_menu_bar_show(bool show)
{
    if(NULL != obj_menu_bar)
    {
        lv_obj_set_hidden(obj_menu_bar, !show);
        ESP_LOGI(LOG_TAG, "%s", show ? "Show" : "Hide");
    }
}

/* ******************************** Event Handler ******************************** */
static void menubar_btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj_btn_power == obj) 
        {
            ESP_LOGI(LOG_TAG, "Button on/off clicked");
            special_function(DBG_ON_OFF);
            return;
        }

        if(obj_btn_menu == obj) 
        {
            ESP_LOGI(LOG_TAG, "Button menu clicked");
            //TODO implement menu
            return;
        }
    }
}
