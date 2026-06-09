#include "pid.h"
#include "M1_SysApp/xlog/xlog.h"

void  pid_init   (PID_t *p, const PID_instance_t *init, const float current_measurement) {
    p->kp = init->kp;  p->ki = init->ki;  p->kd = init->kd;
    p->out_min = init->out_min;  p->out_max = init->out_max;
    p->dt = init->dt;
    p->prev_measurement = current_measurement;
    pid_reset(p);
}

void pid_reset(PID_t *p) {
    p->integral   = 0.0f;
    p->prev_error = 0.0f;
    p->setpoint   = 0.0f;
}

float pid_compute(PID_t *p, float measurement) {
    float error = p->setpoint - measurement;

    float P_out = p->kp * error;

    p->integral += error * p->dt;

    float D_out = 0.0f;
    if (p->dt > 0.0f) {
        D_out = -p->kd * (measurement - p->prev_measurement) / p->dt;
    }

    float out = P_out + (p->ki * p->integral) + D_out;

    if (out > p->out_max) {
        out = p->out_max;
        p->saturated = 1;
        
        if (error > 0.0f) {
            p->integral -= error * p->dt;
        }
    } 
    else if (out < p->out_min) {
        out = p->out_min;
        p->saturated = 2;
        
        if (error < 0.0f) {
            p->integral -= error * p->dt;
        }
    } 
    else {
        p->saturated = 0;
    }

    p->prev_measurement = measurement;
    p->prev_error = error;
    p->output = out;
    return out;
}
