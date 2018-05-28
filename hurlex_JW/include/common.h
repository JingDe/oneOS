/*
 *	显卡的显示控制单元，编址在独立的IO空间，使用in/out指令读写
 *	控制寄存器，数量多，不能一一映射到IO端口的地址空间
 *	使用一个IO端口0x3D4作为控制寄存器的索引
 *	通过另一个端口0x3D5设置该寄存器的值
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include"types.h"

// 端口写一个字节
void outb(uint16_t port, uint8_t value);

// 端口读一个字节
uint8_t inb(uint16_t port);

// 端口读一个字
uint16_t inw(uint16_t port);

// 开启中断
void enable_intr();

// 关闭中断
void disable_intr();

#endif
