#ifndef __BSP_GEEK_429_H
#define __BSP_GEEK_429_H

#include <bsp_fsmc.h>
#include <stdint.h>
//#include <jpg.h>

//peripheral base address.
#define PERIPHERAL_ADDRESS	sdram_addr

//peripheral map.
#define VRAM_OFFSET			0
#define FONT_OFFSET			0x4B0000
#define FONT_EN_OFFSET		0x6B0000
#define FONT_72_OFFSET		0x8B0000
#define FONT_EN_TTF_OFFSET	0xEB0000
#define PIC_OFFSET			0x1108000
#define PAGE_OFFSET			0x110D000

//peripheral map.
#define VRAM_BASE			(uint32_t)PERIPHERAL_ADDRESS
#define VRAM1_BASE			(uint32_t)PERIPHERAL_ADDRESS
#define VRAM2_BASE			(uint32_t)(PERIPHERAL_ADDRESS + VRAM1_MEM_SIZE)
#define FONT_BASE			(uint32_t)(PERIPHERAL_ADDRESS + FONT_OFFSET)
#define FONT_EN_BASE		(uint32_t)(PERIPHERAL_ADDRESS + FONT_EN_OFFSET)
#define FONT_72_BASE		(uint32_t)(PERIPHERAL_ADDRESS + FONT_72_OFFSET)
#define FONT_EN_TTF_BASE	(uint32_t)(PERIPHERAL_ADDRESS + FONT_EN_TTF_OFFSET)
#define FONT_TTF_CACHE_BASE	(uint32_t)(PERIPHERAL_ADDRESS + FONT_EN_TTF_OFFSET + FONT_EN_TTF_MEM_SIZE)
#define PIC_BASE			(uint32_t)(PERIPHERAL_ADDRESS + PIC_OFFSET)
#define MEM_PAGE_DIV_BASE	(uint32_t)(PERIPHERAL_ADDRESS + PAGE_OFFSET)

//peripheral memory size.
#define VRAM_MEM_SIZE			0x4B0000
#define VRAM1_MEM_SIZE			0x258000
#define VRAM2_MEM_SIZE			0x258000
#define FONT_MEM_SIZE			0x200000
#define FONT_EN_MEM_SIZE		0x200000
#define FONT_72_MEM_SIZE		0x600000
	#define FONT_EN_TTF_MEM_SIZE		0xC000
	#define FONT_TTF_CACHE_MEM_SIZE		0x5F4000
#define PICTURE_MEM_SIZE		0x5000
#define MEM_PAGE_DIV_SIZE		0xEF3000

//peripherals.
#define VRAM					((uint32_t*) VRAM_BASE)
#define VRAM1					((uint32_t*) VRAM_BASE)
#define VRAM2					((uint32_t*)(VRAM_BASE + VRAM1_MEM_SIZE))
#define FONT					((char*) FONT_BASE)
#define FONT_EN					((char*) FONT_EN_BASE)
#define FONT_72					((char*) FONT_72_BASE)
#define FONT_EN_TTF				((uint8_t*) FONT_EN_TTF_BASE)
#define FONT_TTF_CACHE			((uint8_t*) FONT_TTF_CACHE_BASE)
#define PIC						((uint32_t*) PIC_BASE)
#define MEM_PAGE				((uint32_t*) MEM_PAGE_DIV_BASE)


//page
#define page_map		((PageMapTypeDef*)PAGE_MAP_START)
#define page_info_map	((PageInfoMapTypeDef*)PAGE_INFO_MAP_START)
//
#define PAGE_SIZE				0x1000
#define PAGE_NUM				(THEORY_PAGE_NUM - PAGE_HEAD_SIZE / PAGE_SIZE)
#define THEORY_PAGE_NUM			(MEM_PAGE_DIV_SIZE / PAGE_SIZE)

#define MEM_PAGE_INFO_SIZE		32

//PAGE_HEAD
#define PAGE_MAP_SIZE			(((32 - (THEORY_PAGE_NUM % 32)) + THEORY_PAGE_NUM/32)*4)
#define PAGE_INFO_MAP_SIZE		PAGE_MAP_SIZE
#define PAGE_INFO_TAB_SIZE		(THEORY_PAGE_NUM * MEM_PAGE_INFO_SIZE)

#define PAGE_HEAD_SIZE			((PAGE_SIZE - (PAGE_MAP_SIZE + PAGE_INFO_MAP_SIZE + PAGE_INFO_TAB_SIZE)%PAGE_SIZE) + (PAGE_MAP_SIZE + PAGE_INFO_MAP_SIZE + PAGE_INFO_TAB_SIZE))

//PAGE_BASE
#define PAGE_MAP_START			MEM_PAGE_DIV_BASE
#define PAGE_INFO_MAP_START		(MEM_PAGE_DIV_BASE + PAGE_MAP_SIZE)
#define PAGE_INFO_TAB_START		(MEM_PAGE_DIV_BASE + PAGE_MAP_SIZE + PAGE_MAP_SIZE)
#define PAGE_START				((PAGE_SIZE - (PAGE_MAP_SIZE + PAGE_INFO_MAP_SIZE + PAGE_INFO_TAB_SIZE)%PAGE_SIZE) + (PAGE_MAP_SIZE + PAGE_INFO_MAP_SIZE + PAGE_INFO_TAB_SIZE) + MEM_PAGE_DIV_BASE)
//#define PAGE_START				
//#define page_info_size	((uint32_t)0x20)
//#define extend		((MemPageExtendTypeDef*)(MEM_PAGE_DIV_BASE + MEM_PAGE_DIV_SIZE))

typedef unsigned int HANDLE;

typedef struct {
	uint32_t	PageMap[PAGE_MAP_SIZE/4];
} PageMapTypeDef;

typedef struct {
	uint32_t	PageInfoMap[PAGE_INFO_MAP_SIZE/4];
} PageInfoMapTypeDef;

typedef struct {
	uint16_t	handle[512];
} HandleMapTypeDef;

typedef struct {
	uint16_t	front_page_def_pos;
	uint16_t	behind_page_def_pos;
	uint16_t	page_used_num;
	uint16_t	page[13];
}MemPageInfoTypeDef;

typedef enum {
	ALLOC_OK = 0,
	ALLOC_FAILED,
	PAGE_FULL,
	PAGE_INFO_FULL,
	PAGE_FREE_OK,
	PAGE_FREE_FAILED,
	PAGE_FREE_ERROR,
	ERR_SIZE,
} MemAllocStatusTypeDef;

typedef enum {
	READ_OK = 0,
	WRITE_OK,
	OPERATION_OK,
	READ_ERR,
	WRITE_ERR,
	OPERATION_ERR,
	READ_FLOW,
	WRITE_FLOW,
}MemOperationStatusTypeDef;

void mem_page_init(void);

MemAllocStatusTypeDef mem_page_alloc(uint16_t* handle, uint32_t size);
MemAllocStatusTypeDef mem_page_free(uint16_t handle);

uint32_t size_of(uint16_t handle);
MemOperationStatusTypeDef mov_out(uint16_t handle, char* src_addr, char* des_addr, uint32_t size);
MemOperationStatusTypeDef mov_in(uint16_t handle, char* src_addr, char* des_addr, uint32_t size);

#endif
