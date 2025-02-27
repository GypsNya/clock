#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include <stm32f4xx_hal.h>
#include <stm32f429xx.h>
#ifndef __CORE_CM7_H
#define __CORE_CM7_H
#endif
#include <stm32f4xx_hal_cortex.h>

void systick_init(void);
void delay_ms(uint32_t n_times);

void TimingDelay_Decrement(void);

#endif
