#include "ui_main.h"
#include "lvgl_port.h"


#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_REC"
#else
    #define LOG_TAG "UI_REC"
#endif

/* UI function declaration */
ui_func_desc_t ui_recognition_func = {
    .name = "ui_recognition",
    .init = ui_recognition_init,
    .show = ui_recognition_show,
    .hide = ui_recognition_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_container = NULL;
static lv_obj_t* obj_slider_flash = NULL;
static lv_obj_t* obj_label_flash = NULL;
static lv_obj_t* obj_take_photo_btn = NULL;
static lv_obj_t* obj_get_photo_btn = NULL;


// /* Static function forward declaration */
static void slider_cb(lv_obj_t* obj, lv_event_t event);
static void btn_cb(lv_obj_t* obj, lv_event_t event);

bool isRecognitionPageActive = false;


void ui_recognition_init(void *data)
{
    (void)data;

    obj_container = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_container, 460, 250);
    lv_obj_align(obj_container, NULL, LV_ALIGN_CENTER, 0, 20);
    lv_page_set_scrl_layout(obj_container, LV_LAYOUT_PRETTY_TOP);
    lv_page_set_scrl_height(obj_container, 250);
    lv_page_set_scrlbar_mode(obj_container, LV_SCRLBAR_MODE_OFF);

    obj_take_photo_btn = lv_obj_create(obj_container, NULL);
    lv_obj_set_size(obj_take_photo_btn, 200, 100);
    lv_obj_set_style_local_bg_color(obj_take_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_take_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_take_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_take_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_take_photo_btn, obj_container, LV_ALIGN_IN_TOP_MID, 90, 100);
    lv_obj_set_auto_realign(obj_take_photo_btn, true);

    obj_get_photo_btn = lv_obj_create(obj_container, NULL);
    lv_obj_set_size(obj_get_photo_btn, 200, 100);
    lv_obj_set_style_local_bg_color(obj_get_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_color(obj_get_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_radius(obj_get_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_border_width(obj_get_photo_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_get_photo_btn, obj_container, LV_ALIGN_IN_TOP_MID, -90, 100);
    lv_obj_set_auto_realign(obj_get_photo_btn, true);

    obj_slider_flash = lv_slider_create(obj_container, NULL);
    lv_obj_set_width(obj_slider_flash, 370);
    lv_obj_set_height(obj_slider_flash, 10);
    lv_obj_align(obj_slider_flash, obj_container, LV_ALIGN_IN_TOP_MID, 0, 250);
    lv_slider_set_range(obj_slider_flash, 80, 150);
    lv_slider_set_value(obj_slider_flash, 0, LV_ANIM_OFF);

    obj_label_flash = lv_label_create(obj_container, NULL);
    lv_label_set_text(obj_label_flash, "Flash [PWM]: 0");
    lv_obj_set_auto_realign(obj_label_flash, true);
    lv_obj_align(obj_label_flash, obj_slider_flash, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_set_style_local_value_str(obj_take_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "TAKE\nPHOTO");
    lv_obj_set_style_local_value_font(obj_take_photo_btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_take_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_take_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_take_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_obj_set_style_local_value_str(obj_get_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, " GET\nPHOTO");
    lv_obj_set_style_local_value_font(obj_get_photo_btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_value_align(obj_get_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
    lv_obj_set_style_local_value_color(obj_get_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_color(obj_get_photo_btn,  LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_obj_set_event_cb(obj_slider_flash, slider_cb);
    lv_obj_set_event_cb(obj_take_photo_btn, btn_cb);
    lv_obj_set_event_cb(obj_get_photo_btn, btn_cb);
}

void ui_recognition_show(void *data)
{
    if(NULL == obj_container)
    {
        ui_recognition_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_container, false);
        lv_obj_set_hidden(obj_slider_flash, false);
        lv_obj_set_hidden(obj_take_photo_btn, false);
        lv_obj_set_hidden(obj_get_photo_btn, false);
    }
    isRecognitionPageActive = true;
}

void ui_recognition_hide(void *data)
{
    (void)data;

    if(NULL != obj_container)
    {
        lv_obj_set_hidden(obj_container, true);
        lv_obj_set_hidden(obj_slider_flash, true);
        lv_obj_set_hidden(obj_take_photo_btn, true);
        lv_obj_set_hidden(obj_get_photo_btn, true);
    }
    isRecognitionPageActive = false;
}

// /* ******************************** Event Handler(s) ******************************** */
static void slider_cb(lv_obj_t* obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED || event == LV_EVENT_RELEASED)
    {
        char buf[64];

        if(obj == obj_slider_flash)
        {
            int flash = lv_slider_get_value(obj_slider_flash);
            sprintf(buf, "Flash [PWM]: %d", flash);
            lv_label_set_text(obj_label_flash, buf);
            if(event == LV_EVENT_RELEASED)
                ESP_LOGI(LOG_TAG, "Flash Value: %d", flash);
        }
    }
}


static void btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == obj_take_photo_btn)
        {
            printf("TAKE PHOTO\n");
        }

         if(obj == obj_get_photo_btn)
        {
            printf("GET PHOTO\n");
        }
    }
}