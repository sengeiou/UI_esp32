#include "ota.h"
#include "variables.h"
#include "types.h"
#include "ca_certificates.h"
#include "utils.h"
#include "spiffs_utils.h"
#include "sdkconfig.h"

#include <thread>
#include <chrono>

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "gui_task.h"

#ifdef ADVANCED_DEBUG
	#define LINE_STRING STRINGIZE(__LINE__)
    #define TAG_OTA LINE_STRING "|" "OTA_TASK"
    #define TAG_OTA_FIRST LINE_STRING "|" "OTA_TASK_FIRST"
    #define TAG_OTA_SECOND LINE_STRING "|" "OTA_TASK_SECOND"
#else
    #define TAG_OTA "OTA_TASK"
    #define TAG_OTA_FIRST "OTA_TASK_FIRST"
    #define TAG_OTA_SECOND "OTA_TASK_SECOND"
#endif

#define BUFFSIZE        4096
static char ota_write_data[BUFFSIZE + 1] = { 0 };

esp_err_t _http_event_handle(esp_http_client_event_t* evt)
{
    switch(evt->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG_OTA, "Http client error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG_OTA, "Http client connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG_OTA, "Http client header sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG_OTA, "Http client header %s -> %s", evt->header_key, evt->header_value);
            if(0 == strcmp(evt->header_key, "Location"))
            {
                ESP_LOGI(TAG_OTA, "Http client header %s -> %s", evt->header_key, evt->header_value);
                strncpy(machineInternalState.ota.fwUrl, evt->header_value, MAX_URL_LENGTH);
            }
            if(0 == strcmp(evt->header_key, "correlationId"))
            {
                ESP_LOGI(TAG_OTA, "Http client header %s -> %s", evt->header_key, evt->header_value);
                strncpy(machineInternalState.ota.correlationId, evt->header_value, MAX_UUID_LENGTH);
            }
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGV(TAG_OTA, "Http client received data, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG_OTA, "Http client finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG_OTA, "Http client disconnect");
            break;
    }
    return ESP_OK;
}

esp_err_t _http_ota_event_handle(esp_http_client_event_t* evt)
{
    switch(evt->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG_OTA, "Http OTA client error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG_OTA, "Http OTA client connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG_OTA, "Http OTA client header sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG_OTA, "Http OTA client header %s -> %s", evt->header_key, evt->header_value);
            if(0 == strcmp(evt->header_key, "Content-Length"))
            {
                ESP_LOGI(TAG_OTA, "Http client header %s -> %s", evt->header_key, evt->header_value);
                machineInternalState.ota.firmwareSize = atol(evt->header_value) + machineInternalState.ota.dataWritten;
                save_firmware_connectivity(false);
            }
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG_OTA, "Http OTA client received data, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG_OTA, "Http OTA client finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG_OTA, "Http OTA client disconnect");
            break;
    }
    return ESP_OK;
}

void ota_failure_end(ota_status status, esp_http_client_handle_t client, bool clean_client, bool close_client)
{
    if(NULL != client)
    {
        if(true == close_client)
            esp_http_client_close(client);
        
        if(true == clean_client)
            esp_http_client_cleanup(client);
    }
    
    ESP_LOGE(TAG_OTA, "Firmware upgrade failed, status %s", ota_status_string[status]);

    if(machineInternalState.ota.currentRetry < CONFIG_MAX_NUMBER_OF_OTA_RETRY)
    {
        machineInternalState.ota.otaStatus = status;
        
        switch(status)
        {
            case DOWNLOADING:
            case INSTALLING:
            {
                //Do nothing
                break;
            }
            case NO_AVAILABLE_PARTITION:
            case MAXIMUM_ATTEMPTS_EXCEEDED:
            case FAILED_SIGNATURE:
            case FAILED:
            {
                strcpy(machineInternalState.ota.errorFwVer, machineInternalState.ota.fwUrl);
                memset(machineInternalState.ota.fwUrl, 0, (MAX_URL_LENGTH * sizeof(char)));
                break;
            }
            case FAILED_REDIRECT:
            case FAILED_DOWNLOAD:
            case FAILED_HTTP_CONNECTION:
            case FAILED_MB_NOT_RESPONDING:
            case SSL_ERROR:
            case FAILED_INSTALLATION_START:
            case FAILED_WRITE_DATA:
            case WRONG_LENGTH_RECEIVED:
            default:
            {
                xEventGroupSetBits(xWifiEvents, WIFI_NEW_OTA_RETRY);
                break;
            }
        }
        
    }
    else
    {
        machineInternalState.ota.otaStatus = MAXIMUM_ATTEMPTS_EXCEEDED;
        memset(machineInternalState.ota.fwUrl, 0, (MAX_URL_LENGTH * sizeof(char)));
        ESP_LOGE(TAG_OTA, "Firmware upgrade failed, max retry number reached (%d/%d)", machineInternalState.ota.currentRetry, CONFIG_MAX_NUMBER_OF_OTA_RETRY);
    }
    
    save_firmware_connectivity(false);
};

const esp_partition_t* check_partitions(const esp_partition_t* configured_partition, const esp_partition_t* running_partition)
{
    configured_partition = esp_ota_get_boot_partition();
    running_partition = esp_ota_get_running_partition();
    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(NULL);

    if(configured_partition != running_partition) 
    {
        ESP_LOGW(TAG_OTA_SECOND, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x", configured_partition->address, running_partition->address);
        ESP_LOGW(TAG_OTA_SECOND, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }
    ESP_LOGI(TAG_OTA_SECOND, "Running partition type %d subtype %d (offset 0x%08x)", running_partition->type, running_partition->subtype, running_partition->address);

    if(NULL == update_partition)
    {
        ota_failure_end(NO_AVAILABLE_PARTITION, NULL, false, false);
    }
    else
    {
        ESP_LOGI(TAG_OTA_SECOND, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);
    }
    return update_partition;
};

void first_step_ota_authentication(void)
{
    ESP_LOGI(TAG_OTA_SECOND, "Starting OTA step 1/2");

    esp_err_t err;
    esp_http_client_handle_t client = NULL;

    ESP_LOGI(TAG_OTA_FIRST, "Using embedded API management certificate");
    esp_http_client_config_t http_config = {
        .url = (const char *)machineInternalState.ota.fwUrl,
        .cert_pem = (const char *)default_ca_api_management,
        #ifdef CONFIG_AZURE_AUTHENTICATION_METHOD_X509
        .client_cert_pem = (const char *)cert.device_certificate_pem,
        .client_key_pem = (const char *)cert.device_private_key_pem,
        #endif
    };

    http_config.disable_auto_redirect = true;
    http_config.max_redirection_count = 0;
    http_config.event_handler = _http_event_handle;
    http_config.is_async = false;
    http_config.timeout_ms = 3600000;  //60 minutes

    client = esp_http_client_init(&http_config);

    if(NULL == client) 
    {
        ESP_LOGE(TAG_OTA_FIRST, "Failed to initialise HTTP connection");
        ota_failure_end(FAILED_HTTP_CONNECTION, client, false, false);
    }

    char deviceId[100];
    //create_device_id(deviceId);
    
    err = esp_http_client_set_header(client, "deviceId", deviceId);
    if(ESP_OK != err)
        ESP_LOGE(TAG_OTA_FIRST, "Failed to add header \"deviceId\"");
    else
        ESP_LOGI(TAG_OTA_FIRST, "Added header \"deviceId\":%s", deviceId);

    #ifdef CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY
    err = esp_http_client_set_header(client, "sharedAccessKey", CONFIG_AZURE_SHARED_ACCESS_KEY);
    if(ESP_OK != err)
        ESP_LOGE(TAG_OTA_FIRST, "Failed to add header \"sharedAccessKey\"");
    else
        ESP_LOGI(TAG_OTA_FIRST, "Added header \"sharedAccessKey\":%s", CONFIG_AZURE_SHARED_ACCESS_KEY);
    #endif

    if(ESP_OK != esp_http_client_open(client, 0))
    {
        ESP_LOGE(TAG_OTA_FIRST, "Failed to open HTTP connection to %s", machineInternalState.ota.fwUrl);
        ota_failure_end(FAILED_REDIRECT, client, true, false);
    }

    esp_http_client_fetch_headers(client);
         
    int statusCode = esp_http_client_get_status_code(client);

    if(statusCode != HttpStatus_Found && statusCode != 200)
    {
        ESP_LOGE(TAG_OTA_FIRST, "Http status code %d, OTA update failed", statusCode);
        ota_failure_end(FAILED_REDIRECT, client, true, true);
    }
    else
    {
        esp_http_client_cleanup(client);
        ESP_LOGI(TAG_OTA_FIRST, "Http status code = %d, redirect to %s", statusCode, machineInternalState.ota.fwUrl);
    }
}

void second_step_ota_download()
{
    setWifiLed(LED_BLINKING, WIFI_OTA_LED_PERIOD, 50);
    ESP_LOGI(TAG_OTA_SECOND, "Starting OTA step 2/2");

    esp_err_t err = ESP_OK;
    esp_ota_handle_t update_handle = 0;
    
    esp_http_client_handle_t client;

    const esp_partition_t* configured_partition = NULL;
    const esp_partition_t* running_partition = NULL;
    const esp_partition_t* update_partition = NULL;
    
    ESP_LOGI(TAG_OTA_SECOND, "Using embedded BlobStaorage certificate");
    esp_http_client_config_t http_ota_config = {
        .url = (const char *)machineInternalState.ota.fwUrl,
        .cert_pem = (const char *)default_ca_blobstorage,
        #ifdef CONFIG_AZURE_AUTHENTICATION_METHOD_X509
        .client_cert_pem = (const char *)cert.device_certificate_pem,
        .client_key_pem = (const char *)cert.device_private_key_pem,
        #endif
    };
    http_ota_config.disable_auto_redirect = true;
    http_ota_config.max_redirection_count = 0;
    http_ota_config.event_handler = _http_ota_event_handle;
    http_ota_config.timeout_ms = 3600000;  //60 minutes

    client = esp_http_client_init(&http_ota_config);
    
    machineInternalState.ota.currentRetry++;
    save_firmware_connectivity(false);

    if((machineInternalState.ota.dataWritten % BUFFSIZE) != 0)
        machineInternalState.ota.dataWritten = (machineInternalState.ota.dataWritten / BUFFSIZE) * BUFFSIZE;

    if(0 != machineInternalState.ota.dataWritten)
    {
        char resumeHeader[30];
        snprintf(resumeHeader, 30, "bytes=%d-", machineInternalState.ota.dataWritten);
        err = esp_http_client_set_header(client, "Range", resumeHeader);
        if(ESP_OK != err)
            ESP_LOGE(TAG_OTA_SECOND, "Failed to add header \"Range\"");
        else
            ESP_LOGI(TAG_OTA_SECOND, "Added header \"Range\":%s", resumeHeader);
    }
    else
    {
        ESP_LOGI(TAG_OTA_SECOND, "No partial download. Starting a new one...");
    }

    if(NULL == client)
    {
        ESP_LOGE(TAG_OTA_SECOND, "Failed to initialize OTA HTTP connection");
        ota_failure_end(FAILED_HTTP_CONNECTION, client, false, false);
    }

    err = esp_http_client_open(client, 0);
    if(ESP_OK != err) 
    {
        ESP_LOGE(TAG_OTA_SECOND, "Failed to open OTA HTTP connection: %s", esp_err_to_name(err));
        ota_failure_end(FAILED_HTTP_CONNECTION, client, true, false);
    }

    esp_http_client_fetch_headers(client);

    bool image_header_was_checked = (0 != machineInternalState.ota.dataWritten);
    
    if((machineInternalState.ota.firmwareSize - machineInternalState.ota.dataWritten) < BUFFSIZE)
    {
        //Read another buffer in case the download has stopped on the last packet (less than BUFFSIZE)
        machineInternalState.ota.dataWritten = (machineInternalState.ota.firmwareSize - machineInternalState.ota.dataWritten);
    }

    uint32_t binary_file_length = machineInternalState.ota.dataWritten;
    uint8_t percent = (machineInternalState.ota.firmwareSize - machineInternalState.ota.dataWritten)/100;
    bool ota_client_initialized = false;

    update_partition = check_partitions(configured_partition, running_partition);

    ESP_LOGI(TAG_OTA_SECOND, "Start download firmware...");

    machineInternalState.ota.otaStatus = DOWNLOADING;
    save_firmware_connectivity(false);

    while(true)
    {
        int data_read = esp_http_client_read(client, ota_write_data, BUFFSIZE);
        if(data_read < 0) 
        {
            ESP_LOGE(TAG_OTA_SECOND, "SSL data read error");
            ota_failure_end(SSL_ERROR, client, true, true);
        } 
        else if(data_read > 0)
        {
            if(false == image_header_was_checked) 
            {
                esp_app_desc_t new_app_info;
                if(data_read > sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t))
                {
                    // check current version with downloading
                    memcpy(&new_app_info, &ota_write_data[sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)], sizeof(esp_app_desc_t));
                    ESP_LOGI(TAG_OTA_SECOND, "New firmware version: %s", new_app_info.version);

                    esp_app_desc_t running_app_info;
                    if(ESP_OK == esp_ota_get_partition_description(running_partition, &running_app_info))
                    {
                        ESP_LOGI(TAG_OTA_SECOND, "Running firmware version: %s", running_app_info.version);
                    }

                    const esp_partition_t* last_invalid_app = esp_ota_get_last_invalid_partition();
                    esp_app_desc_t invalid_app_info;
                    if(ESP_OK == esp_ota_get_partition_description(last_invalid_app, &invalid_app_info))
                    {
                        ESP_LOGI(TAG_OTA_SECOND, "Last invalid firmware version: %s", invalid_app_info.version);
                    }

                    // check current version with last invalid partition
                    if(NULL != last_invalid_app)
                    {
                        if(0 == memcmp(invalid_app_info.version, new_app_info.version, sizeof(new_app_info.version)))
                        {
                            ESP_LOGW(TAG_OTA_SECOND, "New version is the same as invalid version.");
                            ESP_LOGW(TAG_OTA_SECOND, "Previously, there was an attempt to launch the firmware with %s version, but it failed.", invalid_app_info.version);
                            ESP_LOGW(TAG_OTA_SECOND, "The firmware has been rolled back to the previous version.");
                            ota_failure_end(NO_AVAILABLE_PARTITION, client, true, true);
                        }
                    }

                    image_header_was_checked = true;
                } 
                else
                {
                    ESP_LOGE(TAG_OTA_SECOND, "Received package is not fit len");
                    machineInternalState.ota.dataWritten = 0;
                    ota_failure_end(WRONG_LENGTH_RECEIVED, client, true, true);
                }
            }

            if(false == ota_client_initialized)
            {
                err = esp_partition_erase_range(update_partition, machineInternalState.ota.dataWritten, update_partition->size - machineInternalState.ota.dataWritten);
                if(ESP_OK != err)
                    ESP_LOGE(TAG_OTA_SECOND, "Failed to erase partition (%s) at byte %d", esp_err_to_name(err), machineInternalState.ota.dataWritten);
                else
                    ESP_LOGI(TAG_OTA_SECOND, "Update partition erased from byte %d", machineInternalState.ota.dataWritten);

                err = esp_ota_begin_erase(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle, false);

                if(ESP_OK != err)
                {
                    ESP_LOGE(TAG_OTA_SECOND, "esp_ota_begin failed (%s)", esp_err_to_name(err));
                    ota_failure_end(FAILED_INSTALLATION_START, client, true, true);
                }
                else
                {
                    ESP_LOGI(TAG_OTA_SECOND, "esp_ota_begin success");
                }
                ota_client_initialized = true;
            }

            err = esp_ota_write_with_offset(update_handle, (const void *)ota_write_data, BUFFSIZE, machineInternalState.ota.dataWritten);

            if(ESP_OK != err)
            {
                machineInternalState.ota.dataWritten = 0;
                ota_failure_end(FAILED_WRITE_DATA, client, true, true);
            }
            else
            {
                machineInternalState.ota.dataWritten += data_read;
            }
            
            binary_file_length += data_read;
            ESP_LOGD(TAG_OTA_SECOND, "Written image length %d", binary_file_length);
        }
        else // data_read == 0
        {
            /*
            * As esp_http_client_read never returns negative error code, we rely on
            * `errno` to check for underlying transport connectivity closure if any
            */
            if (errno == ECONNRESET || errno == ENOTCONN)
            {
                ESP_LOGE(TAG_OTA_SECOND, "Connection closed, errno = %d", errno);
                ota_failure_end(FAILED_HTTP_CONNECTION, client, true, true);
            }

            if(true == esp_http_client_is_complete_data_received(client))
            {
                ESP_LOGI(TAG_OTA_SECOND, "Connection closed. All data received");
                break;
            }
        }

        uint8_t newPercent = 100*binary_file_length/(machineInternalState.ota.firmwareSize);
        if(percent != newPercent)
        {
            percent = newPercent;
            ESP_LOGI(TAG_OTA_SECOND, "Download progress: %d%c (%d/%d bytes)", (percent > 100 ? 100 : percent), '%', binary_file_length, machineInternalState.ota.firmwareSize);
            save_firmware_connectivity(false);
        }
    }
        
    ESP_LOGI(TAG_OTA_SECOND, "Total Write binary data length: %d", binary_file_length);
    if(true != esp_http_client_is_complete_data_received(client)) 
    {
        ESP_LOGE(TAG_OTA_SECOND, "Error in receiving complete file");
        machineInternalState.ota.dataWritten = 0;
        ota_failure_end(FAILED, client, true, true);
    }

    err = esp_ota_end(update_handle);
    if(ESP_OK != err)
    {
        ESP_LOGE(TAG_OTA_SECOND, "Failed to complete update");
        machineInternalState.ota.dataWritten = 0;
        ota_failure_end(FAILED, client, true, true);
    }
    else
    {
        ESP_LOGI(TAG_OTA_SECOND, "Update done");
    }

    err = esp_ota_set_boot_partition(update_partition);
    if(ESP_OK != err)
    {
        ESP_LOGE(TAG_OTA_SECOND, "esp_ota_set_boot_partition failed (%s)", esp_err_to_name(err));
        machineInternalState.ota.dataWritten = 0;
        ota_failure_end(FAILED, client, true, true);
    }
    else
    {
        ESP_LOGI(TAG_OTA_SECOND, "Boot partition set");
    }


    machineInternalState.ota.otaStatus = SUCCESS;
    memset(machineInternalState.ota.errorFwVer, 0, (MAX_FW_VERSION_LENGTH * sizeof(char)));
    memset(machineInternalState.ota.fwUrl, 0, (MAX_URL_LENGTH * sizeof(char)));
    machineInternalState.ota.currentRetry = 0;
    machineInternalState.ota.dataWritten = 0;
    machineInternalState.ota.firmwareSize = 0;
    save_firmware_connectivity(false);

    setWifiLed(LED_ON, ON_LED_PERIOD, 100);

    firmware_restart(3);
}



/*OTA Task*/
void ota_task(void* pvParameter)
{   
    if(INSTALLING == machineInternalState.ota.otaStatus || DOWNLOADING == machineInternalState.ota.otaStatus)
    {
        ESP_LOGI(TAG_OTA, "Resume a previous OTA update");
        second_step_ota_download();
    }


    ESP_LOGI(TAG_OTA, "Firmware ready for OTA updates");
    EventBits_t bits;
    
    while(WIFI_CONNECTED == machineConnectivity.status)
    {
        bits = xEventGroupWaitBits(xWifiEvents, WIFI_NEW_FIRMWARE_AVAILABLE | WIFI_NEW_OTA_RETRY, pdFALSE, pdFALSE, 1000/portTICK_PERIOD_MS);

        if(bits & WIFI_NEW_FIRMWARE_AVAILABLE)
        {
            xEventGroupClearBits(xWifiEvents, WIFI_NEW_FIRMWARE_AVAILABLE);

            ESP_LOGI(TAG_OTA, "Received new firmware (%s). Starting OTA...", machineInternalState.ota.newFwVer);

            #if (defined(CONFIG_AZURE_ENVIRONMENT_TEST) || defined(CONFIG_AZURE_ENVIRONMENT_PRE_PROD) || defined(CONFIG_AZURE_ENVIRONMENT_PROD))
            first_step_ota_authentication();
            #else
            ESP_LOGI(TAG_OTA, "Using CUSTOM Azure cloud. First step authentication skipped");
            #endif
            second_step_ota_download();
        }

        if(bits & WIFI_NEW_OTA_RETRY)
        {
            xEventGroupClearBits(xWifiEvents, WIFI_NEW_OTA_RETRY);

            ESP_LOGI(TAG_OTA, "Previous OTA failed with reason %s. Retry...", ota_status_string[machineInternalState.ota.otaStatus]);
            second_step_ota_download();
        }
    }
    
    vTaskDelete(nullptr);
}
