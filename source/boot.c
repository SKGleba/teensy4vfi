#include "include/types.h"
#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/gpio.h"
#include "include/teensy.h"

/*
    maybe add some stage3/BS there?
*/
__attribute__((section(".boot.c")))
int c_BOOT(void) { // c extension for s_BOOT
    return 0;
}

/*
    As a bootfail indicator we will use the onboard orange led (PAD_GPIO_B0_03)
        if we failed soon enough, we will also get a blinking red bloader led as a bonus
*/
__attribute__((section(".boot.f")))
int c_BFAIL(void) { // c extension for s_BFAIL
    teensy_pad_logic_set_outdir_direct(TEENSY_PAD_ORANGE_LED);  // GPIO2:GDIR set bit3 => set GPIO2::3 mode to output
    teensy_pad_logic_set_direct(TEENSY_PAD_ORANGE_LED);         // GPIO2:DR_SET set bit3 => set GPIO2::3
    return -1;                                                  // no boot resuming
}