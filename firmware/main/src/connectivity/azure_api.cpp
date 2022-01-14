#include <stdio.h>
#include <stdlib.h>

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
#include "azure_api.h"
#include "utils.h"
#include "system_utils.h"
#include "spiffs_utils.h"
#include "gui_task.h"

#include "lavazza_cloud_protocol.h"

#include "esp_log.h"

#ifdef ADVANCED_DEBUG
    #define AZURE_DPS_TAG LINE_STRING "|" "AZURE_DPS"
    #define AZURE_IOTHUB_TAG LINE_STRING "|" "AZURE_IoTHUB"
    #define AZURE_DEVICE_TWIN_TAG LINE_STRING "|" "AZURE_DEVICETWIN"
    #define LIVE_DIAGNOSTIC_TAG LINE_STRING "|" "LIVE_DIAGNOSTIC"
    #define RESTART_TAG LINE_STRING "|" "REBOOT"
#else
    #define AZURE_DPS_TAG "AZURE_DPS"
    #define AZURE_IOTHUB_TAG "AZURE_IoTHUB"
    #define AZURE_DEVICE_TWIN_TAG "AZURE_DEVICETWIN"
    #define RESTART_TAG "REBOOT"
#endif


MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(PROV_DEVICE_RESULT, PROV_DEVICE_RESULT_VALUE);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(PROV_DEVICE_REG_STATUS, PROV_DEVICE_REG_STATUS_VALUES);

typedef struct
{
    int32_t     numberOfSamples;
    int32_t     frequency;
    char        correlationId[128];
} live_diagnostic_parameters;


void live_diagnostic_task(void* parameters)
{
    int32_t frequency = ((live_diagnostic_parameters*) parameters)->frequency;
    int32_t numberOfSamples = ((live_diagnostic_parameters*) parameters)->numberOfSamples;

    ESP_LOGI(LIVE_DIAGNOSTIC_TAG, "Started: NumOfSamples %d, frequency %d", numberOfSamples, frequency);

    ESP_LOGI(LIVE_DIAGNOSTIC_TAG, "Sent all samples. Stop Live Diagnostic");

    vTaskDelete(nullptr);
}

void restart_module_task(void)
{
    uint8_t restartTimer = 10;
    ESP_LOGW(RESTART_TAG, "Telemetry module will reboot in %d seconds", restartTimer);
    
    firmware_restart(restartTimer);

    vTaskDelete(nullptr);
}

void azure_dps_register_device(PROV_DEVICE_RESULT register_result, const char* iothub_uri, const char* device_id, void* user_context)
{
    if (user_context == NULL)
    {
        ESP_LOGI(AZURE_DPS_TAG, "user_context is NULL");
    }
    else
    {
        IOTHUB_CLIENT_INFO* user_ctx = (IOTHUB_CLIENT_INFO*)user_context;
        if(PROV_DEVICE_RESULT_OK == register_result)
        {
            ESP_LOGI(AZURE_DPS_TAG, "Registration information received from service: %s", iothub_uri);
            ESP_LOGI(AZURE_DPS_TAG, "Registration information received from device Id: %s", device_id);
            (void)mallocAndStrcpy_s(&user_ctx->iothub_uri, iothub_uri);
            (void)mallocAndStrcpy_s(&user_ctx->device_id, device_id);
            user_ctx->registration_complete = 1;            

        }
        else
        {
            ESP_LOGE(AZURE_DPS_TAG, "Failure encountered on registration %s", MU_ENUM_TO_STRING(PROV_DEVICE_RESULT, register_result));
            user_ctx->registration_complete = 2;
        }
    }
}

void azure_dps_registation_status(PROV_DEVICE_REG_STATUS reg_status, void* user_context)
{
    (void)user_context;
    ESP_LOGI(AZURE_DPS_TAG, "Provisioning Status: %s", MU_ENUM_TO_STRING(PROV_DEVICE_REG_STATUS, reg_status));
}

void azure_iothub_send_message(IOTHUB_MESSAGE_HANDLE message)
{
    if(NULL == message)
    {
        ESP_LOGE(AZURE_IOTHUB_TAG, "iotHubMessageHandle is not valid");
    }
    else
    {
        /* ------------------ Code to add properties -------------------- */
        // const char* propText = "critical";
        // MAP_HANDLE propMap = IoTHubMessage_Properties(message);
        // if(MAP_OK != Map_AddOrUpdate(propMap, "alert", propText))
        // {
        //     ESP_LOGE(AZURE_IOTHUB_TAG, "Map_AddOrUpdate failed");
        // }
        /* -------------------------------------------------------------- */

        IOTHUB_CLIENT_RESULT res = IoTHubClient_LL_SendEventAsync(iotHubClientHandle, message, azure_iothub_send_confirmation, NULL);
        if(IOTHUB_CLIENT_OK != res)
        {
            ESP_LOGE(AZURE_IOTHUB_TAG, "IoTHubClient_LL_SendEventAsync failed");
        }
        else
        {
            ESP_LOGD(AZURE_IOTHUB_TAG, "IoTHubClient_LL_SendEventAsync accepted message for transmission to IoT Hub");
        }
    }

    if(pdTRUE == xSemaphoreTake(iothubSemaphore, 50/portTICK_PERIOD_MS))
    {
        IoTHubClient_LL_DoWork(iotHubClientHandle);
        xSemaphoreGive(iothubSemaphore);
    }
    else
    {
        ESP_LOGW(AZURE_IOTHUB_TAG, "Failed to acquire mutex");
    }
    IoTHubMessage_Destroy(message);
}

IOTHUBMESSAGE_DISPOSITION_RESULT azure_iothub_on_message(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    const char* buffer;
    size_t size;
    MAP_HANDLE mapProperties;
    const char* messageId;
    const char* correlationId;

    // Message properties
    if(NULL == (messageId = IoTHubMessage_GetMessageId(message)))
    {
        messageId = "<null>";
    }

    if(NULL == (correlationId = IoTHubMessage_GetCorrelationId(message)))
    {
        correlationId = "<null>";
    }

    // Message content
    if(IOTHUB_MESSAGE_OK != IoTHubMessage_GetByteArray(message, (const unsigned char**)&buffer, &size))
    {
        ESP_LOGE(AZURE_IOTHUB_TAG, "Unable to retrieve the message data");
    }
    else
    {
        ESP_LOGI(AZURE_IOTHUB_TAG, "Received Message");
        ESP_LOGI(AZURE_IOTHUB_TAG, "Message ID: %s", messageId);
        ESP_LOGI(AZURE_IOTHUB_TAG, "Correlation ID: %s", correlationId);
        ESP_LOGI(AZURE_IOTHUB_TAG, "Size: %d", (int)size);
        ESP_LOGI(AZURE_IOTHUB_TAG, "Data: %.*s", (int)size, buffer);
    }

    // Retrieve properties from the message
    mapProperties = IoTHubMessage_Properties(message);
    if(NULL != mapProperties)
    {
        const char*const* keys;
        const char*const* values;
        size_t propertyCount = 0;
        if(MAP_OK == Map_GetInternals(mapProperties, &keys, &values, &propertyCount))
        {
            if(propertyCount > 0)
            {
                size_t index;
                ESP_LOGI(AZURE_IOTHUB_TAG, "Message Properties:");
                for (index = 0; index < propertyCount; index++)
                {
                    ESP_LOGI(AZURE_IOTHUB_TAG, "\tKey: %s Value: %s", keys[index], values[index]);
                }
            }
        }
    }

    /* Some device specific action code goes here... */

    return IOTHUBMESSAGE_ACCEPTED;
}

void azure_iothub_send_confirmation(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    if(IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        ESP_LOGD(AZURE_IOTHUB_TAG, "Confirmation received for message");
    }
    else
    {
        ESP_LOGE(AZURE_IOTHUB_TAG, "Confirmation failed for message with reason: %s", MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    }
}

void azure_iothub_connection_status(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback)
{    
    int* counter = (int*)userContextCallback;

    if(result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        ESP_LOGI(AZURE_IOTHUB_TAG, "Connection Status result: %s", MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS, result));
        ESP_LOGI(AZURE_IOTHUB_TAG, "Connection Status reason: %s", MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason));

        setWifiLed(LED_ON, ON_LED_PERIOD, 100);

        if(NULL != xHandleAzureTx)
            vTaskResume(xHandleAzureTx);
    }
    else
    {
        if(nullptr == userContextCallback)
            return;
   
        (*counter)++;
            
        switch(reason)
        {
            case IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN:
            {
                ESP_LOGW(AZURE_IOTHUB_TAG, "Attempt %d: Id rejected because of expired SAS Token", *counter);
                break;
            }
            case IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED:
            {
                ESP_LOGW(AZURE_IOTHUB_TAG, "Attempt %d: Device disabled by IoT Hub", *counter);
                break;
            }
            case IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL:
            {
                // Bad login credentials, no need to retry again and again - exit immediately
                ESP_LOGE(AZURE_IOTHUB_TAG, "Attempt %d: Invalid Device Id or Shared Access Key", *counter);
                break;
            }
            case IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED:
            {
                // Retry timeout set by policy has expired, exit immediately
                ESP_LOGE(AZURE_IOTHUB_TAG, "Attempt %d: Retry Policy timeout expired", *counter);
                break;
            }
            case IOTHUB_CLIENT_CONNECTION_NO_NETWORK:
            {
                // VMU-C is not connected to a network, exit immediately
                ESP_LOGE(AZURE_IOTHUB_TAG, "Attempt %d: Network error", *counter);
                break;
            }
            case IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR:
            default:
            {
                ESP_LOGE(AZURE_IOTHUB_TAG, "Attempt %d: Unauthorized MQTT connection", *counter);
                break;
            }
        }
    }
}

int azure_iothub_device_method(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback)
{
    (void)userContextCallback;
    (void)payload;
    (void)size;

    ESP_LOGI(AZURE_IOTHUB_TAG, "Method Name: %s", method_name);

    int result = 405;
    char* deviceMethodResponse = "{}";

    if(0 == strcmp(method_name, "makeCoffee"))
    {
        uint8_t preparation = utils::json::getValue<uint8_t>(std::string((const char*)payload), "preparation");
        uint16_t dose = utils::json::getValue<uint16_t>(std::string((const char*)payload), "dose");

        machineInternalState.azureRequest.command = MAKE_COFFEE;
        machineInternalState.azureRequest.desiredDose = dose;
        machineInternalState.azureRequest.desiredPreparation = static_cast<coffee_type_t>(preparation);

        if(machineInternalState.azureRequest.desiredDose != 0)
            ESP_LOGI(AZURE_IOTHUB_TAG, "Request: %s with dose %d", coffee_type_string[preparation], machineInternalState.azureRequest.desiredDose);
        else
            ESP_LOGI(AZURE_IOTHUB_TAG, "Request: %s with standard dose", coffee_type_string[preparation]);

        xEventGroupSetBits(xModuleEvents, EVENT_INTERACTION_CLOUD);
        result = 200;
    }
    else if(0 == strcmp(method_name, "powerOn"))
    {
        machineInternalState.azureRequest.command = POWER_ON;
        xEventGroupSetBits(xModuleEvents, EVENT_INTERACTION_CLOUD);
        result = 200;
    }
    else if(0 == strcmp(method_name, "powerOff"))
    {
        machineInternalState.azureRequest.command = POWER_OFF;
        xEventGroupSetBits(xModuleEvents, EVENT_INTERACTION_CLOUD);
        result = 200;
    }
    else if(0 == strcmp(method_name, "firmwareUpdate"))
    {
        std::string fwVer = utils::json::getValue<std::string>(std::string((const char*)payload), "version");
        std::string fwUrl = utils::json::getValue<std::string>(std::string((const char*)payload), "url");

        if(false == fwUrl.empty() && false == fwVer.empty())
        {
            ESP_LOGI(AZURE_IOTHUB_TAG, "Firmware update. New version %s, url: %s", fwVer.c_str(), fwUrl.c_str());

            strncpy(machineInternalState.ota.fwUrl, fwUrl.c_str(), MAX_URL_LENGTH);
            strncpy(machineInternalState.ota.newFwVer, fwVer.c_str(), MAX_FW_VERSION_LENGTH);
            xEventGroupSetBits(xWifiEvents, WIFI_NEW_FIRMWARE_AVAILABLE);
            result = 200;
        }
        else
        {
            ESP_LOGE(AZURE_IOTHUB_TAG, "Firmware update invalid request. Version is wrong or Url is empty");
        }
    }
    else if (0 == strcmp("isConnected", method_name))
    {
        result = 200;
        deviceMethodResponse = isConnectedResponse(0, "correlationId", nullptr);
    }
    else if (0 == strcmp("turnOnMachine", method_name))
    {
        machineInternalState.azureRequest.command = POWER_ON;
        xEventGroupSetBits(xModuleEvents, EVENT_INTERACTION_CLOUD);
        result = 200;
        // deviceMethodResponse = turnOnOffMachineResponse(status, "correlationId", nullptr);
    }
    else if (0 == strcmp("makeCoffeeByButtons", method_name))
    {
        result = 200;
        // deviceMethodResponse = makeCoffeeByButtonsResponse(status, "correlationId", nullptr);
    }
    else if (0 == strcmp("stopBrewing", method_name))
    {
        result = 200;
        // deviceMethodResponse = stopBrewingResponse(status, "correlationId", nullptr);
    }
    else if (0 == strcmp("turnOffMachine", method_name))
    {
        machineInternalState.azureRequest.command = POWER_OFF;
        xEventGroupSetBits(xModuleEvents, EVENT_INTERACTION_CLOUD);
        result = 200;
        // deviceMethodResponse = turnOnOffMachineResponse(status, "correlationId", "propertyBag");
    }
    else
    {
        // Method not implemented
        result = 405;
        ESP_LOGW(AZURE_IOTHUB_TAG, "Method not implemented");
    }

    switch(result)
    {
        case 200:
        {
            *response_size = strlen(deviceMethodResponse);
            *response = (unsigned char*) malloc(*response_size);
            if(*response != NULL)
            {
                (void)memcpy(*response, deviceMethodResponse, *response_size);
            }
            break;
        }
        case 404:
        {
            *response_size = strlen(deviceMethodResponse);
            *response = (unsigned char*) malloc(*response_size);
            if(*response != NULL)
            {
                (void)memcpy(*response, deviceMethodResponse, *response_size);
            }
            break;
        }
        case 405:
        default:
        {
            *response_size = strlen(deviceMethodResponse);
            *response = (unsigned char*) malloc(*response_size);
            if(*response != NULL)
            {
                (void)memcpy(*response, deviceMethodResponse, *response_size);
            }
            break;
        }
    }

    return result;
}

void azure_iothub_on_device_twin_update(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    (void)size;
    (void)userContextCallback;
    
    ESP_LOGD(AZURE_DEVICE_TWIN_TAG, "New property received: %.*s", (int)size, payLoad);
    ESP_LOGI(AZURE_DEVICE_TWIN_TAG, "New property received. Update type: %s", MU_ENUM_TO_STRING(DEVICE_TWIN_UPDATE_STATE, update_state));

    char fake_property[256] = "{\"fwTMVer\":\"99-0-18-2\",\"fwMBVer\":\"7-0-0-30\",\"mbParameters\":{\"2\":\"18000USA\",\"3\":\"LAVAZZATESTUSA000003\"}}";
    azure_iothub_report_new_device_twin((const char *) fake_property, false, false);
}

void azure_iothub_report_device_twin(int status_code, void* userContextCallback)
{
    (void)userContextCallback;

    ESP_LOGD(AZURE_DEVICE_TWIN_TAG, "Reported properties update completed with result: %d", status_code);
}

void azure_iothub_report_new_device_twin(const char* reportedProperties, bool isNewFirmwareTM, bool isNewFirmwareMB)
{
    if(IOTHUB_CLIENT_OK  == IoTHubDeviceClient_LL_SendReportedState(iotHubClientHandle, (const unsigned char*)reportedProperties, strlen(reportedProperties), azure_iothub_report_device_twin, NULL))
    {
        ESP_LOGI(AZURE_DEVICE_TWIN_TAG, "Report property \"%s\" done", reportedProperties); 
    }
    else
    {
        ESP_LOGE(AZURE_DEVICE_TWIN_TAG, "Failed to report property \"%s\"", reportedProperties); 
    }
    // free((void*)reportedProperties);  
}