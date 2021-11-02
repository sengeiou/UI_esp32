#include "system_utils.h"

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "types.h"

#include <string>
#include <cstring>


void utils::system::read_mac_address(void)
{
    uint8_t mac[6];
    if(ESP_OK == esp_read_mac(mac, ESP_MAC_WIFI_STA))
    {
        ESP_LOGI(SYSTEM_UTILS_TAG, "Wifi MAC address: %02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else
    {
        ESP_LOGE(SYSTEM_UTILS_TAG, "Failed to get WiFi MAC address");
    }

    if(ESP_OK == esp_read_mac(mac, ESP_MAC_BT))
    {
        ESP_LOGI(SYSTEM_UTILS_TAG, "Bluetooth MAC address: %02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else
    {
        ESP_LOGE(SYSTEM_UTILS_TAG, "Failed to get Bluetooth MAC address");
    }
}

void utils::system::flash_encryption_status(void)
{
    uint32_t flash_crypt_cnt = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_FLASH_CRYPT_CNT, &flash_crypt_cnt, 7);
    ESP_LOGI(SYSTEM_UTILS_TAG, "FLASH_CRYPT_CNT eFuse value is %d", flash_crypt_cnt);

    esp_flash_enc_mode_t mode = esp_get_flash_encryption_mode();
    if (mode == ESP_FLASH_ENC_MODE_DISABLED) 
    {
        ESP_LOGI(SYSTEM_UTILS_TAG, "Flash encryption feature is disabled");
    } 
    else 
    {
        ESP_LOGI(SYSTEM_UTILS_TAG, "Flash encryption feature is enabled in %s mode", mode == ESP_FLASH_ENC_MODE_DEVELOPMENT ? "DEVELOPMENT" : "RELEASE");
    }
}

void remove_all_occurrence(char *str, char ch)
{
	int i, j, len;
	
	len = strlen(str);
	
	for(i = 0; i < len; i++)
	{
		if(str[i] == ch)
		{
			for(j = i; j < len; j++)
			{
				str[j] = str[j + 1];
			}
			len--;
			i--;	
		} 
	}	
}


char* security::get_certificate_field(const char* field, const char* buffer)
{
    char* return_field = strstr(buffer, field);
    return_field += 2;
    int i = 0;
    while(*return_field != 0x00)
    {
        return_field++;
        i++;
    }
    *return_field = '\0';
    return_field -= (i-1);
    remove_all_occurrence(return_field, '?');

    return return_field;
}