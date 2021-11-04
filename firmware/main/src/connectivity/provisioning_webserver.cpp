#include "provisioning_webserver.h"
#include "utils.h"
#include "spiffs_utils.h"

#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "mdns.h"
#include "lwip/apps/netbiosns.h"
#include "variables.h"
#include "gui_task.h"

#include "system_utils.h"
#include "sdkconfig.h"

#ifdef ADVANCED_DEBUG
    #define TAG_WS LINE_STRING "|" "AP WebServer"
    #define TAG_WS_WS LINE_STRING "|" "AP WebServer WS"
#else
    #define TAG_WS "AP WebServer"
    #define TAG_WS_WS "AP WebServer WS"
#endif

static httpd_handle_t httpServerInstance = NULL;


extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern const uint8_t menu_html_start[] asm("_binary_menu_html_start");
extern const uint8_t menu_html_end[] asm("_binary_menu_html_end");

static httpd_handle_t wsHand;
static int fd;

/* An HTTP GET handler */
static esp_err_t rootGetHandler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t menuGetHandler(httpd_req_t* req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)menu_html_start, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* An HTTP POST handler */
static esp_err_t provPostHandler(httpd_req_t* req)
{
    char buf[1024];
    int ret, remaining = req->content_len;

    static bool received = false;
    while(remaining > 0)
    {
        /* Read the data for the request */
        int data_to_rec = (remaining < sizeof(buf)) ? remaining : sizeof(buf);
        if((ret = httpd_req_recv(req, buf, data_to_rec)) <= 0) 
        {
            if(ret == HTTPD_SOCK_ERR_TIMEOUT) 
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        //TODO Handle multiple request
        if(false == received)
        {
            ESP_LOGI(TAG_WS, "=========== RECEIVED DATA ==========");
            ESP_LOGI(TAG_WS, "REQUEST %.*s", ret, buf);
            strncpy((char*) machineConnectivity.ssid, utils::json::getValue<std::string>(buf, "network").c_str(), 64);
            strncpy((char*) machineConnectivity.password, utils::json::getValue<std::string>(buf, "password").c_str(), 64);
            if(machineConnectivity.ssid != nullptr)
            {
                ESP_LOGI(TAG_WS, "Provisioning done. New network %s with password %s", machineConnectivity.ssid, machineConnectivity.password);
                xEventGroupSetBits(xWifiEvents, WIFI_CONFIGURATION_BIT);
            }
            else
            {
                ESP_LOGE(TAG_WS, "Invalid network (%s) received", machineConnectivity.ssid);

                memset(machineConnectivity.ssid, 0, 64);
                memset(machineConnectivity.password, 0, 64);
            }
            ESP_LOGI(TAG_WS, "====================================");

            received = true;
        }
    }

    return ESP_OK;
}

static esp_err_t wsHandler(httpd_req_t* req)
{
    if(req->method == HTTP_GET)
    {
        ESP_LOGI(TAG_WS_WS, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    wsHand = req->handle;
    fd = httpd_req_to_sockfd(req);

    //printf("Handler %d (%d), fd %d\n", wsHand, httpServerInstance, fd);

    httpd_ws_frame_t ws_pkt;
    uint8_t* buf = (uint8_t*)calloc(1, 1024);
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    ws_pkt.payload = buf;

    httpd_ws_recv_frame(req, &ws_pkt, 1024);
    ESP_LOGI(TAG_WS_WS, "Packet type: %d", ws_pkt.type);
    ESP_LOGI(TAG_WS_WS, "Frame len is %d", ws_pkt.len);
    ESP_LOGI(TAG_WS_WS, "Packet message: %s", ws_pkt.payload);

    std::string json = std::string((char*)ws_pkt.payload);
    std::string preparationString = utils::json::getValue<std::string>(json, "type");

    if(preparationString == "short")
    {
        machineInternalState.wsRequest.command = MAKE_COFFEE;
        machineInternalState.wsRequest.desiredPreparation = COFFEE_SHORT;
    }
    
    if(preparationString == "long")
    {
        machineInternalState.wsRequest.command = MAKE_COFFEE;
        machineInternalState.wsRequest.desiredPreparation = COFFEE_LONG;
    }
    
    if(preparationString == "powerOn")
    {
        machineInternalState.wsRequest.command = POWER_ON;
        machineInternalState.wsRequest.desiredPreparation = COFFEE_NONE;
    }
    
    if(preparationString == "powerOff")
    {
        machineInternalState.wsRequest.command = POWER_OFF;
        machineInternalState.wsRequest.desiredPreparation = COFFEE_NONE;
    }

    // xEventGroupSetBits(xModuleEvents, EVENT_INTERACTION_LOCAL);
    free(buf);

    return ESP_OK;
}

void ws_task(void* tmp)
{
    std::string data;
    uint8_t status, oldStatus;
    int8_t oldPercent = -1;
    int8_t percent = 0;
    while(true)
    {
        // status = machineInternalState.fsmStatus;
        //percent = (machineInternalState.flowmeterSetPoint != 0) ? 100*machineInternalState.flowmeter/machineInternalState.flowmeterSetPoint : machineInternalState.flowmeter;
        bool toNotify = ((percent != oldPercent) || (status != oldStatus));


        // printf("%d %d, %d %d, %d\n", status, oldStatus, percent, oldPercent, toNotify);
        if(true == toNotify)
        {
            data.clear();
            httpd_ws_frame_t ws_pkt;
            memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
            ws_pkt.type = HTTPD_WS_TYPE_TEXT;
            ws_pkt.final = true;

            data = "{\"status\":\"";
            data += "OK";
            data += "\", \"short\":";
            if(machineInternalState.desiredPreparation == COFFEE_SHORT)
                data += std::to_string(percent);
            else
                data += std::to_string(0);
            data += ", \"long\":";
            if(machineInternalState.desiredPreparation == COFFEE_LONG)
                data += std::to_string(percent);
            else
                data += std::to_string(0);
            data += ", \"free\":";
            data += std::to_string(0);
            data += "}";
            ws_pkt.payload = (uint8_t*) data.c_str();
            ws_pkt.len = strlen(data.c_str());

            httpd_ws_send_frame_async(wsHand, fd, &ws_pkt);

            // printf("SENDING: %s\n", data.c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        oldPercent = percent;
        oldStatus = status;
    }
}

void startHttpServer(void)
{
    httpd_config_t httpServerConfiguration = HTTPD_DEFAULT_CONFIG();

    if(ESP_OK == httpd_start(&httpServerInstance, &httpServerConfiguration))
    {
        ESP_LOGI(TAG_WS, "Start http server");
        httpd_uri_t rootUri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = rootGetHandler
        };
        httpd_uri_t credentialUri = {
            .uri = "/credentials",
            .method = HTTP_POST,
            .handler = provPostHandler
        };
        httpd_uri_t menuUri = {
            .uri = "/menu",
            .method = HTTP_GET,
            .handler = menuGetHandler
        };
        httpd_uri_t wsUri = {
            .uri        = "/menuws",
            .method     = HTTP_GET,
            .handler    = wsHandler,
            .user_ctx   = NULL,
            .is_websocket = true
        };
       
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &rootUri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &credentialUri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &menuUri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &wsUri));

        utils::system::start_thread_core(&ws_task, nullptr, "ws_task", 1024*6, 5, 1);
    }
}
 
void stopHttpServer(void)
{
    if(nullptr != httpServerInstance)
    {
        ESP_ERROR_CHECK(httpd_stop(httpServerInstance));
        httpServerInstance = nullptr;
    }
}













// /* Send HTTP response with the contents of the requested file */
// static esp_err_t rest_common_get_handler(httpd_req_t* req)
// {
//     char filepath[FILE_PATH_MAX];

//     rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
//     strlcpy(filepath, rest_context->base_path, sizeof(filepath));
//     if (req->uri[strlen(req->uri) - 1] == '/') {
//         strlcat(filepath, "/index.html", sizeof(filepath));
//     } else {
//         strlcat(filepath, req->uri, sizeof(filepath));
//     }
//     int fd = open(filepath, O_RDONLY, 0);
//     if (fd == -1) {
//         ESP_LOGE(TAG_WS, "Failed to open file : %s", filepath);
//         /* Respond with 500 Internal Server Error */
//         httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
//         return ESP_FAIL;
//     }

//     set_content_type_from_file(req, filepath);

//     char *chunk = rest_context->scratch;
//     ssize_t read_bytes;
//     do {
//         /* Read file in chunks into the scratch buffer */
//         read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
//         if (read_bytes == -1) {
//             ESP_LOGE(TAG_WS, "Failed to read file : %s", filepath);
//         } else if (read_bytes > 0) {
//             /* Send the buffer contents as HTTP response chunk */
//             if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
//                 close(fd);
//                 ESP_LOGE(TAG_WS, "File sending failed!");
//                 /* Abort sending file */
//                 httpd_resp_sendstr_chunk(req, NULL);
//                 /* Respond with 500 Internal Server Error */
//                 httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
//                 return ESP_FAIL;
//             }
//         }
//     } while (read_bytes > 0);
//     /* Close file after sending complete */
//     close(fd);
//     ESP_LOGI(TAG_WS, "File sending complete");
//     /* Respond with an empty chunk to signal HTTP response completion */
//     httpd_resp_send_chunk(req, NULL, 0);
//     return ESP_OK;
// }

// /* Simple handler for light brightness control */
// static esp_err_t light_brightness_post_handler(httpd_req_t *req)
// {
//     int total_len = req->content_len;
//     int cur_len = 0;
//     char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
//     int received = 0;
//     if (total_len >= SCRATCH_BUFSIZE) {
//         /* Respond with 500 Internal Server Error */
//         httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
//         return ESP_FAIL;
//     }
//     while (cur_len < total_len) {
//         received = httpd_req_recv(req, buf + cur_len, total_len);
//         if (received <= 0) {
//             /* Respond with 500 Internal Server Error */
//             httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
//             return ESP_FAIL;
//         }
//         cur_len += received;
//     }
//     buf[total_len] = '\0';

//     printf("REC POST BUFFER %s\n", buf);
//     cJSON *root = cJSON_Parse(buf);
//     int red = cJSON_GetObjectItem(root, "red")->valueint;
//     int green = cJSON_GetObjectItem(root, "green")->valueint;
//     int blue = cJSON_GetObjectItem(root, "blue")->valueint;
//     ESP_LOGI(TAG_WS, "Light control: red = %d, green = %d, blue = %d", red, green, blue);
//     cJSON_Delete(root);
//     httpd_resp_sendstr(req, "Post control value successfully");
//     return ESP_OK;
// }

// /* Simple handler for getting system handler */
// static esp_err_t system_info_get_handler(httpd_req_t *req)
// {
//     httpd_resp_set_type(req, "application/json");
//     cJSON *root = cJSON_CreateObject();
//     esp_chip_info_t chip_info;
//     esp_chip_info(&chip_info);
//     cJSON_AddStringToObject(root, "version", IDF_VER);
//     cJSON_AddNumberToObject(root, "cores", chip_info.cores);
//     const char *sys_info = cJSON_Print(root);
//     httpd_resp_sendstr(req, sys_info);
//     free((void *)sys_info);
//     cJSON_Delete(root);
//     return ESP_OK;
// }

// /* Simple handler for getting temperature data */
// static esp_err_t temperature_data_get_handler(httpd_req_t *req)
// {
//     httpd_resp_set_type(req, "application/json");
//     cJSON *root = cJSON_CreateObject();
//     cJSON_AddNumberToObject(root, "raw", esp_random() % 20);
//     const char *sys_info = cJSON_Print(root);
//     httpd_resp_sendstr(req, sys_info);
//     free((void *)sys_info);
//     cJSON_Delete(root);
//     return ESP_OK;
// }

// esp_err_t start_rest_server(const char *base_path)
// {
//     rest_server_context_t *rest_context = (rest_server_context_t*)calloc(1, sizeof(rest_server_context_t));
//     strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

//     httpd_handle_t server = NULL;
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();
//     config.uri_match_fn = httpd_uri_match_wildcard;

//     ESP_LOGI(TAG_WS, "Starting HTTP Server");
//     httpd_start(&server, &config);

//     /* URI handler for fetching system info */
//     httpd_uri_t system_info_get_uri = {
//         .uri = "/api/v1/system/info",
//         .method = HTTP_GET,
//         .handler = system_info_get_handler,
//         .user_ctx = rest_context
//     };
//     httpd_register_uri_handler(server, &system_info_get_uri);

//     /* URI handler for fetching temperature data */
//     httpd_uri_t temperature_data_get_uri = {
//         .uri = "/api/v1/temp/raw",
//         .method = HTTP_GET,
//         .handler = temperature_data_get_handler,
//         .user_ctx = rest_context
//     };
//     httpd_register_uri_handler(server, &temperature_data_get_uri);

//     /* URI handler for light brightness control */
//     httpd_uri_t light_brightness_post_uri = {
//         .uri = "/api/v1/light/brightness",
//         .method = HTTP_POST,
//         .handler = light_brightness_post_handler,
//         .user_ctx = rest_context
//     };
//     httpd_register_uri_handler(server, &light_brightness_post_uri);

//     /* URI handler for getting web server files */
//     httpd_uri_t common_get_uri = {
//         .uri = "/*",
//         .method = HTTP_GET,
//         .handler = rest_common_get_handler,
//         .user_ctx = rest_context
//     };
//     httpd_register_uri_handler(server, &common_get_uri);

//     return ESP_OK;
// err_start:
//     free(rest_context);
// err:
//     return ESP_FAIL;
// }

