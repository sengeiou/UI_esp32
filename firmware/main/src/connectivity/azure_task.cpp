#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <chrono>

#include "iothub.h"
#include "iothub_message.h"
#include "iothub_client_version.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/http_proxy_io.h"

#include "iothub_device_client_ll.h"
#include "iothub_client_options.h"
#include "azure_prov_client/prov_device_ll_client.h"
#include "azure_prov_client/prov_security_factory.h"
#include "sdkconfig.h"

#include "iothubtransportmqtt.h"
#include "azure_prov_client/prov_transport_mqtt_client.h"

#include "types.h"
#include "variables.h"
#include "azure_task.h"
#include "azure_api.h"
#include "utils.h"
#include "system_utils.h"
#include "ca_certificates.h"
#include "spiffs_utils.h"
#include "gui_task.h"



#ifdef ADVANCED_DEBUG
    #define AZURE_MAIN_TAG LINE_STRING "|" "AZURE_TASK"
    #define AZURE_TX_TAG LINE_STRING "|" "AZURE_TX_TASK"
#else
    #define AZURE_MAIN_TAG "AZURE_TASK"
    #define AZURE_TX_TAG "AZURE_TX_TASK"
#endif

static bool azure_running;

extern "C" const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description(void)
{
    return NULL;
}

int azure_init(void)
{
    int lReturnValue = -1;

    #if ((CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1) || (CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY == 1))
    IOTHUB_CLIENT_INFO user_ctx;
    memset(&user_ctx, 0, sizeof(IOTHUB_CLIENT_INFO));
    user_ctx.registration_complete = 0;
    user_ctx.sleep_time = 1;
    ESP_LOGI(AZURE_MAIN_TAG, "Provisioning API Version: %s", Prov_Device_LL_GetVersionString());
    #endif
   
    ESP_LOGI(AZURE_MAIN_TAG, "Iothub API Version: %s", IoTHubClient_GetVersionString());
    ESP_LOGI(AZURE_MAIN_TAG, "Lavazza Environment: %s (%s)", AZURE_ENVIRONMENT, CONFIG_DPS_ID_SCOPE);

    bool traceOnAzure = AZURE_DEBUG_ENABLED;

    #if ((CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1) || (CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY == 1))
    SECURE_DEVICE_TYPE hsm_type;

    #if CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1
    hsm_type = SECURE_DEVICE_TYPE_X509;
    (void)prov_dev_security_init(hsm_type);
    #endif

    #if CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY == 1
    hsm_type = SECURE_DEVICE_TYPE_SYMMETRIC_KEY;
    (void)prov_dev_security_init(hsm_type);
    (void)prov_dev_set_symmetric_key_info(cert.device_common_name, CONFIG_AZURE_SHARED_ACCESS_KEY);
    #endif

    PROV_DEVICE_LL_HANDLE dpsAzureHandle = Prov_Device_LL_Create(AZURE_DPS_ADDRESS, AZURE_DPS_SCOPE_ID, Prov_Device_MQTT_Protocol);
    if(NULL == dpsAzureHandle)
    {
        ESP_LOGE(AZURE_MAIN_TAG, "Failed to create DPS client");
    }
    else
    {
        Prov_Device_LL_SetOption(dpsAzureHandle, OPTION_TRUSTED_CERT, default_ca_blobstorage);

        Prov_Device_LL_SetOption(dpsAzureHandle, PROV_OPTION_LOG_TRACE, &traceOnAzure);

        #if CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY == 1    
        Prov_Device_LL_SetOption(dpsAzureHandle, PROV_REGISTRATION_ID, cert.device_common_name);
        #endif

        if(Prov_Device_LL_Register_Device(dpsAzureHandle, azure_dps_register_device, &user_ctx, azure_dps_registation_status, &user_ctx) != PROV_DEVICE_RESULT_OK)
        {
            ESP_LOGE(AZURE_MAIN_TAG, "Failed to register device on DPS");
        }
        else
        {
            do
            {
                Prov_Device_LL_DoWork(dpsAzureHandle);
                ThreadAPI_Sleep(user_ctx.sleep_time);
            } while (user_ctx.registration_complete == 0);
        }
        Prov_Device_LL_Destroy(dpsAzureHandle);
    }

    if (user_ctx.registration_complete != 1)
    {
        ESP_LOGE(AZURE_MAIN_TAG, "Registration to DPS failed");
        lReturnValue = -1;
    }
    else
    {
    #endif
        #if CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1
        ESP_LOGI(AZURE_MAIN_TAG, "Creating IoTHub Device handle (with x509 certificates)");
        iotHubClientHandle = IoTHubDeviceClient_LL_CreateFromDeviceAuth(user_ctx.iothub_uri, user_ctx.device_id, MQTT_Protocol);
        #endif

        #if CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY == 1
        ESP_LOGI(AZURE_MAIN_TAG, "Creating IoTHub Device handle (Shared Access Key)");
        iotHubClientHandle = IoTHubDeviceClient_LL_CreateFromDeviceAuth(user_ctx.iothub_uri, user_ctx.device_id, MQTT_Protocol);
        #endif

        #if CONFIG_AZURE_AUTHENTICATION_METHOD_CONNECTION_STRING == 1
        ESP_LOGI(AZURE_MAIN_TAG, "Creating IoTHub Device handle from connection string");
        iotHubClientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(CONFIG_AZURE_CONNECTION_STRING, MQTT_Protocol);
        #endif

        if(NULL == iotHubClientHandle)
        {
            ESP_LOGE(AZURE_MAIN_TAG, "Failed to create iotHubClientHandle");
            lReturnValue = -1;
        }
        else
        {
            #if CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1
            ESP_LOGI(AZURE_MAIN_TAG, "Created IoTHub Device handle from: %s", user_ctx.iothub_uri);
            #endif

            #if CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1
            IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_TRUSTED_CERT, default_ca_blobstorage);
            #endif

            IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &traceOnAzure);

            IoTHubClient_LL_SetConnectionStatusCallback(iotHubClientHandle, azure_iothub_connection_status, NULL);

            IOTHUB_CLIENT_RESULT res;

            /* Setting DeviceTwin callback, so we can handle configurations */
            res = IoTHubDeviceClient_LL_SetDeviceTwinCallback(iotHubClientHandle, azure_iothub_on_device_twin_update, NULL);
            if(IOTHUB_CLIENT_OK != res)
            {
                ESP_LOGE(AZURE_MAIN_TAG, "Failed to set device twin callback");
                lReturnValue = -1;
            }
            else
            {
                ESP_LOGI(AZURE_MAIN_TAG, "Added device twin callback");
                lReturnValue = 0;
            }

            /* Setting Message callback, so we can receive Commands */
            res = IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, azure_iothub_on_message, NULL);
            if(IOTHUB_CLIENT_OK != res)
            {
                ESP_LOGE(AZURE_MAIN_TAG, "Failed to set message callback");
                lReturnValue = -1;
            }
            else
            {
                ESP_LOGI(AZURE_MAIN_TAG, "Added message callback");
                lReturnValue = 0;
            }
            
            /* Setting DeviceMethod callback, so we can invoke routines */
            res = IoTHubDeviceClient_LL_SetDeviceMethodCallback(iotHubClientHandle, azure_iothub_device_method, NULL);
            if(IOTHUB_CLIENT_OK != res)
            {
                ESP_LOGE(AZURE_MAIN_TAG, "Failed to set device method callback");
                lReturnValue = -1;
            }
            else
            {
                ESP_LOGI(AZURE_MAIN_TAG, "Added device method callback");
                lReturnValue = 0;
            }
        }
    
    #if ((CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1) || (CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY == 1))
    }
    free(user_ctx.iothub_uri);
    free(user_ctx.device_id);
    #endif

    /* Setting KeepAlive timeout */
    int timeout = CONFIG_AZURE_TIMEOUT_KEEP_ALIVE;
    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_KEEP_ALIVE, &timeout);

    return lReturnValue;
}

void azure_task(void *pvParameter)
{
    ESP_LOGI(AZURE_MAIN_TAG, "Starting...");
    
    setWifiLed(LED_BLINKING, WIFI_AZURE_CONNECTION_LED_PERIOD, 50);

    int azure_initialized = azure_init();

    if(0 == azure_initialized)
    {
        iothubSemaphore = xSemaphoreCreateMutex();

        utils::system::start_thread_core(&azure_tx_task, &xHandleAzureTx, "azure_tx_task", 1024*6, 4, 1);

        azure_running = true;

        while(true == azure_running)
        {
            if(pdTRUE == xSemaphoreTake(iothubSemaphore, 50/portTICK_PERIOD_MS))
            {
                IoTHubClient_LL_DoWork(iotHubClientHandle);
                xSemaphoreGive(iothubSemaphore);
            }
            else
            {
                ESP_LOGW(AZURE_MAIN_TAG, "Failed to acquire mutex");
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    else
    {
        //Do nothing
    }
    
    vTaskDelete(nullptr);
}

void azure_deinit()
{
    ESP_LOGI(AZURE_MAIN_TAG, "Stopping azure tasks...");
    azure_running = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ESP_LOGI(AZURE_MAIN_TAG, "Stopping azure tasks...DONE");
    
    if(nullptr != iotHubClientHandle)
    {
        if(iothubSemaphore != nullptr)
            xSemaphoreGive(iothubSemaphore);

        IoTHubDeviceClient_LL_Destroy(iotHubClientHandle);
        IoTHub_Deinit();

        vSemaphoreDelete(iothubSemaphore);
    }

    prov_dev_security_deinit();

    if(nullptr != xHandleAzure)
        xHandleAzure = nullptr;

    if(nullptr != xHandleAzureTx)
        xHandleAzureTx = nullptr;

    ESP_LOGI(AZURE_MAIN_TAG, "Azure de-init done");
}

void azure_tx_task(void *pvParameter)
{   
    ESP_LOGI(AZURE_TX_TAG, "Starting...");
    azure_queue_message_t xQueueAzureTxMsg;

    while(true == azure_running) 
    {
        int waiting_msg = uxQueueMessagesWaiting(xQueueAzureTx);

        /*Only if the message has been correctly received*/
        if(pdFALSE != xQueueReceive(xQueueAzureTx, (void *) &xQueueAzureTxMsg, 500 / portTICK_PERIOD_MS)) 
        {
            ESP_LOGI(AZURE_TX_TAG, "Received new message"); 
            switch(xQueueAzureTxMsg.type)
            {
                case EROGATION_PROGRESS:
                {
                    ESP_LOGI(AZURE_TX_TAG, "MsgType = %d - Payload = preparation %s", xQueueAzureTxMsg.type);
                    char* payload_str = "{\"preparation\":\"COFFEE_LONG\", \"desiredDose\":22, \"currentDose\":2}";
                    IOTHUB_MESSAGE_HANDLE message = IoTHubMessage_CreateFromByteArray((const unsigned char*) payload_str, strlen(payload_str));
                    IoTHubMessage_SetContentTypeSystemProperty(message, "application%2Fjson");
                    IoTHubMessage_SetContentEncodingSystemProperty(message, "utf-8");
                    free(payload_str);
                    azure_iothub_send_message(message);
                    break;
                }
                default:
                    break;
            }
        }
        else if(waiting_msg > 0)
        {
            ESP_LOGE(AZURE_TX_TAG, "Waiting elements in queue: %d", waiting_msg);
        }
        else
        {
            ESP_LOGD(AZURE_TX_TAG, "xQueueAzureTx remaining %d", waiting_msg); 
        }
    }

    vTaskDelete(nullptr);
}
