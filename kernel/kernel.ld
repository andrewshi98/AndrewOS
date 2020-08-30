ENTRY(kernel_start)

/* Assume the core kernel size does not exceed 2GB (Well, have the freedom to change) */
kernel_base = 0xffffffff80000000;

SECTIONS {
	. = kernel_base;

	kernel_start = .;

	.text : { KEEP(*(.text.kernel_start)) *(.text.*) }
	.data : { *(.data.*) *(.rodata.*) }

	.bss : {
		bss_start = .;

		*(.bss)
		*(.bss.*)
		*(COMMON)

		bss_end = .;
	}

	kernel_end = .;

	/DISCARD/ : { *(.eh_frame) *(.comment) }
}