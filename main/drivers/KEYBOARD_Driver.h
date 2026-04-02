#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/*** 电容键盘用户参数部分 ***/
/// IIC通信引脚
#define SC12B_IIC_SCL GPIO_NUM_1    // SCL
#define SC12B_IIC_SDA GPIO_NUM_2    // SDA
/// 中断引脚
#define SC12B_IIC_INT GPIO_NUM_3    // Interrupt
/// 设备地址
#define SC12B_ADDR 0x40
/*** 电容键盘用户参数部分 ***/

/// 设置SDA输入
#define SC12B_IIC_SDA_IN gpio_set_direction(SC12B_IIC_SDA, GPIO_MODE_INPUT) 
/// 设置SDA输出
#define SC12B_IIC_SDA_OUT gpio_set_direction(SC12B_IIC_SDA, GPIO_MODE_OUTPUT)
/// 设置SCL输出
#define SC12B_IIC_SCL_OUT gpio_set_direction(SC12B_IIC_SCL, GPIO_MODE_OUTPUT)

/// 拉高SCL
#define SC12B_IIC_SCL_H gpio_set_level(SC12B_IIC_SCL, 1)
/// 拉低SCL
#define SC12B_IIC_SCL_L gpio_set_level(SC12B_IIC_SCL, 0)

/// 拉高SDA
#define SC12B_IIC_SDA_H gpio_set_level(SC12B_IIC_SDA, 1)
/// 拉低SDA
#define SC12B_IIC_SDA_L gpio_set_level(SC12B_IIC_SDA, 0)
/// 读SDA
#define SC12B_IIC_SDA_READ gpio_get_level(SC12B_IIC_SDA)

void SC12B_IIC_Init(void);          // 键盘初始化
uint8_t SC12B_ADJUST_KEY(void);     // 键盘值修正
void Delay_ms(uint8_t);             // 用户延时

#endif
