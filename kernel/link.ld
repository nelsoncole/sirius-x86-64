/**
 *
 *	File Name: link.ld
 **/
OUTPUT_FORMAT("elf64-x86-64")
ENTRY(_start)
/*SECTIONS
{
	. = 0x401000;
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
}*/
SECTIONS
{
	. = 0x401000;
    _start = .;
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
 
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

    end = .; _end = .; __end = .;
}
