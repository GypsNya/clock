#ifndef __FATFS_SDIO_H
#define __FATFS_SDIO_H

#include <stm32f4xx.h>

#include <diskio.h>
#include <ff.h>

DSTATUS sd_initialize(BYTE lu);
DSTATUS sd_status(BYTE lu);
DRESULT sd_read(BYTE lun, BYTE* buff, DWORD sector, UINT count);
DRESULT sd_write(BYTE lun, const BYTE* buff, DWORD sector, UINT count);
DRESULT sd_ioctl(BYTE lun, BYTE cmd, void* buff);

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);

FRESULT load(char* file_name, uint32_t* memory_address);

#endif
