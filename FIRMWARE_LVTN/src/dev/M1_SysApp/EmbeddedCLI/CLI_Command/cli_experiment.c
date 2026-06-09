#include "cli_experiment.h"
#include "M0_App/AppOS/App_Experiment/app_experiment.h"

void CMD_Exp_Start(EmbeddedCli *cli, char *args, void *context) {
    exp_cmd_t cmd = EXP_CMD_RUN;

    if (experiment_command_queue == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(experiment_command_queue, &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Experiment start OK");
    } else {
        embeddedCliPrint(cli, "Queue full, cannot start");
    }
}

void CMD_Exp_Start_Laser (EmbeddedCli *cli, char *args, void *context) 
{
    const char *laserStr = embeddedCliGetToken(args, 1);
    const char *intensityStr = embeddedCliGetToken(args, 2);
    const char *rateStr = embeddedCliGetToken(args, 3);
    const char *preStr = embeddedCliGetToken(args, 4);
    const char *durationStr = embeddedCliGetToken(args, 5);
    const char *afterStr = embeddedCliGetToken(args, 6);

    char *endptr;
    char buf[128];

    if (laserStr == NULL || intensityStr == NULL || rateStr == NULL || preStr == NULL || durationStr == NULL || afterStr == NULL) {
        embeddedCliPrint(cli,
                "Usage: exp_start_laser <laser_index> <laser_intensity> <sample_rate> <pre_laser> <laser_duration> <after_laser>");
        return;
    }

    uint8_t channel = strtoul(laserStr, &endptr, 0);
    if (*endptr != '\0' || channel == 0 || channel > 24) {
        embeddedCliPrint(cli, "Invalid channel");
        return;
    }

    uint16_t intensity = strtoul(intensityStr, &endptr, 0);
    if (*endptr != '\0' || intensity > 100) {
        embeddedCliPrint(cli, "Invalid intensity");
        return;
    }

    uint16_t sampling_rate = strtoul(rateStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid sample_rate");
        return;
    }

    uint16_t pre_time = strtoul(preStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid pre_laser time");
        return;
    }

    uint16_t laser_duration = strtoul(durationStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid laser_duration");
        return;
    }

    uint16_t after_time = strtoul(afterStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid after_laser time");
        return;
    }


    exp_profile_t profile = {0};
    profile.pre_time_ms = pre_time;
    profile.main_time_ms = laser_duration;
    profile.post_time_ms = after_time;
    profile.sampling_rate_khz = sampling_rate;
    profile.channel = channel;
    profile.laser_intensity = intensity;

    // profile.pre_time_ms = 1000,
    // profile.main_time_ms = 2000,
    // profile.post_time_ms = 1000,

    // profile.sampling_rate_khz = 1,
    
    experiment_set_profile(&profile);

    exp_cmd_t cmd = EXP_CMD_RUN;

    if (experiment_command_queue == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(experiment_command_queue, &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Experiment start OK");
    } else {
        embeddedCliPrint(cli, "Queue full, cannot start");
    }
}

void CMD_Exp_End(EmbeddedCli *cli, char *args, void *context) {
    exp_cmd_t cmd = EXP_CMD_END;

    if (experiment_command_queue == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(experiment_command_queue, &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Experiment end OK");
    } else {
        embeddedCliPrint(cli, "Queue full, cannot end");
    }
}

void CMD_Exp_Set_Profile(EmbeddedCli *cli, char *args, void *context) {
    const char *preStr = embeddedCliGetToken(args, 1);
    const char *sampleStr = embeddedCliGetToken(args, 2);
    const char *postStr = embeddedCliGetToken(args, 3);
    const char *rateStr = embeddedCliGetToken(args, 4);
    const char *laserStr = embeddedCliGetToken(args, 5);

    char *endptr;
    char buf[128];

    if (preStr == NULL || sampleStr == NULL || postStr == NULL ||
            rateStr == NULL || laserStr == NULL) {
        embeddedCliPrint(cli,
                "Usage: exp_set_profile <pre_ms> <sampling_ms> <post_ms> <rate_khz> <laser_intensity>");
        return;
    }

    uint32_t pre_time = strtoul(preStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid pre_time_ms");
        return;
    }

    uint32_t sampling_time = strtoul(sampleStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid main_time_ms");
        return;
    }

    uint32_t post_time = strtoul(postStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid post_time_ms");
        return;
    }

    uint32_t sampling_rate = strtoul(rateStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid sampling_rate_khz");
        return;
    }

    uint32_t laser_intensity = strtoul(laserStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid laser_intensity");
        return;
    }

    /* Range check (t�y b?n ch?nh l?i theo system) */
    if (pre_time > 60000 || sampling_time > 60000 || post_time > 60000) {
        embeddedCliPrint(cli, "Time value too large");
        return;
    }

    if (sampling_rate == 0 || sampling_rate > 1000) {
        embeddedCliPrint(cli, "Invalid sampling_rate_khz (1-1000)");
        return;
    }

    if (laser_intensity > 1000) {
        embeddedCliPrint(cli, "Invalid laser_intensity (0-1000)");
        return;
    }

    /* Fill profile */
    exp_profile_t profile;
    profile.pre_time_ms = (uint16_t) pre_time;
    profile.main_time_ms = (uint16_t) sampling_time;
    profile.post_time_ms = (uint16_t) post_time;
    profile.sampling_rate_khz = (uint16_t) sampling_rate;
    profile.laser_intensity = (uint16_t) laser_intensity;

    experiment_set_profile(&profile);

    snprintf(buf, sizeof (buf),
            "Profile set: pre=%u ms, sample=%u ms, post=%u ms, rate=%u kHz, laser=%u",
            profile.pre_time_ms,
            profile.main_time_ms,
            profile.post_time_ms,
            profile.sampling_rate_khz,
            profile.laser_intensity);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_DLS_setup(EmbeddedCli *cli, char *args, void *context) {

    const char *channelStr = embeddedCliGetToken(args, 1);
    const char *laserCurrentStr = embeddedCliGetToken(args, 2);
    const char *preStr = embeddedCliGetToken(args, 3);
    const char *sampleStr = embeddedCliGetToken(args, 4);
    const char *postStr = embeddedCliGetToken(args, 5);
    const char *photoRateStr = embeddedCliGetToken(args, 6);
    const char *laserRateStr = embeddedCliGetToken(args, 7);

    char *endptr;
    char buf[256];

    if (channelStr == NULL ||
            laserCurrentStr == NULL ||
            preStr == NULL ||
            sampleStr == NULL ||
            postStr == NULL ||
            photoRateStr == NULL ||
            laserRateStr == NULL) {

        embeddedCliPrint(cli,
                "Usage: dls_setup <channel> <laser_intensity> <pre_ms> <sampling_ms> <post_ms> <photo_rate_ksps> <laser_rate_ksps>");
        return;
    }

    /* Parse channel */
    uint32_t channel = strtoul(channelStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid channel");
        return;
    }

    /* Parse laser current */
    uint32_t laser_intensity = strtoul(laserCurrentStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid laser_intensity");
        return;
    }

    /* Parse pre time */
    uint32_t pre_time = strtoul(preStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid pre_time_ms");
        return;
    }

    /* Parse sampling time */
    uint32_t sampling_time = strtoul(sampleStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid main_time_ms");
        return;
    }

    /* Parse post time */
    uint32_t post_time = strtoul(postStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid post_time_ms");
        return;
    }

    /* Parse photo sampling rate */
    uint32_t photo_rate = strtoul(photoRateStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid photo_rate_ksps");
        return;
    }

    /* Parse laser sampling rate */
    uint32_t laser_rate = strtoul(laserRateStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid laser_rate_ksps");
        return;
    }

    /* Range check */
    if (channel < 0 || channel > 24) {
        embeddedCliPrint(cli, "Invalid channel (1-24)");
        return;
    }

    if (laser_intensity > 100) {
        embeddedCliPrint(cli, "Invalid laser_intensity (0-100)");
        return;
    }

    if (pre_time > 60000 ||
            sampling_time > 60000 ||
            post_time > 60000) {

        embeddedCliPrint(cli, "Time value too large");
        return;
    }

    if (photo_rate == 0 || photo_rate > 5000) {
        embeddedCliPrint(cli, "Invalid photo_rate_ksps");
        return;
    }

    if (laser_rate == 0 || laser_rate > 5000) {
        embeddedCliPrint(cli, "Invalid laser_rate_ksps");
        return;
    }

    /* Fill profile */
    exp_profile_t profile;
    profile.channel = (uint8_t) channel;
    profile.laser_intensity = (uint16_t) laser_intensity;
    profile.pre_time_ms = (uint16_t) pre_time;
    profile.main_time_ms = (uint16_t) sampling_time;
    profile.post_time_ms = (uint16_t) post_time;
    profile.sampling_rate_khz = (uint16_t) photo_rate;
    experiment_set_profile(&profile);

    snprintf(buf, sizeof (buf),
            "DLS setup done: ch=%u, laser=%u %%, pre=%u ms, sample=%u ms, post=%u ms, photo_rate=%u kS/s, laser_rate=%u kS/s",
            (unsigned int) channel,
            (unsigned int) laser_intensity,
            (unsigned int) pre_time,
            (unsigned int) sampling_time,
            (unsigned int) post_time,
            (unsigned int) photo_rate,
            (unsigned int) laser_rate);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}