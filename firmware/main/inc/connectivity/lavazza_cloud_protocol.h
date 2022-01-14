#include "utils.h"
#include "converter_utils.h"
#include "spiffs_utils.h"

#include <math.h> 
#include "esp_log.h"

#ifdef ADVANCED_DEBUG
    #define CLOUD_PROTOCOL_TAG LINE_STRING "|" "CLOUD_PROTOCOL"
#else
    #define CLOUD_PROTOCOL_TAG "CLOUD_PROTOCOL"
#endif


#define UUID_STR_LEN 37
typedef uint8_t uuid_t[16];


typedef enum {
    COMMAND_REQUEST_MSG = 1,
    COMMAND_RESPONSE_MSG = 2,
    TELEMETRY_MSG = 3,
    ALERT_MSG = 4,
    CA_PINNING = 5
}cloud_msg_type;


typedef enum  {
    REQUEST_MACHINE_STATE_COMMAND = 1010,
    CA_PINNING_REQUEST  = 1030,
    REQUEST_MACHINE_STATE_RESPONSE = 2010,
    CA_PINNING_RESPONSE  = 2030,
    DESCALING_TELEMETRY = 3020,
    COFFEE_TELEMETRY = 3030,
    ALERT_LIVEDATA = 4030,
    OTA_ALERT = 4050
}cloud_payload_type;

typedef struct
{
    cloud_msg_type msgType;
    cloud_payload_type payloadType;
    char correlationId[MAX_UUID_LENGTH];
}LavazzaMessage;

/*Typedef for configuration*/
typedef struct
{
    /*------- FW update Parameters -------*/
    char fwTMVer[MAX_FW_VERSION_LENGTH]; 
    char fwMBVer[MAX_FW_VERSION_LENGTH];
} Configuration;

inline void create_device_id(char* deviceId)
{
    strcpy(deviceId, "18000USA_LAVAZZATESTUSA000003");
}

inline void uuid_generate(char* out)
{
    esp_fill_random(out, sizeof(uuid_t));

    /* uuid version */
    out[6] = 0x40 | (out[6] & 0xF);

    /* uuid variant */
    out[8] = (0x80 | out[8]) & ~0x40;
}

inline char* lavazza_protocol_message(cloud_msg_type msgType, cloud_payload_type payloadType, const char* corrId, JSON_Value* payload, JSON_Value* propertyBag)
{
    char* result;

    char deviceId[100];
    create_device_id(deviceId);

    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_dotset_number(root_object, "timestamp", utils::time::get_timestamp_seconds());
    json_object_dotset_string(root_object, "deviceId", deviceId);

    char uuid[UUID_STR_LEN];
    uuid_generate(uuid);
    json_object_dotset_string(root_object, "msgId", uuid);

    if(0 != msgType)
        json_object_dotset_number(root_object, "msgType", (int)msgType);
    
    if(0 != payloadType)
        json_object_dotset_number(root_object, "payloadType", (int)payloadType);

    if(NULL != corrId)
        json_object_dotset_string(root_object, "correlationId", corrId);
    else
        json_object_dotset_null(root_object, "correlationId");

    if(NULL != payload)
        json_object_dotset_value(root_object, "payload", payload);
    else
        json_object_dotset_null(root_object, "payload");
        
    if(NULL != propertyBag)
        json_object_dotset_value(root_object, "propertyBag", propertyBag);
    else
        json_object_dotset_null(root_object, "propertyBag");

    result = json_serialize_to_string(root_value);
    json_value_free(root_value);

    ESP_LOGI(CLOUD_PROTOCOL_TAG, "Sending message: %s", result);
    return result;
}

inline char* configuration2json(Configuration* config, bool complete)
{
    char* result;

    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    // Only reported properties:
    if ('\0' != config->fwTMVer[0])
        json_object_dotset_string(root_object, "fwTMVer", config->fwTMVer);

    if ('\0' != config->fwMBVer[0])
        json_object_dotset_string(root_object, "fwMBVer", config->fwMBVer);

    result = json_serialize_to_string(root_value);

    json_value_free(root_value);

    return result;
}


inline char* isConnectedResponse(uint8_t status, char* correlationId, JSON_Value* propertyBag)
{
    JSON_Value* msg_value = json_value_init_object();
    JSON_Object* msg_object = json_value_get_object(msg_value);

    json_object_dotset_number(msg_object, "status", status);
    json_object_dotset_string(msg_object, "msg", "OK Success");

    return lavazza_protocol_message((cloud_msg_type)0, (cloud_payload_type)0, correlationId, msg_value, propertyBag);
}