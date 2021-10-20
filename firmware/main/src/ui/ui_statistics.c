#include "ui_main.h"
#include "lvgl_port.h"

/* UI function declaration */
ui_func_desc_t ui_statistics_func = {
    .name = "ui_statistics",
    .init = ui_statistics_init,
    .show = ui_statistics_show,
    .hide = ui_statistics_hide,
};

// /* LVGL objects defination */
static lv_obj_t* obj_fullstats = NULL;

bool isStatisticsPageActive = false;


void ui_statistics_init(void *data)
{
    (void)data;

    obj_fullstats = lv_table_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj_fullstats, 460, 250);
    lv_obj_set_style_local_bg_color(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_radius(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_border_width(obj_fullstats, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_align(obj_fullstats, NULL, LV_ALIGN_CENTER, 0, 20);

    lv_table_set_col_cnt(obj_fullstats, 2);
    lv_table_set_row_cnt(obj_fullstats, 4);

    lv_table_set_col_width(obj_fullstats, 0, 200);
    lv_table_set_col_width(obj_fullstats, 1, 50);


    lv_table_set_cell_align(obj_fullstats, 0, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 1, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 2, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_align(obj_fullstats, 3, 1, LV_LABEL_ALIGN_RIGHT);

    lv_table_set_cell_value(obj_fullstats, 0, 0, "Short Coffee");
    lv_table_set_cell_value(obj_fullstats, 1, 0, "Medium Coffee");
    lv_table_set_cell_value(obj_fullstats, 2, 0, "Long Coffee");
    lv_table_set_cell_value(obj_fullstats, 3, 0, "Cappuccino");

    lv_table_set_cell_value(obj_fullstats, 0, 1, "24");
    lv_table_set_cell_value(obj_fullstats, 1, 1, "13");
    lv_table_set_cell_value(obj_fullstats, 2, 1, "2");
    lv_table_set_cell_value(obj_fullstats, 3, 1, "6");
}

void ui_statistics_show(void *data)
{
    if(NULL == obj_fullstats)
    {
        ui_statistics_init(data);
    }
    else
    {
        lv_obj_set_hidden(obj_fullstats, false);
    }

    isStatisticsPageActive = true;
}

void ui_statistics_hide(void *data)
{
    (void)data;

    if(NULL != obj_fullstats)
    {
        lv_obj_set_hidden(obj_fullstats, true);
    }

    isStatisticsPageActive = false;
}
