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
static lv_obj_t* obj_bar_coffee = NULL;
static lv_obj_t* obj_bar_milk = NULL;
static lv_obj_t* btn_stop = NULL;
static lv_obj_t* img_stop_btn = NULL;

static lv_obj_t* msgbox = NULL;
static const char* btns[] = { "OK", "" };

/* Extern image variable(s) */
extern void* data_espresso_corto;
extern void* data_espresso;
extern void* data_espresso_lungo;
extern void* data_macchiato;
extern void* data_cappuccino;
extern void* data_latte_macchiato;
extern void* data_dose_libera;
extern void* data_caffe_americano;
extern void* data_acqua_calda;

static uint8_t progressCoffee = 0;
static uint8_t oldProgressCoffee = 0;
static uint8_t progressMilk = 0;
static uint8_t oldProgressMilk = 0;

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
        esp_timer_stop(dismiss_timer);
        ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);

        lv_obj_set_hidden(obj, true);
    }
}

static void btn_stop_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == btn_stop)
        {
            special_function(funcCode);
        }
    }
}

void ui_erogation_completed(void)
{
    switch(preparation.desired_prep)
    {
        case PREP_ESPRESSO_CORTO:
        case PREP_ESPRESSO:
        case PREP_ESPRESSO_LUNGO:
        case PREP_DOSE_LIBERA:
        case PREP_ACQUA_CALDA:
        {
            lv_bar_set_value(obj_bar_coffee, 100, LV_ANIM_ON);
            lv_obj_set_hidden(msgbox, false);
            lv_obj_set_click(btn_stop, false);

            esp_timer_start_once(dismiss_timer, DISMISS_TIMEOUT_MS*1000);
            break;
        }
        case PREP_MACCHIATO:
        case PREP_CAPPUCCINO:
        case PREP_LATTE_MACCHIATO:
        case PREP_CAFFE_AMERICANO:
        case PREP_NONE:
        default:
        {
            break;
        }
    }
}

void ui_erogation_milk_completed(void)
{
    switch(preparation.desired_prep)
    {
        case PREP_ESPRESSO_CORTO:
        case PREP_ESPRESSO:
        case PREP_ESPRESSO_LUNGO:
        case PREP_DOSE_LIBERA:
        case PREP_ACQUA_CALDA:
        case PREP_NONE:
        default:
        {
            break;
        }
        case PREP_MACCHIATO:
        case PREP_CAPPUCCINO:
        case PREP_LATTE_MACCHIATO:
        case PREP_CAFFE_AMERICANO:
        {
            lv_bar_set_value(obj_bar_milk, 100, LV_ANIM_ON);
            lv_obj_set_hidden(msgbox, false);
            lv_obj_set_click(btn_stop, false);

            esp_timer_start_once(dismiss_timer, DISMISS_TIMEOUT_MS*1000);
            break;
        }
    }
}

void ui_erogation_set_target_dose(uint16_t dose)
{
    target_dose = dose;
}

void ui_erogation_update(uint16_t current_dose)
{
    progressCoffee = 100*current_dose/target_dose;

    if(progressCoffee != oldProgressCoffee)
    {
        lv_bar_set_value(obj_bar_coffee, progressCoffee, LV_ANIM_ON);
        ESP_LOGI(LOG_TAG, "Progress %d", progressCoffee);
    }
    else
    {
        ESP_LOGD(LOG_TAG, "%d -> %d", oldProgressCoffee, progressCoffee);
    }

    oldProgressCoffee = progressCoffee;
}

void ui_milk_erogation_update(uint8_t steaming_percent)
{
    progressMilk = steaming_percent;

    if(progressMilk != oldProgressMilk)
    {
        lv_bar_set_value(obj_bar_milk, progressMilk, LV_ANIM_ON);
        ESP_LOGI(LOG_TAG, "Progress %d", progressMilk);
    }
    else
    {
        ESP_LOGD(LOG_TAG, "%d -> %d", oldProgressMilk, progressMilk);
    }

    oldProgressMilk = progressMilk;
}

static void set_preparation_parameters(void)
{
    switch(preparation.desired_prep)
    {
        case PREP_ESPRESSO_CORTO:
            lv_img_set_src(img_stop_btn, data_espresso_corto);
            lv_label_set_text(obj_label, PREP_LABEL_1);
            funcCode = DBG_SHORT_COFFEE;
            target_dose = 3300;
            get_parameter(110, 2);
            break;
        case PREP_ESPRESSO:
            lv_img_set_src(img_stop_btn, data_espresso);
            lv_label_set_text(obj_label, PREP_LABEL_2);
            funcCode = DBG_MEDIUM_COFFEE;
            target_dose = 4000;
            get_parameter(111, 2);
            break;
        case PREP_ESPRESSO_LUNGO:
            lv_img_set_src(img_stop_btn, data_espresso_lungo);
            lv_label_set_text(obj_label, PREP_LABEL_3);
            funcCode = DBG_LONG_COFFEE;
            target_dose = 5500;
            get_parameter(112, 2);
            break;
        case PREP_MACCHIATO:
            lv_img_set_src(img_stop_btn, data_macchiato);
            lv_label_set_text(obj_label, PREP_LABEL_4);
            funcCode = DBG_SHORT_CAPPUCCINO;
            target_dose = 3300;
            get_parameter(113, 2);
            break;
        case PREP_CAPPUCCINO:
            lv_img_set_src(img_stop_btn, data_cappuccino);
            lv_label_set_text(obj_label, PREP_LABEL_5);
            funcCode = DBG_MEDIUM_CAPPUCCINO;
            target_dose = 3000;
            get_parameter(114, 2);
            break;
        case PREP_LATTE_MACCHIATO:
            lv_img_set_src(img_stop_btn, data_latte_macchiato);
            lv_label_set_text(obj_label, PREP_LABEL_6);
            funcCode = DBG_DOUBLE_CAPPUCCINO;
            target_dose = 3300;
            get_parameter(114, 2);
            break;
        case PREP_DOSE_LIBERA:
            lv_img_set_src(img_stop_btn, data_dose_libera);
            lv_label_set_text(obj_label, PREP_LABEL_7);
            funcCode = DBG_FREE_COFFEE;
            target_dose = 9000;
            get_parameter(114, 2);
            break;
        case PREP_CAFFE_AMERICANO:
            lv_img_set_src(img_stop_btn, data_caffe_americano);
            lv_label_set_text(obj_label, PREP_LABEL_8);
            funcCode = DBG_HOT_MILK;
            target_dose = 7500;
            get_parameter(115, 2);
            break;
        case PREP_ACQUA_CALDA:
            lv_img_set_src(img_stop_btn, data_acqua_calda);
            lv_label_set_text(obj_label, PREP_LABEL_9);
            funcCode = DBG_HOT_WATER;
            target_dose = 7500;
            get_parameter(115, 2);
            break;
        case PREP_NONE:
        default:
            break;
    }

    lv_obj_align(img_stop_btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(obj_label, NULL, LV_ALIGN_IN_TOP_MID, 0, EROG_BUTTON_HEIGHT);

    progressCoffee = 0;
    oldProgressCoffee = 0;
    progressMilk = 0;
    oldProgressMilk = 0;

    lv_bar_set_value(obj_bar_coffee, progressCoffee, LV_ANIM_OFF);
    lv_bar_set_value(obj_bar_milk, progressMilk, LV_ANIM_OFF);

    special_function(funcCode);
}

void ui_erogation_init(void *data)
{
    (void)data;

    lvgl_sem_take();
    
    obj_container = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(obj_container, true);
    lv_obj_set_size(obj_container, EROG_CONT_WIDTH, EROG_CONT_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    btn_stop = lv_obj_create(obj_container, NULL);
    lv_obj_set_size(btn_stop, EROG_BUTTON_WIDTH, EROG_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 40);
    lv_obj_set_style_local_bg_color(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_width(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_align(btn_stop, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_event_cb(btn_stop, btn_stop_cb);

    img_stop_btn = lv_img_create(btn_stop, NULL);
    lv_img_set_zoom(img_stop_btn, 512);
    lv_obj_set_auto_realign(img_stop_btn, true);
    lv_obj_align(img_stop_btn, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_label = lv_label_create(obj_container, NULL);
    lv_label_set_recolor(obj_label, true);
    lv_label_set_align(obj_label, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_style_local_value_font(obj_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);
    lv_obj_set_style_local_bg_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_size(obj_label, EROG_LABEL_WIDTH, EROG_LABEL_HEIGHT);
    lv_obj_set_auto_realign(obj_label, true);
    lv_obj_align(obj_label, NULL, LV_ALIGN_IN_TOP_MID, 0, EROG_BUTTON_HEIGHT);
    lv_obj_set_click(obj_label, false);

    obj_bar_coffee = lv_bar_create(obj_container, NULL);
    lv_obj_set_style_local_bg_color(obj_bar_coffee, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_bar_coffee, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(obj_bar_coffee, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(obj_bar_coffee, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(obj_bar_coffee, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, EROG_BAR_BORDER);
    lv_obj_set_size(obj_bar_coffee, EROG_BAR_WIDTH, EROG_BAR_HEIGHT);
    lv_bar_set_range(obj_bar_coffee, 0, 100);
    lv_bar_set_value(obj_bar_coffee, progressCoffee, LV_ANIM_ON);
    lv_obj_align(obj_bar_coffee, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, EROG_COFF_BAR_Y_OFFSET);
    lv_obj_set_click(obj_bar_coffee, false);

    obj_bar_milk = lv_bar_create(obj_container, NULL);
    lv_obj_set_style_local_bg_color(obj_bar_milk, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_bar_milk, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAROON);
    lv_obj_set_style_local_border_color(obj_bar_milk, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_MAROON);
    lv_obj_set_style_local_bg_color(obj_bar_milk, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_obj_set_style_local_border_width(obj_bar_milk, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, EROG_BAR_BORDER);
    lv_obj_set_size(obj_bar_milk, EROG_BAR_WIDTH, EROG_BAR_HEIGHT);
    lv_bar_set_range(obj_bar_milk, 0, 100);
    lv_bar_set_value(obj_bar_milk, progressMilk, LV_ANIM_ON);
    lv_obj_align(obj_bar_milk, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, EROG_MILK_BAR_Y_OFFSET);
    lv_obj_set_click(obj_bar_milk, false);

    msgbox = lv_msgbox_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &erog_label_font);
    lv_msgbox_set_text(msgbox, EROG_DRINK_READY);
    lv_msgbox_add_btns(msgbox, btns);
    lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, EROG_MSGBOX_X_OFFSET, EROG_MSGBOX_Y_OFFSET);
    lv_obj_set_event_cb(msgbox, erogation_done_cb);

    lv_obj_set_hidden(msgbox, true);

    lvgl_sem_give();

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

    ESP_LOGI(LOG_TAG, "Show");
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

    lv_bar_set_value(obj_bar_coffee, 0, LV_ANIM_OFF);
    lv_bar_set_value(obj_bar_milk, 0, LV_ANIM_OFF);

    ui_warning_bar_show(true);
    ui_menu_bar_show(true);

    ESP_LOGI(LOG_TAG, "Hide");
}
