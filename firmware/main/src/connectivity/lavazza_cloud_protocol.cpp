#include "lavazza_cloud_protocol.h"

#include "utils.h"
#include "converter_utils.h"
#include "spiffs_utils.h"

void cloud::helper::create_device_id(char* deviceId)
{
    strcpy(deviceId, "18000USA_LAVAZZATESTUSA000003");
}

void cloud::helper::uuid_generate(char* out)
{
    esp_fill_random(out, sizeof(uuid_t));

    /* uuid version */
    out[6] = 0x40 | (out[6] & 0xF);

    /* uuid variant */
    out[8] = (0x80 | out[8]) & ~0x40;
}

char* cloud::protocol::lavazza_message(cloud_msg_type msgType, cloud_payload_type payloadType, const char* corrId, JSON_Value* payload, JSON_Value* propertyBag)
{
    char* result;

    char deviceId[100];
    cloud::helper::create_device_id(deviceId);

    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_dotset_number(root_object, "timestamp", utils::time::get_timestamp_seconds());
    json_object_dotset_string(root_object, "deviceId", deviceId);

    char uuid[UUID_STR_LEN];
    cloud::helper::uuid_generate(uuid);
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

char* cloud::protocol::configuration2json(Configuration* config, bool complete)
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

char* cloud::protocol::response::isConnected(uint8_t status, char* correlationId, JSON_Value* propertyBag)
{
    JSON_Value* msg_value = json_value_init_object();
    JSON_Object* msg_object = json_value_get_object(msg_value);

    json_object_dotset_number(msg_object, "status", status);
    json_object_dotset_string(msg_object, "msg", "OK Success");

    return cloud::protocol::lavazza_message(static_cast<cloud_msg_type>(0), static_cast<cloud_payload_type>(0), correlationId, msg_value, propertyBag);
}

char* cloud::protocol::response::turnOnOffMachine(uint8_t status, char* correlationId, JSON_Value* propertyBag)
{
    JSON_Value* msg_value = json_value_init_object();
    JSON_Object* msg_object = json_value_get_object(msg_value);
    
    json_object_dotset_number(msg_object, "status", status);

    return cloud::protocol::lavazza_message(static_cast<cloud_msg_type>(0), static_cast<cloud_payload_type>(0), correlationId, msg_value, propertyBag);
}

char* cloud::protocol::response::stopBrewing(uint8_t status, char* correlationId, JSON_Value* propertyBag)
{
    JSON_Value* msg_value = json_value_init_object();
    JSON_Object* msg_object = json_value_get_object(msg_value);
    
    json_object_dotset_number(msg_object, "status", status);

    return cloud::protocol::lavazza_message(static_cast<cloud_msg_type>(0), static_cast<cloud_payload_type>(0), correlationId, msg_value, propertyBag);
}

char* cloud::protocol::response::makeCoffeeByButtons(uint8_t status, char* correlationId, JSON_Value* propertyBag)
{
    JSON_Value* msg_value = json_value_init_object();
    JSON_Object* msg_object = json_value_get_object(msg_value);
    
    json_object_dotset_number(msg_object, "status", status);

    // errors field in payload
    json_object_dotset_boolean(msg_object, "errors.tempError", false);
    json_object_dotset_boolean(msg_object, "errors.heaterTimeoutError", false);
    json_object_dotset_boolean(msg_object, "errors.initError", false);
    json_object_dotset_boolean(msg_object, "errors.motorOverCurrentError", false);
    json_object_dotset_boolean(msg_object, "errors.uiError", false);
    json_object_dotset_boolean(msg_object, "errors.ntcError", false);
    json_object_dotset_boolean(msg_object, "errors.zcError", false);
    json_object_dotset_boolean(msg_object, "errors.wdtResetError", false);

    // warnings field in payload
    json_object_dotset_boolean(msg_object, "warnings.podCounter", false);
    json_object_dotset_boolean(msg_object, "warnings.descalingProcedure", false);
    json_object_dotset_boolean(msg_object, "warnings.emptyTank", false); 
    json_object_dotset_boolean(msg_object, "warnings.drawer", false);

    return cloud::protocol::lavazza_message(static_cast<cloud_msg_type>(0), static_cast<cloud_payload_type>(0), correlationId, msg_value, propertyBag);
}