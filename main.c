#include "board.h"
#include "state_machine.h"
#include "pid.h"

/* ========== PID 控制周期（ms），需和测速窗口对齐 ========== */
#define PID_DT_MS  60

int main(void)
{
    SYSCFG_DL_init();
    Board_Init();
    StateMachine_Init();

    DL_Timer_setCaptureCompareValue(SERVO_PWM_INST, 3000, SERVO_PWM_CHANNEL);

    /* ---- PID 初始化 ---- */
    PID_Controller speed_pid;
    PID_Init(&speed_pid, 2.0f, 1.2f, 0.0f, 500.0f, MOTOR_MAX_SPEED_MM_S);
    PID_SetTarget(&speed_pid, 1500.0f);  // 目标速度 mm/s，按需修改

    uint32_t last_pid    = 0;
    uint32_t last_output = 0;


    while (1)
    {
        uint32_t now = Board_GetTickMs();

        /* ---- PID 速度控制（每 PID_DT_MS 一次） ---- */
        if (now - last_pid >= PID_DT_MS)
        {
            last_pid = now;

            float speed1 = Motor_GetFilteredSpeed1();
            float speed2 = Motor_GetFilteredSpeed2();
            float avg_speed = (speed1 + speed2) * 0.5f;

            float output = PID_Compute(&speed_pid, avg_speed);
            Motor_SetSpeed(output);
        }

        /* ---- UART 遥测（每 100ms 输出，方便调参） ---- */
        if (now - last_output >= 100)
        {
            last_output = now;

            // FireWater: prefix:ch0,ch1,...\r\n
            // ch: Target, AvgSpeed, Output, Speed1, Speed2, RPM1, RPM2
            UART_Printf(Board_GetUART(),
                "%.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f\n",
                speed_pid.target,
                (Motor_GetFilteredSpeed1() + Motor_GetFilteredSpeed2()) * 0.5f,
                speed_pid.output,
                Motor_GetFilteredSpeed1(),
                Motor_GetFilteredSpeed2(),
                Motor_GetEncoder1RPM(),
                Motor_GetEncoder2RPM());
        }
    }
}