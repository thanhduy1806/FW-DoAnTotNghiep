#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>
#include "profile_state.h"
#include "sw_timer.h"

// ─── CẤU TRÚC MÔ HÌNH NHIỆT VẬT LÝ ──────────────────────────────────────────
typedef struct
{
    float current_temp; // Nhiệt độ hiện tại (Celsius)
    float ambient_temp; // Nhiệt độ môi trường (Celsius)
    float dt;           // Bước thời gian (giây)
    float K_heater;     // Hệ số nóng (độ/giây)
    float K_tec;        // Hệ số lạnh (độ/giây)
    float tau;          // Quán tính nhiệt (Time constant)
} ThermalSystem;

// ─── BIẾN TOÀN CỤC MÔ PHỎNG ────────────────────────────────────────────────
static uint8_t last_heater_duty = 0;
static uint8_t last_cooler_duty = 0;
static ThermalSystem v_oven; // Lò nhiệt ảo

// ─── GIẢ LẬP PHẦN CỨNG (MOCK HAL) ──────────────────────────────────────────
void heater_set_duty(ProfileState_t *me, uint8_t duty)
{
    last_heater_duty = duty;
}

void cooler_set_duty(ProfileState_t *me, uint8_t duty)
{
    last_cooler_duty = duty;
}

uint32_t get_system_tick(void)
{
    return (uint32_t)GetTickCount();
}

// ─── HÀM CẬP NHẬT NHIỆT ĐỘ VẬT LÝ ──────────────────────────────────────────
void update_physics_simulation(ProfileState_t *me)
{
    // Chuyển đổi duty từ 0-100 sang 0.0-1.0
    float h_input = last_heater_duty / 100.0f;
    float c_input = last_cooler_duty / 100.0f;

    // 1. Tính toán lượng nhiệt cấp/lấy đi
    float heating = v_oven.K_heater * h_input;
    float cooling = v_oven.K_tec * c_input;

    // 2. Thất thoát nhiệt ra môi trường (Newton's Law of Cooling)
    float loss = (v_oven.current_temp - v_oven.ambient_temp) / v_oven.tau;

    // 3. Phương trình vi phân Euler: T_new = T_old + (dT/dt)*dt
    v_oven.current_temp += (heating - cooling - loss) * v_oven.dt;

    // 4. Đồng bộ hóa với State Machine (Chuyển sang đơn vị 0.01 độ C)
    me->current_main_temp = (int16_t)(v_oven.current_temp * 100.0f);
    me->current_sec_temp = me->current_main_temp + 50; // Sai số nhẹ cho NTC2
}

// ─── CHƯƠNG TRÌNH CHÍNH ───────────────────────────────────────────────────

int main()
{
    // 1. Khởi tạo thông số lò nhiệt vật lý
    v_oven.current_temp = 25.0f;
    v_oven.ambient_temp = 25.0f;
    v_oven.dt = 0.1f;       // Khớp với vòng lặp 100ms
    v_oven.K_heater = 1.2f; // Tăng max 1.2 độ/s
    v_oven.K_tec = 0.5f;    // Giảm max 0.5 độ/s
    v_oven.tau = 60.0f;     // Quán tính nhiệt 60s

    // 2. Cấu hình Profile (Đơn vị nhiệt độ là x100)
    profileData_t myProfile = {
        .set_point = 2800, // Target ban đầu 28.00°C
        .step_count = 3,
        .steps = {
            {.start_point = 2800, .set_point = 4000, .duration = 60, .control_mode = HEATING_MODE},
            {.start_point = 4000, .set_point = 4000, .duration = 30, .control_mode = HOLD_ON_SETPOINT_MODE},
            {.start_point = 4000, .set_point = 2800, .duration = 60, .control_mode = COOLING_MODE},
        }};

    // 3. Khởi tạo State Machine
    ProfileState_t sm = {
        .data = &myProfile,
        .current_state = PROFILE_STATE_DISABLED,
        .current_step = -1,
        .current_main_temp = 2500,
    };

    printf("--- KHOI CHAY MO PHONG LO NHIET VAT LY ---\n");
    printf("Time(s) | State | Setpoint | Temp(C) | Heater%% | Cooler%%\n");
    printf("----------------------------------------------------------\n");

    // 4. Kích hoạt hệ thống
    profileSM_handle_event(&sm, PROFILE_EVENT_ENABLED);

    // 5. Vòng lặp mô phỏng
    for (int i = 0; i < 10000; i++)
    { // Chạy lâu hơn để thấy sự thay đổi

        // if (i == 10)
        // {
        //     profileSM_handle_event(&sm, PROFILE_EVENT_START);
        // }

        // Cập nhật nhiệt độ vật lý (Thay thế hàm cũ)
        update_physics_simulation(&sm);

        // Cập nhật Logic điều khiển
        sw_timer_tick(&sm.timer);

        if (sw_timer_is_expired(&sm.timer))
        {
            profileSM_handle_event(&sm, PROFILE_EVENT_TIMER_EXPIRED);
        }
        else
        {
            profileSM_handle_event(&sm, PROFILE_EVENT_NONE);
        }

        // In kết quả mỗi 1 giây
        if (i % 10 == 0)
        {
            printf("%7.1f | %5d | %8.2f | %7.2f | %7d | %7d\n",
                   i * 0.1f,
                   sm.current_state,
                   sm.pid.setpoint,
                   sm.current_main_temp / 100.0f,
                   last_heater_duty,
                   last_cooler_duty);
        }

        if (sm.current_state == PROFILE_STATE_DISABLED && i > 100)
        {
            printf("----------------------------------------------------------\n");
            printf("MO PHONG KET THUC!\n");
            break;
        }

        Sleep(100); // 100ms real-time
    }

    return 0;
}