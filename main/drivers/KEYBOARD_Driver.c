#include "KEYBOARD_Driver.h"

/**
 * @brief 用户毫秒延时
 * @param time 延时时间 (毫秒)
*/
void Delay_ms(uint8_t time)
{
    vTaskDelay(time / portTICK_PERIOD_MS);
}

/**
 * @brief SC12B启动信号
*/
void SC12B_IIC_START(void)
{
    SC12B_IIC_SDA_OUT;
    SC12B_IIC_SCL_OUT;
    SC12B_IIC_SDA_H;
    SC12B_IIC_SCL_H;
    Delay_ms(1);
    SC12B_IIC_SDA_L;
    Delay_ms(1);
    SC12B_IIC_SCL_L;
    Delay_ms(1);
}

/**
 * @brief SC12B停止信号
*/
void SC12B_IIC_STOP(void)
{
    SC12B_IIC_SCL_L;
    SC12B_IIC_SDA_OUT;
    SC12B_IIC_SDA_L;
    Delay_ms(1);
    SC12B_IIC_SCL_H;
    Delay_ms(1);
    SC12B_IIC_SDA_H;
}

/**
 * @brief 主到从应答
*/
void SC12B_IIC_ACK(uint8_t x)
{
    SC12B_IIC_SCL_L;
    SC12B_IIC_SDA_OUT;
    if (x == 1)
    {
        SC12B_IIC_SDA_H;
    }else{
        SC12B_IIC_SDA_L;
    }
    Delay_ms(1);
    SC12B_IIC_SCL_H;
    Delay_ms(1);
    SC12B_IIC_SCL_L;
}

/**
 * @brief 读取一个字节数据 并获取应答
*/
uint8_t SC12B_IIC_SendByteAndGetNACK(uint8_t data)
{
    SC12B_IIC_SDA_OUT;
    for (int i = 0; i < 8; i++)
    {
        SC12B_IIC_SCL_L;
        Delay_ms(1);
        if (((data >> 7) & 0b00000001) == 1)
        {
            SC12B_IIC_SDA_H;
        }
        else
        {
            SC12B_IIC_SDA_L;
        }
        Delay_ms(1);
        SC12B_IIC_SCL_H;
        Delay_ms(1);
        data <<= 1;
    }
    SC12B_IIC_SCL_L;
    Delay_ms(3);
    SC12B_IIC_SDA_IN;
    Delay_ms(3);
    SC12B_IIC_SCL_H;
    Delay_ms(1);
    for (int i = 250; i > 0; i--)
    {
        if (SC12B_IIC_SDA_READ == 0)
        {
            SC12B_IIC_SCL_L;
            return 0;
        }
    }
    SC12B_IIC_SCL_L;
    return 1;
}


/**
 * @brief 读取SC12B寄存器值
 * @return 0xFF 错误值
*/
uint8_t SC12B_IIC_ReadKey(void)
{
    SC12B_IIC_START();
    if (SC12B_IIC_SendByteAndGetNACK((SC12B_ADDR << 1) | 0b00000001) == 1)
    {
        /// 如果接收应答出错
        SC12B_IIC_STOP();
        return 0xFF;
    }
    SC12B_IIC_SDA_IN;
    /// 轮询到的SC12B的通道号
    uint8_t i = 0;
    /// 保存按键值
    uint8_t key_content = 0;
    while (i < 8)
    {
        i += 1;
        SC12B_IIC_SCL_L;
        Delay_ms(1);
        SC12B_IIC_SCL_H;
        /// 如果之前没有读到过按键值，且读取的SDA电平是高电平
        /// 则读取按键值
        if (key_content == 0 && SC12B_IIC_SDA_READ == 1)
        {
            key_content = i;
        }
        Delay_ms(1);
    }
    // 读取按键值成功
    if (key_content != 0)
    {
        SC12B_IIC_ACK(1);
        SC12B_IIC_STOP();
        return key_content;
    }
    SC12B_IIC_ACK(0);
    SC12B_IIC_SDA_IN;
    /// 如果通道D7~D0没有读取到按键值，则继续轮询按键值
    while (i < 16)
    {
        i += 1;
        SC12B_IIC_SCL_L;
        Delay_ms(1);
        SC12B_IIC_SCL_H;
        if (key_content == 0 && SC12B_IIC_SDA_READ == 1)
        {
            key_content = i;
        }
        Delay_ms(1);
    }
    SC12B_IIC_ACK(1);
    SC12B_IIC_STOP();
    return key_content;
}

/**
 * @brief 键盘返回值校准 (按照用户实际情况修改)
*/
uint8_t SC12B_ADJUST_KEY(void)
{
    uint8_t key_content = SC12B_IIC_ReadKey();
    if (key_content == 1) return 1;
    else if (key_content == 2) return 2;
    else if (key_content == 3) return 3;
    else if (key_content == 4) return 4;
    else if (key_content == 5) return 5;
    else if (key_content == 6) return 6;
    else if (key_content == 7) return 7;
    else if (key_content == 8) return 8;
    else if (key_content == 9) return 9;
    else if (key_content == 11) return 0;
    else if (key_content == 10) return 'M';
    else if (key_content == 12) return '#';
    else return 0xFF;
}

/**
 * @brief GPIO初始化
*/
void SC12B_IIC_Init(void)
{
    gpio_config_t io_conf;
    /// 通信数据脚
    io_conf.intr_type = GPIO_INTR_DISABLE;  // 禁用中断
    io_conf.mode = GPIO_MODE_OUTPUT;        // 输出模式
    io_conf.pin_bit_mask = ((1ULL << SC12B_IIC_SCL) | (1ULL << SC12B_IIC_SDA));  // 选择引脚
    io_conf.pull_down_en = 0;   // 禁用下拉
    io_conf.pull_up_en = 1;     // 使能上拉
    gpio_config(&io_conf);

    /// 中断脚
    io_conf.intr_type = GPIO_INTR_POSEDGE;  // 上升沿中断
    io_conf.mode = GPIO_MODE_INPUT;         // 输入模式
    io_conf.pin_bit_mask = (1ULL << SC12B_IIC_INT); // 选择引脚
    gpio_config(&io_conf);
}
