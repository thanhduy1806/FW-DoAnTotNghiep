/* 
 * File:   cli_laser.h
 * Author: HTSANG
 *
 * Created on March 18, 2026, 8:50 AM
 */

#ifndef CLI_LASER_H
#define	CLI_LASER_H

#include "../CLI_Src/embedded_cli.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void CMD_Laser_Int_Set_DAC(EmbeddedCli *cli, char *args, void *context);
void CMD_Laser_Int_Turn_On_Channel(EmbeddedCli *cli, char *args, void *context);
void CMD_Laser_Int_Turn_Off_Channel(EmbeddedCli *cli, char *args, void *context);
void CMD_Laser_Int_Turn_Off_All(EmbeddedCli *cli, char *args, void *context);

#endif	/* CLI_LASER_H */

