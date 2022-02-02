#ifndef SYSTEM_UTILS_H_
#define SYSTEM_UTILS_H_

#include <esp_log.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <esp_pthread.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"

#if CONFIG_IDF_TARGET_ESP32 == 1
#include "soc/efuse_reg.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#elif CONFIG_IDF_TARGET_ESP32S2 == 1

#elif CONFIG_IDF_TARGET_ESP32S3 == 1

#endif


#include "types.h"


#ifdef ADVANCED_DEBUG
    #define SYSTEM_UTILS_TAG LINE_STRING "|" "SYSTEM_UTILS"
#else
    #define SYSTEM_UTILS_TAG "SYSTEM_UTILS"
#endif


namespace utils::system
{
    inline void print_thread_info(TaskHandle_t handler = nullptr)
    {
        std::stringstream ss;

        ss << "Core id: " << xPortGetCoreID()
        << ", prio: " << uxTaskPriorityGet(handler)
        << ", minimum free stack: " << uxTaskGetStackHighWaterMark(handler) << " bytes";
        ESP_LOGI(SYSTEM_UTILS_TAG, "%s", ss.str().c_str());
    }

    inline esp_err_t start_thread(TaskFunction_t function, TaskHandle_t* handler, const char* name, int stack, int prio, void* params = nullptr)
    {
        esp_err_t ret_val = xTaskCreate(function, name, stack, params, (tskIDLE_PRIORITY + prio), handler);

        if(pdFAIL == ret_val)
            ESP_LOGE(SYSTEM_UTILS_TAG, "Unable allocate memory for %s", name);
        else
            ESP_LOGI(SYSTEM_UTILS_TAG, "%s correctly started", name);

        return ret_val;
    }

    inline esp_err_t start_thread_core(TaskFunction_t function, TaskHandle_t* handler, const char* name, int stack, int prio, int core_id, void* params = nullptr)
    {
        esp_err_t ret_val = xTaskCreatePinnedToCore(function, name, stack, params, (tskIDLE_PRIORITY + prio), handler, core_id);

        if(pdFAIL == ret_val)
            ESP_LOGE(SYSTEM_UTILS_TAG, "Unable allocate memory for %s", name);
        else
            ESP_LOGI(SYSTEM_UTILS_TAG, "%s correctly started", name);

        return ret_val;
    }

    inline void print_task_stack_size(const char* TAG)
    {
        ESP_LOGI(TAG, "task stack: %d", uxTaskGetStackHighWaterMark(NULL));
    }

    inline void print_heap_size(const char* TAG)
    {
        ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
    }

    inline void print_memory(const char* TAG)
    {
        print_heap_size(TAG);
        print_task_stack_size(TAG);
    }

    void read_mac_address(void);

    void flash_encryption_status(void);
}

namespace security
{
    char* get_certificate_field(const char* field, const char* buffer);
}

#endif // SYSTEM_UTILS_H_