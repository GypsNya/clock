#include <bsp_fsmc.h>

SDRAM_HandleTypeDef sdram_handler;

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	UNUSED(hsdram);
	
	sdram_clk_en();
	sdram_port_clk_en();
	
	GPIO_InitStruct.Pin = sdram_c_port_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = sdram_d_port_pin;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = sdram_e_port_pin;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = sdram_f_port_pin;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = sdram_g_port_pin;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

void sdram_init(void) {
	FMC_SDRAM_TimingTypeDef FMC_SDRAM_TimingStruct;
	
	sdram_handler.Instance = FMC_SDRAM_DEVICE;
	sdram_handler.Init.SDBank = FMC_SDRAM_BANK1;
	sdram_handler.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
	sdram_handler.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
	sdram_handler.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
	sdram_handler.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
	sdram_handler.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
	sdram_handler.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	sdram_handler.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
	sdram_handler.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
	sdram_handler.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
	
	FMC_SDRAM_TimingStruct.LoadToActiveDelay = 2;
	FMC_SDRAM_TimingStruct.ExitSelfRefreshDelay = 7;
	FMC_SDRAM_TimingStruct.SelfRefreshTime = 6;
	FMC_SDRAM_TimingStruct.RowCycleDelay = 6;
	FMC_SDRAM_TimingStruct.WriteRecoveryTime = 2;
	FMC_SDRAM_TimingStruct.RPDelay = 2;
	FMC_SDRAM_TimingStruct.RCDDelay = 2;
	
	HAL_SDRAM_Init(&sdram_handler, &FMC_SDRAM_TimingStruct);
	sdram_init_seq(&sdram_handler);
	HAL_SDRAM_ProgramRefreshRate(&sdram_handler, 683);
}

void sdram_init_seq(SDRAM_HandleTypeDef *hsdram) {
	UNUSED(hsdram);
	sdram_send_cmd(1, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);
	delay_ms(1);
	sdram_send_cmd(1, FMC_SDRAM_CMD_PALL, 1, 0);
	sdram_send_cmd(1, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 1, 0);
	
	sdram_send_cmd(
		1,
		FMC_SDRAM_CMD_LOAD_MODE,
		1,
		SDRAM_MODEREG_BURST_LENGTH_8 |
		SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
		SDRAM_MODEREG_CAS_LATENCY_3 |
		SDRAM_MODEREG_OPERATING_MODE_STANDARD |
		SDRAM_MODEREG_WRITEBURST_MODE_SINGLE
	);
}

HAL_StatusTypeDef sdram_send_cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval) {
	uint32_t bank = 0;
	FMC_SDRAM_CommandTypeDef Command;
	
	if (bankx == 1) bank = FMC_SDRAM_CMD_TARGET_BANK1;
	else if (bankx == 2) bank = FMC_SDRAM_CMD_TARGET_BANK2;
	else if (bankx == 3) bank = FMC_SDRAM_CMD_TARGET_BANK1_2;
	else return HAL_ERROR;
	
	Command.CommandMode = cmd;
	Command.CommandTarget = bank;
	Command.AutoRefreshNumber = refresh;
	Command.ModeRegisterDefinition = regval;
	return HAL_SDRAM_SendCommand(&sdram_handler, &Command, 0x1000);
}
