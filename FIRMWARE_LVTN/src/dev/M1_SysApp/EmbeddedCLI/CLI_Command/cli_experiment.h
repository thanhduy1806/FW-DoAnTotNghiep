/* 
 * File:   cli_experiment.h
 * Author: HTSANG
 *
 * Created on March 23, 2026, 10:54 AM
 */

#ifndef CLI_EXPERIMENT_H
#define	CLI_EXPERIMENT_H

#include "../CLI_Src/embedded_cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CMD_Exp_Start(EmbeddedCli *cli, char *args, void *context);
void CMD_Exp_Set_Profile(EmbeddedCli *cli, char *args, void *context);
void CMD_Exp_Start_Laser (EmbeddedCli *cli, char *args, void *context);
void CMD_DLS_setup(EmbeddedCli *cli, char *args, void *context);
void CMD_Exp_End(EmbeddedCli *cli, char *args, void *context);

#endif	/* CLI_EXPERIMENT_H */

