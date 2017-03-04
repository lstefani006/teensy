/*-stack 0x2000 */
/*-heap 0x2000 */
-e Entry

--diag_suppress=10063

/* SPECIFY THE SYSTEM MEMORY MAP */

MEMORY
{
   CPPI_MEM         : org = 0x01E20000  len = 0x00002000
   SHARED_RAM       : org = 0x80000000  len = 0x00020000
   DDR_MEM          : org = 0xC1080000  len = 0x01F80000
   DDR_MEM_NO_CACHE : org = 0xC3000000  len = 0x01000000
}

/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY */

SECTIONS
{
   .text:Entry   : load > 0xC1080000
   .text         : load > DDR_MEM
   .data         : load > DDR_MEM
   .bss          : load > DDR_MEM
                   RUN_START(bss_start),
                   RUN_END(bss_end)
   .const        : load > DDR_MEM
   .cinit        : load > DDR_MEM
   .sysmem       : load > DDR_MEM
   .stack        : load > 0xC3FF7FFC
   .ram_cppi     : load > CPPI_MEM
   .ram_no_cache : load > DDR_MEM_NO_CACHE
}
