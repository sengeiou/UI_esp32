#ifndef UTILS_H_
#define UTILS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "types.h"
#include "freertos/queue.h"

#include "soc/efuse_reg.h"
#include "esp_efuse.h"
#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse_table.h"

#include <sys/time.h>


void get_time_from_sntp(bool& sync);

namespace utils::time
{
    inline int64_t get_timestamp_milliseconds(void)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
    }

    inline int64_t get_timestamp_seconds(void)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec;
    }

    inline int64_t get_uptime_milliseconds(void)
    {
        return esp_timer_get_time()*0.001;
    }

    inline int64_t get_uptime_seconds(void)
    {
        return get_uptime_milliseconds()*0.001;
    }
}

void firmware_restart(uint8_t delay_sec);

void init_queue(void);

#endif // UTILS_H_