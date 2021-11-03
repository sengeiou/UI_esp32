#ifndef DBG_TAKS_H_
#define DBG_TAKS_H_

#include "types.h"


/* Helper functions */
void get_parameter(uint16_t id, uint8_t size);
void set_parameter(uint16_t id, uint32_t value);

void enable_livedata(void);
void disable_livedata(void);

void special_function(dbg_special_func_code_t cmd);

void dbg_task(void *pvParameter);

#endif // DBG_TAKS_H_