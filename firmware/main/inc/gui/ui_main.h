#pragma once

#include "lvgl.h"
#include "esp_log.h"
#include "types.h"
#include "board.h"
#include "lvgl_gui.h"

#if (CONFIG_BOARD_LAVAZZA_4_3 == 1)
#include "defines_4_3.h"
#elif (CONFIG_BOARD_LAVAZZA_3_5 == 1)
#include "defines_3_5.h"
#else
//TODO create your custom define files
#include "defines_4_3.h"
#endif

#define COLOR_THEME LV_COLOR_BLACK
#define COLOR_DEEP  lv_color_make(246, 174, 61)
#define COLOR_TEXT  LV_COLOR_WHITE
#define COLOR_BG    LV_COLOR_BLACK

#define _UI_FUNC_DEF_(ui_name)                  \
    extern ui_func_desc_t ui_##ui_name##_func;  \
    void ui_##ui_name##_init(void *data);       \
    void ui_##ui_name##_show(void *data);       \
    void ui_##ui_name##_hide(void *data); 

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ui_state_dis = 0,       /* UI not initialized yet */
    ui_state_show,          /* UI is active and showing */
    ui_state_hide,          /* UI is hidden */
} ui_state_t;

typedef enum {
    wifi_status_item_off = 0,
    wifi_status_item_configured,
    wifi_status_item_connecting,
    wifi_status_item_connecting_cloud,
    wifi_status_item_connected,
} wifi_status_item_t;

typedef enum {
    UI_SHOW_OVERRIDE = 0,   /* Show new page and replace its position in call queue */
    UI_SHOW_PEDDING,        /* Show UI and add it behind call queue's tail */
    UI_SHOW_BACKPORT,       /* Show UI in previous call queue */
} ui_show_mode_t;

typedef enum {
    ui_preprations_item_coffee_short = 0,
    ui_preprations_item_coffee_medium,
    ui_preprations_item_coffee_long,
    ui_preprations_item_coffee_free,
    ui_preprations_item_cappuccino_short,
    ui_preprations_item_cappuccino_medium,
    ui_preprations_item_cappuccino_double,
    ui_preprations_item_milk_hot,
} ui_preprations_item_t;

typedef struct {
    char *path;             /* File path */
    void **data;            /* Pointer of data to store in RAM */
} ui_data_fetch_t;

typedef struct {
    char *name;             /* UI name */
    void (*init)(void *);   /* init function pointer */
    void (*show)(void *);   /* show function pointer */
    void (*hide)(void *);   /* hide function pointer */
} ui_func_desc_t;


typedef struct {
    bool descaling;
    bool water_empty;
    bool pod_extracted;
    bool pod_full;
    bool generic;
} ui_warning_t;

typedef struct {
    coffee_type_t desired_prep;
    ui_warning_t  warnings;
} ui_preparation_t;

/**
 * @brief Initialize main UI, including resources loading.
 * 
 */
void ui_main(void);

/**
 * @brief Load resource from SD card to RAM.
 * 
 * @param path The path of image.
 * @param dst Address to save image data.
 */
void ui_laod_resource(const char *path, void **dst);


/**
 * @brief Show UI with call queue control.
 * 
 * @param ui UI to show. Can be NULL if mode is UI_SHOW_BACKPORT.
 * @param mode UI show mode, see `ui_show_mode_t`. All function is listed below.
 */
void ui_show(ui_func_desc_t *ui, ui_show_mode_t mode);

/* UI function defination */
_UI_FUNC_DEF_(preparations);
_UI_FUNC_DEF_(erogation);
_UI_FUNC_DEF_(standby);
_UI_FUNC_DEF_(fault);
_UI_FUNC_DEF_(menu);
_UI_FUNC_DEF_(wifi);
_UI_FUNC_DEF_(settings);
_UI_FUNC_DEF_(statistics);
_UI_FUNC_DEF_(auto_cleaning);
_UI_FUNC_DEF_(semiauto_cleaning);


/**
 * @brief Init menu bar.
 * 
 */
void ui_menu_bar_init(void);

/**
 * @brief Init warning bar.
 * 
 */
void ui_warning_bar_init(void);

/**
 * @brief Show or hide menu bar.
 * 
 * @param show True if show.
 */
void ui_menu_bar_show(bool show);

/**
 * @brief Show or hide warning bar.
 * 
 * @param show True if show.
 */
void ui_warning_bar_show(bool show);

/**
 * @brief Get page object used as container.
 * 
 * @return lv_obj_t* object pointer of page. NULL if not initialized.
 */
lv_obj_t *ui_page_get_obj(void);

/**
 * @brief Update the target dose value.
 * 
 */
void ui_erogation_set_target_dose(uint16_t dose);

/**
 * @brief Update current cleaning information of cleaning page. Call it when preparation data is updated.
 * 
 */
void ui_cleaning_auto_update(uint8_t current_step, uint8_t total_step);
void ui_cleaning_semiauto_update(uint8_t current_step, uint8_t total_step);

/**
 * @brief Update current preparation information of preparations page. Call it when preparation data is updated.
 * 
 */
void ui_erogation_update(uint16_t current_dose);
void ui_milk_erogation_update(uint8_t steaming_percent);


/**
 * @brief Complete current preparation information of preparations page. Call it when preparation is done.
 * 
 */
void ui_erogation_completed(void);
void ui_erogation_milk_completed(void);

/**
 * @brief Update milk preparations information.
 * 
 */
void ui_preparations_enable_milk_preparations(bool enable);

/**
 * @brief Set desired preparation.
 * 
 */
void ui_preparations_set_desired(coffee_type_t prep);


/**
 * @brief Update machine warning (descaling, pod full, water empty and generic).
 * 
 */
void ui_warning_bar_set_descaling_warning(bool warning);
void ui_warning_bar_set_pod_warning(bool warning);
void ui_warning_bar_set_water_empty_warning(bool warning);
void ui_warning_bar_set_generic_warning(bool warning);


/**
 * @brief Update machine statistics.
 * 
 */
void ui_statistics_update_data(uint16_t parId, uint32_t value);

/**
 * @brief Update wifi status.
 * 
 */
void ui_wifi_update_status(wifi_status_item_t status);

#if ENABLE_CAPS_RECOGNITION_MODULE == 1
/**
 * @brief Update the recognition status.
 * 
 */
void ui_warning_bar_update_recognition_status(void);
#endif

#ifdef __cplusplus
}
#endif
