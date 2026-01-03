/**
 * buzzer.c
 * 蜂鸣器控制器
 */

#include "buzzer.h"
#include "common.h"

#define BUZZER_PIN 14

/**
 * 初始化蜂鸣器
 */
void buzzer_init(void)
{
	// 配置GPF14为输出
	GPFCON = (GPFCON & ~(3 << 28)) | (1 << 28);

	// 初始状态：关闭蜂鸣器
	buzzer_off();
}

/**
 * 打开蜂鸣器
 */
void buzzer_on(void)
{
	GPFDAT |= (1 << BUZZER_PIN);
}

/**
 * 关闭蜂鸣器
 */
void buzzer_off(void)
{
	GPFDAT &= ~(1 << BUZZER_PIN);
}