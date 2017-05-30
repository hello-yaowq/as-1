/**
 * BlueThunder Linker Script for the raspberry Pi!
 *
 *
 *
 **/
MEMORY
{
	RESERVED	(r)		: ORIGIN = 0x00000000, LENGTH = 32K
	FLASH    	(rwx)	: ORIGIN = 0x00030000, LENGTH = 64K
}

ENTRY(FlashHeader)

SECTIONS
{
    .text :
    {
        *(.rodata.FlashHeader)
        *(.text*)
        *(.rodata*)
    } > FLASH
}
