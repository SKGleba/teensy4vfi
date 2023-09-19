#
# stage 2 : minimal setup, flash=>ram & jump to reset
#

.syntax unified
.thumb
.cpu cortex-m7

.section .boot.s

.global s_BOOT
.thumb_func
s_BOOT:
    ldr r0,=0x400AC000;                     # IOMUXC_GPR
    ldr r1,=cfg_prog_flexram_bank_config;   # set RAM banks : ITCM, DTCM, OCRAM
1:                                          # init mem stuff
    str r1,[r0, #0x44]
    movs r1, #0x7;                          # enable ITCM & DTCM, use FLEXRAM_BANK_CFG
    str r1,[r0, #0x40]
    mov r1,#0xaa0000;                       # ITCM and DTCM = 512KB, kinda stupid
    str r1,[r0, #0x38]
2:                                          # prep args for cpy [isz] @ ccode => ITCM
    ldr r0,=(cfg_prog_itcm_flash_start - 4)
    ldr r1,=cfg_prog_itcm_flash_end
    mvn r2, #0x3
3:                                          # memcpy
    ldr r3, [r0, #0x4]!
    str r3, [r2, #0x4]!
    cmp r0, r1
    bne 3b
4:                                          # check if already copied data
    ldr r1,=cfg_prog_dtcm_flash_end
    cmp r0, r1
    beq 7f
5:                                          # check if there even is data to copy
    ldr r0,=cfg_prog_dtcm_flash_size
    cmp r0,#0
    beq 7f
6:                                          # prep args for cpy [dsz] @ data => DTCM
    ldr r0,=(cfg_prog_dtcm_flash_start - 4)
    mov r2, #0x20000000;                    # DTCM_START
    sub r2, r2, #0x4
    b 3b
7:                                          # TODO: modes?
    ldr sp,=cfg_prog_sp_addr
8:                                          # optional C extension
    bl c_BOOT
    cmp r0,#0
    bne s_BFAIL

s_BITCM:                                    # prep done, jump to ITCM (|1 cuz thumb)
    ldr pc,=0x1

s_BFAIL:                                    # boot failed
    mov r4, r0
1:                                          # optional C extension
    mov r0, r4
    bl c_BFAIL
2:                                          # maybe we have a magic c_BFAIL
    cmp r0,#0
    beq s_BITCM
3:                                          # infiniloop
    wfi
    b 3b
