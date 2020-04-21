ENTRY(_start)
SECTIONS
{
	. = 4K;
 
	.text BLOCK(4K) : ALIGN(4K)
	{
        code = .;
		*(.text)
	}
 
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	.data BLOCK(4K) : ALIGN(4K)
	{
		data = .;
		*(.data)
	}
 
	.bss BLOCK(4K) : ALIGN(4K)
	{
        sbss = .;
		*(.bss)
	}

	end_kernel = .;
}
