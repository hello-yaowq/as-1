/* Linker script to configure memory regions. 
 * Need modifying for a specific board. 
 *   FLASH.ORIGIN: starting address of flash
 *   FLASH.LENGTH: length of flash
 *   RAM.ORIGIN: starting address of RAM bank 0
 *   RAM.LENGTH: length of RAM bank 0
 */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
MEMORY
{
    /* All is DRAM space */
    FLASH (rx)  : ORIGIN = 0x30000000, LENGTH = 5M  
    RAM  (rwx)  : ORIGIN = 0x30500000, LENGTH = 5M 
	IRAM (rwx)  : ORIGIN = 0x40000000, LENGTH = 4K  
}

ENTRY(_start)

knl_system_stack_size = 4096;

SECTIONS
{
	.text :
	{
	    *(.startup*)
		*(.text*)
		__ctors_start__ = .;
		*(.ctors*)
		__ctors_end__ = .;
		*(.rodata*)
	} > FLASH

	__etext = .;
		
	.data : AT (__etext)
	{
		. = ALIGN(4);
		__data_start__ = .;
		*(.data*)
		__data_end__ = .;
	} > RAM

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM
	
	.init_stack ALIGN(16) (NOLOAD) : 
	{	
	   . = . + 1024;  
	    _irq_stack_start = .;
		. = . + 1024; 	
		_fiq_stack_start = .; 
        . = . + 512;   
        _undefined_stack_start = .;
        . = . + 512;   
        _aboknl_stack_start = .;
        . = . + 4096; 
        _svc_stack_start = .;
       knl_system_stack    = .; 
       . = . + knl_system_stack_size;   
       knl_system_stack_top = .;
	} > RAM
}
