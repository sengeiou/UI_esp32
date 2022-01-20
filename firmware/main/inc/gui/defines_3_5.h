#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "lvgl.h"
#include "sdkconfig.h"


// extern lv_font_t font_clock_32;
// extern lv_font_t font_clock_108;
extern lv_font_t font_en_18;
// extern lv_font_t font_en_20;
extern lv_font_t font_en_24;
// extern lv_font_t font_en_28;
// extern lv_font_t font_en_32;
// extern lv_font_t font_en_36;
// extern lv_font_t font_en_40;
// extern lv_font_t font_en_48;
// extern lv_font_t font_en_bold_16;
// extern lv_font_t font_en_bold_20;
extern lv_font_t font_en_bold_24;
// extern lv_font_t font_en_bold_28;
// extern lv_font_t font_en_bold_32;
// extern lv_font_t font_en_bold_36;
extern lv_font_t font_en_bold_48;
// extern lv_font_t font_en_bold_60;
// extern lv_font_t font_en_bold_72;
// extern lv_font_t font_en_20_compress;
// extern lv_font_t font_kb_24;
// extern lv_font_t font_symbol_28;
// extern lv_font_t font_en_thin_20;
// extern lv_font_t font_bar_symbol;

inline lv_font_t default_small_font  = lv_font_montserrat_16;
inline lv_font_t default_big_font    = lv_font_montserrat_32;

// Standby Page
#define QR_CODE_SIZE    250

// Descaling Page
inline lv_font_t desc_label_font  = font_en_bold_48;
inline lv_font_t desc_button_font = font_en_24;
#define DESC_LABEL_WIDTH        400
#define DESC_LABEL_HEIGHT       100
#define DESC_LABEL_X_OFFSET     0
#define DESC_LABEL_Y_OFFSET     -80
#define DESC_BAR_WIDTH          400
#define DESC_BAR_HEIGHT         12
#define DESC_BAR_BORDER         2
#define DESC_BAR_X_OFFSET       0
#define DESC_BAR_Y_OFFSET       30
#define DESC_BUTTON_WIDTH       200
#define DESC_BUTTON_HEIGHT      100
#define DESC_BUTTON_RADIUS      10
#define DESC_BUTTON_BORDER      0
#define DESC_BUTTON_X_OFFSET    0
#define DESC_BUTTON_Y_OFFSET    50

// Erogation Page
inline lv_font_t erog_chart_font  = font_en_18;
inline lv_font_t erog_label_font  = font_en_24;
inline lv_font_t erog_button_font = lv_font_montserrat_32;
#define EROG_CHART_WIDTH        460
#define EROG_CHART_HEIGHT       200
#define EROG_CHART_RADIUS       5
#define EROG_CHART_BORDER       0
#define EROG_CHART_X_OFFSET     0
#define EROG_CHART_Y_OFFSET     0
#define EROG_CHART_PAD_LEFT     40
#define EROG_CHART_PAD_BOTTOM   30
#define EROG_CHART_PAD_RIGHT    20
#define EROG_LABEL_WIDTH        400
#define EROG_LABEL_HEIGHT       100
#define EROG_BAR_WIDTH          400
#define EROG_BAR_HEIGHT         20
#define EROG_BAR_BORDER         2
#define EROG_BAR_X_OFFSET       0
#define EROG_BAR_Y_OFFSET       20
#define EROG_BUTTON_WIDTH       58
#define EROG_BUTTON_RADIUS      5
#define EROG_BUTTON_BORDER      0
#define EROG_BUTTON_X_OFFSET    2
#define EROG_BUTTON_Y_OFFSET    2
#define EROG_MSGBOX_X_OFFSET    0
#define EROG_MSGBOX_Y_OFFSET    0

// Cleaning Pages
inline lv_font_t clean_label_font  = font_en_bold_48;
inline lv_font_t clean_button_font = font_en_24;
#define CLEAN_LABEL_WIDTH        400
#define CLEAN_LABEL_HEIGHT       100
#define CLEAN_LABEL_X_OFFSET     0
#define CLEAN_LABEL_Y_OFFSET     -80
#define CLEAN_BAR_WIDTH          400
#define CLEAN_BAR_HEIGHT         12
#define CLEAN_BAR_BORDER         2
#define CLEAN_BAR_X_OFFSET       0
#define CLEAN_BAR_Y_OFFSET       30
#define CLEAN_BUTTON_WIDTH       200
#define CLEAN_BUTTON_HEIGHT      100
#define CLEAN_BUTTON_RADIUS      10
#define CLEAN_BUTTON_BORDER      0
#define CLEAN_BUTTON_X_OFFSET    10
#define CLEAN_BUTTON_Y_OFFSET    40

// General purpose Page
#define GENERAL_WIDTH        480
#define GENERAL_HEIGHT       320
#define GENERAL_X_OFFSET     0
#define GENERAL_Y_OFFSET     25

// Preferences Page
inline lv_font_t pref_button_font = font_en_24;
#define PREF_BUTTON_WIDTH       140
#define PREF_BUTTON_HEIGHT      110
#define PREF_BUTTON_RADIUS      10
#define PREF_BUTTON_BORDER      0
#define PREF_BUTTON_X_CENTER    160
#define PREF_BUTTON_Y_CENTER    65
#define PREF_BUTTON_Y_OFFSET    20

// Preparation Page
#define PREP_TAB_WIDTH          470
#define PREP_TAB_HEIGHT         250
#define PREP_TAB_PAD            10
#define PREP_TAB_X_OFFSET       0
#define PREP_TAB_Y_OFFSET       20
#define PREP_IMAGE_ZOOM         170
#define PREP_BUTTON_WIDTH       180
#define PREP_BUTTON_HEIGHT      100
#define PREP_BUTTON_RADIUS      15
#define PREP_BUTTON_BORDER      2
#define PREP_BUTTON_X_OFFSET    20
#define PREP_BUTTON_Y_OFFSET    20
#define PREP_IMAGES_X_OFFSET    0
#define PREP_IMAGES_Y_OFFSET    0

// Settings Page
#define SETT_CONT_WIDTH         460
#define SETT_CONT_HEIGHT        250
#define SETT_TAB_PAD            10
#define SETT_CONT_X_OFFSET      0
#define SETT_CONT_Y_OFFSET      20
#define SETT_SLIDER_WIDTH       370
#define SETT_SLIDER_HEIGHT      10
#define SETT_SLIDER_X_OFFSET    0
#define SETT_SLIDER_Y0_OFFSET   20
#define SETT_SLIDER_YD_OFFSET   60
#define SETT_LABEL_X_OFFSET     0
#define SETT_LABEL_Y_OFFSET     10

// Statistics Page
#define STAT_CONT_WIDTH         460
#define STAT_CONT_HEIGHT        250
#define STAT_CONT_X_OFFSET      0
#define STAT_CONT_Y_OFFSET      20
#define STAT_COL1_WIDTH         260
#define STAT_COL2_WIDTH         100

//Status bar
inline lv_font_t stsbar_button_font = lv_font_montserrat_32;
#define STSBAR_WIDTH                   480
#define STSBAR_HEIGHT                  60
#define STSBAR_X_OFFSET                0
#define STSBAR_Y_OFFSET                0
#define STSBAR_RADIUS                  0
#define STSBAR_BORDER                  0
#define STSBAR_BUTTON_WIDTH            58
#define STSBAR_BUTTON_SETT_X_OFFSET    -15
#define STSBAR_BUTTON_WIFI_X_OFFSET    15
#if CONFIG_ENABLE_CAPS_RECOGNITION_MODULE == 1
#define STSBAR_BUTTON_DESC_X_OFFSET    -30
#define STSBAR_BUTTON_POD_X_OFFSET     -95
#define STSBAR_BUTTON_WATER_X_OFFSET   30
#else
#define STSBAR_BUTTON_DESC_X_OFFSET    0
#define STSBAR_BUTTON_POD_X_OFFSET     -65
#define STSBAR_BUTTON_WATER_X_OFFSET   65
#endif
#define STSBAR_BUTTON_RECO_X_OFFSET    95
#define STSBAR_BUTTON_Y_OFFSET         0
#define STSBAR_IMG_ZOOM                128
#define STSBAR_IMG_X_OFFSET            0
#define STSBAR_IMG_Y_OFFSET            0
#define STSBAR_BUTTON_RADIUS           5
#define STSBAR_BUTTON_BORDER           0


#endif /* _DEFINES_H_ */