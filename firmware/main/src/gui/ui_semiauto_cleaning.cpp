#include "ui_main.h"
#include "dbg_task.h"

#include "string_it.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_SEMIAUTO_CLEAN"
#else
    #define LOG_TAG "UI_SEMIAUTO_CLEAN"
#endif

/* UI function declaration */
ui_func_desc_t ui_semiauto_cleaning_func = {
    .name = "ui_semiauto_cleaning",
    .init = ui_semiauto_cleaning_init,
    .show = ui_semiauto_cleaning_show,
    .hide = ui_semiauto_cleaning_hide,
};


static ui_state_t ui_semiauto_cleaning_state = ui_state_dis;


// /* LVGL objects defination */
static lv_obj_t* obj_container = NULL;
static lv_obj_t* obj_title_label  = NULL;
static lv_obj_t* obj_message_label = NULL;
static lv_obj_t* obj_bar = NULL;
static lv_obj_t* obj_btn_stop = NULL;
static lv_obj_t* obj_btn_start = NULL;
static lv_obj_t* img_icon = NULL;

/* Extern image variable(s) */
extern void* data_cleaning_page;

static uint8_t progress = 0;
static uint8_t oldProgress = 0;

void ui_cleaning_semiauto_update(uint8_t current_step, uint8_t total_step)
{    
    progress = 100*(current_step + 1)/(total_step + 1);
    progress = (progress >= 100) ? 100 : progress;
    ESP_LOGI(LOG_TAG, "UPDATE: %d/%d %d", current_step, total_step, progress);

    if(oldProgress != progress)
    {
        if(current_step == (total_step))    //LAST STEP
        {
            lv_label_set_text(obj_message_label, SEMIAUTOCLEANING_MESSAGE_DONE);
        }
        else if(current_step < total_step)
        {
            char buf[64];
            sprintf(buf, "%s (%d/%d)", SEMIAUTOCLEANING_MESSAGE_PROGRESS, current_step + 1, total_step + 1);
            lv_label_set_text(obj_message_label, buf);
        }
        else
        {
            //Do nothing
        }
        lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);

        oldProgress = progress;
    }
}

static void btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == obj_btn_stop)
        {
            special_function(DBG_ON_OFF);
        }

        if(obj == obj_btn_start)
        {
            special_function(DBG_SEMIAUTO_CLEANING);
        }
    }
}

void ui_semiauto_cleaning_init(void *data)
{
    (void)data;

    lvgl_sem_take();
    
    obj_container = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(obj_container, true);
    lv_obj_set_size(obj_container, CLEAN_CONT_WIDTH, CLEAN_CONT_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    obj_title_label = lv_label_create(obj_container, NULL);
    lv_label_set_recolor(obj_title_label, true);
    lv_label_set_align(obj_title_label, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_style_local_value_font(obj_title_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);
    lv_obj_set_style_local_bg_color(obj_title_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(obj_title_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_size(obj_title_label, CLEAN_TITLE_LABEL_WIDTH, CLEAN_TITLE_LABEL_HEIGHT);
    lv_obj_set_auto_realign(obj_title_label, true);
    lv_obj_align(obj_title_label, NULL, LV_ALIGN_IN_TOP_MID, CLEAN_TITLE_LABEL_X_OFFSET, CLEAN_TITLE_LABEL_Y_OFFSET);
    lv_label_set_text(obj_title_label, SEMIAUTOCLEANING_TITLE);
    lv_obj_set_click(obj_title_label, false);

    obj_btn_stop = lv_obj_create(obj_container, NULL);
    lv_obj_set_size(obj_btn_stop, CLEAN_BUTTON_WIDTH, CLEAN_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, CLEAN_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(obj_btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_width(obj_btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, CLEAN_BUTTON_BORDER);
    lv_obj_set_style_local_text_color(obj_btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_str(obj_btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, SEMIAUTOCLEANING_STOP_BUTTON);
    lv_obj_align(obj_btn_stop, NULL, LV_ALIGN_IN_TOP_MID, -CLEAN_BUTTON_X_OFFSET, CLEAN_BUTTON_Y_OFFSET);
    lv_obj_set_event_cb(obj_btn_stop, btn_cb);

    img_icon = lv_img_create(obj_container, NULL);
    lv_img_set_src(img_icon, data_cleaning_page);
    lv_img_set_zoom(img_icon, 256);
    lv_obj_set_auto_realign(img_icon, true);
    lv_obj_align(img_icon, NULL, LV_ALIGN_IN_TOP_MID, CLEAN_IMAGE_X_OFFSET, CLEAN_IMAGE_Y_OFFSET);

    obj_btn_start = lv_obj_create(obj_container, NULL);
    lv_obj_set_size(obj_btn_start, CLEAN_BUTTON_WIDTH, CLEAN_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(obj_btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, CLEAN_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(obj_btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(obj_btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_width(obj_btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, CLEAN_BUTTON_BORDER);
    lv_obj_set_style_local_text_color(obj_btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_value_str(obj_btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, SEMIAUTOCLEANING_START_BUTTON);
    lv_obj_align(obj_btn_start, NULL, LV_ALIGN_IN_TOP_MID, CLEAN_BUTTON_X_OFFSET, CLEAN_BUTTON_Y_OFFSET);
    lv_obj_set_event_cb(obj_btn_start, btn_cb);

    obj_message_label = lv_label_create(obj_container, NULL);
    lv_label_set_recolor(obj_message_label, true);
    lv_label_set_align(obj_message_label, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_style_local_value_font(obj_message_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_30);
    lv_obj_set_style_local_bg_color(obj_message_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(obj_message_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_size(obj_message_label, CLEAN_MESS_LABEL_WIDTH, CLEAN_MESS_LABEL_HEIGHT);
    lv_obj_set_auto_realign(obj_message_label, true);
    lv_label_set_text(obj_message_label, SEMIAUTOCLEANING_MESSAGE_READY);
    lv_obj_align(obj_message_label, NULL, LV_ALIGN_IN_TOP_MID, CLEAN_MESS_LABEL_X_OFFSET, CLEAN_MESS_LABEL_Y_OFFSET);
    lv_obj_set_click(obj_message_label, false);

    obj_bar = lv_bar_create(obj_container, NULL);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, CLEAN_BAR_BORDER);
    lv_obj_set_size(obj_bar, CLEAN_BAR_WIDTH, CLEAN_BAR_HEIGHT);
    lv_bar_set_range(obj_bar, 0, 100);
    lv_bar_set_value(obj_bar, 0, LV_ANIM_ON);
    lv_obj_align(obj_bar, NULL, LV_ALIGN_IN_TOP_MID, CLEAN_BAR_X_OFFSET, CLEAN_BAR_Y_OFFSET);
    lv_obj_set_click(obj_bar, false);

    lvgl_sem_give();

    ui_semiauto_cleaning_state = ui_state_show;
}

void ui_semiauto_cleaning_show(void *data)
{
    if(ui_state_dis == ui_semiauto_cleaning_state)
    {
        ui_semiauto_cleaning_init(data);
    }
    else
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, false);
        }

        ui_semiauto_cleaning_state = ui_state_show;
    }

    progress = 0;
    oldProgress = 0;
    lv_bar_set_value(obj_bar, progress, LV_ANIM_OFF);
    
    ui_warning_bar_show(false);
    ui_menu_bar_show(false);

    ESP_LOGI(LOG_TAG, "Show");
}

void ui_semiauto_cleaning_hide(void *data)
{
    (void)data;

    if(ui_state_show == ui_semiauto_cleaning_state)
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, true);
        }

        ui_semiauto_cleaning_state = ui_state_hide;
    }

    ui_warning_bar_show(true);
    ui_menu_bar_show(true);

    ESP_LOGI(LOG_TAG, "Hide");
}
