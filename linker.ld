ENTRY(_start)
SECTIONS
{
	. = 4K;
 
	/* Executable code */
	.text BLOCK(4K) : ALIGN(4K)
	{
        text_start = .;
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
        end_data = .;
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
        sbss = .;
		*(COMMON)
		*(.bss)
        ebss = .;
        end_kernel = .;
	}
}
