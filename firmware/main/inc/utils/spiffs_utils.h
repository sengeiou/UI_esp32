#ifndef SPIFFS_UTILS_H_
#define SPIFFS_UTILS_H_

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_spiffs.h>

#include "converter_utils.h"

#include "types.h"

#define BASE_PATH_SPIFFS                "/data" 
#define BASE_PATH_SPIFFS_LOGS           "/logs" 

#define FIRMWARE_CONNECTIVITY_FILE      BASE_PATH_SPIFFS "/" "connectivity.json"


/*-------------------- HELPER ---------------------*/
void parse_firmware_connectivity(void);
void parse_firmware_configuration(void);
void save_firmware_connectivity(bool print);
void save_firmware_configuration(bool print);

/*--------------------- PROTO ---------------------*/
esp_err_t init_data_storage_spiffs(void);
esp_err_t init_log_storage_spiffs(void);
void init_spiffs_memory(void);

void create_file(const char* filename);
bool file_exists(const char* filename);
char* read_file(const char* filename);
void delete_file(const char* filename);

void print_files_in_dir(const char * dir_name);
int get_number_of_files(const char* dir_name);

#endif /* SPIFFS_UTILS_H_ */