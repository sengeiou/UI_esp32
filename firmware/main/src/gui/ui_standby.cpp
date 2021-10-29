#include "ui_main.h"
#include "lvgl_port.h"
#include "lv_qrcode.h"
#include "dbg_task.h"

#define LOG_TAG "UI_STANDBY"

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
    lv_obj_set_size(obj_button, 480, 320);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(obj_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_button, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(obj_button, btn_cb);
   
    img_qr = lv_qrcode_create(lv_scr_act(), 250, LV_COLOR_BLACK, LV_COLOR_WHITE);
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

    char qr_text[] = "https://www.lavazza.it/it.html";
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
        ui_preparations_set_power(true);
        ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
    }
}
