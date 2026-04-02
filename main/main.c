#include <stdio.h>
#include "drivers/KEYBOARD_Driver.h"

/// 队列句柄指针
static QueueHandle_t gpio_event_queue = NULL;   // 存储产生中断的GPIO

/// 中断回调
static void IRAM_ATTR gpio_interrupt_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;  // 产生回调时, arg作为产生中断的GPIO引脚号
    xQueueSendFromISR(gpio_event_queue, &gpio_num, NULL);   // 将GPIO引脚号添加到队列gpio_event_queue
}

/// 将下面的函数注册为freertos task, 用来轮询
static void process_gpio_interrupt(void *arg)
{
    uint32_t gpio_num;
    for (;;)
    {
        /// 如果队列中有GPIO中断事件, 存储GPIO引脚号至gpio_num
        if (xQueueReceive(gpio_event_queue, &gpio_num, portMAX_DELAY))
        {
            if (gpio_num == SC12B_IIC_INT)  // 判断中断引脚号
            {
                uint8_t key_num = SC12B_ADJUST_KEY();  // 读取按键值并输出
                printf("press key: %d\r\n", key_num);
            }
        }
        Delay_ms(100);
    }
}

static void ISR_QUEUE_Init(void)
{
    gpio_event_queue = xQueueCreate(10, sizeof(uint32_t));  // 创建一个10元素队列, 用于存储中断事件
    xTaskCreate(process_gpio_interrupt, "process_gpio_interrupt", 2048, NULL, 10, NULL);    // 注册任务, 分配2048栈, 优先级10
    gpio_install_isr_service(0);    // 开启默认中断服务
    gpio_isr_handler_add(SC12B_IIC_INT, gpio_interrupt_handler, (void *)SC12B_IIC_INT); // 针对SC12B_IIC_INT添加回调函数
}

void app_main(void) // idf入口函数, 该名称被注册成一个任务
{
    SC12B_IIC_Init();
    ISR_QUEUE_Init();
}
