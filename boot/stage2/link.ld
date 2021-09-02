/**
 *
 *	File Name: link.ld
 **/
OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)
SECTIONS
{
	. = 0x11000;
	_start = .;
	
	.text :
	{ 
		code = .; _code = .; __code = .;
		code = .;
		*(.text)
		. = ALIGN(4096);
	}
	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		*(COMMON)
		. = ALIGN(4096);
	}
	
	.eh_frame :
	{
		eh_frame = .;
		*(.eh_frame)
		. = ALIGN(4096);
	}

	end = .; _end = .; __end = .;
}
