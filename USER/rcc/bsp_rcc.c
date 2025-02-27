#include <bsp_rcc.h>

void HSE_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q) {
	RCC_OscInitTypeDef Rcc_OscInitStruct;
	RCC_ClkInitTypeDef Rcc_ClkInitStruct;
	
	//__HAL_RCC_PWR_CLK_ENABLE();
	//__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
	Rcc_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	Rcc_OscInitStruct.HSEState = RCC_HSE_ON;
	Rcc_OscInitStruct.LSEState = RCC_LSE_ON;
	Rcc_OscInitStruct.HSIState = RCC_HSI_ON;
	Rcc_OscInitStruct.LSIState = RCC_LSI_ON;
	Rcc_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	Rcc_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	Rcc_OscInitStruct.PLL.PLLM = m;		//div hse.
	Rcc_OscInitStruct.PLL.PLLN = n;		//mul dived hse.
	Rcc_OscInitStruct.PLL.PLLP = p;		//
	Rcc_OscInitStruct.PLL.PLLQ = q;		//div hse for peripheral.
	if (HAL_RCC_OscConfig(&Rcc_OscInitStruct) != HAL_OK) {
		while (1);
	}
	
	//if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
	//	while (1);
	//}
	
	Rcc_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	Rcc_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	Rcc_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	Rcc_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	Rcc_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&Rcc_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK) {
		while(1);
	}
	//for get revision identifier.
	//HAL_GetREVID()
	return;
}
