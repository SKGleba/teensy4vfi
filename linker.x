/* 
	linker "inspired" by teensy4 core's linker
		https://github.com/PaulStoffregen/cores/blob/master/teensy4/imxrt1062.ld
*/
MEMORY
{
	ITCM (rwx): ORIGIN = 0x00000000, LENGTH = 512K
    DTCM (rwx): ORIGIN = 0x20000000, LENGTH = 512K
	OCRAM (rwx): ORIGIN = 0x20200000, LENGTH = 512K
	FLASH (rwx): ORIGIN = 0x60000000, LENGTH = 1984K
}

ENTRY(some_other_header)

SECTIONS
{
	.boot : {
		*(.boot.header)
		FILL(0xFF)
		. = ORIGIN(FLASH) + 0x1000;
		*(.boot.some_other_header)
		*(.boot*)
		. = ALIGN(1024);
	} > FLASH

	.itcm : {
		*(.text.vectors)
		*(.text.glitch_funcs)
		*(.text*)
		. = ALIGN(4);
	} > ITCM  AT > FLASH

    .dtcm : {
		*(.rodata*)
		*(.data*)
		. = ALIGN(4);
	} > DTCM  AT > FLASH

	.bss ALIGN(4) : {
		cfg_prog_bss_start = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(32);
		cfg_prog_bss_end = .;
	} > DTCM

	/* set ITCM, DTCM and OCRAM banks */
	cfg_prog_itcm_block_count = (SIZEOF(.itcm) + 0x7FFF) >> 15;
	cfg_prog_flexram_bank_config = 0xAAAAAAAA | ((1 << (cfg_prog_itcm_block_count * 2)) - 1);

	/* for stage2's copy_sections */
	cfg_prog_itcm_flash_start = LOADADDR(.itcm);
	cfg_prog_itcm_flash_size = SIZEOF(.itcm);
    cfg_prog_itcm_flash_end = cfg_prog_itcm_flash_start + cfg_prog_itcm_flash_size;
	cfg_prog_dtcm_flash_start = LOADADDR(.dtcm);
	cfg_prog_dtcm_flash_size = SIZEOF(.dtcm);
    cfg_prog_dtcm_flash_end = cfg_prog_dtcm_flash_start + cfg_prog_dtcm_flash_size;

	/* set SP to end of DTCM */
	cfg_prog_sp_addr = ORIGIN(DTCM) + ((16 - cfg_prog_itcm_block_count) << 15);

	/* overall size for flashcfg */
    cfg_prog_size = SIZEOF(.boot) + SIZEOF(.itcm) + SIZEOF(.dtcm);
}
