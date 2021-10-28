#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sdkconfig.h>

#include "hsm_client_data.h"
#include "variables.h"

static const char* const COMMON_NAME = CONFIG_DEVICE_COMMON_NAME;

typedef struct CUSTOM_HSM_INFO_TAG
{
    const char* certificate;
    const char* common_name;
    const char* key;
    const char* symm_key;
    const char* registration_name;
} CUSTOM_HSM_INFO;

int hsm_client_x509_init()
{
    return 0;
}

void hsm_client_x509_deinit()
{
}


HSM_CLIENT_HANDLE custom_hsm_create()
{
    HSM_CLIENT_HANDLE result;
    CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*) malloc(sizeof(CUSTOM_HSM_INFO));
    if (hsm_info == NULL)
    {
        (void)printf("Failued allocating hsm info\r\n");
        result = NULL;
    }
    else
    {
        hsm_info->certificate = (const char *) cert.device_certificate_pem;
        hsm_info->key = (const char *) cert.device_private_key_pem;
        hsm_info->common_name = COMMON_NAME;
        hsm_info->common_name = (const char *) cert.device_common_name;
        #if CONFIG_AZURE_SHARED_ACCESS_KEY == 1
        hsm_info->symm_key = CONFIG_AZURE_SHARED_ACCESS_KEY;
        #endif
        hsm_info->registration_name = (const char *) cert.device_common_name;
        result = hsm_info;
    }
    return result;
}

void custom_hsm_destroy(HSM_CLIENT_HANDLE handle)
{
    if (handle != NULL)
    {
        CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*)handle;
        // Free anything that has been allocated in this module
        free(hsm_info);
    }
}

char* custom_hsm_get_certificate(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified\r\n");
        result = NULL;
    }
    else
    {
        CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*)handle;
        size_t len = strlen(hsm_info->certificate);
        if ((result = (char*)malloc(len + 1)) == NULL)
        {
            (void)printf("Failure allocating certificate\r\n");
            result = NULL;
        }
        else
        {
            strcpy(result, hsm_info->certificate);
        }
    }
    return result;
}

char* custom_hsm_get_key(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified\r\n");
        result = NULL;
    }
    else
    {
        CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*)handle;
        size_t len = strlen(hsm_info->key);
        if ((result = (char*)malloc(len + 1)) == NULL)
        {
            (void)printf("Failure allocating certificate\r\n");
            result = NULL;
        }
        else
        {
            strcpy(result, hsm_info->key);
        }
    }
    return result;
}

char* custom_hsm_get_common_name(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified\r\n");
        result = NULL;
    }
    else
    {
        CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*)handle;
        size_t len = strlen(hsm_info->common_name);
        if ((result = (char*)malloc(len + 1)) == NULL)
        {
            (void)printf("Failure allocating certificate\r\n");
            result = NULL;
        }
        else
        {
            strcpy(result, hsm_info->common_name);
        }
    }
    return result;
}

char* custom_hsm_symm_key(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified\r\n");
        result = NULL;
    }
    else
    {
        // TODO: Malloc the symmetric key for the iothub 
        // The SDK will call free() this value
        CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*)handle;
        size_t len = strlen(hsm_info->symm_key);
        if ((result = (char*)malloc(len + 1)) == NULL)
        {
            (void)printf("Failure allocating symmetric key\r\n");
            result = NULL;
        }
        else
        {
            strcpy(result, hsm_info->symm_key);
        }
    }
    return result;
}

char* custom_hsm_get_registration_name(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified\r\n");
        result = NULL;
    }
    else
    {
        // TODO: Malloc the registration name for the iothub 
        // The SDK will call free() this value
        CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*)handle;
        size_t len = strlen(hsm_info->registration_name);
        if ((result = (char*)malloc(len + 1)) == NULL)
        {
            (void)printf("Failure allocating registration name\r\n");
            result = NULL;
        }
        else
        {
            strcpy(result, hsm_info->registration_name);
        }
    }
    return result;
}

int custom_hsm_client_set_key_info(HSM_CLIENT_HANDLE handle, const char* reg_name, const char* symm_key)
{
    int result = 0;
    if (handle == NULL || reg_name == NULL || symm_key == NULL)
    {
        (void)printf("Invalid parameter specified handle: %p, reg_name: %p, symm_key: %p\r\n", handle, reg_name, symm_key);
        result = -1;
    }
    else
    {
        CUSTOM_HSM_INFO* hsm_info = (CUSTOM_HSM_INFO*)handle;

        char* temp_reg_name;
        char* temp_key;
        if (mallocAndStrcpy_s(&temp_reg_name, reg_name) != 0)
        {
            (void)printf("Failure allocating registration name\r\n");
            result = -1;
        }
        else if (mallocAndStrcpy_s(&temp_key, symm_key) != 0)
        {
            (void)printf("Failure allocating symmetric key\r\n");
            free(temp_reg_name);
            result = -1;
        }
        else
        {
            hsm_info->symm_key = temp_key;
            hsm_info->registration_name = temp_reg_name;
            result = 0;
        }
    }
    return result;
}

// Defining the v-table for the x509 hsm calls
static const HSM_CLIENT_X509_INTERFACE x509_interface =
{
    custom_hsm_create,
    custom_hsm_destroy,
    custom_hsm_get_certificate,
    custom_hsm_get_key,
    custom_hsm_get_common_name
};

const HSM_CLIENT_X509_INTERFACE* hsm_client_x509_interface()
{
    // x509 interface pointer
    return &x509_interface;
}

static const HSM_CLIENT_KEY_INTERFACE symm_key_interface =
{
    custom_hsm_create,
    custom_hsm_destroy,
    custom_hsm_symm_key,
    custom_hsm_get_registration_name,
    custom_hsm_client_set_key_info
};

const HSM_CLIENT_KEY_INTERFACE* hsm_client_key_interface()
{
    return &symm_key_interface;
}