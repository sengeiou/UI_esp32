#ifndef TYPES_H_
#define TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "version.h"

#ifdef ADVANCED_DEBUG
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)
#endif

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define CURRENT_VERSION_FIRMWARE STRINGIZE(VERSION_MAJOR) "-" STRINGIZE(VERSION_MINOR) "-" STRINGIZE(VERSION_PATCH) "-" STRINGIZE(VERSION_BUILD)

#define MAX_FW_VERSION_LENGTH       (16)
#define MAX_UUID_LENGTH             (36)
#define MAX_URL_LENGTH              (256)

/*Typedef for certificates*/
typedef struct
{
    char* device_certificate_pem;
    char* device_private_key_pem;
    char* device_common_name;
} Certificates;

typedef struct IOTHUB_CLIENT_INFO_TAG
{
    unsigned int sleep_time;
    char* iothub_uri;
    char* access_key_name;
    char* device_key;
    char* device_id;
    int registration_complete;
} IOTHUB_CLIENT_INFO;


#define FOREACH_CLOUD_COMMAND(CLOUD_COMMAND) \
        CLOUD_COMMAND(POWER_ON)   \
		CLOUD_COMMAND(POWER_OFF)   \
        CLOUD_COMMAND(MAKE_COFFEE)  \
		CLOUD_COMMAND(FIRMWARE_UPDATE)

enum cloud_command_t {
    FOREACH_CLOUD_COMMAND(GENERATE_ENUM)
};

static const char* cloud_command_string[] = {
    FOREACH_CLOUD_COMMAND(GENERATE_STRING)
};

#define FOREACH_COFFEE_TYPE(COFFEE_TYPE)    \
        COFFEE_TYPE(PREP_NONE)              \
        COFFEE_TYPE(PREP_ESPRESSO_CORTO)    \
		COFFEE_TYPE(PREP_ESPRESSO)          \
        COFFEE_TYPE(PREP_ESPRESSO_LUNGO)    \
        COFFEE_TYPE(PREP_MACCHIATO)         \
        COFFEE_TYPE(PREP_CAPPUCCINO)        \
        COFFEE_TYPE(PREP_LATTE_MACCHIATO)   \
        COFFEE_TYPE(PREP_DOSE_LIBERA)       \
        COFFEE_TYPE(PREP_CAFFE_AMERICANO)   \
        COFFEE_TYPE(PREP_ACQUA_CALDA)


enum coffee_type_t {
    FOREACH_COFFEE_TYPE(GENERATE_ENUM)
};

static const char* coffee_type_string[] = {
    FOREACH_COFFEE_TYPE(GENERATE_STRING)
};


#define FOREACH_OTA_STATUS(OTA_STATUS) \
        OTA_STATUS(SUCCESS)   \
        OTA_STATUS(FAILED)  \
        OTA_STATUS(FAILED_MB_NOT_RESPONDING)  \
        OTA_STATUS(SAME_VERSION_DOWNLOADED)   \
        OTA_STATUS(FAILED_REDIRECT)  \
        OTA_STATUS(FAILED_DOWNLOAD)  \
        OTA_STATUS(FAILED_HTTP_CONNECTION)  \
        OTA_STATUS(STARTING)  \
        OTA_STATUS(SAME_VERSION)  \
        OTA_STATUS(FAILED_SIGNATURE) \
        OTA_STATUS(OLDER_VERSION) \
        OTA_STATUS(MAXIMUM_ATTEMPTS_EXCEEDED) \
        OTA_STATUS(NO_AVAILABLE_PARTITION) \
        OTA_STATUS(SSL_ERROR) \
        OTA_STATUS(FAILED_INSTALLATION_START) \
        OTA_STATUS(FAILED_WRITE_DATA) \
        OTA_STATUS(WRONG_LENGTH_RECEIVED)   \
        OTA_STATUS(DOWNLOADING)   \
		OTA_STATUS(INSTALLING)

enum ota_status {
    FOREACH_OTA_STATUS(GENERATE_ENUM)
};
typedef enum ota_status ota_status;

static const char *ota_status_string[] = {
    FOREACH_OTA_STATUS(GENERATE_STRING)
};

typedef struct
{
	coffee_type_t			desiredPreparation;
	struct
	{
		ota_status otaStatus;
		char correlationId[MAX_UUID_LENGTH];
		char fwUrl[MAX_URL_LENGTH];
		char newFwVer[MAX_FW_VERSION_LENGTH]; 
		char errorFwVer[MAX_FW_VERSION_LENGTH]; 
		uint32_t firmwareSize;
		uint32_t dataWritten;
		uint8_t currentRetry;
		bool fileDownloaded;
	} ota;

	struct
	{
		cloud_command_t command;
		coffee_type_t	desiredPreparation;
		uint16_t		desiredDose;
	} azureRequest;

	struct
	{
		cloud_command_t command;
		coffee_type_t	desiredPreparation;
		uint16_t		desiredDose;
	} wsRequest;	
} internal_state_t;


#define FOREACH_MACHINE_CONNECTIVITY_STATUS(MACHINE_CONNECTIVITY_STATUS) \
        MACHINE_CONNECTIVITY_STATUS(WIFI_NOT_CONFIGURED)   \
		MACHINE_CONNECTIVITY_STATUS(WIFI_CONFIGURED)   \
        MACHINE_CONNECTIVITY_STATUS(WIFI_CONNECTED)  \
		MACHINE_CONNECTIVITY_STATUS(WIFI_LAVAZZA_CONNECTED)

enum machine_connectivity_status_t {
    FOREACH_MACHINE_CONNECTIVITY_STATUS(GENERATE_ENUM)
};

static const char* machine_connectivity_status_string[] = {
    FOREACH_MACHINE_CONNECTIVITY_STATUS(GENERATE_STRING)
};

typedef struct
{
	bool wifiEnabled;
	machine_connectivity_status_t status;
    char ssid[64];
    char password[64];
} machine_connectivity_t;

typedef enum
{
	EROGATION_PROGRESS
} azure_message_type_t;

typedef union
{
    struct
	{
		uint8_t preparation;
		uint16_t desiredDose;
		uint16_t currentDose;
	} erogation_status_t;
} azure_queue_payload_t;

typedef struct
{
	azure_message_type_t 	type; 
    azure_queue_payload_t 	payload;
} azure_queue_message_t;

typedef enum
{
    DBG_NONE = 0x00,
    DBG_ON_OFF = 0x04,
    DBG_SHORT_COFFEE = 0x05,
    DBG_MEDIUM_COFFEE = 0x06,
    DBG_LONG_COFFEE = 0x07,
    DBG_FREE_COFFEE = 0x08,
    DBG_SHORT_CAPPUCCINO = 0x11,
    DBG_MEDIUM_CAPPUCCINO = 0x12,
    DBG_DOUBLE_CAPPUCCINO = 0x13,
    DBG_HOT_MILK = 0x14,
    DBG_HOT_WATER = 0x15,
    DBG_AUTO_CLEANING = 0x19,
    DBG_SEMIAUTO_CLEANING = 0x1D,
    DBG_FACTORY_RESET = 0x1C
} dbg_special_func_code_t;

typedef enum
{
    CAM_NONE = 0x00,
    CAM_TAKE_PHOTO = 0x01,
    CAM_GET_PHOTO = 0x02,
    CAM_ENABLE_LED = 0x03,
    CAM_DISABLE_LED = 0x04,
} camera_func_code_t;

typedef struct
{
    bool isFault;
    bool milkHeadPresence;

    struct
    {
        uint8_t temperature;
        uint16_t dose;
    } erogation;

    struct
    {
        uint8_t recipeId;
        uint8_t totalSteps;
        uint8_t currentSteps;
    } cleaning;

    struct
    {
        uint8_t percent;
    } steaming;
    
    struct
    {
        bool descaling;
        bool podFull;
        bool waterEmpty;
    } warnings;

    struct
    {
        coffee_type_t coffeeType;
    } cloudReq;
} gui_status_t;

#endif // TYPES_H_
