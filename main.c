#include "board.h"

int state;

int main(void)
{
    SYSCFG_DL_init();
    Board_Init();

    uint32_t last_imu    = 0;
    uint32_t last_output = 0;

    while (1)
    {
        uint32_t now = Board_GetTickMs();

        // 每 2ms 执行一次 IMU 采样 + 姿态解算
        if (now - last_imu >= 2)
        {
            last_imu = now;
            IMU_Update();
        }

        // 每 10ms 输出一次姿态数据
        if (now - last_output >= 10)
        {
            last_output = now;

            float q0, q1, q2, q3;
            float roll, pitch, yaw;
            float accel[3];
            IMU_GetQuaternion(&q0, &q1, &q2, &q3);
            IMU_GetEuler(&roll, &pitch, &yaw);
            IMU_GetAccel(accel);

            UART_Handle *uart = Board_GetUART();
            UART_Printf(uart,
                        "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                        q0, q1, q2, q3,
                        roll, pitch, yaw,
                        accel[0], accel[1], accel[2]);
        }
    }
}
