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
.global s_glitch
.thumb_func
s_glitch:
    push {r0-r12, lr}
    # preload main vars
    ldr r0, =g_glitch_varray_main
    ldmia r0!, {r3-r8}

    # preload func-specific args
    ldr r0, =g_glitch_varray
    ldmia r0!, {r9-r11}

    # ready, wait for trigger
1:
    ldr r1,[r11]
    and r1,r1,r10
    cmp r1,r9
    bne 1b

    # wait [offset] * [offset_mult]
2:
    mov r2, r3
3:
    subs r2, #0x1
    bne 3b
    subs r4, #0x1
    bne 2b

    # drive
    str r6,[r7]

    # wait [width]
4:
    subs r5, #0x1
    bne 4b

    # stop
    str r6,[r8]

    # bye
    pop {r0-r12, pc}



.data
.section .data.glitch_vars

.macro ADD_GLITCH_VAR name value:vararg
.global g_glitch_\name
g_glitch_\name:
	.word \value
.endm

.global g_glitch_varray_main
g_glitch_varray_main:
ADD_GLITCH_VAR offset 0x10000000
ADD_GLITCH_VAR offset_mult 1
ADD_GLITCH_VAR width 0x10000000
ADD_GLITCH_VAR driver_ports 1<<24
ADD_GLITCH_VAR driver_set_reg 0x401B8084
ADD_GLITCH_VAR driver_clr_reg 0x401B8088

.global g_glitch_varray
g_glitch_varray:
ADD_GLITCH_VAR trigger_exp_state 1<<25
ADD_GLITCH_VAR trigger_ports 1<<25
ADD_GLITCH_VAR trigger_data_reg 0x401B8000
