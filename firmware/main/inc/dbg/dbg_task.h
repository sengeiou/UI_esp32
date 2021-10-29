#ifndef DBG_TAKS_H_
#define DBG_TAKS_H_

#include "types.h"


/* Helper functions */
void enable_livedata(void);
void disable_livedata(void);

void special_function(dbg_special_func_code_t cmd);

void dbg_task(void *pvParameter);

#endif // DBG_TAKS_H_