#include "ui_main.h"
#include "lvgl_port.h"
#include <time.h>


#define RADIUS          62
#define BUTTON_SIZE     62
#define WARN_SIZE       40

#define RAD_X_CENTER    125
#define RAD_Y_CENTER    0
#define ROUNDER         15

ui_preparation_t    preparation;

#define LOG_TAG "UI_PREP"

/* UI function declaration */
ui_func_desc_t ui_preparations_func = {
    .name = "ui_preparations",
    .init = ui_preparations_init,
    .show = ui_preparations_show,
    .hide = ui_preparations_hide,
};

static ui_state_t ui_preparations_state = ui_state_dis;

/* LVGL objects defination */
static lv_obj_t* obj_coffee_short = NULL;
static lv_obj_t* obj_coffee_medium = NULL;
static lv_obj_t* obj_coffee_long = NULL;
static lv_obj_t* obj_coffee_free = NULL;
static lv_obj_t* obj_cappuccino_short = NULL;
static lv_obj_t* obj_cappuccino_medium = NULL;
static lv_obj_t* obj_cappuccino_double = NULL;
static lv_obj_t* obj_milk_hot = NULL;
static lv_obj_t* obj_on_off = NULL;
static lv_obj_t* obj_temp_boost = NULL;
static lv_obj_t* obj_foam_boost = NULL;
static lv_obj_t* obj_water_warn = NULL;
static lv_obj_t* obj_descaling_warn = NULL;
static lv_obj_t* obj_pod_warn = NULL;

static lv_obj_t* img_on_off = NULL;
static lv_obj_t* img_coffee_short = NULL;
static lv_obj_t* img_coffee_medium = NULL;
static lv_obj_t* img_coffee_long = NULL;
static lv_obj_t* img_coffee_free = NULL;
static lv_obj_t* img_cappuccino_short = NULL;
static lv_obj_t* img_cappuccino_medium = NULL;
static lv_obj_t* img_cappuccino_double = NULL;
static lv_obj_t* img_milk_hot = NULL;
static lv_obj_t* img_temp_boost = NULL;
static lv_obj_t* img_foam_boost = NULL;
static lv_obj_t* img_descaling_warn = NULL;
static lv_obj_t* img_pod_warn = NULL;
static lv_obj_t* img_water_warn = NULL;



static bool isCappuccinoEnable = false;
static bool isMachinePowerOn = false;
static bool isWarningDescaling = false;
static bool isWarningPod = false;
static bool isWarningWater = false;
static bool isBoostTemp = false;
static bool isBoostFoam = false;

// /* Extern image variable(s) */
extern void* data_on_off;
extern void* data_short_coffee;
extern void* data_medium_coffee;
extern void* data_long_coffee;
extern void* data_free_coffee;
extern void* data_short_cappuccino;
extern void* data_medium_cappuccino;
extern void* data_double_cappuccino;
extern void* data_hot_milk;
extern void* data_descaling_warning;
extern void* data_water_warning;
extern void* data_pod_warning;
extern void* data_foam_boost;
extern void* data_temp_boost;

/* Static function forward declaration */
static void btn_cb(lv_obj_t* obj, lv_event_t event);
static void btn_warning_cb(lv_obj_t *obj, lv_event_t event);
static void btn_boost_cb(lv_obj_t *obj, lv_event_t event);
static void descaling_cb(lv_obj_t *obj, lv_event_t event);


void ui_preparations_init(void *data)
{
    preparation.desired_prep = NONE;
    preparation.tempBoost = false;
    preparation.foamBoost = false;
    preparation.isError = false;

    (void)data;

    /* Preparation page */
    obj_on_off = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_on_off, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_on_off, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_on_off, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_on_off, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_on_off, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_on_off, NULL, LV_ALIGN_CENTER, 0, RAD_Y_CENTER -120);

    img_on_off = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_on_off, data_on_off);
    lv_img_set_zoom(img_on_off, 128);
    lv_obj_set_style_local_image_recolor_opa(img_on_off, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_on_off, obj_on_off, LV_ALIGN_CENTER, 0, 0);

    obj_descaling_warn = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_descaling_warn, WARN_SIZE, WARN_SIZE);
    lv_obj_set_style_local_bg_color(obj_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_descaling_warn, NULL, LV_ALIGN_CENTER, 0, RAD_Y_CENTER -50 +10);

    img_descaling_warn = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_descaling_warn, data_descaling_warning);
    lv_img_set_zoom(img_descaling_warn, 128);
    lv_obj_set_style_local_image_recolor_opa(img_descaling_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_descaling_warn, obj_descaling_warn, LV_ALIGN_CENTER, 0, 0);

    obj_water_warn = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_water_warn, WARN_SIZE, WARN_SIZE);
    lv_obj_set_style_local_bg_color(obj_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_water_warn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_water_warn, NULL, LV_ALIGN_CENTER, 0, RAD_Y_CENTER +10);

    img_water_warn = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_water_warn, data_water_warning);
    lv_img_set_zoom(img_water_warn, 128);
    lv_obj_set_style_local_image_recolor_opa(img_water_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_water_warn, obj_water_warn, LV_ALIGN_CENTER, 0, 0);

    obj_pod_warn = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_pod_warn, WARN_SIZE, WARN_SIZE);
    lv_obj_set_style_local_bg_color(obj_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_pod_warn, NULL, LV_ALIGN_CENTER, 0, RAD_Y_CENTER +50 +10);

    img_pod_warn = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_pod_warn, data_pod_warning);
    lv_img_set_zoom(img_pod_warn, 128);
    lv_obj_set_style_local_image_recolor_opa(img_pod_warn, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_pod_warn, obj_pod_warn, LV_ALIGN_CENTER, 0, 0);

    obj_temp_boost = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_temp_boost, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_temp_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_temp_boost, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_temp_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_temp_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_temp_boost, NULL, LV_ALIGN_CENTER, 0 -50, RAD_Y_CENTER +120);

    img_temp_boost = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_temp_boost, data_temp_boost);
    lv_img_set_zoom(img_temp_boost, 128);
    lv_obj_set_style_local_image_recolor_opa(img_temp_boost, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_temp_boost, obj_temp_boost, LV_ALIGN_CENTER, 0, 0);

    obj_foam_boost = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_foam_boost, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_foam_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_foam_boost, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_foam_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_foam_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_foam_boost, NULL, LV_ALIGN_CENTER, 0 +50, RAD_Y_CENTER +120);

    img_foam_boost = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_foam_boost, data_foam_boost);
    lv_img_set_zoom(img_foam_boost, 128);
    lv_obj_set_style_local_image_recolor_opa(img_foam_boost, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_foam_boost, obj_foam_boost, LV_ALIGN_CENTER, 0, 0);

    obj_coffee_short = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_coffee_short, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_coffee_short, NULL, LV_ALIGN_CENTER, 0 -RAD_X_CENTER -RADIUS, RAD_Y_CENTER);

    img_coffee_short = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_coffee_short, data_short_coffee);
    lv_img_set_zoom(img_coffee_short, 128);
    lv_obj_set_style_local_image_recolor_opa(img_coffee_short, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_coffee_short, obj_coffee_short, LV_ALIGN_CENTER, 0, 0);

    obj_coffee_long = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_coffee_long, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_coffee_long, NULL, LV_ALIGN_CENTER, 0 -RAD_X_CENTER +RADIUS, RAD_Y_CENTER);

    img_coffee_long = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_coffee_long, data_long_coffee);
    lv_img_set_zoom(img_coffee_long, 128);
    lv_obj_set_style_local_image_recolor_opa(img_coffee_long, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_coffee_long, obj_coffee_long, LV_ALIGN_CENTER, 0, 0);

    obj_coffee_medium = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_coffee_medium, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_coffee_medium, NULL, LV_ALIGN_CENTER, 0 -RAD_X_CENTER, RAD_Y_CENTER -RADIUS);

    img_coffee_medium = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_coffee_medium, data_medium_coffee);
    lv_img_set_zoom(img_coffee_medium, 128);
    lv_obj_set_style_local_image_recolor_opa(img_coffee_medium, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_coffee_medium, obj_coffee_medium, LV_ALIGN_CENTER, 0, 0);

    obj_coffee_free = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_coffee_free, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_coffee_free, NULL, LV_ALIGN_CENTER, 0 -RAD_X_CENTER, RAD_Y_CENTER +RADIUS);

    img_coffee_free = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_coffee_free, data_free_coffee);
    lv_img_set_zoom(img_coffee_free, 128);
    lv_obj_set_style_local_image_recolor_opa(img_coffee_free, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_coffee_free, obj_coffee_free, LV_ALIGN_CENTER, 0, 0);

    obj_cappuccino_short = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_cappuccino_short, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_cappuccino_short, NULL, LV_ALIGN_CENTER, 0 +RAD_X_CENTER -RADIUS, RAD_Y_CENTER);

    img_cappuccino_short = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_cappuccino_short, data_short_cappuccino);
    lv_img_set_zoom(img_cappuccino_short, 128);
    lv_obj_set_style_local_image_recolor_opa(img_cappuccino_short, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_cappuccino_short, obj_cappuccino_short, LV_ALIGN_CENTER, 0, 0);

    obj_cappuccino_double = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_cappuccino_double, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_cappuccino_double, NULL, LV_ALIGN_CENTER, 0 +RAD_X_CENTER +RADIUS, RAD_Y_CENTER);

    img_cappuccino_double = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_cappuccino_double, data_double_cappuccino);
    lv_img_set_zoom(img_cappuccino_double, 128);
    lv_obj_set_style_local_image_recolor_opa(img_cappuccino_double, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_cappuccino_double, obj_cappuccino_double, LV_ALIGN_CENTER, 0, 0);

    obj_cappuccino_medium = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_cappuccino_medium, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_cappuccino_medium, NULL, LV_ALIGN_CENTER, 0 +RAD_X_CENTER, RAD_Y_CENTER -RADIUS);
    
    img_cappuccino_medium = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_cappuccino_medium, data_medium_cappuccino);
    lv_img_set_zoom(img_cappuccino_medium, 128);
    lv_obj_set_style_local_image_recolor_opa(img_cappuccino_medium, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_cappuccino_medium, obj_cappuccino_medium, LV_ALIGN_CENTER, 0, 0);

    obj_milk_hot = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_milk_hot, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_local_bg_color(obj_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ROUNDER);
    lv_obj_set_style_local_border_width(obj_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_milk_hot, NULL, LV_ALIGN_CENTER, 0 +RAD_X_CENTER, RAD_Y_CENTER +RADIUS);

    img_milk_hot = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_milk_hot, data_hot_milk);
    lv_img_set_zoom(img_milk_hot, 128);
    lv_obj_set_style_local_image_recolor_opa(img_milk_hot, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_align(img_milk_hot, obj_milk_hot, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_event_cb(obj_coffee_short, btn_cb);
    lv_obj_set_event_cb(obj_coffee_medium, btn_cb);
    lv_obj_set_event_cb(obj_coffee_long, btn_cb);
    lv_obj_set_event_cb(obj_coffee_free, btn_cb);
    lv_obj_set_event_cb(obj_cappuccino_short, btn_cb);
    lv_obj_set_event_cb(obj_cappuccino_double, btn_cb);
    lv_obj_set_event_cb(obj_cappuccino_medium, btn_cb);
    lv_obj_set_event_cb(obj_milk_hot, btn_cb);
    lv_obj_set_event_cb(obj_on_off, btn_cb);
    lv_obj_set_event_cb(obj_descaling_warn, btn_warning_cb);
    lv_obj_set_event_cb(obj_water_warn, btn_warning_cb);
    lv_obj_set_event_cb(obj_pod_warn, btn_warning_cb);
    lv_obj_set_event_cb(obj_temp_boost, btn_boost_cb);
    lv_obj_set_event_cb(obj_foam_boost, btn_boost_cb);

    ui_status_bar_show(isMachinePowerOn);
    ui_preparations_update();

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
        lv_obj_set_hidden(obj_on_off, false);
        lv_obj_set_hidden(obj_coffee_short, false);
        lv_obj_set_hidden(obj_coffee_medium, false);
        lv_obj_set_hidden(obj_coffee_long, false);
        lv_obj_set_hidden(obj_coffee_free, false);
        lv_obj_set_hidden(obj_cappuccino_short, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_cappuccino_double, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_cappuccino_medium, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_milk_hot, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_descaling_warn, !isWarningDescaling);
        lv_obj_set_hidden(obj_water_warn, !isWarningWater);
        lv_obj_set_hidden(obj_pod_warn, !isWarningPod);
        lv_obj_set_hidden(obj_temp_boost, false);
        lv_obj_set_hidden(obj_foam_boost, !isCappuccinoEnable);

        lv_obj_set_hidden(img_on_off, false);
        lv_obj_set_hidden(img_coffee_short, false);
        lv_obj_set_hidden(img_coffee_medium, false);
        lv_obj_set_hidden(img_coffee_long, false);
        lv_obj_set_hidden(img_coffee_free, false);
        lv_obj_set_hidden(img_cappuccino_short, !isCappuccinoEnable);
        lv_obj_set_hidden(img_cappuccino_double, !isCappuccinoEnable);
        lv_obj_set_hidden(img_cappuccino_medium, !isCappuccinoEnable);
        lv_obj_set_hidden(img_milk_hot, !isCappuccinoEnable);
        lv_obj_set_hidden(img_descaling_warn, !isWarningDescaling);
        lv_obj_set_hidden(img_water_warn, !isWarningWater);
        lv_obj_set_hidden(img_pod_warn, !isWarningPod);
        lv_obj_set_hidden(img_temp_boost, false);
        lv_obj_set_style_local_image_recolor(img_temp_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (isBoostTemp) ? LV_COLOR_WHITE : LV_COLOR_GRAY);
        lv_obj_set_hidden(img_foam_boost, !isCappuccinoEnable);
        lv_obj_set_style_local_image_recolor(img_foam_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (isBoostFoam) ? LV_COLOR_WHITE : LV_COLOR_GRAY);

        lv_obj_set_style_local_image_recolor(img_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);

        lv_obj_set_style_local_image_recolor(img_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        lv_obj_set_style_local_image_recolor(img_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        lv_obj_set_style_local_image_recolor(img_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        
        ui_preparations_state = ui_state_show;
    }
}

void ui_preparations_hide(void *data)
{
    if(ui_state_show == ui_preparations_state)
    {
        lv_obj_set_hidden(obj_coffee_short, true);
        lv_obj_set_hidden(obj_coffee_medium, true);
        lv_obj_set_hidden(obj_coffee_long, true);
        lv_obj_set_hidden(obj_coffee_free, true);
        lv_obj_set_hidden(obj_cappuccino_short, true);
        lv_obj_set_hidden(obj_cappuccino_double, true);
        lv_obj_set_hidden(obj_cappuccino_medium, true);
        lv_obj_set_hidden(obj_milk_hot, true);
        lv_obj_set_hidden(obj_on_off, true);
        lv_obj_set_hidden(obj_descaling_warn, true);
        lv_obj_set_hidden(obj_water_warn, true);
        lv_obj_set_hidden(obj_pod_warn, true);
        lv_obj_set_hidden(obj_temp_boost, true);
        lv_obj_set_hidden(obj_foam_boost, true);

        lv_obj_set_hidden(img_coffee_short, true);
        lv_obj_set_hidden(img_coffee_medium, true);
        lv_obj_set_hidden(img_coffee_long, true);
        lv_obj_set_hidden(img_coffee_free, true);
        lv_obj_set_hidden(img_cappuccino_short, true);
        lv_obj_set_hidden(img_cappuccino_double, true);
        lv_obj_set_hidden(img_cappuccino_medium, true);
        lv_obj_set_hidden(img_milk_hot, true);
        lv_obj_set_hidden(img_on_off, true);
        lv_obj_set_hidden(img_descaling_warn, true);
        lv_obj_set_hidden(img_water_warn, true);
        lv_obj_set_hidden(img_pod_warn, true);
        lv_obj_set_hidden(img_temp_boost, true);
        lv_obj_set_hidden(img_foam_boost, true);

        ui_preparations_state = ui_state_hide;
    }
}

static void descaling_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        if(0 == lv_msgbox_get_active_btn(obj))
        {
            ESP_LOGI(LOG_TAG, "Descaling abort");
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            lv_obj_del(obj);
        }

        if(1 == lv_msgbox_get_active_btn(obj))
        {
            ESP_LOGI(LOG_TAG, "Descaling start");
            ui_show(&ui_descaling_func, UI_SHOW_OVERRIDE);
            lv_obj_del(obj);
        }
    }
}

static void btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj_on_off == obj)
        {
            ESP_LOGI(LOG_TAG, "MachinePowerOn");
            isMachinePowerOn = !isMachinePowerOn;
            ui_preparations_set_power(isMachinePowerOn);
        }

        if(obj_coffee_short == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE SHORT CLICK");
            preparation.desired_prep = COFFEE_SHORT;
            preparation.tempBoost = isBoostTemp;
            preparation.foamBoost = isBoostFoam;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_coffee_medium == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE MEDIUM CLICK");
            preparation.desired_prep = COFFEE_MEDIUM;
            preparation.tempBoost = isBoostTemp;
            preparation.foamBoost = isBoostFoam;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_coffee_long == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE LONG CLICK");
            preparation.desired_prep = COFFEE_LONG;
            preparation.tempBoost = isBoostTemp;
            preparation.foamBoost = isBoostFoam;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(obj_coffee_free == obj)
        {
            ESP_LOGI(LOG_TAG, "COFFEE FREE CLICK");
            preparation.desired_prep = COFFEE_FREE;
            preparation.tempBoost = isBoostTemp;
            preparation.foamBoost = isBoostFoam;
            preparation.isError = false;
            ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
        }

        if(true == isCappuccinoEnable)
        {
            if(obj_cappuccino_short == obj)
            {
                ESP_LOGI(LOG_TAG, "CAPPUCCINO SHORT CLICK");
                preparation.desired_prep = CAPPUCCINO_SHORT;
                preparation.tempBoost = isBoostTemp;
                preparation.foamBoost = isBoostFoam;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }

            if(obj_cappuccino_medium == obj)
            {
                ESP_LOGI(LOG_TAG, "CAPPUCCINO MEDIUM CLICK");
                preparation.desired_prep = CAPPUCCINO_MEDIUM;
                preparation.tempBoost = isBoostTemp;
                preparation.foamBoost = isBoostFoam;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }

            if(obj_cappuccino_double == obj)
            {
                ESP_LOGI(LOG_TAG, "CAPPUCCINO DOUBLE CLICK");
                preparation.desired_prep = CAPPUCCINO_DOUBLE;
                preparation.tempBoost = isBoostTemp;
                preparation.foamBoost = isBoostFoam;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }

            if(obj_milk_hot == obj)
            {
                ESP_LOGI(LOG_TAG, "MILK HOT CLICK");
                preparation.desired_prep = HOT_MILK;
                preparation.tempBoost = isBoostTemp;
                preparation.foamBoost = isBoostFoam;
                preparation.isError = false;
                ui_show(&ui_erogation_func, UI_SHOW_OVERRIDE);
            }
        }
    }
}

static void btn_warning_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj_descaling_warn == obj)
        {
            static const char* btns[] = { "OK", "START", "" };
            lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
            lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
            lv_msgbox_set_text(msgbox, "DESCALING NEEDED");
            lv_msgbox_add_btns(msgbox, btns);
            lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_event_cb(msgbox, descaling_cb);
        }

        if(obj_water_warn == obj)
        {
            static const char* btns[] = { "OK", "" };
            lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
            lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
            lv_msgbox_set_text(msgbox, "WATER EMPTY DETECTED");
            lv_msgbox_add_btns(msgbox, btns);
            lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
        }

        if(obj_pod_warn == obj)
        {
            static const char* btns[] = { "OK", "" };
            lv_obj_t* msgbox = lv_msgbox_create(lv_scr_act(), NULL);
            lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &font_en_20);
            lv_msgbox_set_text(msgbox, "POD CONTAINER FULL");
            lv_msgbox_add_btns(msgbox, btns);
            lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, 0, 0);
        }
    }
}

static void btn_boost_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj_temp_boost == obj)
        {
            isBoostTemp = !isBoostTemp;
            ESP_LOGI(LOG_TAG, "TEMPERATURE BOOST %s", isBoostTemp ? "ACTIVE" : "DISABLED");
            lv_obj_set_style_local_image_recolor(img_temp_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (isBoostTemp) ? LV_COLOR_WHITE : LV_COLOR_GRAY);
        }

        if(obj_foam_boost == obj)
        {
            isBoostFoam = !isBoostFoam;
            ESP_LOGI(LOG_TAG, "FOAM BOOST %s", isBoostFoam ? "ACTIVE" : "DISABLED");
            lv_obj_set_style_local_image_recolor(img_foam_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (isBoostFoam) ? LV_COLOR_WHITE : LV_COLOR_GRAY);
        }
    }
}

void ui_preparations_update(void)
{
    lv_obj_set_hidden(obj_on_off, false);
    lv_obj_set_hidden(img_on_off, false);
    if(true == isMachinePowerOn)
    {
        lv_obj_set_style_local_image_recolor(img_on_off, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_obj_set_hidden(obj_coffee_short, false);
        lv_obj_set_hidden(obj_coffee_medium, false);
        lv_obj_set_hidden(obj_coffee_long, false);
        lv_obj_set_hidden(obj_coffee_free, false);
        lv_obj_set_hidden(obj_cappuccino_short, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_cappuccino_double, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_cappuccino_medium, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_milk_hot, !isCappuccinoEnable);
        lv_obj_set_hidden(obj_descaling_warn, !isWarningDescaling);
        lv_obj_set_hidden(obj_water_warn, !isWarningWater);
        lv_obj_set_hidden(obj_pod_warn, !isWarningPod);
        lv_obj_set_hidden(obj_temp_boost, false);
        lv_obj_set_hidden(obj_foam_boost, !isCappuccinoEnable);

        lv_obj_set_hidden(img_coffee_short, false);
        lv_obj_set_hidden(img_coffee_medium, false);
        lv_obj_set_hidden(img_coffee_long, false);
        lv_obj_set_hidden(img_coffee_free, false);
        lv_obj_set_hidden(img_cappuccino_short, !isCappuccinoEnable);
        lv_obj_set_hidden(img_cappuccino_double, !isCappuccinoEnable);
        lv_obj_set_hidden(img_cappuccino_medium, !isCappuccinoEnable);
        lv_obj_set_hidden(img_milk_hot, !isCappuccinoEnable);
        lv_obj_set_hidden(img_descaling_warn, !isWarningDescaling);
        lv_obj_set_hidden(img_water_warn, !isWarningWater);
        lv_obj_set_hidden(img_pod_warn, !isWarningPod);
        lv_obj_set_hidden(img_temp_boost, false);
        lv_obj_set_style_local_image_recolor(img_temp_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (isBoostTemp) ? LV_COLOR_WHITE : LV_COLOR_GRAY);
        lv_obj_set_hidden(img_foam_boost, !isCappuccinoEnable);
        lv_obj_set_style_local_image_recolor(img_foam_boost, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (isBoostFoam) ? LV_COLOR_WHITE : LV_COLOR_GRAY);

        lv_obj_set_style_local_image_recolor(img_coffee_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_coffee_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_coffee_long, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_coffee_free, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_cappuccino_short, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_cappuccino_double, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_cappuccino_medium, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_image_recolor(img_milk_hot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);

        lv_obj_set_style_local_image_recolor(img_descaling_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        lv_obj_set_style_local_image_recolor(img_pod_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        lv_obj_set_style_local_image_recolor(img_water_warn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
    }
    else
    {
        lv_obj_set_style_local_image_recolor(img_on_off, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_hidden(obj_coffee_short, true);
        lv_obj_set_hidden(obj_coffee_medium, true);
        lv_obj_set_hidden(obj_coffee_long, true);
        lv_obj_set_hidden(obj_coffee_free, true);
        lv_obj_set_hidden(obj_cappuccino_short, true);
        lv_obj_set_hidden(obj_cappuccino_double, true);
        lv_obj_set_hidden(obj_cappuccino_medium, true);
        lv_obj_set_hidden(obj_milk_hot, true);
        lv_obj_set_hidden(obj_descaling_warn, true);
        lv_obj_set_hidden(obj_water_warn, true);
        lv_obj_set_hidden(obj_pod_warn, true);
        lv_obj_set_hidden(obj_temp_boost, true);
        lv_obj_set_hidden(obj_foam_boost, true);

        lv_obj_set_hidden(img_coffee_short, true);
        lv_obj_set_hidden(img_coffee_medium, true);
        lv_obj_set_hidden(img_coffee_long, true);
        lv_obj_set_hidden(img_coffee_free, true);
        lv_obj_set_hidden(img_cappuccino_short, true);
        lv_obj_set_hidden(img_cappuccino_double, true);
        lv_obj_set_hidden(img_cappuccino_medium, true);
        lv_obj_set_hidden(img_milk_hot, true);
        lv_obj_set_hidden(img_descaling_warn, true);
        lv_obj_set_hidden(img_water_warn, true);
        lv_obj_set_hidden(img_pod_warn, true);
        lv_obj_set_hidden(img_temp_boost, true);
        lv_obj_set_hidden(img_foam_boost, true);
    }
}

void ui_preparations_enable_cappuccino(bool enable)
{
    isCappuccinoEnable = enable;
    isBoostFoam = false;
    ui_preparations_update();
}

void ui_preparations_set_power(bool on)
{
    isMachinePowerOn = on;
    isBoostTemp = false;
    isBoostFoam = false;
    ui_status_bar_show(on);

    if(false == isMachinePowerOn)
        ui_show(&ui_standby_func, UI_SHOW_OVERRIDE);
}

void ui_preparations_set_warning(bool descaling, bool pod_full, bool water_empty)
{
    ESP_LOGI(LOG_TAG, "Warning UPDATE");
    bool isToUpdate = false;
    if(isWarningDescaling != descaling)
    {
        isToUpdate |= true;
        isWarningDescaling = descaling;
    }
    if(isWarningPod != pod_full)
    {
        isToUpdate |= true;
        isWarningPod = pod_full;
    }
    if(isWarningWater != water_empty)
    {
        isToUpdate |= true;
        isWarningWater = water_empty;
    }

    if(true == isToUpdate)
    {
        ESP_LOGI(LOG_TAG, "Warning UPDATE");
        lv_obj_set_click(obj_descaling_warn, isWarningDescaling);
        lv_obj_set_click(obj_water_warn, isWarningWater);
        lv_obj_set_click(obj_pod_warn, isWarningPod);
        ui_preparations_update();
    }
}