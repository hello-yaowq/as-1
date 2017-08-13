ENTRY(_start)

SECTIONS {
/* Entry point of Tinix
 * It must be as same as 'KernelEntryPointPhyAddr' in load.inc!!!
 */
    . = 0x400400;
	.init : {
		KEEP(*(.init))
	}

	.text : {
		*(.text*)
	}
    _etext = .;

	.data : {
		*(.data*)
	}

	.bss :
	{
		__bss_start = .;	
		*(.bss)
		*(.bss.*)
	}
	__bss_end = .;
    _end = .;

}

