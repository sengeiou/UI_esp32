#include "ui_main.h"
#include "dbg_task.h"

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

// /* LVGL objects defination */
static lv_obj_t* obj_graph = NULL;
static lv_obj_t* obj_label = NULL;
static lv_obj_t* obj_bar = NULL;
static lv_obj_t* btn_stop = NULL;
static lv_obj_t* msgbox = NULL;
static const char* btns[] = { "OK", "" };

static lv_chart_series_t* obj_temp_series = NULL;
static lv_chart_series_t* obj_pressure_series = NULL;

static uint8_t progress = 0;
static uint8_t oldProgress = 0;
static bool stop = false;
static void set_preparation_parameters(void);

static dbg_special_func_code_t funcCode;

extern ui_preparation_t preparation;
static int target_dose;
bool isErogationPageActive = false;

static void erogation_done_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_VALUE_CHANGED == event)
    {
        if(0 == lv_msgbox_get_active_btn(obj))
        {
            progress = 0;
            oldProgress = 0;
            stop = true;
            lv_chart_clear_serie(obj_graph, obj_temp_series);
            lv_chart_clear_serie(obj_graph, obj_pressure_series);
            lv_chart_set_series_axis(obj_graph, obj_temp_series, LV_CHART_AXIS_PRIMARY_Y);
            lv_chart_set_series_axis(obj_graph, obj_pressure_series, LV_CHART_AXIS_SECONDARY_Y);
            ui_show(&ui_preparations_func, UI_SHOW_OVERRIDE);
            lv_obj_set_hidden(obj, true);
        }
    }
}

static void btn_stop_cb(lv_obj_t *obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        progress = 0;
        oldProgress = 0;
        stop = true;
        special_function(funcCode);
        lv_obj_set_hidden(msgbox, false);
        lv_obj_set_click(btn_stop, false);
    }
}

void simulator_erogation_task(void* data)
{
    (void)data;

    uint8_t temperature = 100;
    float pressure = 5.5;
    uint16_t current_dose = 0;
    stop = false;
    while(false == stop)
    {
        if(progress < 100)
        {
            if(oldProgress != progress)
            {
                temperature += (rand()%4 -2);
                pressure -= 0.01*(rand()%3);
            }
            current_dose += (rand()%70);
            oldProgress = progress;

            ui_erogation_update(current_dose, temperature, pressure);
            printf("Stop: %d | Dose: %d | Progress: %d \n", stop, current_dose, progress);
        }
        else
        {
            vTaskDelay(100);
            progress = 0;
            oldProgress = 0;
            stop = true;
            lv_obj_set_hidden(msgbox, false);
        }

        vTaskDelay(8);
    }

    vTaskDelete(NULL);
}

void ui_erogation_completed(void)
{
    lv_bar_set_value(obj_bar, 100, LV_ANIM_ON);
    lv_obj_set_hidden(msgbox, false);
    lv_obj_set_click(btn_stop, false);
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
        if(progress < 100)
        {
            lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);

            for(int8_t i = oldProgress + 1; i <= progress; i++)
            {
                obj_temp_series->points[i] = temperature;
                obj_pressure_series->points[i] = pressure;
            }
            lv_chart_refresh(obj_graph);
            ESP_LOGI(LOG_TAG, "Progress %d | temp: %d | pressure: %.2f", progress, temperature, pressure);
        }
        else
        {
            vTaskDelay(10);
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
        case COFFEE_SHORT:
            lv_label_set_text(obj_label, "COFFEE SHORT");
            funcCode = DBG_SHORT_COFFEE;
            target_dose = 4650;
            get_parameter(110, 2);
            break;
        case COFFEE_MEDIUM:
            lv_label_set_text(obj_label, "COFFEE MEDIUM");
            funcCode = DBG_MEDIUM_COFFEE;
            target_dose = 6000;
            get_parameter(111, 2);
            break;
        case COFFEE_LONG:
            lv_label_set_text(obj_label, "COFFEE LONG");
            funcCode = DBG_LONG_COFFEE;
            target_dose = 8500;
            get_parameter(112, 2);
            break;
        case COFFEE_FREE:
            lv_label_set_text(obj_label, "COFFEE FREE");
            funcCode = DBG_FREE_COFFEE;
            target_dose = 15000;
            get_parameter(113, 2);
            break;
        case CAPPUCCINO_SHORT:
            lv_label_set_text(obj_label, "CAPPUCCINO SHORT");
            funcCode = DBG_SHORT_CAPPUCCINO;
            target_dose = 6500;
            get_parameter(114, 2);
            break;
        case CAPPUCCINO_MEDIUM:
            lv_label_set_text(obj_label, "CAPPUCCINO MEDIUM");
            funcCode = DBG_MEDIUM_CAPPUCCINO;
            target_dose = 6500;
            get_parameter(114, 2);
            break;
        case CAPPUCCINO_DOUBLE:
            lv_label_set_text(obj_label, "CAPPUCCINO DOUBLE");
            funcCode = DBG_DOUBLE_CAPPUCCINO;
            target_dose = 6500;
            get_parameter(114, 2);
            break;
        case HOT_MILK:
            lv_label_set_text(obj_label, "HOT MILK");
            funcCode = DBG_HOT_MILK;
            target_dose = 8500;
            get_parameter(115, 2);
            break;
        case HOT_WATER:
            lv_label_set_text(obj_label, "HOT WATER");
            funcCode = DBG_HOT_WATER;
            target_dose = 8500;
            get_parameter(115, 2);
            break;
        case COFFEE_NONE:
        default:
            break;
    }
    special_function(funcCode);
    lv_obj_align(obj_label, obj_graph, LV_ALIGN_OUT_TOP_MID, 0, -20);
    // lv_chart_set_point_count(obj_graph, target_dose+1);
}

void ui_erogation_init(void *data)
{
    (void)data;

    obj_graph = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_graph, EROG_CHART_WIDTH, EROG_CHART_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, EROG_CHART_RADIUS);
    lv_obj_set_style_local_border_width(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, EROG_CHART_BORDER);
    lv_obj_set_style_local_value_font(obj_graph, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &erog_chart_font);
    lv_obj_set_style_local_text_color(obj_graph, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(obj_graph, NULL, LV_ALIGN_CENTER, EROG_CHART_X_OFFSET, EROG_CHART_Y_OFFSET);
    lv_chart_set_type(obj_graph, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(obj_graph, 101);
    lv_chart_set_update_mode(obj_graph, LV_CHART_UPDATE_MODE_SHIFT);
    lv_obj_set_click(obj_graph, false);

    // /*Add a faded are effect*/
    // lv_obj_set_style_local_bg_opa(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    // lv_obj_set_style_local_bg_grad_dir(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    // lv_obj_set_style_local_bg_main_stop(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    // lv_obj_set_style_local_bg_grad_stop(obj_graph, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 100);      /*Transparent on the bottom*/   

    obj_temp_series = lv_chart_add_series(obj_graph, LV_COLOR_RED);
    obj_pressure_series = lv_chart_add_series(obj_graph, LV_COLOR_BLUE);

    lv_chart_set_y_range(obj_graph, LV_CHART_AXIS_PRIMARY_Y, 0, 150);
    lv_chart_set_y_range(obj_graph, LV_CHART_AXIS_SECONDARY_Y, 2, 7);

    const char* x_list_of_values = "0\n20\n40\n60\n80\n100";
    const char* y_list_of_values = "30\n50\n70\n90\n110\n130\n150";

    lv_obj_set_style_local_pad_left(obj_graph,  LV_CHART_PART_BG, LV_STATE_DEFAULT, EROG_CHART_PAD_LEFT);
    lv_obj_set_style_local_pad_bottom(obj_graph,  LV_CHART_PART_BG, LV_STATE_DEFAULT, EROG_CHART_PAD_BOTTOM);
    lv_obj_set_style_local_pad_right(obj_graph,  LV_CHART_PART_BG, LV_STATE_DEFAULT, EROG_CHART_PAD_RIGHT);

    lv_chart_set_div_line_count(obj_graph, 5, 9);
    lv_chart_set_x_tick_length(obj_graph, 0, 0);
    lv_chart_set_y_tick_length(obj_graph, 0, 0);
    lv_chart_set_x_tick_texts(obj_graph, x_list_of_values, 6,  LV_CHART_AXIS_DRAW_LAST_TICK);
    lv_chart_set_y_tick_texts(obj_graph, y_list_of_values, 7,  LV_CHART_AXIS_DRAW_LAST_TICK | LV_CHART_AXIS_INVERSE_LABELS_ORDER);
    lv_chart_refresh(obj_graph);

    obj_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_text_color(obj_label, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_size(obj_label, EROG_LABEL_WIDTH, EROG_LABEL_HEIGHT);
    lv_obj_set_style_local_value_font(obj_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32);
    lv_obj_set_click(obj_label, false);

    obj_bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_border_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_set_style_local_bg_color(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_border_width(obj_bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, EROG_BAR_BORDER);
    lv_obj_set_size(obj_bar, EROG_BAR_WIDTH, EROG_BAR_HEIGHT);
    lv_bar_set_range(obj_bar, 0, 100);
    lv_bar_set_value(obj_bar, progress, LV_ANIM_ON);
    lv_obj_align(obj_bar, obj_graph, LV_ALIGN_OUT_BOTTOM_MID, EROG_BAR_X_OFFSET, EROG_BAR_Y_OFFSET);
    lv_obj_set_click(obj_bar, false);

    btn_stop = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_width(btn_stop, EROG_BUTTON_WIDTH);
    lv_obj_set_style_local_radius(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, EROG_BUTTON_RADIUS);
    lv_obj_set_style_local_bg_color(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, EROG_BUTTON_BORDER);
    lv_obj_set_style_local_value_font(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &erog_button_font);
    lv_obj_set_style_local_value_color(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_value_str(btn_stop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_STOP);
    lv_obj_align(btn_stop, NULL, LV_ALIGN_IN_TOP_RIGHT, EROG_BUTTON_X_OFFSET, EROG_BUTTON_Y_OFFSET);
    lv_obj_set_event_cb(btn_stop, btn_stop_cb);

    msgbox = lv_msgbox_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(msgbox, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, &erog_label_font);
    lv_msgbox_set_text(msgbox, "Your drink\nis ready!");
    lv_msgbox_add_btns(msgbox, btns);
    lv_obj_align(msgbox, NULL, LV_ALIGN_CENTER, EROG_MSGBOX_X_OFFSET, EROG_MSGBOX_Y_OFFSET);
    lv_obj_set_event_cb(msgbox, erogation_done_cb);

    lv_obj_set_hidden(msgbox, true);
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
        lv_obj_set_hidden(btn_stop, false);
        lv_obj_set_hidden(msgbox, true);
    }

    lv_obj_set_click(btn_stop, true);
    lv_chart_clear_serie(obj_graph, obj_temp_series);
    lv_chart_clear_serie(obj_graph, obj_pressure_series);

    ui_status_bar_show(false);
    isErogationPageActive = true;
    funcCode = DBG_NONE;
    set_preparation_parameters();

    //xTaskCreate(simulator_erogation_task, "Erogation Simulator Task", 4*1024, NULL, 5, NULL);
}

void ui_erogation_hide(void *data)
{
    (void)data;

    if(NULL != obj_label)
    {
        lv_obj_set_hidden(obj_label, true);
        lv_obj_set_hidden(obj_graph, true);
        lv_obj_set_hidden(obj_bar, true);
        lv_obj_set_hidden(btn_stop, true);
        lv_obj_set_hidden(msgbox, true);
    }

    lv_bar_set_value(obj_bar, 0, LV_ANIM_OFF);

    ui_status_bar_show(true);
    isErogationPageActive = false;
}
