#include "stdio.h"
#include "buzzer.h"
#include "common.h"
void timer_init(unsigned long utimer,unsigned long uprescaler,unsigned long udivider,unsigned long utcntb,unsigned long utcmpb);
void reset_tick();

void set_mode(unsigned long m);
void apply_mode();

int main()
{
    buzzer_init();
	timer_init(0,65,4,62500,0);
	GPKCON0 = 0x11110000;
	GPKDATA = 0x000000f0;
	GPNCON = 0;

	int dat = 0;
	while (1)
	{
		dat = GPNDAT;
        if((~dat) & (1<<0)) {
			set_mode(1);
			reset_tick();
        }
		if((~dat) & (1<<1)) {
			set_mode(2);
			reset_tick();
		}
		if((~dat) & (1<<2)) {
			set_mode(3);
			reset_tick();
		}
		if((~dat) & (1<<3)) {
			apply_mode();
			reset_tick();
		}
	}

	return 0;
}
