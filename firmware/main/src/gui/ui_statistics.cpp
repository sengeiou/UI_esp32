#include "ui_main.h"
#include "dbg_task.h"

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

// /* LVGL objects defination */
static lv_obj_t* obj_fullstats = NULL;
static lv_obj_t* obj_container = NULL;

bool isStatisticsPageActive = false;

static void update_parameters_from_machine(void)
{
    static const uint16_t parNum[] = {300, 301, 302, 303, 304, 305, 306, 307, 308};

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

void ui_statistics_init(void *data)
{
    (void)data;

    obj_container = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_container, 460, 250);
    lv_obj_align(obj_container, NULL, LV_ALIGN_CENTER, 0, 20);
    lv_page_set_scrl_layout(obj_container, LV_LAYOUT_PRETTY_TOP);
    lv_page_set_scrl_height(obj_container, 250);
    lv_page_set_scrlbar_mode(obj_container, LV_SCRLBAR_MODE_AUTO);

    obj_fullstats = lv_table_create(obj_container, NULL);
    // lv_obj_set_size(obj_fullstats, 460, 250);
    lv_obj_set_style_local_bg_color(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_border_width(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_table_set_col_cnt(obj_fullstats, 2);
    lv_table_set_row_cnt(obj_fullstats, 9);

    lv_table_set_col_width(obj_fullstats, 0, 260);
    lv_table_set_col_width(obj_fullstats, 1, 100);

    lv_table_set_cell_align(obj_fullstats, 0, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 1, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 2, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 3, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 4, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 5, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 6, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 7, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 8, 1, LV_LABEL_ALIGN_RIGHT);

    lv_table_set_cell_value(obj_fullstats, 0, 0, "Total Deliveries");
    lv_table_set_cell_value(obj_fullstats, 1, 0, "Short Coffee");
    lv_table_set_cell_value(obj_fullstats, 2, 0, "Medium Coffee");
    lv_table_set_cell_value(obj_fullstats, 3, 0, "Long Coffee");
    lv_table_set_cell_value(obj_fullstats, 4, 0, "Free Coffee");
    lv_table_set_cell_value(obj_fullstats, 5, 0, "Short Cappuccino");
    lv_table_set_cell_value(obj_fullstats, 6, 0, "Medium Cappuccino");
    lv_table_set_cell_value(obj_fullstats, 7, 0, "Double Cappuccino");
    lv_table_set_cell_value(obj_fullstats, 8, 0, "Hot Milk");

    lv_table_set_cell_value(obj_fullstats, 0, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 1, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 2, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 3, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 4, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 5, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 6, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 7, 1, "--");
    lv_table_set_cell_value(obj_fullstats, 8, 1, "--");
}

void ui_statistics_show(void *data)
{
    if(NULL == obj_container)
    {
        ui_statistics_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_container, false);
        lv_obj_set_hidden(obj_fullstats, false);
    }

    update_parameters_from_machine();

    isStatisticsPageActive = true;
}

void ui_statistics_hide(void *data)
{
    (void)data;

    if(NULL != obj_container)
    {
        lv_obj_set_hidden(obj_container, true);
        lv_obj_set_hidden(obj_fullstats, true);
    }

    isStatisticsPageActive = false;
}
