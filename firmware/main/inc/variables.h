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
#define EVENT_INTERACTION_LOCAL             (1 << 1)

#define WIFI_CONNECTED_BIT              (1 << 0)
#define WIFI_DISCONNECTED_BIT           (1 << 1)
#define WIFI_CONFIGURATION_BIT          (1 << 2)
#define WIFI_NEW_FIRMWARE_AVAILABLE     (1 << 3)
#define WIFI_NEW_OTA_RETRY              (1 << 4)


/* Queue handlers */
inline QueueHandle_t xQueueDbgUartTx, xQueueAzureTx;

/* Events handlers */
inline  EventGroupHandle_t xWifiEvents, xModuleEvents;

/* Task handlers */
inline TaskHandle_t xHandleWiFi, xHandleMainTask, xHandleGui;
inline TaskHandle_t xHandleAzure, xHandleAzureTx, xHandleOta;
inline TaskHandle_t xHandleUi, xHandleDbg, xHandleDbgRx, xHandleDbgTx, xHandleDbgLiveData;

/* Azure variables */
inline Certificates cert;
inline IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
inline SemaphoreHandle_t iothubSemaphore;

/* Machine registers */
inline machine_connectivity_t machineConnectivity;

/* Machine current state */
inline internal_state_t machineInternalState;

#endif // VARIABLES_H_