#if DT_NODE_HAS_STATUS(DT_NODELABEL(dsp_share_ram), okay)
GROUP_START(DSPRAM)

	SECTION_DATA_PROLOGUE(DSPRAM_DATA,,)
	{
		. = ALIGN(4);
		__dspram_data_start = .;
		*(.dspram_data)
		*(.dspram_text)
		*(".dspram_text*")
		*(".dspram_data*")
		__dspram_data_end =  ALIGN(4);
	} GROUP_DATA_LINK_IN(DSPRAM, ROMABLE_REGION)

	SECTION_DATA_PROLOGUE(DSPRAM_BSS,(NOLOAD),)
	{
		. = ALIGN(4);
		__dspram_bss_start = .;
		*(.dspram_bss)
		*(".dspram_bss*")
		__dspram_bss_end = ALIGN(4);
	} GROUP_DATA_LINK_IN(DSPRAM, DSPRAM)

	__dspram_data_load_start = LOADADDR(DSPRAM_DATA);

GROUP_END(DSPRAM)
#endif
