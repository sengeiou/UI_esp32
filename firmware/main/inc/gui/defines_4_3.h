#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "lvgl.h"
#include "sdkconfig.h"

#define LV_STATE_ALL    (LV_STATE_DEFAULT | LV_STATE_PRESSED)

inline lv_font_t default_tiny_font   = lv_font_montserrat_16;
inline lv_font_t default_small_font  = lv_font_montserrat_26;
inline lv_font_t default_medium_font = lv_font_montserrat_34;
inline lv_font_t default_big_font    = lv_font_montserrat_48;


inline lv_font_t fault_message_font   = lv_font_montserrat_26;

// Standby Page
inline lv_font_t standby_message_font   = lv_font_montserrat_26;
#define QR_CODE_SIZE            250
#define STANDBY_CONT_WIDTH      800
#define STANDBY_CONT_HEIGHT     480
#define STANDBY_LOGO_WIDTH      500
#define STANDBY_LOGO_HEIGHT     180
#define QR_CODE_Y_OFFSET        (STANDBY_LOGO_HEIGHT + QR_CODE_SIZE/2 - STANDBY_CONT_HEIGHT/2)
#define STANDBY_LABEL_WIDTH     600
#define STANDBY_LABEL_HEIGHT    50

//Warning bar
inline lv_font_t warning_popup_font     = lv_font_montserrat_26;
#define WARNBAR_WIDTH                   70
#define WARNBAR_HEIGHT                  480
#define WARNBAR_BUTTON_WIDTH            60
#define WARNBAR_BUTTON_HEIGHT           60
#define WARNBAR_BUTTON_RADIUS           0
#define WARNBAR_BUTTON_BORDER           0

//Menu bar
inline lv_font_t menubar_button_font    = lv_font_montserrat_34;
#define MENUBAR_WIDTH                   70
#define MENUBAR_HEIGHT                  480
#define MENUBAR_BUTTON_WIDTH            68
#define MENUBAR_BUTTON_HEIGHT           68
#define MENUBAR_BUTTON_OFFSET           75
#define MENUBAR_BUTTON_RADIUS           0
#define MENUBAR_BUTTON_BORDER           0

// Preparation Page
inline lv_font_t preparation_font       = lv_font_montserrat_16;
inline lv_font_t preparation_popup_font = lv_font_montserrat_26;
#define PREP_CONT_WIDTH         660
#define PREP_CONT_HEIGHT        480
#define PREP_CONT_X_OFFSET      0
#define PREP_CONT_Y_OFFSET      0
#define PREP_BUTTON_WIDTH       220
#define PREP_BUTTON_HEIGHT      160
#define PREP_BUTTON_RADIUS      15
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
#define EROG_LABEL_Y_OFFSET     (EROG_BUTTON_HEIGHT + 50)
#define EROG_MSGBOX_X_OFFSET    0
#define EROG_MSGBOX_Y_OFFSET    0

// Menu Page
inline lv_font_t menu_list_font     = lv_font_montserrat_26;
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
inline lv_font_t wifi_button_font       = lv_font_montserrat_34;
inline lv_font_t wifi_text_font         = lv_font_montserrat_26;
#define WIFI_PASS_AREA_WIDTH            600
#define WIFI_PASS_AREA_HEIGHT           50
#define WIFI_SCAN_LIST_WIDTH            600
#define WIFI_SCAN_LIST_HEIGHT           50
#define WIFI_SWITCH_WIDTH               300
#define WIFI_SWITCH_HEIGHT              50
#define WIFI_SCAN_BUTTON_WIDTH          200
#define WIFI_SCAN_BUTTON_HEIGHT         50
#define WIFI_SWITCH_X_OFFSET            130
#define WIFI_SCAN_BUTTON_X_OFFSET       -40
#define WIFI_FIRST_LINE_Y_OFFSET        30
#define WIFI_SCAN_LIST_X_OFFSET         40
#define WIFI_SCAN_LIST_Y_OFFSET         110
#define WIFI_PASS_AREA_Y_OFFSET         20

// Statistics Page
inline lv_font_t statistics_font        = lv_font_montserrat_26;
#define STAT_TAB_WIDTH          650
#define STAT_TAB_HEIGHT         450
#define STAT_TAB_X_OFFSET       130
#define STAT_TAB_Y_OFFSET       0
#define STAT_COL1_WIDTH         400
#define STAT_COL2_WIDTH         100
#define STAT_MANUAL_SCROLL      70

// Cleaning Pages
inline lv_font_t clean_title_font      = lv_font_montserrat_48;
inline lv_font_t clean_button_font     = lv_font_montserrat_34;
inline lv_font_t clean_message_font    = lv_font_montserrat_26;
#define CLEAN_CONT_WIDTH               800
#define CLEAN_CONT_HEIGHT              480
#define CLEAN_TITLE_LABEL_WIDTH        660
#define CLEAN_TITLE_LABEL_HEIGHT       80
#define CLEAN_TITLE_LABEL_X_OFFSET     0
#define CLEAN_TITLE_LABEL_Y_OFFSET     20
#define CLEAN_IMAGE_WIDTH              200
#define CLEAN_IMAGE_HEIGHT             200
#define CLEAN_IMAGE_X_OFFSET           0
#define CLEAN_IMAGE_Y_OFFSET           120
#define CLEAN_MESS_LABEL_WIDTH         660
#define CLEAN_MESS_LABEL_HEIGHT        60
#define CLEAN_MESS_LABEL_X_OFFSET      0
#define CLEAN_MESS_LABEL_Y_OFFSET      340
#define CLEAN_BAR_WIDTH                600
#define CLEAN_BAR_HEIGHT               40
#define CLEAN_BAR_BORDER               3
#define CLEAN_BAR_X_OFFSET             0
#define CLEAN_BAR_Y_OFFSET             400
#define CLEAN_BUTTON_WIDTH             130
#define CLEAN_BUTTON_HEIGHT            200
#define CLEAN_BUTTON_RADIUS            15
#define CLEAN_BUTTON_BORDER            0
#define CLEAN_BUTTON_X_OFFSET          260
#define CLEAN_BUTTON_Y_OFFSET          120


// Descaling Page (TBD)
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

#endif /* _DEFINES_H_ */