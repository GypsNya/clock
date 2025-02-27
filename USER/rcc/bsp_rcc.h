#ifndef __BSP_RCC_H
#define __BSP_RCC_H

#include <stm32f4xx_hal.h>

#ifndef __CORE_CM7_H
#define __CORE_CM7_H
#endif

#include <stm32f429xx.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_pwr_ex.h>
#include <stm32f4xx_hal_flash_ex.h>

void HSE_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q);

#endif
