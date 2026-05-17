#include "key.h"

// 外部变量
extern uint8_t state;

// 按键硬件配置数组，移植后需要根据实际情况修改
static const struct {
    int pin;
    KeyActiveLevel_t active_level;
} key_config[KEY_NUM] = {
    {GPIO_KEYs_KEY1_PIN, KEY_ACTIVE_HIGH},
    {GPIO_KEYs_KEY2_PIN, KEY_ACTIVE_HIGH},
    {GPIO_KEYs_KEY3_PIN, KEY_ACTIVE_HIGH},
    {GPIO_KEYs_KEY4_PIN, KEY_ACTIVE_HIGH}
};

// 按键数组
Key_t keys[KEY_NUM];

static void Key_TickHandler(void);


/**
 * @brief 按键模块初始化函数
 * 
 * 该函数完成以下工作：
 * 
 * 1. 遍历所有按键，从配置数组中读取引脚和有效电平信息
 * 
 * 2. 初始化每个按键的状态为空闲状态
 * 
 * 3. 清零消抖计数器和触发标志
 * 
 * 4. 注册PIT定时器Tick回调函数用于按键扫描
 */
void Key_Init(void)
{
    // 初始化所有按键的配置
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        keys[i].pin = key_config[i].pin;
        keys[i].active_level = key_config[i].active_level;
        keys[i].state = KEY_STATE_IDLE;
        keys[i].debounce_cnt = 0;
        keys[i].flag = 0;
    }

    // 注册定时器Tick回调函数，实现周期性按键扫描
    PIT_Control_Tick_RegisterCallback(Key_TickHandler);
}


/**
 * @brief 读取按键的逻辑状态
 * 
 * 根据按键配置的极性（高电平有效或低电平有效），将GPIO原始电平转换为逻辑值。
 * 
 * @param key_index 按键索引，用于从keys数组中获取对应按键的配置信息
 * @return uint8_t 按键的逻辑状态：1表示按键按下，0表示按键未按下
 */
static uint8_t Key_ReadLogic(uint8_t key_index)
{
    // 读取GPIO原始电平
    uint32_t raw_level = DL_GPIO_readPins(KEY_PORT, keys[key_index].pin);
    
    // 根据配置的极性进行转换
    if (keys[key_index].active_level == KEY_ACTIVE_LOW)
    {
        // 低电平有效
        return !raw_level ? 1 : 0;
    }
    else
    {
        // 高电平有效
        return raw_level ? 1 : 0;
    }
}


/**
 * @brief 按键扫描与状态机处理函数
 * 
 * 该函数实现基于有限状态机的按键扫描逻辑，完成以下工作：
 * 
 * 1. 遍历所有按键，读取其逻辑电平状态（统一转换为1=按下，0=未按下）
 * 
 * 2. 根据当前按键状态执行相应的状态转移：
 * 
 *    - KEY_STATE_IDLE: 检测按键按下动作，通过连续多次检测实现消抖
 * 
 *    - KEY_STATE_PRESSED: 等待按键释放，通过连续多次检测确认释放
 * 
 *    - KEY_STATE_RELEASED: 保持触发标志位，等待主程序读取清除
 * 
 * 3. 使用消抖计数器（KEY_DEBOUNCE_COUNT）过滤机械抖动干扰
 * 
 * 4. 在按键从按下到释放的完整过程结束后，置位flag标志供外部查询
 * @note 按键触发标志需要通过 Key_GetFlag() 函数读取并自动清除
 */
void Key_Scan(void)
{
    for (uint8_t i = 0; i < KEY_NUM; i++) {
        // 获取统一的逻辑电平
        uint8_t logic_level = Key_ReadLogic(i);
        
        switch (keys[i].state)
        {
            case KEY_STATE_IDLE:
                // 空闲状态：检测是否有按下动作
                if (logic_level == 1)
                {
                    keys[i].debounce_cnt++;
                    if (keys[i].debounce_cnt >= KEY_DEBOUNCE_COUNT)
                    {
                        // 连续检测到按下，进入按下状态
                        keys[i].state = KEY_STATE_PRESSED;
                        keys[i].debounce_cnt = 0;
                    }
                }
                else
                {
                    keys[i].debounce_cnt = 0;
                }
                break;
                
            case KEY_STATE_PRESSED:
                // 按下确认状态：等待按键释放
                if (logic_level == 0)
                {
                    keys[i].debounce_cnt++;
                    if (keys[i].debounce_cnt >= KEY_DEBOUNCE_COUNT)
                    {
                        // 连续检测到释放，进入释放状态
                        keys[i].state = KEY_STATE_RELEASED;
                        keys[i].flag = 1;      // 置位触发标志
                        keys[i].debounce_cnt = 0;
                    }
                }
                break;
                
            case KEY_STATE_RELEASED:
                // 已触发状态：保持标志位，直到主程序通过 Key_GetFlag 读取并清除
                break;
                
            default:
                // 异常状态复位
                keys[i].state = KEY_STATE_IDLE;
                keys[i].debounce_cnt = 0;
                break;
        }
    }
}


/**
 * @brief 获取按键触发标志并清除状态
 * 
 * 该函数用于查询指定按键是否被触发，如果检测到按键触发事件，
 * 则自动清除标志位并重置按键状态为空闲。
 * 
 * @param key_index 按键索引（范围：0 ~ KEY_NUM-1）
 * @return uint8_t 按键触发标志值
 * 
 * - !0：表示按键已被触发
 * 
 * - 0：表示按键未被触发或索引无效
 */
uint8_t Key_GetFlag(uint8_t key_index)
{
    // 边界检测
    if (key_index >= KEY_NUM)
    {
        return 0;
    }
    
    uint8_t flag = keys[key_index].flag;
    if (flag)
    {
        // 清除标志并重置状态，准备下一次检测
        keys[key_index].flag = 0;
        keys[key_index].state = KEY_STATE_IDLE;
    }
    return flag;
}


/**
 * @brief 按键定时处理回调函数
 * 
 * 该函数作为PIT定时器的Tick回调函数，周期性执行按键扫描和处理逻辑。
 * 
 * 当检测到按键触发时，更新全局state变量为按键编号（索引+1）
 * 
 * @note 此函数无参数和返回值，由定时器系统自动调用
 * @note state变量为外部全局变量，用于向其他模块传递按键事件信息
 */
static void Key_TickHandler(void)
{
    Key_Scan();
    for (int i = 0; i < KEY_NUM; i++)
    {
        if (Key_GetFlag(i))
        {
            DL_GPIO_togglePins(GPIO_LEDs_PORT, GPIO_LEDs_GPIO_LED_PIN);
            state = i + 1;
        }
    }
}