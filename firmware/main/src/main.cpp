#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "nvs_flash.h"

#include "thread"
#include "chrono"

#include "utils.h"
#include "types.h"
#include "variables.h"
#include "system_utils.h"
#include "spiffs_utils.h"
#include "esp_task_wdt.h"
#include "gui_task.h"
#include "dbg_task.h"


#include "wifi_task.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/platform.h"


#ifdef ADVANCED_DEBUG
    #define MEMORY_CHECK_TAG LINE_STRING "|" "MEMORY_CHECK"
    #define TAG_SECURITY LINE_STRING "|" "SECURITY"
    #define MAIN_TAG LINE_STRING "|" "MAIN"
#else
    #define MEMORY_CHECK_TAG "MEMORY_CHECK"
    #define TAG_SECURITY "SECURITY"
    #define MAIN_TAG "MAIN"
#endif

#define MAIN_LOOP_PERIOD_MS             100
#define DEVICE_CERT_PARTITION_LABEL 	"device_cert"
#define DEVICE_PRV_KEY_PARTITION_LABEL 	"device_prv_key"

void read_certificates_from_partition(void)
{
    char *cert_data;
    const void *map_ptr;
    spi_flash_mmap_handle_t map_handle;

    /*Get Device Certificate partition infos*/
    const esp_partition_t *partition_device_cert = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, DEVICE_CERT_PARTITION_LABEL);
    assert(partition_device_cert != NULL);
    
    /*Map the cert_partition to data memory*/
    ESP_ERROR_CHECK(esp_partition_mmap(partition_device_cert, 0, partition_device_cert->size, SPI_FLASH_MMAP_DATA, &map_ptr, &map_handle));
    
    /*Read back the written verification data using the mapped memory pointer*/
    cert.device_certificate_pem = (char*) malloc(sizeof(char) * partition_device_cert->size);
    memcpy(cert.device_certificate_pem, map_ptr, partition_device_cert->size);
    char* certAddressEnd = strstr(cert.device_certificate_pem, "-----END CERTIFICATE-----");
    assert(certAddressEnd != NULL);
    *(certAddressEnd + strlen("-----END CERTIFICATE-----")) = '\0';

    /*Unmap mapped memory*/
    spi_flash_munmap(map_handle);
    

    /*Get Device Private Key partition infos*/
    const esp_partition_t *partition_device_prv_key = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, DEVICE_PRV_KEY_PARTITION_LABEL);
    assert(partition_device_prv_key != NULL);
    
    /*Map the cert_partition to data memory*/
    ESP_ERROR_CHECK(esp_partition_mmap(partition_device_prv_key, 0, partition_device_prv_key->size, SPI_FLASH_MMAP_DATA, &map_ptr, &map_handle));
    
    /*Read back the written verification data using the mapped memory pointer*/
    cert.device_private_key_pem = (char*) malloc(sizeof(char) * partition_device_prv_key->size);
    memcpy(cert.device_private_key_pem, map_ptr, partition_device_prv_key->size);
    char* pkAddressEnd = strstr(cert.device_private_key_pem, "-----END RSA PRIVATE KEY-----");
    assert(pkAddressEnd != NULL);
    *(pkAddressEnd + strlen("-----END RSA PRIVATE KEY-----")) = '\0';

    /*Unmap mapped memory*/
    spi_flash_munmap(map_handle);
    
    ESP_LOGD(TAG_SECURITY, "Device Certificate size: %d (%d)", strlen(cert.device_certificate_pem), sizeof(char) * partition_device_cert->size);
    ESP_LOGD(TAG_SECURITY, "Device Private Key size: %d (%d)", strlen(cert.device_private_key_pem), sizeof(char) * partition_device_prv_key->size);

    mbedtls_x509_crt deviceCert;
    mbedtls_x509_crt_init(&deviceCert);
    int ret = mbedtls_x509_crt_parse(&deviceCert, (const unsigned char *) cert.device_certificate_pem, strlen(cert.device_certificate_pem)+1);
    if(ret != 0)
    {
        ESP_LOGE(TAG_SECURITY, "Failed to extract common name (CN) from certificate. Error code: %d", ret);

        cert.device_common_name = (char*) malloc(sizeof(char) * strlen("deviceID"));

        strcpy(cert.device_common_name, "deviceID");
        ESP_LOGW(TAG_SECURITY, "DEFAULT COMMON NAME: %s", cert.device_common_name);
    }
    else
    {
        char tmp[2048];
        mbedtls_x509_dn_gets(tmp, 2048, &deviceCert.issuer);
        ESP_LOGI(TAG_SECURITY, "Issuer: %s", tmp);
        
        mbedtls_x509_dn_gets(tmp, 2048, &deviceCert.subject);
        ESP_LOGD(TAG_SECURITY, "Subject: %s", tmp);
        
        char* subject_common_name = security::get_certificate_field("CN=", tmp);
        cert.device_common_name = (char*) malloc(sizeof(char) * (strlen(subject_common_name)+1));

        strncpy(cert.device_common_name, subject_common_name, strlen(subject_common_name)+1);

        ESP_LOGI(TAG_SECURITY, "Subject Common Name (CN): %s", cert.device_common_name);
    }
}


void memory_check_task(void* p)
{
    UBaseType_t uxHighWaterMark;

    static uint32_t freeheap;
    while(true)
    {
        freeheap = esp_get_free_heap_size();
        if(freeheap > 20000)
            ESP_LOGI(MEMORY_CHECK_TAG, "Free heap memory: %u", freeheap);
        else if(freeheap > 10000)
            ESP_LOGW(MEMORY_CHECK_TAG, "Free heap memory: %u", freeheap);
        else
            ESP_LOGE(MEMORY_CHECK_TAG, "Free heap memory: %u", freeheap);

        if(nullptr != xHandleWiFi)
            ESP_LOGD(MEMORY_CHECK_TAG, "%s -> %d", "wifi_task", uxTaskGetStackHighWaterMark(xHandleWiFi));
        
        if(nullptr != xHandleAzure)
            ESP_LOGD(MEMORY_CHECK_TAG, "%s -> %d", "azure_task", uxTaskGetStackHighWaterMark(xHandleAzure));
        
        if(nullptr != xHandleAzureTx)
            ESP_LOGD(MEMORY_CHECK_TAG, "%s -> %d", "azure_tx_task", uxTaskGetStackHighWaterMark(xHandleAzureTx));

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

extern "C" void esp_task_wdt_isr_user_handler(void)
{
    ESP_LOGE(MAIN_TAG, "Failed to reset watchdog. Rebooting");
    firmware_restart(0);
}

void init_firmware()
{
    ESP_LOGI(MAIN_TAG, "Firmware version: %s", CURRENT_VERSION_FIRMWARE);

    utils::system::flash_encryption_status();
    nvs_flash_init();

    xModuleEvents = xEventGroupCreate();
    xGuiEvents = xEventGroupCreate();
    xWifiEvents = xEventGroupCreate();
    
    /*Read Certificates from Partition*/
    #if (CONFIG_AZURE_AUTHENTICATION_METHOD_X509 == 1 || CONFIG_AZURE_AUTHENTICATION_METHOD_SHARED_ACCESS_KEY == 1)
    read_certificates_from_partition();
    #endif
    
    init_queue();
    init_spiffs_memory();
    init_gpio();
    esp_task_wdt_init(60, false); //Init watchdog
}

void start_tasks()
{
    // utils::system::start_thread(&memory_check_task, nullptr, "memory_task", 1024*4, 4);

    utils::system::start_thread_core(&dbg_task, &xHandleDbg, "dbg_task", 1024*4, 4, 0);
    utils::system::start_thread_core(&gui_task, &xHandleGui, "gui_task", 1024*32, 4, 1);

    if(true == machineConnectivity.wifiEnabled)
        utils::system::start_thread_core(&wifi_task, &xHandleWiFi, "wifi_task", 1024*8, 4, 1);
}

void main_task(void* p)
{
    if(ESP_OK != esp_task_wdt_add(nullptr))
        ESP_LOGE(MAIN_TAG, "Failed to add watchdog to Main Task");

    EventBits_t uxBits;
    while(true)
    {
        uxBits = xEventGroupWaitBits(xModuleEvents, EVENT_INTERACTION_CLOUD, pdFALSE, pdFALSE, MAIN_LOOP_PERIOD_MS/portTICK_PERIOD_MS);

        if(EVENT_INTERACTION_CLOUD & uxBits)
        {
            xEventGroupClearBits(xModuleEvents, EVENT_INTERACTION_CLOUD);
            
            switch(machineInternalState.azureRequest.command)
            {
                case POWER_ON:
                {
                    if(false == guiInternalState.powerOn)
                    {
                        special_function(DBG_ON_OFF);
                    }
                    break;
                }
                case POWER_OFF:
                {
                    if(true == guiInternalState.powerOn)
                    {
                        special_function(DBG_ON_OFF);
                    }
                    break;
                }
                case MAKE_COFFEE:
                {
                    switch(machineInternalState.azureRequest.desiredPreparation)
                    {
                        case COFFEE_SHORT:
                        {
                            guiInternalState.cloudReq.coffeeType = COFFEE_SHORT;
                            xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            break;
                        }
                        case COFFEE_MEDIUM:
                        {
                            guiInternalState.cloudReq.coffeeType = COFFEE_MEDIUM;
                            xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            break;
                        }
                        case COFFEE_LONG:
                        {
                            guiInternalState.cloudReq.coffeeType = COFFEE_LONG;
                            xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            break;
                        }
                        case CAPPUCCINO_SHORT:
                        {
                            if(true == guiInternalState.milkHeadPresence)
                            {
                                guiInternalState.cloudReq.coffeeType = CAPPUCCINO_SHORT;
                                xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            }
                            break;
                        }
                        case CAPPUCCINO_MEDIUM:
                        {
                            if(true == guiInternalState.milkHeadPresence)
                            {
                                guiInternalState.cloudReq.coffeeType = CAPPUCCINO_MEDIUM;
                                xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            }
                            break;
                        }
                        case CAPPUCCINO_DOUBLE:
                        {
                            if(true == guiInternalState.milkHeadPresence)
                            {
                                guiInternalState.cloudReq.coffeeType = CAPPUCCINO_DOUBLE;
                                xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            }
                            break;
                        }
                        case HOT_MILK:
                        {
                            if(true == guiInternalState.milkHeadPresence)
                            {
                                guiInternalState.cloudReq.coffeeType = HOT_MILK;
                                xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            }
                            break;
                        }
                        case HOT_WATER:
                        {
                            guiInternalState.cloudReq.coffeeType = HOT_WATER;
                            xEventGroupSetBits(xGuiEvents, GUI_CLOUD_REQUEST_BIT);
                            break;
                        }
                        default:
                        {
                            //Do nothing
                            break;
                        }
                    }
                    break;
                }
                case FIRMWARE_UPDATE:
                {
                    //To be done
                    break;
                }
                default:
                {
                    //Do nothing
                    break;
                }
            }
        }

        if(ESP_OK != esp_task_wdt_reset())
        {
            ESP_LOGE(MAIN_TAG, "Failed to reset watchdog");
        }

        ESP_LOGV(MAIN_TAG, "Main task running...");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}


extern "C" void app_main(void)
{   
    init_firmware();

    start_tasks();

    utils::system::start_thread_core(&main_task, &xHandleMainTask, "main_task", 1024*6, 5, 0);

    vTaskDelete(nullptr);
}