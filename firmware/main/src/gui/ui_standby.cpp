#include "ui_main.h"
#include "lv_qrcode.h"
#include "dbg_task.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_STANDBY"
#else
    #define LOG_TAG "UI_STANDBY"
#endif

/* UI function declaration */
ui_func_desc_t ui_standby_func = {
    .name = "ui_standby",
    .init = ui_standby_init,
    .show = ui_standby_show,
    .hide = ui_standby_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_button = NULL;
static lv_obj_t* img_qr = NULL;

/* Static function forward declaration */
static void btn_cb(lv_obj_t *obj, lv_event_t event);

void ui_standby_init(void *data)
{
    (void)data;

    obj_button = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_button, BOARD_LCD_WIDTH, BOARD_LCD_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_button, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(obj_button, btn_cb);
   
    img_qr = lv_qrcode_create(lv_scr_act(), QR_CODE_SIZE, LV_COLOR_WHITE, LV_COLOR_BLACK);
    lv_obj_align(img_qr, NULL, LV_ALIGN_CENTER, 0, 0);
}

void ui_standby_show(void *data)
{
    if(NULL == obj_button)
    {
        ui_standby_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_button, false);
        lv_obj_set_hidden(img_qr, false);
    }

    char qr_text[] = "18000USA_LAVAZZATESTUSA000003";
    lv_qrcode_update(img_qr, qr_text, strlen(qr_text));
}

void ui_standby_hide(void *data)
{
    (void)data;

    if(NULL != obj_button)
    {
        lv_obj_set_hidden(obj_button, true);
        lv_obj_set_hidden(img_qr, true);
    }
}


/* ******************************** Event Handler(s) ******************************** */
static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        special_function(DBG_ON_OFF);
        ui_preparations_set_power(true);
    }
}
