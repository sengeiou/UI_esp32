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


namespace cloud
{
    namespace helper
    {
        void create_device_id(char* deviceId);
        void uuid_generate(char* out);
    }

    namespace protocol
    {
        char* lavazza_message(cloud_msg_type msgType, cloud_payload_type payloadType, const char* corrId, JSON_Value* payload, JSON_Value* propertyBag);
        char* configuration2json(Configuration* config, bool complete);

        namespace response
        {
            char* isConnected(uint8_t status, char* correlationId, JSON_Value* propertyBag);
            char* turnOnOffMachine(uint8_t status, char* correlationId, JSON_Value* propertyBag);
            char* stopBrewing(uint8_t status, char* correlationId, JSON_Value* propertyBag);
            char* makeCoffeeByButtons(uint8_t status, char* correlationId, JSON_Value* propertyBag);
        }
    }
}