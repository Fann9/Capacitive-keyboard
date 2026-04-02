## 功能演示：

![7SC3sOZb_converted_compressed_compressed.gif](https://image.lceda.cn/oshwhub/pullImage/24045831eb224cce975c05940a4c27ef.gif)

(我无法找到更好的工具使我的gif压缩至3M以下，如果你有办法，请帮助我)
## 项目简介
本项目通过SC12B芯片，实现了触摸电容键盘的功能，输出包含IIC，BCD数字，BCD模拟

## 项目功能
该键盘可以代替传统的按键键盘，可以通过替换灵敏度电路来自定义灵敏度(详细说明需查看芯片手册说明)，通过IIC等协议读取按键寄存器，释放主控引脚硬件资源，为更多的实际项目提供拓展

## 原理解析（硬件说明）

本项目由以下部分组成，电源部分，地址选择部分，触摸电容按键部分，通信接口部分。通过检测触摸电容值的变化，输出对应的逻辑信号至通信接口。

![image.png](https://image.lceda.cn/oshwhub/pullImage/317c5982f5374e479b754a4b1009f344.png)

图1--电源电路：
电源电路采用简单的接口直流供电，接入两颗滤波电容过滤噪声，提供干净的电源。
SC12B支持宽幅电压输入，输入电压为2.5~6.0V。
* ( 这里需要注意，接入的电压会影响IIC等接口的输出电压，需要与MCU的GPIO最大输入电压进行比对，数据读取错误，甚至烧毁MCU！)
* ( 例如ESP32的GPIO标准高电平为3.3，则键盘的输入供电接入3.3V即可正常通信 )

![image.png](https://image.lceda.cn/oshwhub/pullImage/9656ffe92b1f4b29a43d90bc701332d9.png)

图2--IIC通信选址：
SC12B通过ASEL的引脚输入状态来改变按键寄存器地址。
如图所示，拉高时IIC地址为0x4，拉低时IIC地址为0x42，悬空时IIC地址为0x40。
* ( 实际焊接时，选址用的电阻焊接一个即可三个都焊会引起电源短路！)

![image.png](https://image.lceda.cn/oshwhub/pullImage/4ba4d5d8dc39434b836262042a4c65cb.png)
![image.png](https://image.lceda.cn/oshwhub/pullImage/885e47b9d3404b2dbd040e3758c94e46.png)

图3--触摸电容按键：
按照芯片手册说明，触摸PAD离铺地 1.5MM 以上，同时正背面镂空。
触摸信号线离铺地距离保持在 15mil 以上，且相邻触摸信号线之间也要尽量保持在 15mil 以上，避免产生串扰。
如果要修改触摸按键，需要遵守以上规则。按键实际面积大小与灵敏度成正比

## 软件代码
这里提供基于ESP_IDF示例代码，需idf编译烧录
代码中的以下部分需要按照用户的实际情况进行修改
(文件位于main\drivers\KEYBOARD_Driver.h)
代码仅包含触摸按键部分，并有着较为完善的注释，用户可以根据具体需求进行代码的移植。
```
/*** 电容键盘用户参数部分 ***/
/// IIC通信引脚
#define SC12B_IIC_SCL GPIO_NUM_1    // SCL
#define SC12B_IIC_SDA GPIO_NUM_2    // SDA
/// 中断引脚
#define SC12B_IIC_INT GPIO_NUM_3    // Interrupt
/// 设备地址
#define SC12B_ADDR 0x40
/*** 电容键盘用户参数部分 ***/
```
代码仓库:<br>
GitHub: [Capacitive-keyboard](https://github.com/Fann9/Capacitive-keyboard)<br>
Gitee: [SC12B电容键盘](https://gitee.com/fan-an9/Capacitive-keyboard)<br>
立创开源: [SC12B触控电容键盘](https://oshwhub.com/eda_adbdjngo/project_fhqpqlfg)


## 注意事项
* 首次上电前请先检查电源是否短路
* 重置地址后需要键盘完全下电后重启(电源指示灯熄灭)
* 电源指示灯会根据接入的电压值而产生不同的亮度，如果指示灯较暗为正常现象
* BCD提供模拟与数字输出，与IIC输出可同时工作。选用模拟输出时需要严格注意电阻的选型

## If it makes help to you, please click stars.
代码参考视频
https://www.bilibili.com/video/BV1jHnRzJEdw?spm_id_from=333.788.player.switch&amp;vd_source=2e36a7d02dc3171caf247a94e1848e69
