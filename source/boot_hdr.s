#
#   Based on info from IMXRT1060 ref man
#       and teensy4 flash image
#

.syntax unified
.thumb
.cpu cortex-m7

.section .boot.header

flashcfg: # size is 0x200, FlexSPI config block
    .word 0x42464346
    .word 0x56010000
    .word 0
    .byte 1
    .byte 3
    .byte 3
    .byte 0
    .rept 52
    .byte 0
    .endr
    .byte 1
    .byte 4
    .byte 8
    .byte 0
    .word 0, 0
    .word 0x00200000, 0
    .word 0, 0
    .word 0, 0, 0, 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0x0A1804EB, 0x26043206, 0, 0, 0x24040405, 0, 0, 0
    .word 0, 0, 0, 0, 0x00000406, 0, 0, 0
    .word 0, 0, 0, 0, 0x08180420, 0, 0, 0
    .word 0, 0, 0, 0, 0, 0, 0, 0
    .word 0x081804D8, 0, 0, 0, 0x08180402, 0x00002004, 0, 0
    .word 0, 0, 0, 0, 0x00000460, 0, 0, 0
    .rept 32
    .word 0
    .endr
    .word 256 
    .word 4096
    .word 1
    .word 0
    .word 0x00010000
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

prognfo:
.word 0x60000000, cfg_prog_size, 0

.section .boot.some_other_header
.global some_other_header
some_other_header:
    .word 0x402000D1
    .word s_BOOT+1
    .word 0, 0
    .word prognfo
    .word some_other_header
    .word 0, 0
