#include "db_temperature_profile.h"
#include "stdio.h"

const profileData_t t_profile_init = {
    .enabled = 0,
    .start = 0,
    .main_ntc = 0,
    .sec_ntc = 1,
    .set_point = 2500, /* 25.00*C */
    .main_sec_delta = 50, /* 5.00*C */
    .steps =
    {
        {0}
    },
    .step_count = 0
};

static profileData_t temp_profiles[PROFILE_MAX_NUM];

profileData_t * DB_temp_profile_get_pointer(uint8_t index) {
    return &temp_profiles[index];
}

void DB_temp_profile_init(void) {
    for (uint32_t i = 0; i < PROFILE_MAX_NUM; i++) {
        temp_profiles[i] = t_profile_init;
    }

}

void DB_temp_profile_write(uint32_t index, const profileData_t *t_profile) {
    if (index < PROFILE_MAX_NUM) {
        temp_profiles[index] = *t_profile;
    }
}

void DB_temp_profile_read(uint32_t index, profileData_t *t_profile) {
    if (index < PROFILE_MAX_NUM) {
        *t_profile = temp_profiles[index];
    }
}

void DB_temp_profile_start(uint32_t index) {
    if (index < PROFILE_MAX_NUM) {
        temp_profiles[index].enabled = 1;
    }
}

void DB_temp_profile_stop(void) {
    for (uint32_t i = 0; i < PROFILE_MAX_NUM; i++) {
        temp_profiles[i].enabled = 0;
    }
}

void DB_temp_profile_start_steps(uint32_t index) {
    if (index < PROFILE_MAX_NUM) {
        temp_profiles[index].start = 1;
    }
}

void DB_temp_profile_stop_steps(void) {
    for (uint32_t i = 0; i < PROFILE_MAX_NUM; i++) {
        temp_profiles[i].start = 0;
    }
}

void DB_temp_profile_display(EmbeddedCli *cli, uint8_t pf_idx) {

    if (pf_idx >= PROFILE_MAX_NUM) {
        embeddedCliPrint(cli, "[ERROR] Invalid profile index.");
        return;
    }

    const profileData_t *pf = &temp_profiles[pf_idx];
    char buf[128];

    // ===== Header =====
    snprintf(buf, sizeof (buf),
            "====== Profile %u ======",
            pf_idx);
    embeddedCliPrint(cli, buf);

    // ===== Basic info =====
    snprintf(buf, sizeof (buf),
            "Enabled        : %u",
            pf->enabled);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf),
            "Start          : %u",
            pf->start);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf),
            "Main NTC       : %u",
            pf->main_ntc);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf),
            "Sec NTC        : %u",
            pf->sec_ntc);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf),
            "TEC mask       : %u",
            pf->tec_mask);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf),
            "Heater mask    : %u",
            pf->heater_mask);
    embeddedCliPrint(cli, buf);
    // nhi?t ?? d?ng x100 ? chia 100.0
    snprintf(buf, sizeof (buf),
            "Setpoint       : %.2f *C",
            pf->set_point / 100.0f);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf),
            "Main-Sec Delta : %.2f *C",
            pf->main_sec_delta / 10.0f);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof (buf),
            "Step Count     : %u",
            pf->step_count);
    embeddedCliPrint(cli, buf);

    // ===== Steps =====
    if (pf->step_count == 0) {
        embeddedCliPrint(cli, "[INFO] No steps configured.");
    } else {

        for (uint8_t i = 0; i < pf->step_count; i++) {

            const step_t *s = &pf->steps[i];

            snprintf(buf, sizeof (buf),
                    "Step[%u]: start=%.2f*C, set=%.2f*C, dur=%ds, mode=%d",
                    i,
                    s->start_point / 100.0f,
                    s->set_point / 100.0f,
                    s->duration,
                    s->control_mode);

            embeddedCliPrint(cli, buf);
        }
    }

    embeddedCliPrint(cli, "========================\r\n");
}

Std_ReturnType DB_temp_profile_validation(EmbeddedCli *cli) {
    Std_ReturnType ret = ERROR_OK;
    char buf[96];

    for (uint8_t i = 0; i < PROFILE_MAX_NUM; i++) {
        // skip profile unused
        if (temp_profiles[i].tec_mask == 0 && temp_profiles[i].heater_mask == 0) {
            continue;
        }

        for (uint8_t j = i + 1; j < PROFILE_MAX_NUM; j++) {
            if (temp_profiles[j].tec_mask == 0 && temp_profiles[j].heater_mask == 0) {
                continue;
            }

            // ===== TEC MASK =====
            uint32_t tec_conflict = temp_profiles[i].tec_mask & temp_profiles[j].tec_mask;

            if (tec_conflict) {
                ret = ERROR_FAIL;
                snprintf(buf, sizeof (buf), "[ERROR] TEC mask conflict: profile %u & %u (mask=0x%X)",
                        i, j, tec_conflict);
                embeddedCliPrint(cli, buf);
            }

            // ===== HEATER MASK =====
            uint32_t heater_conflict =
                    temp_profiles[i].heater_mask & temp_profiles[j].heater_mask;
            if (heater_conflict) {
                ret = ERROR_FAIL;
                snprintf(buf, sizeof (buf), "[ERROR] Heater mask conflict: profile %u & %u (mask=0x%X)",
                        i, j, heater_conflict);
                embeddedCliPrint(cli, buf);
            }
        }
    }

    if (ret == ERROR_OK) {
        embeddedCliPrint(cli, "[OK] All profiles are valid.");
    }

    return ret;
}