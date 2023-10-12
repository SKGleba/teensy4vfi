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
    push {r1-r12, lr}
    # preload vars & args
1:
    ldmia r0!, {r3-r12}

    # ready, wait for trigger
2:
    ldr r1,[r11]
    and r1,r1,r10
    cmp r1,r9
    bne 2b

    # wait [offset] * [offset_mult]
3:
    mov r2, r3
4:
    subs r2, #0x1
    bne 4b
    subs r4, #0x1
    bne 3b

    # drive
    str r6,[r7]

    # wait [width]
5:
    subs r5, #0x1
    bne 5b

    # stop
    str r6,[r8]

    # execute next if/in chain
    movs r0, r12
    bne 1b

    # bye
    pop {r1-r12, pc}
