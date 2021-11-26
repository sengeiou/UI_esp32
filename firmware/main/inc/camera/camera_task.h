#ifndef CAMERA_TAKS_H_
#define CAMERA_TAKS_H_

#include "types.h"


/* Helper functions */
void camera_function(camera_func_code_t cmd);

void cam_task(void *pvParameter);

#endif // CAMERA_TAKS_H_