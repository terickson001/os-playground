ENTRY(_start)
SECTIONS
{
	. = 4K;
	.text :
	{
		code = .; _code = .; __code = .;
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
        . = ALIGN(4096);
	}

	end_kernel = .; _end_kernel = .; __end_kernel = .;
}