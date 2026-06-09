#ifndef PID_H
#define PID_H
#include <stdint.h>

typedef struct {
    float kp, ki, kd;
    float setpoint;
    float integral;
    float prev_error;
    float output;
    float out_min, out_max;
    float prev_measurement; // Dùng cho thành phần D
    float dt;               /* sample time (s) */
    uint8_t saturated;        /* saturation flag, 0 mean no sat, 1 mean positive saturation, 2 mean negative saturation */
} PID_t;

typedef struct {
    float kp, ki, kd;
    float out_min, out_max;
    float dt;               /* sample time (s) */
} PID_instance_t;

void  pid_init   (PID_t *p, const PID_instance_t *init, const float current_measurement);
void  pid_reset  (PID_t *p);
float pid_compute(PID_t *p, float measurement);

#endif /* PID_H */