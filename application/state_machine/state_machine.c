#include "state_machine.h"

/* ========== 路径序列表（航向角占位，上赛道实测后填入） ========== */

// TASK1: A → B（左边直线）
static const PathSegment_t path_task1[] = {
    { SEG_STRAIGHT, -90.0f },   // AB 左边直线，航向朝下
};

// TASK2: A → C(上半圆弧) → D(右边直线) → B(下半圆弧) → A(左边直线)
static const PathSegment_t path_task2[] = {
    { SEG_ARC,        0.0f },   // AC 上半圆弧
    { SEG_STRAIGHT, -90.0f },   // CD 右边直线，航向朝下
    { SEG_ARC,        0.0f },   // DB 下半圆弧
    { SEG_STRAIGHT,  90.0f },   // BA 左边直线，航向朝上
};

// TASK3: A → C(对角线直) → B(弧) → D(对角线直) → A(弧)
// 航向角待上赛道实测后填入
static const PathSegment_t path_task3[] = {
    { SEG_STRAIGHT,  0.0f },    // AC 弦直线（航向角实测）
    { SEG_ARC,       0.0f },    // 弧线段
    { SEG_STRAIGHT,  0.0f },    // BD 弦直线（航向角实测）
    { SEG_ARC,       0.0f },    // 弧线段
};

/* ========== 内部状态 ========== */

static QuestionState_t s_current_state = STATE_IDLE;

static const PathSegment_t *s_path;     // 当前任务的路径表指针
static uint8_t              s_seg_count;// 当前路径表的段数
static uint8_t              s_seg_index;// 当前段索引

static uint8_t              s_lap_count;      // 当前圈数（TASK4 专用）
static bool                 s_finished;       // 任务是否完成
static bool                 s_waiting_leave;  // 弦线段：等待先离开起点黑线

/* ========== 按键回调 ========== */

static void Key_Handler(uint8_t key_index)
{
    // 按键 1-4 映射到 TASK1-TASK4
    s_current_state = (QuestionState_t)(key_index + 1);

    // LED 翻转作为按键反馈
    DL_GPIO_togglePins(GPIO_LEDs_PORT, GPIO_LEDs_GPIO_LED_PIN);
}

/* ========== 公共 API ========== */

void StateMachine_Init(void)
{
    Key_RegisterCallback(Key_Handler);
}

QuestionState_t StateMachine_GetState(void)
{
    return s_current_state;
}

void StateMachine_StartTask(QuestionState_t task)
{
    s_finished  = false;
    s_lap_count = 1;
    s_seg_index = 0;
    s_waiting_leave = false;

    switch (task)
    {
        case STATE_TASK1:
            s_path      = path_task1;
            s_seg_count = sizeof(path_task1) / sizeof(path_task1[0]);
            break;
        case STATE_TASK2:
            s_path      = path_task2;
            s_seg_count = sizeof(path_task2) / sizeof(path_task2[0]);
            break;
        case STATE_TASK3:
            s_path      = path_task3;
            s_seg_count = sizeof(path_task3) / sizeof(path_task3[0]);
            break;
        case STATE_TASK4:
            s_path      = path_task3;   // TASK4 = TASK3 × 4 圈
            s_seg_count = sizeof(path_task3) / sizeof(path_task3[0]);
            break;
        default:
            s_path      = NULL;
            s_seg_count = 0;
            break;
    }

    // 如果第一段是直线段，小车可能停在起点黑线上（如 Task3 弦线起点 A）
    // 需要先离开起点黑线，再检测终点黑线
    if (s_path != NULL && s_seg_count > 0
        && s_path[0].type == SEG_STRAIGHT)
    {
        s_waiting_leave = true;
    }
}

SegmentType_t StateMachine_GetCurrentSegment(void)
{
    if (s_finished || s_path == NULL || s_seg_index >= s_seg_count)
        return SEG_STOP;

    return s_path[s_seg_index].type;
}

float StateMachine_GetTargetHeading(void)
{
    if (s_path != NULL && s_seg_index < s_seg_count
        && s_path[s_seg_index].type == SEG_STRAIGHT)
    {
        return s_path[s_seg_index].heading_deg;
    }
    return 0.0f;
}

void StateMachine_SegmentDone(void)
{
    s_seg_index++;
    s_waiting_leave = false;  // 段切换时总是清除（后续段不需要弦线 debounce）
    if (s_seg_index >= s_seg_count)
    {
        if (s_current_state == STATE_TASK4 && s_lap_count < 4)
        {
            s_lap_count++;
            s_seg_index = 0;    // 回到第一段
        }
        else
        {
            s_finished = true;
        }
    }
}

bool StateMachine_IsFinished(void)
{
    return s_finished;
}

uint8_t StateMachine_GetLapCount(void)
{
    return s_lap_count;
}

bool StateMachine_NeedLeaveFirst(void)
{
    return s_waiting_leave;
}

void StateMachine_LeftLine(void)
{
    s_waiting_leave = false;
}
