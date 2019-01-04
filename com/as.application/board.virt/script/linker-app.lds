MEMORY
{
	RAM			(rwx)	: ORIGIN = 0x40000000, LENGTH = 128M

	PCINP		(rw)	: ORIGIN = 0x10000000, LENGTH = 256M
	PCIP		(rw)	: ORIGIN = 0x3eff0000, LENGTH = 64K
}

ENTRY(_start)
SECTIONS
{
	.init : {
		KEEP(*(.init))
	} > RAM

	.text : {
		. = ALIGN(8);
		*(.text*)
        /* section information for shell */
        . = ALIGN(8);
        __ssymtab_start = .;
        KEEP(*(SSymTab))
        __ssymtab_end = .;
        /* section information for ELF Symbols */
        . = ALIGN(8);
        __elfsymtab_start = .;
        KEEP(*(ELFSymTab))
        __elfsymtab_end = .;
        . = ALIGN(8);
        autostart_processes = .;
        KEEP(*(ProtoSymTab))
        KEEP(*(ProtoSymTabEnd))
	} > RAM
    _etext = .;

	.data : {
		. = ALIGN(8);
		*(.data*)
		. = ALIGN(8);
		*(.rodata*)
	} > RAM

	.bss :
	{
		. = ALIGN(8);
		__bss_start = .;	
		*(.bss)
		*(.bss.*)
	} > RAM
	__bss_end = .;
    _end = .;
	. = ALIGN(8);
	. = . + 0x4000; /* 16kB of stack memory */
	stack_top = .;

	.pcinp :
	{
		*(.pcinp)
		*(.pcinp.*)
	} > PCINP

	.pcip :
	{
		*(.pcip)
		*(.pcip.*)
	} > PCIP
}