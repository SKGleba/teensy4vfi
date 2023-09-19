# classic style exception vectors
# TODO

.syntax unified
.thumb
.cpu cortex-m7

.section .text.vectors

.global exv
.global exha

exv:
    ldr pc,reset_addr

exha:
reset_addr:
    .word init
