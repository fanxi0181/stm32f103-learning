// app_common_err.h 公共错误返回值
//   传感器错误码已迁至 sensor_common.h

#ifndef INC_APP_COMMON_ERR_H_
#define INC_APP_COMMON_ERR_H_

#include "app_common.h"

#ifdef ERR_AUTOMATIC_PRINTING_MODULE_ENABLED
void ERR_Printf_Task(void *argument);
#endif

#endif //INC_APP_COMMON_ERR_H_
