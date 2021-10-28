#ifndef GUI_TASK_H_
#define GUI_TASK_H_

#include "types.h"

#define ON_LED_PERIOD                   0
#define OFF_LED_PERIOD                  0

#define WIFI_AP_LED_PERIOD                  3000
#define WIFI_STA_CONNECTION_LED_PERIOD      1000
#define WIFI_AZURE_CONNECTION_LED_PERIOD    500
#define WIFI_OTA_LED_PERIOD                 250

inline void setWifiLed(ui_led_mode_t mode, uint16_t period, uint8_t duty, bool force = true)
{

}

/*Main GUI Task*/
void gui_task(void* data);

#endif // GUI_TASK_H_