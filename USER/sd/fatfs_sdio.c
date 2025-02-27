#include <fatfs_sdio.h>
#include <bsp_sd.h>

static volatile DSTATUS Stat = STA_NOINIT;
FIL file_handle;

DSTATUS disk_initialize (BYTE pdrv) {
	DSTATUS stat = STA_NOINIT;
	HAL_StatusTypeDef result;

	switch (pdrv) {
	case DEV_RAM :
		stat = STA_NODISK;
		// translate the reslut code here
		return stat;

	case DEV_MMC :
		result = sd_init();
		if (result == HAL_OK) {
			stat &= ~STA_NOINIT;
		}
		// translate the reslut code here
		return stat;

	case DEV_USB :
		stat = STA_NODISK;
		// translate the reslut code here
		return stat;
	}
	return STA_NOINIT;
}
DSTATUS disk_status (BYTE pdrv) {
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case DEV_RAM :
		stat = STA_NODISK;
		// translate the reslut code here
		return stat;

	case DEV_MMC :
		if (HAL_SD_GetCardState(&SD_HandleStrcut) == HAL_SD_CARD_TRANSFER) {
			stat &= ~STA_NOINIT;
		}
		// translate the reslut code here
		return stat;

	case DEV_USB :
		stat = STA_NODISK;
		// translate the reslut code here
		return stat;
	}
	return stat;
}
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
	DRESULT res = RES_ERROR;
	uint32_t timeout;
	
	RX_Flag = 0;
	
	switch (pdrv) {
	case DEV_RAM :
		// translate the reslut code here
		return RES_ERROR;

	case DEV_MMC :
		if(HAL_SD_ReadBlocks_DMA(&SD_HandleStrcut, buff, sector, count) == HAL_OK) {
			timeout = HAL_GetTick();
			while((RX_Flag == 0) && (HAL_GetTick() - timeout) < SD_TIMEOUT);
			if(RX_Flag == 0) {
				res = RES_ERROR;
			} else {
				RX_Flag = 0;
				timeout = HAL_GetTick();
				while((HAL_GetTick() - timeout) < SD_TIMEOUT) {
					if (HAL_SD_GetCardState(&SD_HandleStrcut) == HAL_SD_CARD_TRANSFER) {
						res = RES_OK;
						break;
					}
				}
			}
		}
		// translate the reslut code here
		return res;

	case DEV_USB :
		// translate the reslut code here
		return RES_ERROR;
	}
	return res;
}
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
	DRESULT res = RES_ERROR;
	uint32_t timeout;
	TX_Flag = 0;

	switch (pdrv) {
	case DEV_RAM :
		// translate the reslut code here
		return RES_ERROR;

	case DEV_MMC :
		if (HAL_SD_WriteBlocks_DMA(&SD_HandleStrcut, (uint8_t*)buff, sector, count) == HAL_OK) {
			timeout = HAL_GetTick();
			while((TX_Flag == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT));
			if (TX_Flag == 0) {
				res = RES_ERROR;
			} else {
				TX_Flag = 0;
				timeout = HAL_GetTick();
				while((HAL_GetTick() - timeout) < SD_TIMEOUT) {
					if (HAL_SD_GetCardState(&SD_HandleStrcut) == HAL_SD_CARD_TRANSFER) {
						res = RES_OK;
						break;
					}
				}
			}
		}
		// translate the reslut code here
		return res;

	case DEV_USB :
		// translate the reslut code here
		return RES_ERROR;
	}
	return res;
}
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff) {
	DRESULT res = RES_ERROR;
	HAL_SD_CardInfoTypeDef card_info;
	//HAL_SD_CardInfoTypeDef.BlockNbr is SECTOR number.
	//HAL_SD_CardInfoTypeDef.BlockSize is SECTOR size.
	//HAL_SD_CardInfoTypeDef.BlockSize and BlockNbr 
	//is actual blocksize and blocknumber of sd card.
	
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	
	switch (pdrv) {
	case DEV_RAM :
		// translate the reslut code here
		return RES_ERROR;

	case DEV_MMC :
		if (HAL_SD_GetCardInfo(&SD_HandleStrcut, &card_info) != HAL_OK) {
			return RES_NOTRDY;
		}
		switch (cmd) {
			case CTRL_SYNC:
				res = RES_OK;
				break;
			case GET_SECTOR_COUNT:
				//LogBlockNbr = BlockNbr * BlockSize / 512 ==
				//LogBlockNbr = actual_card_size / LogBlockSize(512)
				*(DWORD*)buff = card_info.LogBlockNbr;
				res = RES_OK;
				break;
			case GET_SECTOR_SIZE:
				//LogBlockSize is logic block size normally equal with 512.
				*(DWORD*)buff = card_info.LogBlockSize;
				res = RES_OK;
				break;
			case GET_BLOCK_SIZE:
				*(DWORD*)buff = card_info.BlockSize;
				res = RES_OK;
				break;
			default:
				res = RES_PARERR;
		}
		return res;

	case DEV_USB :
		// translate the reslut code here
		return RES_ERROR;
	}
	return res;
}

DWORD get_fattime (void) {
	return 0;
}

FRESULT load(char* file_name, uint32_t* memory_address) {
	FRESULT result = FR_OK;
	UINT load_success_num;
	UINT file_size;
	
	result = f_open(&file_handle, file_name, FA_READ);
	if (result == FR_OK) {
		file_size = f_size(&file_handle);
		result = f_read(&file_handle, memory_address, file_size, &load_success_num);
		if ((result != FR_OK) || (file_size != load_success_num)) {
			return result;
		}
		result = f_close(&file_handle);
	}
	return result;
	/*if (result != FR_OK) {
		printf("sd card mount failed.\r\n");
		while(1);
	} else {
		if (f_open(&file_handle, "1:test.txt", FA_READ) == FR_OK) {
			if (f_read(&file_handle, FONT, 20, &load_success_num) != FR_OK) {
				printf("file open failed.\r\n");
				while(1);
			}
			printf("FONT load success.");
		} else {
			printf("file open failed.\r\n");
			while(1);
		}
	}*/
}
