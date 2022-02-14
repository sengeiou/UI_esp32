#include "ui_main.h"
#include <time.h>
#include "dbg_task.h"

#include "string_it.h"

ui_preparation_t    preparation;

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_PREP"
#else
    #define LOG_TAG "UI_PREP"
#endif

/* UI function declaration */
ui_func_desc_t ui_preparations_func = {
    .name = "ui_preparations",
    .init = ui_preparations_init,
    .show = ui_preparations_show,
    .hide = ui_preparations_hide,
};

static ui_state_t ui_preparations_state = ui_state_dis;

/* LVGL objects defination */
static lv_obj_t* obj_container = NULL;

static lv_obj_t* obj_espresso_corto = NULL;
static lv_obj_t* obj_espresso = NULL;
static lv_obj_t* obj_espresso_lungo = NULL;
static lv_obj_t* obj_macchiato = NULL;
static lv_obj_t* obj_cappuccino = NULL;
static lv_obj_t* obj_latte_macchiato = NULL;
static lv_obj_t* obj_dose_libera = NULL;
static lv_obj_t* obj_caffe_americano = NULL;
static lv_obj_t* obj_acqua_calda = NULL;

static lv_obj_t* img_espresso_corto = NULL;
static lv_obj_t* img_espresso = NULL;
static lv_obj_t* img_espresso_lungo = NULL;
static lv_obj_t* img_macchiato = NULL;
static lv_obj_t* img_cappuccino = NULL;
static lv_obj_t* img_latte_macchiato = NULL;
static lv_obj_t* img_dose_libera = NULL;
static lv_obj_t* img_caffe_americano = NULL;
static lv_obj_t* img_acqua_calda = NULL;

static lv_obj_t* label_espresso_corto = NULL;
static lv_obj_t* label_espresso = NULL;
static lv_obj_t* label_espresso_lungo = NULL;
static lv_obj_t* label_macchiato = NULL;
static lv_obj_t* label_cappuccino = NULL;
static lv_obj_t* label_latte_macchiato = NULL;
static lv_obj_t* label_dose_libera = NULL;
static lv_obj_t* label_caffe_americano = NULL;
static lv_obj_t* label_acqua_calda = NULL;

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


static bool popup_open = false;


/* Static function forward declaration */
static void btn_cb(lv_obj_t* obj, lv_event_t event);

static void basic_popup_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        if(0 == lv_msgbox_get_active_btn(obj))
        {
            ESP_LOGI(LOG_TAG, "Popup closed");
            lv_obj_del(obj);
        }
    }
}

void ui_preparations_set_desired(coffee_type_t prep)
{
    preparation.desired_prep = prep;
}   

static bool check_blocking_warnings()
{
    bool isBlockingWarnings = false;
    // isBlockingWarnings |= preparation.warnings.descaling; //Not blocking
    isBlockingWarnings |= preparation.warnings.water_empty;
    isBlockingWarnings |= preparation.warnings.pod_full;
    isBlockingWarnings |= preparation.warnings.generic;         //Not handled now
    isBlockingWarnings |= preparation.warnings.pod_extracted;   //Not handled now

    if(true == isBlockingWarnings)
    {
        if(false == popup_open)
        {
            static const char* btns[] = { "OK", "" };
            lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
            lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &stsbar_msgbox_font);
            lv_msgbox_set_text(msgbox, "Blocking warning detected. Please check warnings!!");
            lv_msgbox_add_btns(msgbox, btns);
            lv_obj_align(msgbox, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
            lv_obj_set_event_cb(msgbox, basic_popup_cb);
            popup_open = true;
        }
        printf("WARNINGS: (%d) | %d | %d |%d\n", preparation.warnings.descaling, preparation.warnings.water_empty, preparation.warnings.pod_full, preparation.warnings.pod_extracted);
    }

    return isBlockingWarnings;
}

static void configure_button_style(lv_obj_t* obj)
{
    lv_btn_set_checkable(obj, false);
    lv_obj_set_size(obj, PREP_BUTTON_WIDTH, PREP_BUTTON_HEIGHT);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, PREP_BUTTON_RADIUS);
    lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, PREP_BUTTON_BORDER);
    lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, 2*PREP_BUTTON_BORDER);
    lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
}

static void configure_image_style(lv_obj_t* obj, const void* src)
{
    lv_img_set_src(obj, src);
    lv_img_set_zoom(obj, 256);
    // lv_obj_set_style_local_image_recolor_opa(obj, LV_IMG_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_CHECKED | LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_HOVERED, LV_OPA_70);
    // lv_obj_set_style_local_image_recolor(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_CHECKED | LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_HOVERED, LV_COLOR_GRAY);
    // lv_obj_set_style_local_image_recolor_opa(obj, LV_IMG_PART_MAIN, LV_STATE_PRESSED, LV_OPA_100);
    // lv_obj_set_style_local_image_recolor(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
}

static void configure_label_style(lv_obj_t* obj, const char* text)
{
    lv_label_set_recolor(obj, true);
    lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(obj, text);
    lv_obj_set_size(obj, PREP_LABEL_WIDTH, PREP_LABEL_HEIGHT);
    lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);  //TODO check
}

void scr_event_cb(lv_obj_t * obj, lv_event_t e)
{
    if(e == LV_EVENT_GESTURE)
    {
        lv_gesture_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

        switch(dir)
        {
            case LV_GESTURE_DIR_TOP:
            {
                printf("Scroll: %d (TOP)\n", dir);
                break;
            }
            case LV_GESTURE_DIR_BOTTOM:
            {
                printf("Scroll: %d (BOTTOM)\n", dir);
                break;
            }
            case LV_GESTURE_DIR_LEFT:
            {
                printf("Scroll: %d (LEFT)\n", dir);
                break;
            }
            case LV_GESTURE_DIR_RIGHT:
            {
                printf("Scroll: %d (RIGHT)\n", dir);
                break;
            }
        }
    }
}

void ui_preparations_init(void *data)
{
    (void)data;

    /* Preparation page */
    obj_container = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_container, PREP_CONT_WIDTH, PREP_CONT_HEIGHT);
    
    // First ROW [ Espresso | Espresso Corto | Espresso Lungo]
    obj_espresso_corto = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_espresso_corto);
    lv_obj_align(obj_espresso_corto, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    img_espresso_corto = lv_img_create(obj_espresso_corto, NULL);
    configure_image_style(img_espresso_corto, data_espresso_corto);
    lv_obj_align(img_espresso_corto, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_espresso_corto = lv_label_create(obj_espresso_corto, NULL);
    configure_label_style(label_espresso_corto, PREP_LABEL_1);

    obj_espresso = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_espresso);
    lv_obj_align(obj_espresso, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    img_espresso = lv_img_create(obj_espresso, NULL);
    configure_image_style(img_espresso, data_espresso);
    lv_obj_align(img_espresso, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_espresso = lv_label_create(obj_espresso, NULL);
    configure_label_style(label_espresso, PREP_LABEL_2);

    obj_espresso_lungo = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_espresso_lungo);
    lv_obj_align(obj_espresso_lungo, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    img_espresso_lungo = lv_img_create(obj_espresso_lungo, NULL);
    configure_image_style(img_espresso_lungo, data_espresso_lungo);
    lv_obj_align(img_espresso_lungo, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_espresso_lungo = lv_label_create(obj_espresso_lungo, NULL);
    configure_label_style(label_espresso_lungo, PREP_LABEL_3);

    // Second ROW [Macchiato | Cappuccino | Latte macchiato]
    obj_macchiato = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_macchiato);
    lv_obj_align(obj_macchiato, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

    img_macchiato = lv_img_create(obj_macchiato, NULL);
    configure_image_style(img_macchiato, data_macchiato);
    lv_obj_align(img_macchiato, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_macchiato = lv_label_create(obj_macchiato, NULL);
    configure_label_style(label_macchiato, PREP_LABEL_4);

    obj_cappuccino = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_cappuccino);
    lv_obj_align(obj_cappuccino, NULL, LV_ALIGN_CENTER, 0, 0);

    img_cappuccino = lv_img_create(obj_cappuccino, NULL);
    configure_image_style(img_cappuccino, data_cappuccino);
    lv_obj_align(img_cappuccino, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_cappuccino = lv_label_create(obj_cappuccino, NULL);
    configure_label_style(label_cappuccino, PREP_LABEL_5);

    obj_latte_macchiato = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_latte_macchiato);
    lv_obj_align(obj_latte_macchiato, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);

    img_latte_macchiato = lv_img_create(obj_latte_macchiato, NULL);
    configure_image_style(img_latte_macchiato, data_latte_macchiato);
    lv_obj_align(img_latte_macchiato, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_latte_macchiato = lv_label_create(obj_latte_macchiato, NULL);
    configure_label_style(label_latte_macchiato, PREP_LABEL_6);

    // Third ROW [Dose libera | Caffè americano | Acqua calda]
    obj_dose_libera = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_dose_libera);
    lv_obj_align(obj_dose_libera, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

    img_dose_libera = lv_img_create(obj_dose_libera, NULL);
    configure_image_style(img_dose_libera, data_dose_libera);
    lv_obj_align(img_dose_libera, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_dose_libera = lv_label_create(obj_dose_libera, NULL);
    configure_label_style(label_dose_libera, PREP_LABEL_7);

    obj_caffe_americano = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_caffe_americano);
    lv_obj_align(obj_caffe_americano, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    img_caffe_americano = lv_img_create(obj_caffe_americano, NULL);
    configure_image_style(img_caffe_americano, data_caffe_americano);
    lv_obj_align(img_caffe_americano, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_caffe_americano = lv_label_create(obj_caffe_americano, NULL);
    configure_label_style(label_caffe_americano, PREP_LABEL_8);

    obj_acqua_calda = lv_btn_create(obj_container, NULL);
    configure_button_style(obj_acqua_calda);
    lv_obj_align(obj_acqua_calda, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

    img_acqua_calda = lv_img_create(obj_acqua_calda, NULL);
    configure_image_style(img_acqua_calda, data_acqua_calda);
    lv_obj_align(img_acqua_calda, NULL, LV_ALIGN_IN_TOP_MID, PREP_IMAGES_X_OFFSET, PREP_IMAGES_Y_OFFSET);

    label_acqua_calda = lv_label_create(obj_acqua_calda, NULL);
    configure_label_style(label_acqua_calda, PREP_LABEL_9);

    lv_obj_set_event_cb(obj_espresso, btn_cb);
    lv_obj_set_event_cb(obj_espresso_corto, btn_cb);
    lv_obj_set_event_cb(obj_espresso_lungo, btn_cb);
    lv_obj_set_event_cb(obj_macchiato, btn_cb);
    lv_obj_set_event_cb(obj_cappuccino, btn_cb);
    lv_obj_set_event_cb(obj_latte_macchiato, btn_cb);
    lv_obj_set_event_cb(obj_dose_libera, btn_cb);
    lv_obj_set_event_cb(obj_caffe_americano, btn_cb);
    lv_obj_set_event_cb(obj_acqua_calda, btn_cb);

    /* Add UI gestures */
    lv_obj_set_event_cb(lv_scr_act(), scr_event_cb);

    ui_preparations_state = ui_state_show;
}

void ui_preparations_show(void *data)
{
    if(ui_state_dis == ui_preparations_state)
    {
        ui_preparations_init(data);
    }
    else
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, false);
        }

        ui_preparations_state = ui_state_show;
    }

    // ui_warning_bar_show(true);
}

void ui_preparations_hide(void *data)
{
    if(ui_state_show == ui_preparations_state)
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, true);
        }

        ui_preparations_state = ui_state_hide;
    }
}

static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj_espresso_corto == obj)
        {
            ESP_LOGI(LOG_TAG, "ESPRESSO CORTO clicked");
            preparation.desired_prep = PREP_ESPRESSO_CORTO;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_espresso == obj)
        {
            ESP_LOGI(LOG_TAG, "ESPRESSO clicked");
            preparation.desired_prep = PREP_ESPRESSO;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_espresso_lungo == obj)
        {
            ESP_LOGI(LOG_TAG, "ESPRESSO LUNGO clicked");
            preparation.desired_prep = PREP_ESPRESSO_LUNGO;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_macchiato == obj)
        {
            ESP_LOGI(LOG_TAG, "MACCHIATO clicked");
            preparation.desired_prep = PREP_MACCHIATO;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_cappuccino == obj)
        {
            ESP_LOGI(LOG_TAG, "CAPPUCCINO clicked");
            preparation.desired_prep = PREP_CAPPUCCINO;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_latte_macchiato == obj)
        {
            ESP_LOGI(LOG_TAG, "LATTE MACCHIATO clicked");
            preparation.desired_prep = PREP_LATTE_MACCHIATO;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_dose_libera == obj)
        {
            ESP_LOGI(LOG_TAG, "DOSE LIBERA clicked");
            preparation.desired_prep = PREP_DOSE_LIBERA;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }


        if(obj_caffe_americano == obj)
        {
            ESP_LOGI(LOG_TAG, "CAFFE AMERICANO clicked");
            preparation.desired_prep = PREP_CAFFE_AMERICANO;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_acqua_calda == obj)
        {
            ESP_LOGI(LOG_TAG, "ACQUA CALDA clicked");
            preparation.desired_prep = PREP_ACQUA_CALDA;
            if(false == check_blocking_warnings())
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }
    }
}

void ui_preparations_enable_milk_preparations(bool enable)
{
    //TODO
}