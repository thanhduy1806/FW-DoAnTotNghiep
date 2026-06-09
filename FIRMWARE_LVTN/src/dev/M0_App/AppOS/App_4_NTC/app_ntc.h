#ifndef app_ntc_H
#define app_ntc_H

#include "os.h"
#include "semphr.h"
#include "M0_App/AppOS/Database/DB_Temperature/db_temperature.h"

extern SemaphoreHandle_t ntc_mutex;
extern const osThreadAttr_t ntc_attr;

void App_NTCTask(void *param);

void ntc_init(void);

#endif /* app_ntc_H */