#ifdef __cplusplus
extern "C" {
#endif

#include "ui_main.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_PAGE"
#else
    #define LOG_TAG "UI_PAGE"
#endif

ui_func_desc_t ui_page_func = {
    .name = "Page",
    .init = ui_page_init,
    .show = ui_page_show,
    .hide = ui_page_hide,
};

static lv_obj_t *page = NULL;
static lv_obj_t *label_tittle = NULL;

static void btn_back_cb(lv_obj_t *obj, lv_event_t event);

void ui_page_init(void *data)
{
    page = lv_obj_create(lv_scr_act(), NULL);

    /* Set object's attributes */
    lv_obj_set_click(page, false);
    lv_obj_set_size(page, GENERAL_WIDTH, GENERAL_HEIGHT);

    /* Set object's style */
    lv_obj_set_style_local_radius(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_border_width(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_color(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(page, NULL, LV_ALIGN_CENTER, GENERAL_X_OFFSET, GENERAL_Y_OFFSET);

    /* Create a back button */
    lv_obj_t *btn_back = lv_btn_create(page, NULL);
    lv_obj_set_size(btn_back, 30, 30);
    lv_obj_set_event_cb(btn_back, btn_back_cb);
    lv_obj_set_style_local_bg_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_value_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, COLOR_THEME);
    lv_obj_set_style_local_value_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
    lv_obj_set_style_local_outline_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_outline_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(btn_back, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);        
    lv_obj_set_style_local_value_str(btn_back, LV_BTN_PART_MAIN,LV_STATE_DEFAULT, LV_SYMBOL_LEFT);
    lv_obj_set_style_local_value_font(btn_back, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_40);
    lv_obj_align(btn_back, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 5);

    lv_obj_t* bar_div = lv_bar_create(page, NULL);
    lv_obj_set_size(bar_div, 480, 2);
    lv_obj_set_style_local_bg_color(bar_div, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_THEME);
    lv_obj_align(bar_div, NULL, LV_ALIGN_IN_TOP_MID, 0 , 60);

    /* Create a title for page */
    label_tittle = lv_label_create(page, NULL);
    lv_label_set_text(label_tittle, (const char *)data);
    lv_obj_set_style_local_text_font(label_tittle, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_32);
    lv_obj_align(label_tittle, btn_back, LV_ALIGN_OUT_RIGHT_MID, 15, 3);
}

void ui_page_show(void *data)
{
    if (NULL != page) {
        lv_label_set_text(label_tittle, (const char *) data);
        lv_obj_set_hidden(page, false);
    } else {
        ui_page_init(data);
    }
}

void ui_page_hide(void *data)
{
    (void) data;

    if (NULL != page) {
        lv_obj_set_hidden(page, true);
    }
}

lv_obj_t *ui_page_get_obj(void)
{
    return page;
}

static void btn_back_cb(lv_obj_t *obj, lv_event_t event)
{
    if (LV_EVENT_CLICKED == event) {
        ui_show(NULL, UI_SHOW_BACKPORT);
    }
}

#ifdef __cplusplus
} // closing brace for extern "C"
#endif