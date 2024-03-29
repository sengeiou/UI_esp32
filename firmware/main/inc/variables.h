#ifndef VARIABLES_H_
#define VARIABLES_H_



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "nvs.h"

#include "types.h"
#include <map>

#include "iothub.h"
#include "iothub_client_ll.h"

/* Events handlers */
#define EVENT_INTERACTION_CLOUD             (1 << 0)

#define WIFI_CONNECTED_BIT              (1 << 0)
#define WIFI_DISCONNECTED_BIT           (1 << 1)
#define WIFI_CONFIGURATION_BIT          (1 << 2)
#define WIFI_NEW_FIRMWARE_AVAILABLE     (1 << 3)
#define WIFI_NEW_OTA_RETRY              (1 << 4)
#define WIFI_RESET_BIT                  (1 << 5)


#define GUI_MACHINE_ON                  (1 << 0)
#define GUI_MACHINE_OFF                 (1 << 1)
#define GUI_STATISTICS_DATA_BIT         (1 << 2)
#define GUI_SETTINGS_DATA_BIT           (1 << 3)
#define GUI_NEW_EROGATION_DATA_BIT      (1 << 4)
#define GUI_NEW_MILK_EROGATION_DATA_BIT (1 << 5)
#define GUI_STOP_EROGATION_BIT          (1 << 6)
#define GUI_STOP_MILK_EROGATION_BIT     (1 << 7)
#define GUI_ENABLE_MILK_BIT             (1 << 8)
#define GUI_WARNINGS_BIT                (1 << 9)
#define GUI_MACHINE_FAULT_BIT           (1 << 10)
#define GUI_CLOUD_REQUEST_BIT           (1 << 11)
#define GUI_NEW_CLEANING_DATA_BIT       (1 << 12)



/* Queue handlers */
inline QueueHandle_t xQueueDbgUartTx, xQueueCamUartTx, xQueueAzureTx;

/* Events handlers */
inline  EventGroupHandle_t xWifiEvents, xModuleEvents, xGuiEvents;

/* Task handlers */
inline TaskHandle_t xHandleWiFi, xHandleMainTask, xHandleGui;
inline TaskHandle_t xHandleAzure, xHandleAzureTx, xHandleOta;
inline TaskHandle_t xHandleDbg, xHandleDbgRx, xHandleDbgTx, xHandleDbgLiveData;
inline TaskHandle_t xHandleCam, xHandleCamRx, xHandleCamTx;

/* Azure variables */
inline Certificates cert;
inline IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
inline SemaphoreHandle_t iothubSemaphore;

/* Machine registers */
inline machine_connectivity_t machineConnectivity;

/* Machine current state */
inline internal_state_t machineInternalState;
inline gui_status_t     guiInternalState;
#endif // VARIABLES_H_