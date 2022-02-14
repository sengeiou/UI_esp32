#include "ui_main.h"
#include "lv_qrcode.h"
#include "dbg_task.h"

#include "string_it.h"

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

/* LVGL objects defination */
static lv_obj_t* obj_button = NULL;
static lv_obj_t* img_qr = NULL;

static lv_obj_t* obj_logo = NULL;
static lv_obj_t* img_logo = NULL;

static lv_obj_t* obj_label = NULL;


/* Extern image variable(s) */
extern void* data_logo;

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
   
    obj_logo = lv_obj_create(obj_button, NULL);
    lv_obj_set_size(obj_logo, STANDBY_LOGO_WIDTH, STANDBY_LOGO_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_logo, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_logo, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj_logo, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(obj_logo, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_logo, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    img_logo = lv_img_create(obj_logo, NULL);
    lv_img_set_src(img_logo, data_logo);
    lv_img_set_zoom(img_logo, 256);
    lv_obj_align(img_logo, NULL, LV_ALIGN_CENTER, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    img_qr = lv_qrcode_create(obj_button, QR_CODE_SIZE, LV_COLOR_WHITE, LV_COLOR_BLACK);
    lv_obj_align(img_qr, NULL, LV_ALIGN_CENTER, 0, QR_CODE_Y_OFFSET);
    
    char qr_text[] = "18000USA_LAVAZZATESTUSA000003";
    lv_qrcode_update(img_qr, qr_text, strlen(qr_text));

    obj_label = lv_label_create(obj_button, NULL);
    lv_label_set_recolor(obj_label, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(obj_label, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_label_set_text(obj_label, STANDBY_MESSAGE);
    lv_obj_set_size(obj_label, STANDBY_LABEL_WIDTH, STANDBY_LABEL_HEIGHT);
    lv_obj_align(obj_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
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
    }
}

void ui_standby_hide(void *data)
{
    (void)data;

    if(NULL != obj_button)
    {
        lv_obj_set_hidden(obj_button, true);
    }
}


/* ******************************** Event Handler(s) ******************************** */
static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        special_function(DBG_ON_OFF);
    }
}
