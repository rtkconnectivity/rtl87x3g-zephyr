/* #if DT_NODE_HAS_STATUS(DT_NODELABLE(trace), okay) */
GROUP_START(TRACE)

	SECTION_PROLOGUE(REALTEK_LOG_TRACE,,SUBALIGN(4))
	{
		__trace_start = .;
		KEEP(* (.TRACE_HEADER))
		*(.TRACE)
		*(".TRACE.*")
		__trace_end = .;
	} GROUP_LINK_IN(TRACE)

GROUP_END(TRACE)
/* #endif */
