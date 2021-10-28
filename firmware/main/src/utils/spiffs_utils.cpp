#include "utils.h"
#include "spiffs_utils.h"
#include "converter_utils.h"
#include "types.h"
#include "variables.h"

#include "dirent.h"
#include "parson.h"

#ifdef ADVANCED_DEBUG
    #define SPIFFS_UTILS_TAG LINE_STRING "|" "SPIFFS UTILS"
#else
    #define SPIFFS_UTILS_TAG "SPIFFS UTILS"
#endif


void print_files_in_dir(const char * dir_name)
{
    DIR* dir;
    struct dirent* de;

    dir = opendir(dir_name);
    ESP_LOGI(SPIFFS_UTILS_TAG, "Directory: %s", dir_name);
    while(dir)
    {
        de = readdir(dir);
        if(!de) 
            break;
        
        ESP_LOGI(SPIFFS_UTILS_TAG, "%i %s", de->d_type, de->d_name);
    }
    closedir(dir);
}

int get_number_of_files(const char* dir_name)
{
    int file_count = 0;
    DIR* dir;
    struct dirent* entry;

    dir = opendir(dir_name);

    while(NULL != (entry = readdir(dir)))
    {
        if(DT_REG == entry->d_type)
        { 
            file_count++;
        }
    }
    closedir(dir);

    ESP_LOGI(SPIFFS_UTILS_TAG, "Found %d %s in dir %s", file_count, (file_count > 1) ? "files" : "file", dir_name);

    return file_count;
}

void parse_firmware_connectivity(void)
{
    char* json_file = read_file(FIRMWARE_CONNECTIVITY_FILE);
    if(NULL != json_file)
    {
        machineConnectivity.wifiEnabled = utils::json::getValue<bool>(json_file, "wifiEnabled");
        machineConnectivity.status = static_cast<machine_connectivity_status_t>(utils::json::getValue<uint8_t>(json_file, "status"));
        strncpy((char*) machineConnectivity.ssid, utils::json::getValue<std::string>(json_file, "network.ssid").c_str(), 64);
        strncpy((char*) machineConnectivity.password, utils::json::getValue<std::string>(json_file, "network.password").c_str(), 64);

        machineInternalState.ota.otaStatus = static_cast<ota_status>(utils::json::getValue<uint8_t>(json_file, "ota.status"));
        strncpy((char*) machineInternalState.ota.newFwVer, utils::json::getValue<std::string>(json_file, "ota.newFirmwareVersion").c_str(), MAX_FW_VERSION_LENGTH);
        strncpy((char*) machineInternalState.ota.errorFwVer, utils::json::getValue<std::string>(json_file, "ota.errorFirmwareVersion").c_str(), MAX_FW_VERSION_LENGTH);
        strncpy((char*) machineInternalState.ota.fwUrl, utils::json::getValue<std::string>(json_file, "ota.url").c_str(), MAX_URL_LENGTH);
        machineInternalState.ota.currentRetry = utils::json::getValue<uint8_t>(json_file, "ota.retry");
        machineInternalState.ota.dataWritten = utils::json::getValue<uint32_t>(json_file, "ota.dataWritten");
        strncpy((char*) machineInternalState.ota.correlationId, utils::json::getValue<std::string>(json_file, "ota.correlationId").c_str(), MAX_UUID_LENGTH);
        free(json_file);
    }
    else
    {
        ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to read connectivity file");
    }
    save_firmware_connectivity(true);
}


void parse_firmware_configuration(void)
{
    parse_firmware_connectivity();
}

void save_firmware_connectivity(bool print)
{
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
   
    json_object_dotset_boolean(root_object, "wifiEnabled", machineConnectivity.wifiEnabled);
    json_object_dotset_number(root_object, "status", machineConnectivity.status);
    json_object_dotset_string(root_object, "network.ssid", machineConnectivity.ssid);
    json_object_dotset_string(root_object, "network.password", machineConnectivity.password);

    json_object_dotset_number(root_object, "ota.status", machineInternalState.ota.otaStatus);
    json_object_dotset_string(root_object, "ota.newFirmwareVersion", machineInternalState.ota.newFwVer);
    json_object_dotset_string(root_object, "ota.errorFirmwareVersion", machineInternalState.ota.errorFwVer);
    json_object_dotset_string(root_object, "ota.url", machineInternalState.ota.fwUrl);
    json_object_dotset_number(root_object, "ota.retry", machineInternalState.ota.currentRetry);
    json_object_dotset_number(root_object, "ota.dataWritten", machineInternalState.ota.dataWritten);
    json_object_dotset_string(root_object, "ota.correlationId", machineInternalState.ota.correlationId);

    char* json = NULL;
    json = json_serialize_to_string(root_value);

    if(NULL != json)
    {
        FILE* fp = fopen(FIRMWARE_CONNECTIVITY_FILE, "w");
        if(fp != NULL)
        {
            fprintf(fp, "%s", json);
            fclose(fp);

            if(true == print)
                ESP_LOGI(SPIFFS_UTILS_TAG, "Saved firmware connectivity: %s", json);
            else
                ESP_LOGI(SPIFFS_UTILS_TAG, "Firmware connectivity updated");
        }
        else
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to open file to save connectivity");
        }
    }
    else
    {
        ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to serialize json connectivity");
    }
    
    json_value_free(root_value);
    free(json);
}

void save_firmware_configuration(bool print)
{
    save_firmware_connectivity(print);
}

void create_file(const char* filename)
{
    struct stat st;

    if(false == file_exists(filename))
    {
        FILE* f = fopen(filename, "w");
        if(NULL != f)
            ESP_LOGI(SPIFFS_UTILS_TAG, "File %s created", filename);
        else
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to create file %s", filename);
        fclose(f);
    }
}

bool file_exists(const char* filename)
{
    bool ret_val = false;

    struct stat st;
    if(0 == stat(filename, &st)) 
    {
        ret_val = true;
        ESP_LOGI(SPIFFS_UTILS_TAG, "File %s exists", filename);
    }
    else
    {
        ret_val = false;
        ESP_LOGI(SPIFFS_UTILS_TAG, "File %s does not exists", filename);
    }
    return ret_val;
}


char* read_file(const char* filename)
{
    char* content = NULL;

    FILE* infile = fopen(filename, "r");
    if(NULL == infile)
    {
        ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to open configuration file %s", filename);
        content = NULL;
    }
    else
    {
        fseek(infile, 0L, SEEK_END);
        size_t fileSize = ftell(infile);
        fseek(infile, 0L, SEEK_SET);
        
        content = (char*)malloc(fileSize + 1);

        if(NULL == content)
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Configuration file %s is empty", filename);
            content = NULL;
        }
        else
        {
            size_t size = fread(content, 1, fileSize, infile);
            content[size] = 0;
            fclose(infile);
            
            ESP_LOGD(SPIFFS_UTILS_TAG, "Loaded firmware config: %s", content);
        }
    }

    return content;
}

void delete_file(const char* filename)
{
    if(0 == remove(filename))
        ESP_LOGI(SPIFFS_UTILS_TAG, "File %s deleted", filename);
    else
        ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to delete file %s", filename);
}

esp_err_t init_data_storage_spiffs(void)
{
    esp_err_t ret = ESP_FAIL;

    ESP_LOGI(SPIFFS_UTILS_TAG, "Initializing SPIFFS partition data");
    
    esp_vfs_spiffs_conf_t conf = {
        .base_path = BASE_PATH_SPIFFS,
        .partition_label = "data_storage",
        .max_files = 10,
        .format_if_mount_failed = true
    };

    ret = esp_vfs_spiffs_register(&conf);

    if(ESP_OK != ret) 
    {
        if(ESP_FAIL == ret) 
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to mount or format filesystem");
        }
        else if(ESP_ERR_NOT_FOUND == ret) 
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to find SPIFFS partition data_storage");
        } 
        else 
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to initialize SPIFFS partition data_storage (error:%s)", esp_err_to_name(ret));
        }
        return ret;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);

    if(ESP_OK != ret) 
    {
        ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to get SPIFFS partition data_storage information (%s)", esp_err_to_name(ret));
    } 
    else 
    {
        ESP_LOGI(SPIFFS_UTILS_TAG, "Partition data_storage size: total: %d, used: %d", total, used);
    }
        
    create_file(FIRMWARE_CONNECTIVITY_FILE);
    parse_firmware_configuration();
    return ret;
}

esp_err_t init_log_storage_spiffs(void)
{
    esp_err_t ret = ESP_FAIL;

    ESP_LOGI(SPIFFS_UTILS_TAG, "Initializing SPIFFS partition Logs");
        
    esp_vfs_spiffs_conf_t conf = {
        .base_path = BASE_PATH_SPIFFS_LOGS,
        .partition_label = "log_storage",
        .max_files = 5,
        .format_if_mount_failed = true
    };

    ret = esp_vfs_spiffs_register(&conf);

    if(ESP_OK != ret) 
    {
        if(ESP_FAIL == ret) 
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to mount or format filesystem");
        }
        else if(ESP_ERR_NOT_FOUND == ret) 
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to find SPIFFS partition log_storage");
        } 
        else 
        {
            ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to initialize SPIFFS partition log_storage (error:%s)", esp_err_to_name(ret));
        }
        return ret;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);

    if(ESP_OK != ret) 
    {
        ESP_LOGE(SPIFFS_UTILS_TAG, "Failed to get SPIFFS partition log_storage information (%s)", esp_err_to_name(ret));
    } 
    else 
    {
        ESP_LOGI(SPIFFS_UTILS_TAG, "Partition log_storage size: total: %d, used: %d", total, used);
    }

    return ret;
}

void init_spiffs_memory(void)
{
    init_data_storage_spiffs();
    // init_log_storage_spiffs();
}