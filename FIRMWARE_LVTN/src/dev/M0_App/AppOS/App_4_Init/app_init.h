#ifndef app_growup_H
#define app_growup_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os.h"

extern const osThreadAttr_t init_attr;

void App_InitTask(void *param);

#ifdef __cplusplus
}
#endif

#endif /* app_growup_H */