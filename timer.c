#include "stdio.h"
#include "buzzer.h"

#define PRIORITY 	    	(*((volatile unsigned long *)0x7F008280))
#define SERVICE     		(*((volatile unsigned long *)0x7F008284))
#define SERVICEPEND 		(*((volatile unsigned long *)0x7F008288))
#define VIC0IRQSTATUS  		(*((volatile unsigned long *)0x71200000))
#define VIC0FIQSTATUS  		(*((volatile unsigned long *)0x71200004))
#define VIC0RAWINTR    		(*((volatile unsigned long *)0x71200008))
#define VIC0INTSELECT  		(*((volatile unsigned long *)0x7120000c))
#define VIC0INTENABLE  		(*((volatile unsigned long *)0x71200010))
#define VIC0INTENCLEAR 		(*((volatile unsigned long *)0x71200014))
#define VIC0PROTECTION 		(*((volatile unsigned long *)0x71200020))
#define VIC0SWPRIORITYMASK 	(*((volatile unsigned long *)0x71200024))
#define VIC0PRIORITYDAISY  	(*((volatile unsigned long *)0x71200028))
#define VIC0ADDRESS        	(*((volatile unsigned long *)0x71200f00))

#define		PWMTIMER_BASE			(0x7F006000)
#define		TCFG0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x00)) )
#define		TCFG1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x04)) )
#define		TCON      	( *((volatile unsigned long *)(PWMTIMER_BASE+0x08)) )
#define		TCNTB0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x0C)) )
#define		TCMPB0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x10)) )
#define		TCNTO0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x14)) )
#define		TCNTB1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x18)) )
#define		TCMPB1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x1C)) )
#define		TCNTO1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x20)) )
#define		TCNTB2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x24)) )
#define		TCMPB2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x28)) )
#define		TCNTO2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x2C)) )
#define		TCNTB3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x30)) )
#define		TCMPB3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x34)) )
#define		TCNTO3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x38)) )
#define		TCNTB4    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x3C)) )
#define		TCNTO4    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x40)) )
#define		TINT_CSTAT 	( *((volatile unsigned long *)(PWMTIMER_BASE+0x44)) )

typedef void (isr) (void);
extern void asm_timer_irq();

volatile unsigned long tick = 0;

volatile unsigned long buzzer_state = 0;
volatile unsigned long duration_ns = 10;
volatile unsigned long duration_we = 10;
volatile unsigned long mode = 0;
volatile unsigned long tmp_mode = 0;
volatile unsigned long feedback = 0;

void reset_tick() {
    TCON &= ~(1<<0);

    tick = 0;
    TCON |= (1<<1);
    TCON &= ~(1<<1);
    TCON |= (1<<0) | (1<<3);

    unsigned long uTmp = TINT_CSTAT;
    TINT_CSTAT = uTmp | (1<<0);
    VIC0ADDRESS = 0x0;
}

void set_mode(unsigned long m) {
    GPKDATA = 0xff;
    buzzer_off();
    tmp_mode = m;
    feedback = 1;
}

void apply_mode() {
    mode = tmp_mode;
    if(mode == 1) {
        duration_ns = 20;
        duration_we = 20;
    } else if(mode == 2) {
        duration_ns = 10;
        duration_we = 10;
    } else {
        duration_ns = 5;
        duration_we = 20;
    }
}

void irq_init(void)
{
	/* 在中断控制器里使能timer0中断 */
	VIC0INTENABLE |= (1<<23);

	VIC0INTSELECT =0;

	isr** isr_array = (isr**)(0x7120015C);

	isr_array[0] = (isr*)asm_timer_irq;

	/*将GPK4-GPK7配置为输出口*/
	GPKCON0 = 0x11110000;
	
	/*熄灭四个LED灯*/
	GPKDATA = 0xff;
}

// timer0中断的中断处理函数
void do_irq()
{
	unsigned long uTmp;
    unsigned long led_occupied = 0;
    if(feedback) {
        if(tmp_mode == 1) {
            if(tick<80) {
                if((tick/20)&1) {
                    GPKDATA = 0xff;
                } else {
                    GPKDATA = 0x0f;
                }
                led_occupied = 1;
            } else {
                led_occupied = 0;
                feedback = 0;
            }
        } else if(tmp_mode == 2) {
            if(tick<40) {
                if((tick/10)&1) {
                    buzzer_off();
                } else {
                    buzzer_on();
                }
            } else {
                buzzer_off();
                feedback = 0;
            }
        } else if(tmp_mode==3) {
            if(tick < 20) {
                if(tick == 0) {
                    buzzer_on();
                }
                if(tick<7) {
                    if(tick&1) {
                        GPKDATA = 0x0f;
                    } else {
                        GPKDATA = 0xff;
                    }
                }
                led_occupied = 1;
            } else {
                buzzer_off();
                led_occupied = 0;
                feedback = 0;
            }
        }
    }
    if (!led_occupied){
        unsigned long T_NS = duration_ns+7;
        unsigned long T_WE = duration_we+7;
        // 由于灯本来是亮着的，如果以六刻记，实际上第一刻亮会与先前的亮连续，只能观察到三灭两亮，两次闪烁
        // 故我这里定义 灭 - 亮 - 灭 - 亮 - 灭 - 亮 - 灭 共七刻为三次闪烁
        const unsigned long T = T_WE + T_NS;
        unsigned tmp_tick = tick;
        while(tmp_tick>=T) {
            tmp_tick-=T;
        }
        if(!feedback) {
            tick = tmp_tick;
        }
        if(tmp_tick<T_NS) {
            if(tmp_tick >= duration_ns) {
                if(tmp_tick&1) {
                    GPKDATA = 0x9f;
                } else {
                    GPKDATA = 0xff;
                }
            } else {
                GPKDATA = 0x9f;
            }
        } else {
            if(tmp_tick-T_NS >= duration_we) {
                if((tmp_tick-T_NS)&1) {
                    GPKDATA = 0x6f;
                } else {
                    GPKDATA = 0xff;
                }
            } else {
                GPKDATA = 0x6f;
            }
        }
    }

    tick++;
    uTmp = TINT_CSTAT;
    TINT_CSTAT = uTmp;
    VIC0ADDRESS=0x0;
}

// 初始化timer
void timer_init(unsigned long utimer,unsigned long uprescaler,unsigned long udivider,unsigned long utcntb,unsigned long utcmpb)
{
	unsigned long temp0;

	// 定时器的输入时钟 = PCLK / ( {prescaler value + 1} ) / {divider value} = PCLK/(65+1)/16=62500hz

	//设置预分频系数为66
	temp0 = TCFG0;
	temp0 = (temp0 & (~(0xff00ff))) | (uprescaler<<0);
	TCFG0 = temp0;

	// 16分频
	temp0 = TCFG1;
	temp0 = (temp0 & (~(0xf<<4*utimer))& (~(1<<20))) |(udivider<<4*utimer);
	TCFG1 = temp0;

	// 1s = 62500hz
	TCNTB0 = utcntb;
	TCMPB0 = utcmpb;

	// 手动更新
	TCON |= 1<<1;

	// 清手动更新位
	TCON &= ~(1<<1);

	// 自动加载和启动timer0
	TCON |= (1<<0)|(1<<3);

	// 使能timer0中断
	temp0 = TINT_CSTAT;
	temp0 = (temp0 & (~(1<<utimer)))|(1<<(utimer));
	TINT_CSTAT = temp0;
}


