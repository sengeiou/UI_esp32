#ifndef AZURE_API_H_
#define AZURE_API_H_

#include "iothub_client.h"
#include "azure_prov_client/prov_device_client.h"
#include <stdbool.h>

void azure_dps_register_device(PROV_DEVICE_RESULT register_result, const char* iothub_uri, const char* device_id, void* user_context);
void azure_dps_registation_status(PROV_DEVICE_REG_STATUS reg_status, void* user_context);

void azure_iothub_send_message(IOTHUB_MESSAGE_HANDLE message);
void azure_iothub_send_telemetry(IOTHUB_MESSAGE_HANDLE message, char* filename);

IOTHUBMESSAGE_DISPOSITION_RESULT azure_iothub_on_message(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);
void azure_iothub_connection_status(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback);
void azure_iothub_send_confirmation(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);

int azure_iothub_device_method(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback);

void azure_iothub_on_device_twin_update(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback);
void azure_iothub_report_device_twin(int status_code, void* userContextCallback);

void azure_iothub_report_new_device_twin(const char* reportedProperties, bool isNewFirmwareTM, bool isNewFirmwareMB);

#endif // AZURE_API_H_