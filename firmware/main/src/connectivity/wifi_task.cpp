#include "wifi_task.h"
#include "utils.h"
#include "spiffs_utils.h"

#include <string>
#include <thread>
#include <chrono>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "provisioning_webserver.h"
#include "sdkconfig.h"
#include "variables.h"
#include "system_utils.h"
#include "azure_task.h"
#include "ota.h"
#include "gui_task.h"


#ifdef ADVANCED_DEBUG
    #define TAG_WIFI LINE_STRING "|" "WiFi Task"
#else
    #define TAG_WIFI "WiFi Task"
#endif

#define WIFI_CONNECTION_REFRESH_PERIOD_MS   1000

static esp_netif_t* netifCfg;
bool syncTime;

/*WiFi event handler*/
static void wifi_station_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
        ESP_LOGI(TAG_WIFI, "WIFI station start");
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP) 
    {
        ESP_LOGI(TAG_WIFI, "WIFI station stop");
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) 
    {
        syncTime = true;
        ESP_LOGI(TAG_WIFI, "WIFI station connected");
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        syncTime = false;
        ESP_LOGI(TAG_WIFI, "WIFI station disconnected");

        if(WIFI_NOT_CONFIGURED != machineConnectivity.status && true == machineConnectivity.wifiEnabled)
        {
            esp_wifi_connect();
            ESP_LOGI(TAG_WIFI, "WIFI is enabled and configured. Retry to connect to the network %s...", machineConnectivity.ssid);
        }
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Got ip: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(xWifiEvents, WIFI_CONNECTED_BIT);
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "WIFI station lost ip");
        xEventGroupSetBits(xWifiEvents, WIFI_DISCONNECTED_BIT);
    }    
}

static void wifi_ap_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{   
    if(event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Device "MACSTR" connected, AID=%d", MAC2STR(event->mac), event->aid);
    }
    else if(event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Device "MACSTR" disconnected, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

/*WiFi init function*/
void wifi_init(void)
{
    switch(machineConnectivity.status)
    {
        case WIFI_NOT_CONFIGURED:
        {
            setWifiLed(LED_BLINKING, WIFI_AP_LED_PERIOD, 20);
            wifi_ap_init();
            startHttpServer();
            ESP_LOGI(TAG_WIFI, "WiFi not provisioned. Stating in AccessPoint mode");
            break;
        }
        case WIFI_CONFIGURED:
        case WIFI_CONNECTED:
        case WIFI_LAVAZZA_CONNECTED:
        {
            setWifiLed(LED_BLINKING, WIFI_STA_CONNECTION_LED_PERIOD, 50);
            stopHttpServer();
            wifi_sta_init();
            ESP_LOGI(TAG_WIFI, "WiFi provisioned. Stating in Station mode. Trying to connect to network: %s with password %s", machineConnectivity.ssid, machineConnectivity.password);

            esp_wifi_connect();
            break;
        }
        default:
        {
            //Do nothing
            break;
        }
    }

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_station_event_handler, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_station_event_handler, nullptr));

    ESP_LOGI(TAG_WIFI, "WIFI initialized");
}

void wifi_deinit(void)
{
    stopHttpServer();
    
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_station_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_station_event_handler));

    esp_wifi_disconnect();

    ESP_ERROR_CHECK(esp_wifi_stop());

    ESP_ERROR_CHECK(esp_wifi_deinit());

    if(nullptr != netifCfg)
    {
        esp_wifi_clear_default_wifi_driver_and_handlers(netifCfg);
        esp_netif_destroy(netifCfg);
        netifCfg = nullptr;
    }

    ESP_LOGI(TAG_WIFI, "WIFI de-initialized");
}

void wifi_ap_init(void)
{
    netifCfg = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_ap_config = {};
    char hotspotName[24] = "NewRangeOCS_UI";
    strncpy((char*)wifi_ap_config.ap.ssid, hotspotName, 15);
    wifi_ap_config.ap.ssid_len = 15;
    char hotspotPwd[] = CONFIG_WIFI_AP_PASSWORD;
    strncpy((char*)wifi_ap_config.ap.password, hotspotPwd, 32);
    wifi_ap_config.ap.max_connection = 5;
    wifi_ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_ap_config.ap.channel = 1;
    wifi_ap_config.ap.ssid_hidden = false;
    ESP_LOGI(TAG_WIFI, "AP created. SSID %.15s, password %s", hotspotName, hotspotPwd);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_WIFI, "WIFI AccessPoint initialized");
}

void wifi_sta_init(void)
{
    netifCfg = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_sta_config = {};
    strncpy((char*)wifi_sta_config.sta.ssid, machineConnectivity.ssid, 64);
    strncpy((char*)wifi_sta_config.sta.password, machineConnectivity.password, 64);
    wifi_sta_config.sta.bssid_set = false;
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_sta_config.sta.pmf_cfg.capable = true;
    wifi_sta_config.sta.pmf_cfg.required = false;
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_WIFI, "WIFI STATION initialized");
}

void wifi_sta_ap_init(bool hidden)
{
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_sta_config = {};
    strncpy((char*)wifi_sta_config.sta.ssid, machineConnectivity.ssid, 64);
    strncpy((char*)wifi_sta_config.sta.password, machineConnectivity.password, 64);
    wifi_sta_config.sta.bssid_set = false;
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_sta_config.sta.pmf_cfg.capable = true;
    wifi_sta_config.sta.pmf_cfg.required = false;

    wifi_config_t wifi_ap_config = {};
    char hotspotName[24] = "NewRangeOCS_UI";
    strncpy((char*)wifi_ap_config.ap.ssid, hotspotName, 15);
    wifi_ap_config.ap.ssid_len = 15;
    char hotspotPwd[] = CONFIG_WIFI_AP_PASSWORD;
    strncpy((char*)wifi_ap_config.ap.password, hotspotPwd, 32);
    wifi_ap_config.ap.max_connection = 5;
    wifi_ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_ap_config.ap.channel = 1;
    wifi_ap_config.ap.ssid_hidden = (uint8_t)hidden;
    ESP_LOGI(TAG_WIFI, "AP created. SSID %.15s, password %s, hidden %s", hotspotName, hotspotPwd, hidden ? "ON" : "OFF");

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_WIFI, "WIFI station and AP initialized");
}


/*Main WiFi Task*/
void wifi_task(void *pvParameter)
{
    if(nullptr != netifCfg)
    {
        esp_wifi_clear_default_wifi_driver_and_handlers(netifCfg);
        esp_netif_destroy(netifCfg);
        netifCfg = nullptr;
    }

    machineConnectivity.wifiEnabled = true;
    
    if(true == machineConnectivity.wifiEnabled)
    {
        EventBits_t bits;

        esp_err_t err = nvs_flash_init();
        if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);

        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        wifi_init();
    
        while(true == machineConnectivity.wifiEnabled)
        {
            bits = xEventGroupWaitBits(xWifiEvents, WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT | WIFI_CONFIGURATION_BIT, pdFALSE, pdFALSE, WIFI_CONNECTION_REFRESH_PERIOD_MS/portTICK_PERIOD_MS);

            if(bits & WIFI_CONNECTED_BIT)
            {
                xEventGroupClearBits(xWifiEvents, WIFI_CONNECTED_BIT);
                machineConnectivity.status = WIFI_CONNECTED;
                ESP_LOGI(TAG_WIFI, "Connected to ap SSID:%s password:%s", machineConnectivity.ssid, machineConnectivity.password);

                setWifiLed(LED_BLINKING, WIFI_AZURE_CONNECTION_LED_PERIOD, 50);

                get_time_from_sntp(syncTime);

                if(true == syncTime)
                {
                    utils::system::start_thread_core(&azure_task, &xHandleAzure, "azure_task", 1024*16, 4, 1);
                    utils::system::start_thread_core(&ota_task, &xHandleOta, "ota_task", 1024*8, 4, 1);
                }
            }

            if(bits & WIFI_DISCONNECTED_BIT)
            {
                xEventGroupClearBits(xWifiEvents, WIFI_DISCONNECTED_BIT);
                ESP_LOGI(TAG_WIFI, "Disconnected from ap SSID:%s password:%s", machineConnectivity.ssid, machineConnectivity.password);
                
                azure_deinit();
            
                if(nullptr != xHandleOta)
                    xHandleOta = nullptr;
            }

            if(bits & WIFI_CONFIGURATION_BIT)
            {
                xEventGroupClearBits(xWifiEvents, WIFI_CONFIGURATION_BIT);
                machineConnectivity.status = WIFI_CONFIGURED;
                save_firmware_connectivity(true);

                wifi_deinit();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                wifi_init();
            }
        }

        azure_deinit();
            
        if(nullptr != xHandleOta)
            xHandleOta = nullptr;

        esp_wifi_disconnect();
        wifi_deinit();

        ESP_ERROR_CHECK(esp_event_loop_delete_default());

        ESP_LOGI(TAG_WIFI, "Wifi stopped");
    }
    else
    {
        ESP_LOGI(TAG_WIFI, "Wifi disabled");
    }

    setWifiLed(LED_OFF, OFF_LED_PERIOD, 100);
    vTaskDelete(nullptr);
}
