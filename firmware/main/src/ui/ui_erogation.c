#include "ui_main.h"
#include "lvgl_port.h"

/* UI function declaration */
ui_func_desc_t ui_erogation_func = {
    .name = "ui_erogation",
    .init = ui_erogation_init,
    .show = ui_erogation_show,
    .hide = ui_erogation_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_graph = NULL;
static lv_obj_t* obj_label = NULL;
static lv_obj_t* obj_bar = NULL;
static lv_chart_series_t* obj_temp_series = NULL;
static lv_chart_series_t* obj_pressure_series = NULL;

static int progress = 0;

static void set_preparation_parameters(void);


extern ui_preparation_t preparation;
static int dose;
bool isErogationPageActive = false;


void simulator_erogation_task(void* data)
{
    (void)data;

    set_preparation_parameters();

    int temperature = 100;
    float pressure = 5.5;
    while(progress <= dose)
    {
        temperature += (rand()%4 -2);
        pressure -= 0.01*(rand()%3);
        lv_bar_set_value(obj_bar, 100*progress/dose, LV_ANIM_ON);
        obj_temp_series->points[progress] = temperature;
        obj_pressure_series->points[progress] = pressure;
        progress++;
        lv_chart_refresh(obj_graph);
        vTaskDelay(10);
    }

    vTaskDelay(300);

    progress = 0;
    lv_chart_clear_serie(obj_graph, obj_temp_series);
    lv_chart_clear_serie(obj_graph, obj_pressure_series);
    lv_chart_set_series_axis(obj_graph, obj_temp_series, LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_series_axis(obj_graph, obj_pressure_series, LV_CHART_AXIS_SECONDARY_Y);

    ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
    vTaskDelete(NULL);
}

static void set_preparation_parameters(void)
{
    switch(preparation.desired_prep)
    {
        case COFFEE_SHORT:
            lv_label_set_text(obj_label, "COFFEE SHORT");
            dose = 115;
            break;
        case COFFEE_MEDIUM:
            lv_label_set_text(obj_label, "COFFEE MEDIUM");
            dose = 150;
            break;
        case COFFEE_LONG:
            lv_label_set_text(obj_label, "COFFEE LONG");
            dose = 190;
            break;
        case COFFEE_FREE:
            lv_label_set_text(obj_label, "COFFEE FREE");
            dose = 300;
            break;
        case CAPPUCCINO_SHORT:
            lv_label_set_text(obj_label, "CAPPUCCINO SHORT");
            dose = 115;
            break;
        case CAPPUCCINO_MEDIUM:
            lv_label_set_text(obj_label, "CAPPUCCINO MEDIUM");
            dose = 200;
            break;
        case CAPPUCCINO_DOUBLE:
            lv_label_set_text(obj_label, "CAPPUCCINO DOUBLE");
            dose = 250;
            break;
        case HOT_MILK:
            lv_label_set_text(obj_label, "HOT MILK");
            dose = 150;
            break;
        case NONE:
        default:
            break;
    }
    lv_obj_align(obj_label, obj_graph, LV_ALIGN_OUT_TOP_MID, 0, -20);
    lv_chart_set_point_count(obj_graph, dose+1);
    lv_chart_set_y_range(obj_graph, LV_CHART_AXIS_PRIMARY_Y, 0,  130);
    lv_chart_set_y_range(obj_graph, LV_CHART_AXIS_SECONDARY_Y, 2,  8);
}

void ui_erogation_init(void *data)
{
    (void)data;

    obj_graph = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_graph, 460, 220);
    lv_obj_set_style_local_bg_color(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_border_width(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_graph, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(obj_graph, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(obj_graph, 500);
    lv_chart_set_update_mode(obj_graph, LV_CHART_UPDATE_MODE_SHIFT);

    /*Add a faded are effect*/
    lv_obj_set_style_local_bg_opa(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/

    obj_temp_series = lv_chart_add_series(obj_graph, LV_COLOR_RED);
    obj_pressure_series = lv_chart_add_series(obj_graph, LV_COLOR_BLUE);

    obj_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_text_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_size(obj_label, 400, 100);
    lv_obj_set_style_local_value_font(obj_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_40);

    obj_bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, 2);
    lv_obj_set_size(obj_bar, 400, 20);
    lv_bar_set_range(obj_bar, 0, 100);
    lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);
    lv_obj_align(obj_bar, obj_graph, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

void ui_erogation_show(void *data)
{
    if(NULL == obj_label)
    {
        ui_erogation_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_label, false);
        lv_obj_set_hidden(obj_graph, false);
        lv_obj_set_hidden(obj_bar, false);
    }

    isErogationPageActive = true;
    xTaskCreate(simulator_erogation_task, "Erogation Simulator Task", 4*1024, NULL, 5, NULL);
}

void ui_erogation_hide(void *data)
{
    (void)data;

    lv_obj_set_hidden(obj_label, true);
    lv_obj_set_hidden(obj_graph, true);
    lv_obj_set_hidden(obj_bar, true);
    isErogationPageActive = false;
}
