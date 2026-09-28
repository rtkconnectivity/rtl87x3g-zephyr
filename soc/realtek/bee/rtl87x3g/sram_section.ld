#if DT_NODE_HAS_STATUS(DT_NODELABEL(sram), okay)
GROUP_START(SRAM)

	SECTION_DATA_PROLOGUE(SRAM_DATA,,)
	{
		. = ALIGN(4);
		__sram_data_start = .;
		*(.sram_data)
		*(.sram_text)
		*(".sram_data*")
		*(".sram_text*")
		__sram_data_end = ALIGN(4);
	} GROUP_DATA_LINK_IN(SRAM, ROMABLE_REGION)

	SECTION_DATA_PROLOGUE(SRAM_BSS,(NOLOAD),)
	{
		. = ALIGN(4);
		__sram_bss_start = .;
		*(.sram_bss)
		*(".sram_bss*")
		__sram_bss_end = ALIGN(4);
	} GROUP_DATA_LINK_IN(SRAM, SRAM)

	__sram_data_load_start = LOADADDR(SRAM_DATA);

GROUP_END(SRAM)
#endif
