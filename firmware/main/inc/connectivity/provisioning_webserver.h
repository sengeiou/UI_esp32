#ifndef PROVISIONING_WEBSERVER_H_
#define PROVISIONING_WEBSERVER_H_

#include "esp_err.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

void startHttpServer(void);
void stopHttpServer(void);


#endif // PROVISIONING_WEBSERVER_H_