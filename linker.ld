ENTRY(_start)
SECTIONS
{
	. = 1M;
 
	.boot :
	{
		*(.multiboot)
	}

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
		edata = .;
	}
 
	.bss BLOCK(4K) : ALIGN(4K)
	{
        sbss = .;
		*(.bss)
		ebss = .;
	}

	end_kernel = .;
}
