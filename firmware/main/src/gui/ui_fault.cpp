#include "ui_main.h"
#include "lvgl_port.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_FAULT"
#else
    #define LOG_TAG "UI_FAULT"
#endif

/* UI function declaration */
ui_func_desc_t ui_fault_func = {
    .name = "ui_fault",
    .init = ui_fault_init,
    .show = ui_fault_show,
    .hide = ui_fault_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_label = NULL;
static lv_obj_t* obj_image = NULL;

extern void* data_fault;



void ui_fault_init(void *data)
{
    (void)data;

    obj_image = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(obj_image, data_fault);
    lv_img_set_zoom(obj_image, 256);
    lv_obj_align(obj_image, NULL, LV_ALIGN_CENTER, 0, -30);

    obj_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_size(obj_label, 400, 60);
    lv_obj_set_style_local_value_font(obj_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_bold_48);
    lv_obj_align(obj_label, obj_image, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_label_set_text(obj_label, "FAULT");
    lv_label_set_align(obj_label, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_auto_realign(obj_label, true);
}

void ui_fault_show(void *data)
{
    ui_status_bar_show(false);
    if(NULL == obj_label)
    {
        ui_fault_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_image, false);
        lv_obj_set_hidden(obj_label, false);
    }
}

void ui_fault_hide(void *data)
{
    (void)data;
    
    if(NULL != obj_label)
    {
        lv_obj_set_hidden(obj_image, true);
        lv_obj_set_hidden(obj_label, true);
    }
}


/* ******************************** Event Handler(s) ******************************** */
