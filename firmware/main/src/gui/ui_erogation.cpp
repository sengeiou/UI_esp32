#include "ui_main.h"
#include "dbg_task.h"
#include "esp_timer.h"
#include "string_it.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_EROG"
#else
    #define LOG_TAG "UI_EROG"
#endif

/* UI function declaration */
ui_func_desc_t ui_erogation_func = {
    .name = "ui_erogation",
    .init = ui_erogation_init,
    .show = ui_erogation_show,
    .hide = ui_erogation_hide,
};

static ui_state_t ui_erogation_state = ui_state_dis;


// /* LVGL objects defination */
static lv_obj_t* obj_container = NULL;
static lv_obj_t* obj_label = NULL;
static lv_obj_t* obj_bar = NULL;
static lv_obj_t* btn_stop = NULL;


static lv_obj_t* msgbox = NULL;
static const char* btns[] = { "OK", "" };


static uint8_t progress = 0;
static uint8_t oldProgress = 0;
static void set_preparation_parameters(void);

static dbg_special_func_code_t funcCode;

extern ui_preparation_t preparation;
static int target_dose;

esp_timer_handle_t dismiss_timer;

#define DISMISS_TIMEOUT_MS  3000

static void erogation_done_cb(lv_obj_t *obj, lv_event_t event);

static void dismiss_timer_cb(void* arg)
{
    erogation_done_cb(msgbox, LV_EVENT_VALUE_CHANGED);
}

static void erogation_done_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        // if(0 == lv_msgbox_get_active_btn(obj))
        // {
            esp_timer_stop(dismiss_timer);
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            lv_obj_set_hidden(obj, true);
        // }
    }
}

static void btn_stop_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        special_function(funcCode);
    }
}

void ui_erogation_completed(void)
{
    lv_bar_set_value(obj_bar, 100, LV_ANIM_ON);
    lv_obj_set_hidden(msgbox, false);
    lv_obj_set_click(btn_stop, false);

    esp_timer_start_once(dismiss_timer, DISMISS_TIMEOUT_MS*1000);
}

void ui_erogation_set_target_dose(uint16_t dose)
{
    target_dose = dose;
}

void ui_erogation_update(uint16_t current_dose, uint8_t temperature, float pressure)
{
    progress = 100*current_dose/target_dose;

    if(progress != oldProgress)
    {
        lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);
        ESP_LOGI(LOG_TAG, "Progress %d | temp: %d | pressure: %.2f", progress, temperature, pressure);

        if(progress >= 100)
        {
            ui_erogation_completed();
        }
    }
    else
    {
        ESP_LOGD(LOG_TAG, "%d -> %d", oldProgress, progress);
    }

    oldProgress = progress;
}

static void set_preparation_parameters(void)
{
    switch(preparation.desired_prep)
    {
        case PREP_ESPRESSO_CORTO:
            lv_label_set_text(obj_label, PREP_LABEL_1);
            funcCode = DBG_SHORT_COFFEE;
            target_dose = 4650;
            get_parameter(110, 2);
            break;
        case PREP_ESPRESSO:
            lv_label_set_text(obj_label, PREP_LABEL_2);
            funcCode = DBG_MEDIUM_COFFEE;
            target_dose = 6000;
            get_parameter(111, 2);
            break;
        case PREP_ESPRESSO_LUNGO:
            lv_label_set_text(obj_label, PREP_LABEL_3);
            funcCode = DBG_LONG_COFFEE;
            target_dose = 8500;
            get_parameter(112, 2);
            break;
        case PREP_MACCHIATO:
            lv_label_set_text(obj_label, PREP_LABEL_4);
            funcCode = DBG_SHORT_CAPPUCCINO;
            target_dose = 15000;
            get_parameter(113, 2);
            break;
        case PREP_CAPPUCCINO:
            lv_label_set_text(obj_label, PREP_LABEL_5);
            funcCode = DBG_MEDIUM_CAPPUCCINO;
            target_dose = 6500;
            get_parameter(114, 2);
            break;
        case PREP_LATTE_MACCHIATO:
            lv_label_set_text(obj_label, PREP_LABEL_6);
            funcCode = DBG_DOUBLE_CAPPUCCINO;
            target_dose = 6500;
            get_parameter(114, 2);
            break;
        case PREP_DOSE_LIBERA:
            lv_label_set_text(obj_label, PREP_LABEL_7);
            funcCode = DBG_FREE_COFFEE;
            target_dose = 6500;
            get_parameter(114, 2);
            break;
        case PREP_CAFFE_AMERICANO:
            lv_label_set_text(obj_label, PREP_LABEL_8);
            funcCode = DBG_HOT_MILK;
            target_dose = 8500;
            get_parameter(115, 2);
            break;
        case PREP_ACQUA_CALDA:
            lv_label_set_text(obj_label, PREP_LABEL_9);
            funcCode = DBG_HOT_WATER;
            target_dose = 8500;
            get_parameter(115, 2);
            break;
        case PREP_NONE:
        default:
            break;
    }

    progress = 0;
    oldProgress = 0;

    special_function(funcCode);
}

void ui_erogation_init(void *data)
{
    (void)data;

    obj_container = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_container, EROG_CONT_WIDTH, EROG_CONT_HEIGHT);

    btn_stop = lv_obj_create(obj_container, NULL);
    lv_obj_set_size(btn_stop, EROG_BUTTON_WIDTH, EROG_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_color(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAROON);
    lv_obj_set_style_local_border_color(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(btn_stop, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_event_cb(btn_stop, btn_stop_cb);

    //img_stop_btn = lv_obj_create(btn_stop, NULL);
    //TODO

    obj_label = lv_label_create(obj_container, NULL);
    lv_obj_set_style_local_bg_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_size(obj_label, EROG_LABEL_WIDTH, EROG_LABEL_HEIGHT);
    lv_obj_set_style_local_value_font(obj_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32);
    lv_obj_align(obj_label, NULL, LV_ALIGN_IN_TOP_MID, 0, EROG_BUTTON_HEIGHT);
    lv_obj_set_click(obj_label, false);

    obj_bar = lv_bar_create(obj_container, NULL);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, EROG_BAR_BORDER);
    lv_obj_set_size(obj_bar, EROG_BAR_WIDTH, EROG_BAR_HEIGHT);
    lv_bar_set_range(obj_bar, 0, 100);
    lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);
    lv_obj_align(obj_bar, NULL, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_click(obj_bar, false);

    msgbox = lv_msgbox_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &erog_label_font);
    lv_msgbox_set_text(msgbox, EROG_DRINK_READY);
    lv_msgbox_add_btns(msgbox, btns);
    lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, EROG_MSGBOX_X_OFFSET, EROG_MSGBOX_Y_OFFSET);
    lv_obj_set_event_cb(msgbox, erogation_done_cb);

    lv_obj_set_hidden(msgbox, true);

    const esp_timer_create_args_t dismiss_timer_args = {
            .callback = &dismiss_timer_cb,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) dismiss_timer,
            .name = "dismiss_timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&dismiss_timer_args, &dismiss_timer));

    ui_erogation_state = ui_state_show;
}

void ui_erogation_show(void *data)
{
    if(ui_state_dis == ui_erogation_state)
    {
        ui_erogation_init(data);
    }
    else
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, false);
        }

        ui_erogation_state = ui_state_show;
    }

    esp_timer_stop(dismiss_timer);

    lv_obj_set_click(btn_stop, true);

    ui_warning_bar_show(false);
    ui_menu_bar_show(false);
    funcCode = DBG_NONE;
    set_preparation_parameters();
}

void ui_erogation_hide(void *data)
{
    (void)data;

    if(ui_state_show == ui_erogation_state)
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, true);
        }

        ui_erogation_state = ui_state_hide;
    }

    esp_timer_stop(dismiss_timer);
    lv_bar_set_value(obj_bar, 0, LV_ANIM_OFF);
    ui_warning_bar_show(true);
    ui_menu_bar_show(true);
}
