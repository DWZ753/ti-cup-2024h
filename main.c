#include "board.h"
#include "state_machine.h"
#include "pid.h"
#include "angle.h"
#include "tracking.h"

/* ========== 控制周期（ms） ========== */
#define IMU_UPDATE_DT_MS      10
#define ANGLE_PID_DT_MS       80

/* ========== 丢线防抖 ========== */
#define LINE_LOST_DEBOUNCE     5    // 连续 N 次丢线才算真丢线

/* ========== 固定速度（开环，不需要速度环 PID） ========== */
#define SPEED_ARC          1200.0f  // 弧线段慢速，防止冲出黑线
#define SPEED_STRAIGHT     1800.0f  // 直线段快速，拉高平均速度

/* ========== 循迹 PID 参数 ========== */
#define TRACKING_KP          2.0f
#define TRACKING_KI          0.0f
#define TRACKING_KD          0.5f
#define TRACKING_INT_LIMIT  50.0f
#define TRACKING_OUT_LIMIT 100.0f   // 循迹舵机可满幅

int main(void)
{
    SYSCFG_DL_init();
    Board_Init();
    StateMachine_Init();

    /* ---- 循迹 PID 初始化（弧线段使用） ---- */
    PID_Controller tracking_pid;
    PID_Init(&tracking_pid,
             TRACKING_KP, TRACKING_KI, TRACKING_KD,
             TRACKING_INT_LIMIT, TRACKING_OUT_LIMIT);
    PID_SetTarget(&tracking_pid, 0.0f);  // 目标：黑线居中

    /* ---- 角度 PID 初始化（直线段使用） ---- */
    Angle_Init();

    /* ---- 时序变量 ---- */
    uint32_t last_imu       = 0;
    uint32_t last_angle_pid = 0;
    uint32_t last_output    = 0;

    /* ---- 状态机变化跟踪 ---- */
    QuestionState_t last_state = STATE_IDLE;

    /* ---- 丢线防抖计数器 ---- */
    uint8_t lost_debounce = 0;

    char buf[128];

    while (1)
    {
        uint32_t now = Board_GetTickMs();

        /* ======== IMU 姿态更新（每 IMU_UPDATE_DT_MS） ======== */
        if (now - last_imu >= IMU_UPDATE_DT_MS)
        {
            last_imu = now;
            IMU_Update();
        }

        /* ======== 按键检测 & 任务启动 ======== */
        QuestionState_t st = StateMachine_GetState();
        if (st != last_state)
        {
            last_state = st;
            if (st != STATE_IDLE)
            {
                StateMachine_StartTask(st);
                lost_debounce = 0;
            }
            else
            {
                Motor_Brake();
            }
        }

        /* ======== 当前段类型 → 控制模式 ======== */
        SegmentType_t seg = StateMachine_GetCurrentSegment();
        uint8_t mask = Grayscale_ReadAll();
        bool on_line = (mask != 0xFF);  // 任意传感器检测到黑线

        if (seg == SEG_ARC)
        {
            /* 循迹模式：灰度 → 循迹 PID → 舵机 */
            Angle_Enable(false);
            Motor_SetSpeed(SPEED_ARC);

            float position = Tracking_CalcPosition(mask);
            if (position != 99.0f)  // 未丢线
            {
                float steering = PID_Compute(&tracking_pid, position);
                Servo_SetValue((int32_t)steering);
            }
        }
        else if (seg == SEG_STRAIGHT)
        {
            /* 角度保持模式：IMU → 角度 PID → 舵机 */
            Angle_Enable(true);
            Angle_SetTarget(StateMachine_GetTargetHeading());
            Motor_SetSpeed(SPEED_STRAIGHT);
        }
        else  // SEG_STOP
        {
            Angle_Enable(false);
            Motor_Brake();
        }

        /* ======== 角度 PID 定时计算 ======== */
        if (now - last_angle_pid >= ANGLE_PID_DT_MS)
        {
            last_angle_pid = now;
            Angle_Compute();
        }

        /* ======== 段切换检测 ======== */
        if (seg == SEG_ARC)
        {
            if (!on_line)
            {
                // 弧线段：黑线消失 → 丢线防抖
                if (++lost_debounce >= LINE_LOST_DEBOUNCE)
                {
                    lost_debounce = 0;
                    StateMachine_SegmentDone();
                }
            }
            else
            {
                lost_debounce = 0;  // 线还在，清零防抖
            }
        }
        else if (seg == SEG_STRAIGHT)
        {
            if (on_line)
            {
                // 直线段：检测到黑线 → 到达顶点
                if (StateMachine_NeedLeaveFirst())
                {
                    // 弦线段：还在起点黑线上，忽略
                }
                else
                {
                    StateMachine_SegmentDone();
                }
            }
            else
            {
                // 已离开黑线，通知状态机（弦线段 debounce）
                if (StateMachine_NeedLeaveFirst())
                {
                    StateMachine_LeftLine();
                }
                lost_debounce = 0;
            }
        }

        /* ======== 终点停车 ======== */
        if (StateMachine_IsFinished())
        {
            Motor_Brake();
            Buzzer_Beep(500);
        }

        /* ======== 显示（每 100ms） ======== */
        if (now - last_output >= 100)
        {
            last_output = now;

            //oled显示当前状态
            sprintf(buf, "State %d", StateMachine_GetState());
            OLED_ShowString(32, 3, buf, 16);
            sprintf(buf, "Lap %d", StateMachine_GetLapCount());
            OLED_ShowString(32, 5, buf, 16);

            float roll, pitch, yaw;
            IMU_GetEuler(&roll, &pitch, &yaw);

            UART_Printf(Board_GetUART(),
                "%d, %.1f, %.1f, %ld, %d, %d, %.1f, %.1f, %.1f\n",
                seg,                                    // 当前段类型
                Angle_GetTarget(),                      // 目标航向（°）
                Angle_GetPIDOutput(),                   // 角度 PID 输出
                (int32_t)Angle_GetServoValue(),          // 实际舵机值
                on_line,                                // 是否检测到黑线
                lost_debounce,                         // 丢线防抖计数
                roll, pitch, yaw);
        }
    }
}
