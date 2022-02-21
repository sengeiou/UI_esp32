#include "ui_main.h"
#include "lv_qrcode.h"
#include "dbg_task.h"

#include "string_it.h"

#ifdef ADVANCED_DEBUG
    #define LOG_TAG LINE_STRING "|" "UI_STATS"
#else
    #define LOG_TAG "UI_STATS"
#endif

/* UI function declaration */
ui_func_desc_t ui_statistics_func = {
    .name = "ui_statistics",
    .init = ui_statistics_init,
    .show = ui_statistics_show,
    .hide = ui_statistics_hide,
};

static ui_state_t ui_statistics_state = ui_state_dis;


/* LVGL objects defination */
static lv_obj_t* obj_container = NULL;

static lv_obj_t* obj_button_up      = NULL;
static lv_obj_t* obj_button_down    = NULL;
static lv_obj_t* obj_button_refresh = NULL;
static lv_obj_t* obj_button_back    = NULL;

static lv_obj_t* obj_stat_container = NULL;
static lv_obj_t* obj_fullstats      = NULL;

/* Static function forward declaration */
static void menu_btn_cb(lv_obj_t *obj, lv_event_t event);


static void update_parameters_from_machine(void)
{
    static const uint16_t parNum[] = {300, 301, 302, 303, 304, 305, 306, 307, 308, 309};

    for(uint8_t i = 0; i < sizeof(parNum)/sizeof(uint16_t); i++)
    {
        get_parameter(parNum[i], 4);
        vTaskDelay(2);
    }
}


void ui_statistics_update_data(uint16_t parId, uint32_t value)
{
    if(parId >= 300 && parId < 310)
    {
        char buff[4];
        sprintf(buff, "%d", value);
        lv_table_set_cell_value(obj_fullstats, parId-300, 1, buff);
    }
}

static void configure_main_button(lv_obj_t* button, const char* text)
{
    lv_obj_set_size(button, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_RADIUS);
    lv_obj_set_style_local_border_width(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_BORDER);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(button, LV_OBJ_PART_MAIN, LV_STATE_ALL, &menubar_button_font);
    lv_obj_set_event_cb(button, menu_btn_cb);

    lv_obj_t* label = lv_label_create(button, NULL);
    lv_label_set_recolor(label, true);
    lv_label_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

static void configure_arrow_button(lv_obj_t* button, const char* text)
{
    lv_obj_set_size(button, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
    lv_obj_set_style_local_radius(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_RADIUS);
    lv_obj_set_style_local_border_width(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, MENU_BUTTON_BORDER);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_color(button, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_font(button, LV_OBJ_PART_MAIN, LV_STATE_ALL, &menubar_button_font);
    lv_obj_set_event_cb(button, menu_btn_cb);

    lv_obj_t* label = lv_label_create(button, NULL);
    lv_label_set_recolor(label, true);
    lv_label_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

void ui_statistics_init(void *data)
{
    (void)data;

    lvgl_sem_take();

    obj_container = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_hidden(obj_container, true);
    lv_obj_set_size(obj_container, MENU_CONT_WIDTH, MENU_CONT_HEIGHT);
    lv_obj_set_style_local_bg_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_color(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(obj_container, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_container, NULL, LV_ALIGN_CENTER, 0, 0);

    obj_button_refresh = lv_btn_create(obj_container, NULL);
    configure_main_button(obj_button_refresh, LV_SYMBOL_REFRESH);
    lv_obj_align(obj_button_refresh, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, -40);   

    obj_button_back = lv_btn_create(obj_container, NULL);
    configure_main_button(obj_button_back, LV_SYMBOL_CLOSE);
    lv_obj_align(obj_button_back, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 40);    

    obj_button_up = lv_btn_create(obj_container, NULL);
    configure_arrow_button(obj_button_up, LV_SYMBOL_UP);
    lv_obj_align(obj_button_up, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, -120);   

    obj_button_down = lv_btn_create(obj_container, NULL);
    configure_arrow_button(obj_button_down, LV_SYMBOL_DOWN);
    lv_obj_align(obj_button_down, NULL, LV_ALIGN_IN_LEFT_MID, MENU_BUTTON_X_OFFSET, 120);   

    obj_stat_container = lv_page_create(obj_container, NULL);
    lv_obj_set_size(obj_stat_container, STAT_TAB_WIDTH, STAT_TAB_HEIGHT);
    lv_obj_align(obj_stat_container, NULL, LV_ALIGN_IN_LEFT_MID, STAT_TAB_X_OFFSET, STAT_TAB_Y_OFFSET);
    lv_page_set_scrl_layout(obj_stat_container, LV_LAYOUT_PRETTY_TOP);
    lv_page_set_scrl_height(obj_stat_container, STAT_TAB_HEIGHT);
    lv_page_set_scrlbar_mode(obj_stat_container, LV_SCRLBAR_MODE_AUTO);

    obj_fullstats  = lv_table_create(obj_stat_container, NULL);
    lv_obj_set_style_local_bg_color(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_PRESSED | LV_STATE_FOCUSED, 0);
    lv_obj_set_style_local_border_width(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_PRESSED | LV_STATE_FOCUSED, 0);
    lv_obj_set_style_local_text_font(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_PRESSED | LV_STATE_FOCUSED, &statistics_font);
    lv_obj_align(obj_fullstats, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_table_set_col_cnt(obj_fullstats, 2);
    lv_table_set_row_cnt(obj_fullstats, 10);
    lv_table_set_col_width(obj_fullstats, 0, STAT_COL1_WIDTH);
    lv_table_set_col_width(obj_fullstats, 1, STAT_COL2_WIDTH);

    lv_table_set_cell_align(obj_fullstats, 0, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 1, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 2, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 3, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 4, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 5, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 6, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 7, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 8, 1, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(obj_fullstats, 9, 1, LV_LABEL_ALIGN_LEFT);

    lv_table_set_cell_value(obj_fullstats, 0, 0, "Total Deliveries");
    lv_table_set_cell_value(obj_fullstats, 1, 0, "Espresso corto");
    lv_table_set_cell_value(obj_fullstats, 2, 0, "Espresso");
    lv_table_set_cell_value(obj_fullstats, 3, 0, "Espresso lungo");
    lv_table_set_cell_value(obj_fullstats, 4, 0, "Dose libera");
    lv_table_set_cell_value(obj_fullstats, 5, 0, "Macchiato");
    lv_table_set_cell_value(obj_fullstats, 6, 0, "Cappuccino");
    lv_table_set_cell_value(obj_fullstats, 7, 0, "Latte macchiato");
    lv_table_set_cell_value(obj_fullstats, 8, 0, "Latte caldo");
    lv_table_set_cell_value(obj_fullstats, 9, 0, "Acqua calda");

    lv_table_set_cell_value(obj_fullstats, 0, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 1, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 2, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 3, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 4, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 5, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 6, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 7, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 8, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 9, 1, "--");

    lvgl_sem_give();

    ui_statistics_state = ui_state_show;
}

void ui_statistics_show(void *data)
{
    if(ui_state_dis == ui_statistics_state)
    {
        ui_statistics_init(data);
    }
    else
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, false);
        }

        ui_statistics_state = ui_state_show;
    }

    update_parameters_from_machine();

    ui_warning_bar_show(false);
    ui_menu_bar_show(false);

    ESP_LOGI(LOG_TAG, "Show");
}

void ui_statistics_hide(void *data)
{
    (void)data;

    if(ui_state_show == ui_statistics_state)
    {
        if(NULL != obj_container)
        {
            lv_obj_set_hidden(obj_container, true);
        }

        ui_statistics_state = ui_state_hide;
    }

    ESP_LOGI(LOG_TAG, "Hide");
}


/* ******************************** Event Handler(s) ******************************** */
static void menu_btn_cb(lv_obj_t* obj, lv_event_t event)
{
    if(LV_EVENT_CLICKED == event)
    {
        if(obj == obj_button_refresh)
        {
            ESP_LOGI(LOG_TAG, "Refresh statistics");
            update_parameters_from_machine();
        }
        
        if(obj == obj_button_back)
        {
            ui_show(&ui_menu_func, UI_SHOW_OVERRIDE);
        }

        if(obj == obj_button_up)
        {
            lv_page_scroll_ver(obj_stat_container, 70);
        }

        if(obj == obj_button_down)
        {
            lv_page_scroll_ver(obj_stat_container, -70);
        }
    }
}
