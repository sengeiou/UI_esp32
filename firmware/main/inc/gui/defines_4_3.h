#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "lvgl.h"

extern lv_font_t font_clock_32;
extern lv_font_t font_clock_108;
extern lv_font_t font_en_18;
extern lv_font_t font_en_20;
extern lv_font_t font_en_24;
extern lv_font_t font_en_28;
extern lv_font_t font_en_32;
extern lv_font_t font_en_36;
extern lv_font_t font_en_40;
extern lv_font_t font_en_48;
extern lv_font_t font_en_bold_16;
extern lv_font_t font_en_bold_20;
extern lv_font_t font_en_bold_24;
extern lv_font_t font_en_bold_28;
extern lv_font_t font_en_bold_32;
extern lv_font_t font_en_bold_36;
extern lv_font_t font_en_bold_48;
extern lv_font_t font_en_bold_60;
extern lv_font_t font_en_bold_72;
extern lv_font_t font_en_bold_24;
extern lv_font_t font_en_bold_28;
extern lv_font_t font_en_bold_48;
extern lv_font_t font_en_20_compress;
extern lv_font_t font_kb_24;
extern lv_font_t font_symbol_28;
extern lv_font_t font_en_thin_20;
extern lv_font_t font_bar_symbol;

// Standby Page
#define QR_CODE_SIZE    350

// Descaling Page
inline lv_font_t desc_label_font  = font_en_bold_60;
inline lv_font_t desc_button_font = font_en_28;
#define DESC_LABEL_WIDTH        660
#define DESC_LABEL_HEIGHT       150
#define DESC_LABEL_X_OFFSET     0
#define DESC_LABEL_Y_OFFSET     -120
#define DESC_BAR_WIDTH          660
#define DESC_BAR_HEIGHT         16
#define DESC_BAR_BORDER         3
#define DESC_BAR_X_OFFSET       0
#define DESC_BAR_Y_OFFSET       45
#define DESC_BUTTON_WIDTH       330
#define DESC_BUTTON_HEIGHT      150
#define DESC_BUTTON_RADIUS      15
#define DESC_BUTTON_BORDER      0
#define DESC_BUTTON_X_OFFSET    0
#define DESC_BUTTON_Y_OFFSET    75

// Erogation Page
inline lv_font_t erog_chart_font  = font_en_20;
inline lv_font_t erog_label_font  = font_en_24;
inline lv_font_t erog_button_font = lv_font_montserrat_36;
#define EROG_CHART_WIDTH        760
#define EROG_CHART_HEIGHT       300
#define EROG_CHART_RADIUS       7
#define EROG_CHART_BORDER       0
#define EROG_CHART_X_OFFSET     0
#define EROG_CHART_Y_OFFSET     0
#define EROG_CHART_PAD_LEFT     66
#define EROG_CHART_PAD_BOTTOM   45
#define EROG_CHART_PAD_RIGHT    33
#define EROG_LABEL_WIDTH        660
#define EROG_LABEL_HEIGHT       150
#define EROG_BAR_WIDTH          660
#define EROG_BAR_HEIGHT         30
#define EROG_BAR_BORDER         3
#define EROG_BAR_X_OFFSET       0
#define EROG_BAR_Y_OFFSET       30
#define EROG_BUTTON_WIDTH       97
#define EROG_BUTTON_RADIUS      7
#define EROG_BUTTON_BORDER      0
#define EROG_BUTTON_X_OFFSET    3
#define EROG_BUTTON_Y_OFFSET    3
#define EROG_MSGBOX_X_OFFSET    0
#define EROG_MSGBOX_Y_OFFSET    0

// Cleaning Pages
inline lv_font_t clean_label_font  = font_en_bold_60;
inline lv_font_t clean_button_font = font_en_28;
#define CLEAN_LABEL_WIDTH        660
#define CLEAN_LABEL_HEIGHT       150
#define CLEAN_LABEL_X_OFFSET     0
#define CLEAN_LABEL_Y_OFFSET     -120
#define CLEAN_BAR_WIDTH          660
#define CLEAN_BAR_HEIGHT         16
#define CLEAN_BAR_BORDER         3
#define CLEAN_BAR_X_OFFSET       0
#define CLEAN_BAR_Y_OFFSET       45
#define CLEAN_BUTTON_WIDTH       330
#define CLEAN_BUTTON_HEIGHT      150
#define CLEAN_BUTTON_RADIUS      15
#define CLEAN_BUTTON_BORDER      0
#define CLEAN_BUTTON_X_OFFSET    16
#define CLEAN_BUTTON_Y_OFFSET    60

// General purpose Page
#define GENERAL_WIDTH        800
#define GENERAL_HEIGHT       480
#define GENERAL_X_OFFSET     0
#define GENERAL_Y_OFFSET     38

// Preferences Page
inline lv_font_t pref_button_font = font_en_28;
#define PREF_BUTTON_WIDTH       230
#define PREF_BUTTON_HEIGHT      165
#define PREF_BUTTON_RADIUS      15
#define PREF_BUTTON_BORDER      0
#define PREF_BUTTON_X_CENTER    260
#define PREF_BUTTON_Y_CENTER    98
#define PREF_BUTTON_Y_OFFSET    30

// Preparation Page
#define PREP_TAB_WIDTH          780
#define PREP_TAB_HEIGHT         375
#define PREP_TAB_PAD            15
#define PREP_TAB_X_OFFSET       0
#define PREP_TAB_Y_OFFSET       30
#define PREP_IMAGE_ZOOM         210
#define PREP_BUTTON_WIDTH       300
#define PREP_BUTTON_HEIGHT      150
#define PREP_BUTTON_RADIUS      20
#define PREP_BUTTON_BORDER      3
#define PREP_BUTTON_X_OFFSET    33
#define PREP_BUTTON_Y_OFFSET    30
#define PREP_IMAGES_X_OFFSET    0
#define PREP_IMAGES_Y_OFFSET    0

// Settings Page
#define SETT_CONT_WIDTH         780
#define SETT_CONT_HEIGHT        375
#define SETT_TAB_PAD            15
#define SETT_CONT_X_OFFSET      0
#define SETT_CONT_Y_OFFSET      30
#define SETT_SLIDER_WIDTH       610
#define SETT_SLIDER_HEIGHT      15
#define SETT_SLIDER_X_OFFSET    0
#define SETT_SLIDER_Y0_OFFSET   30
#define SETT_SLIDER_YD_OFFSET   90
#define SETT_LABEL_X_OFFSET     0
#define SETT_LABEL_Y_OFFSET     15

#endif /* _DEFINES_H_ */