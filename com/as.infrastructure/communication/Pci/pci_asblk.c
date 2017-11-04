/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
#ifdef USE_PCI
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "pci_core.h"
#include "asdebug.h"
#ifdef USE_FATFS
#include "diskio.h"
#endif

#define AS_LOG_FATFS 0
#define AS_LOG_EXTFS 0
/* Definitions of physical drive number for each drive */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 : default */
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
/* ============================ [ MACROS    ] ====================================================== */
enum {
	IMG_FATFS = 0,
	IMG_EXT4,
};
enum{
	REG_BLKID     = 0x00,
	REG_BLKSZ     = 0x04,
	REG_BLKNBR    = 0x08,
	REG_DATA      = 0x0C,
	REG_LENGTH    = 0x10,
	REG_BLKSTATUS = 0x14,
	REG_CMD       = 0x18,
};
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static pci_dev *pdev = NULL;
static void* __iobase= NULL;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int PciBlk_Init(uint32_t blkid)
{
	imask_t mask;
	if(NULL == __iobase)
	{
		pdev = find_pci_dev_from_id(0xcaac,0x0003);
		if(NULL != pdev)
		{
			__iobase = (void*)(pdev->mem_addr[1]);
			enable_pci_resource(pdev);
		}
	}

	asAssert(__iobase);

	Irq_Save(mask);
	writel(__iobase+REG_BLKID, blkid);
	writel(__iobase+REG_CMD, 0); /* cmd init */
	Irq_Restore(mask);

	return 0;
}

int PciBlk_Read(uint32_t blkid, uint32_t blksz, uint32_t blknbr, uint8_t* data)
{
	uint32_t i;
	imask_t mask;

	asAssert(__iobase);

	Irq_Save(mask);
	writel(__iobase+REG_BLKID, blkid);
	writel(__iobase+REG_BLKSZ, blksz);
	writel(__iobase+REG_BLKNBR, blknbr);
	writel(__iobase+REG_CMD, 1); /* cmd read */
	for(i=0; i < blksz; i++)
	{
		data[i] = readl(__iobase+REG_DATA);
	}
	Irq_Restore(mask);

	return 0;
}

int PciBlk_Write(uint32_t blkid, uint32_t blksz, uint32_t blknbr, uint8_t* data)
{
	uint32_t i;
	imask_t mask;

	asAssert(__iobase);

	Irq_Save(mask);
	writel(__iobase+REG_BLKID, blkid);
	writel(__iobase+REG_BLKSZ, blksz);
	writel(__iobase+REG_BLKNBR, blknbr);

	for(i=0; i < blksz; i++)
	{
		writel(__iobase+REG_DATA,data[i]);
	}

	writel(__iobase+REG_CMD, 2); /* cmd write */
	Irq_Restore(mask);

	return 0;
}

int PciBlk_Size(uint32_t blkid, uint32_t *size)
{
	imask_t mask;

	asAssert(__iobase);

	Irq_Save(mask);
	writel(__iobase+REG_BLKID, blkid);

	*size = readl(__iobase+REG_LENGTH);
	Irq_Restore(mask);

	return 0;
}

#ifdef USE_FATFS
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	ASLOG(FATFS,"%s %d\n",__func__,pdrv);

	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
		stat = RES_OK;
		break;

	case DEV_USB :
		break;
	}
	return stat;
}

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	ASLOG(FATFS,"%s %d\n",__func__,pdrv);
	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		if( 0 == PciBlk_Init(IMG_FATFS))
		{
			stat = 0;
		}
		else
		{
			stat = STA_NODISK;
		}
		break;
	}
	case DEV_USB :
		break;
	}
	return stat;
}

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_PARERR;
	ASLOG(FATFS,"%s %d %d %d\n",__func__,pdrv,sector,count);
	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		res = RES_OK;
		while((RES_OK == res) && (count > 0))
		{
			if(0 != PciBlk_Read(IMG_FATFS,512,sector,buff))
			{
				res = RES_ERROR;
			}
			count --;
			buff += 512;
		};
		break;
	}

	case DEV_USB :
		break;
	}

	return res;
}

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_PARERR;
	ASLOG(FATFS,"%s %d %d %d\n",__func__,pdrv,sector,count);
	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		res = RES_OK;
		while((RES_OK == res) && (count > 0))
		{
			if(0 != PciBlk_Write(IMG_FATFS,512,sector,buff))
			{
				res = RES_ERROR;
			}
			count --;
			buff += 512;
		};
		break;
	}
	case DEV_USB :
		break;
	}

	return res;
}

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;
	ASLOG(FATFS,"%s %d %d\n",__func__,pdrv,cmd);
	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		switch (cmd) {
		case CTRL_SYNC:
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT:
		{
			if(0 == PciBlk_Size(IMG_FATFS,buff))
			{
				*(DWORD*)buff = *(DWORD*)buff/512;
				res = RES_OK;
			}
			break;
		}
		case GET_SECTOR_SIZE:
		{
			*(DWORD*)buff = 512;
			res = RES_OK;
			break;
		}
		case GET_BLOCK_SIZE:
			if(0 == PciBlk_Size(IMG_FATFS,buff))
			{
				res = RES_OK;
			}
			break;
		}
		break;
	}
	case DEV_USB :
		break;
	}

	return res;
}

DWORD get_fattime (void)
{
	return 0;
}

#endif /* USE_FATFS */

#ifdef USE_LWEXT4
void ext_mount(void)
{

}
#endif

#endif /* USE_PCI */
