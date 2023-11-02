#
# glitch funcs
#

.syntax unified
.thumb
.cpu cortex-m7

.text
.section .text.glitch_funcs

#
# s_glitch
# registers:
#
#   r0-r2: gp
#   
#   r3: offset
#   r4: offset mult
#   r5: width
#   r6: driver mask
#   r7: driver SET reg
#   r8: driver CLR reg
#
#   r9: trigger expected bitfield
#   r10: trigger reg mask
#   r11: trigger DATA reg
#
#   r12: next glitch config
#
.global s_glitch
.thumb_func
s_glitch:
    push.w {r1-r12, lr}
    # preload vars & args
1:
    ldmia.w r0!, {r3-r12}

    # ready, wait for trigger
2:
    ldr.w r1,[r11]
    and.w r1,r1,r10
    cmp.w r1,r9
    bne.w 2b

    # wait [offset] * [offset_mult]
3:
    mov.w r2, r3
4:
    subs.w r2, #0x1
    bne.w 4b
    subs.w r4, #0x1
    bne.w 3b

    # drive
    str.w r6,[r7]

    # wait [width]
5:
    subs.w r5, #0x1
    bne.w 5b

    # stop
    str.w r6,[r8]

    # execute next if/in chain
    movs.w r0, r12
    bne.w 1b

    # bye
    pop.w {r1-r12, pc}
