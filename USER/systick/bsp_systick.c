#include <bsp_systick.h>
#include <bsp_led.h>

static __IO uint32_t TimingDelay;

void systick_init(void) {
	if (HAL_SYSTICK_Config(SystemCoreClock / 1000) == 1) {
		//"HAL_SYSTICK_Config" return 0 mean config success, return 1 mean config failed.
		while(1) {
			//led1 turn on mean, systick config fault.
			led(led1, led_on)
		}
	}
}
void delay_ms(uint32_t n_times) {
	TimingDelay = n_times;
	while (TimingDelay != 0);
}

void TimingDelay_Decrement(void) {
	if (TimingDelay != 0) {
		TimingDelay--;
	}
}
