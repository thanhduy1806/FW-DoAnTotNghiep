#ifndef     DB_TEMP_PROFILE_H
#define     DB_TEMP_PROFILE_H
#include "define.h"
#include "M0_App/AppOS/App_6_Temperature_Control/profile_state.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Src/embedded_cli.h"

void DB_temp_profile_write(uint32_t index, const profileData_t *t_profile);
void DB_temp_profile_read(uint32_t index, profileData_t *t_profile);

profileData_t * DB_temp_profile_get_pointer(uint8_t index);

void DB_temp_profile_init(void);
void DB_temp_profile_start(uint32_t index);
void DB_temp_profile_stop(void);

void DB_temp_profile_start_steps(uint32_t index);
void DB_temp_profile_stop_steps(void);

uint8_t DB_temp_profile_tec_mask_invailable(void);
uint8_t DB_temp_profile_heater_mask_invailable(void);

void DB_temp_profile_display(EmbeddedCli *cli, uint8_t pf_idx);
Std_ReturnType DB_temp_profile_validation(EmbeddedCli *cli);

#endif