#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "lvgl.h"
#include "board.h"
#include "sdkconfig.h"

#define LV_STATE_ALL    (LV_STATE_DEFAULT | LV_STATE_PRESSED)


// extern lv_font_t font_clock_32;
// extern lv_font_t font_clock_108;
// extern lv_font_t font_en_18;
extern lv_font_t font_en_20;
extern lv_font_t font_en_24;
extern lv_font_t font_en_28;
// extern lv_font_t font_en_32;
// extern lv_font_t font_en_36;
// extern lv_font_t font_en_40;
// extern lv_font_t font_en_48;
// extern lv_font_t font_en_bold_16;
// extern lv_font_t font_en_bold_20;
// extern lv_font_t font_en_bold_24;
// extern lv_font_t font_en_bold_28;
// extern lv_font_t font_en_bold_32;
// extern lv_font_t font_en_bold_36;
// extern lv_font_t font_en_bold_48;
extern lv_font_t font_en_bold_60;
// extern lv_font_t font_en_bold_72;
// extern lv_font_t font_en_20_compress;
// extern lv_font_t font_kb_24;
// extern lv_font_t font_symbol_28;
// extern lv_font_t font_en_thin_20;
// extern lv_font_t font_bar_symbol;


inline lv_font_t default_small_font  = lv_font_montserrat_24;
inline lv_font_t default_big_font    = lv_font_montserrat_40;


// Standby Page
#define QR_CODE_SIZE            250
#define STANDBY_CONT_WIDTH      800
#define STANDBY_CONT_HEIGHT     480
#define STANDBY_LOGO_WIDTH      500
#define STANDBY_LOGO_HEIGHT     180
#define QR_CODE_Y_OFFSET        (STANDBY_LOGO_HEIGHT + QR_CODE_SIZE/2 - STANDBY_CONT_HEIGHT/2)
#define STANDBY_LABEL_WIDTH     600
#define STANDBY_LABEL_HEIGHT    50

//Warning bar
inline lv_font_t stsbar_button_font = lv_font_montserrat_40;
inline lv_font_t stsbar_msgbox_font = font_en_28;
#define WARNBAR_WIDTH                   70
#define WARNBAR_HEIGHT                  480
#define WARNBAR_BUTTON_WIDTH            60
#define WARNBAR_BUTTON_HEIGHT           60
#define WARNBAR_BUTTON_RADIUS           0
#define WARNBAR_BUTTON_BORDER           0

//Menu bar
#define MENUBAR_WIDTH                   70
#define MENUBAR_HEIGHT                  480
#define MENUBAR_BUTTON_WIDTH            68
#define MENUBAR_BUTTON_HEIGHT           68
#define MENUBAR_BUTTON_OFFSET           75
#define MENUBAR_BUTTON_RADIUS           0
#define MENUBAR_BUTTON_BORDER           0

// Preparation Page
#define PREP_CONT_WIDTH         660
#define PREP_CONT_HEIGHT        480
#define PREP_CONT_X_OFFSET      0
#define PREP_CONT_Y_OFFSET      0
#define PREP_BUTTON_WIDTH       220
#define PREP_BUTTON_HEIGHT      160
#define PREP_BUTTON_RADIUS      0
#define PREP_BUTTON_BORDER      2
#define PREP_BUTTON_Y_OFFSET    -20
#define PREP_LABEL_WIDTH        220
#define PREP_LABEL_HEIGHT       25
#define PREP_LABEL_Y_OFFSET     -50
#define PREP_IMAGES_X_OFFSET    0
#define PREP_IMAGES_Y_OFFSET    0
#define PREP_POPUP_WIDTH        600
#define PREP_POPUP_HEIGHT       300

// Erogation Page
inline lv_font_t erog_label_font  = font_en_24;
#define EROG_CONT_WIDTH         800
#define EROG_CONT_HEIGHT        480
#define EROG_LABEL_WIDTH        600
#define EROG_LABEL_HEIGHT       100
#define EROG_BAR_WIDTH          600
#define EROG_BAR_HEIGHT         30
#define EROG_BAR_BORDER         5
#define EROG_COFF_BAR_Y_OFFSET  -20
#define EROG_MILK_BAR_Y_OFFSET  -80
#define EROG_BUTTON_WIDTH       300
#define EROG_BUTTON_HEIGHT      300
#define EROG_MSGBOX_X_OFFSET    0
#define EROG_MSGBOX_Y_OFFSET    0

// Menu Page
#define MENU_CONT_WIDTH             800
#define MENU_CONT_HEIGHT            480
#define MENU_BUTTON_WIDTH           60
#define MENU_BUTTON_HEIGHT          60
#define MENU_BUTTON_X_OFFSET        20
#define MENU_BUTTON_Y_OFFSET        60
#define MENU_BUTTON_RADIUS          35
#define MENU_BUTTON_BORDER          5
#define MENU_LIST_WIDTH             680
#define MENU_LIST_HEIGHT            440
#define MENU_LIST_X_OFFSET          100

//Wifi Page
#define WIFI_PASS_AREA_WIDTH            600
#define WIFI_PASS_AREA_HEIGHT           50
#define WIFI_SCAN_LIST_WIDTH            600
#define WIFI_SCAN_LIST_HEIGHT           50
#define WIFI_SCAN_BUTTON_WIDTH          200
#define WIFI_SCAN_BUTTON_HEIGHT         50
#define WIFI_SCAN_BUTTON_X_OFFSET       50
#define WIFI_SCAN_BUTTON_Y_OFFSET       30
#define WIFI_SCAN_LIST_Y_OFFSET         20
#define WIFI_PASS_AREA_Y_OFFSET         20


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

// Settings Page
#define SETT_CONT_WIDTH         780
#define SETT_CONT_HEIGHT        375
#define SETT_TAB_PAD            20
#define SETT_CONT_X_OFFSET      5
#define SETT_CONT_Y_OFFSET      30
#define SETT_SLIDER_WIDTH       610
#define SETT_SLIDER_HEIGHT      15
#define SETT_SLIDER_X_OFFSET    0
#define SETT_SLIDER_Y0_OFFSET   30
#define SETT_SLIDER_YD_OFFSET   90
#define SETT_LABEL_X_OFFSET     0
#define SETT_LABEL_Y_OFFSET     15

// Statistics Page
#define STAT_CONT_WIDTH         780
#define STAT_CONT_HEIGHT        375
#define STAT_CONT_X_OFFSET      0
#define STAT_CONT_Y_OFFSET      30
#define STAT_COL1_WIDTH         430
#define STAT_COL2_WIDTH         160

#endif /* _DEFINES_H_ */