#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/debug.h"
#include "include/dcdc.h"

#include "include/ccm.h"

void ccm_control_gate(int device, int activity_mode, bool wait) {
    // break down the reg update op into smaller ops, it shows fetch-update hazards
    //      and optimized its the same as a cute one-liner anyways
    uint32_t reg_data = ccm.ccgr[CCM_CCGR_DEVICE_REG_N(device)];
    reg_data &= ~(BITNVAL(CCM_CCGR_DEVICE_BIT_N(device), CCM_CCGR_DEVICE_BITMASK));
    reg_data |= BITNVAL(CCM_CCGR_DEVICE_BIT_N(device), activity_mode & CCM_CCGR_DEVICE_BITMASK);
    // maybe disable interrupts between fetch-update, and what about auto-switches and pendings?
    ccm.ccgr[CCM_CCGR_DEVICE_REG_N(device)] = reg_data;
    while (wait && (
        (ccm.ccgr[CCM_CCGR_DEVICE_REG_N(device)] & BITNVAL(CCM_CCGR_DEVICE_BIT_N(device), CCM_CCGR_DEVICE_BITMASK))
        != BITNVAL(CCM_CCGR_DEVICE_BIT_N(device), activity_mode & CCM_CCGR_DEVICE_BITMASK)
        )) {
    };
}

/*
    Calculate ccm_set_core_clkf's core_clkf arg
        for a given desired arm frequency

    Based on the i.MX RT1060 Processor Reference Manual
        Rev. 3, 07/2021
            https://www.nxp.com/docs/en/reference-manual/IMXRT1060RM.pdf
                page 1010

    Fpll = Fref * div_select / 2
        Fpll: PLL_ARM output freq, range 650-1300 Mhz
        Fref: 24Mhz osci by default <= we set it to that anyways here

    Farm = Fpll / arm_podf / ahb_podf, range up to 600Mhz without oc, 984Mhz with oc
        Farm: ARM clock freq

    Fahb = Farm, TC^
        Fahb: ARM High Speed Bus clock freq

    Fipg = Fahb / ipg_podf, range up to 150Mhz => TODO: confirm
        Fipg: IPS Bus clock freq

    Core_CLKF = div_select | (arm_podf << 8) | (ahb_podf << 16) | (ipg_podf << 24)
*/
int ccm_calculate_core_clkf(int desired_freq) {
    int final_div_select = 0;
    int final_arm_podf = 0;
    int final_ahb_podf = 0;
    int final_Farm = 0;
    int ipg_podf = 0;

    if (desired_freq < CCM_MIN_ARM_FREQ || desired_freq > CCM_MELT_ARM_FREQ)
        return -1;

    int min_diff = 0x7fffffff;
    for (int div_select = 1; div_select <= ANATOP_PLL_ARM_BITMASK_DIV_SELECT; div_select++) {
        int Fpll = CCM_PLL_ARM_REF_OSC_FREQ * (div_select >> 1);
        if (Fpll >= CCM_MIN_PLL_ARM_FREQ && Fpll <= CCM_MAX_PLL_ARM_FREQ) {
            for (int ahb_podf = 1; ahb_podf <= (CCM_CBCDR_BITMASK_AHB_PODF + 1); ahb_podf++) {
                for (int arm_podf = 1; arm_podf <= (CCM_CACCR_BITMASK_ARM_PODF + 1); arm_podf++) {
                    int Farm = (Fpll / ahb_podf) / arm_podf;
                    int diff = Farm - desired_freq;
                    if (diff < 0)
                        diff = -diff;
                    if (diff < min_diff) {
                        min_diff = diff;
                        final_Farm = Farm;
                        final_div_select = div_select;
                        final_ahb_podf = ahb_podf;
                        final_arm_podf = arm_podf;
                    }
                }
            }
        }
    }

    if (!final_div_select)
        return -2;

    ipg_podf = (final_Farm + (CCM_MAX_IPG_FREQ - 1)) / CCM_MAX_IPG_FREQ;
    if (ipg_podf > (CCM_MAX_ARM_FREQ / CCM_MAX_IPG_FREQ))
        ipg_podf = (CCM_MAX_ARM_FREQ / CCM_MAX_IPG_FREQ);

    uint32_t clkf = BITNVAL(CCM_CORE_CLKF_BITS_DIV_SELECT, final_div_select)
        | BITNVAL(CCM_CORE_CLKF_BITS_ARM_PODF, final_arm_podf)
        | BITNVAL(CCM_CORE_CLKF_BITS_AHB_PODF, final_ahb_podf)
        | BITNVAL(CCM_CORE_CLKF_BITS_IPG_PODF, ipg_podf);

    /*
    printf("calculate_core_cklf: %x\n desired f: %x\n got f: %x\n div_select: %x\n arm_podf: %x\n ahb_podf: %x\n ipg_podf: %x\n",
        clkf,
        desired_freq,
        final_Farm,
        final_div_select,
        final_arm_podf,
        final_ahb_podf,
        ipg_podf
    );*/

    return clkf;
}

int ccm_set_core_clkf(int core_clkf, int desired_freq) {
    // we can slowly calc args here
    if (!core_clkf && desired_freq) {
        core_clkf = ccm_calculate_core_clkf(desired_freq);
        if (core_clkf < 0)
            return -1;
    }

    // extract args
    int div_select = (core_clkf >> CCM_CORE_CLKF_BITS_DIV_SELECT) & ANATOP_PLL_ARM_BITMASK_DIV_SELECT;
    int arm_podf = (core_clkf >> CCM_CORE_CLKF_BITS_ARM_PODF) & ((CCM_CACCR_BITMASK_ARM_PODF << 1) | 1);
    int ahb_podf = (core_clkf >> CCM_CORE_CLKF_BITS_AHB_PODF) & ((CCM_CBCDR_BITMASK_AHB_PODF << 1) | 1);
    int ipg_podf = (core_clkf >> CCM_CORE_CLKF_BITS_IPG_PODF) & ((CCM_CBCDR_BITMASK_IPG_PODF << 1) | 1);
    if (!(div_select && arm_podf && ahb_podf && ipg_podf))
        return -2;

    // validate
    int Farm = ((CCM_PLL_ARM_REF_OSC_FREQ * (div_select >> 1)) / arm_podf) / ahb_podf;
    printf("ccm_set_core_clkf: %X [ %X | %X | %X | %X ]\n", Farm, div_select, arm_podf, ahb_podf, ipg_podf);
    if (Farm < CCM_MIN_ARM_FREQ || ((Farm > CCM_MAX_OC_ARM_FREQ) && (Farm != CCM_MELT_ARM_FREQ)))
        return -3;

    // min soc voltage for overdrive: 1.25v, OC: 1.4v, melt: 1.6v
    int new_voltage = dcdc_get_vdd_soc();
    if (Farm > CCM_1150MV_MAX_ARM_FREQ) {
        if (Farm > CCM_1250MV_MAX_ARM_FREQ) {
            if (Farm > CCM_1400MV_MAX_ARM_FREQ)
                new_voltage = 1575; // RIP
            else
                new_voltage = 1400;
        } else
            new_voltage = 1250;
    } else
        new_voltage = 1150;

    // increase voltage if needed
    dcdc_ctrl_vdd_soc(new_voltage, true, true, true);

    // put arm clock on bypass while we change params
    anatop.pll_arm.clr = BITNVAL(ANATOP_PLL_ARM_BITS_BYPASS_CLK_SRC, ANATOP_PLL_ARM_BITMASK_BYPASS_CLK_SRC); // 24mhz osci | ALSO CHANGES REFCLK
    anatop.pll_arm.set = BITN(ANATOP_PLL_ARM_BITS_BYPASS);
    while (!(anatop.pll_arm.dr & BITN(ANATOP_PLL_ARM_BITS_LOCK))) {};

    // set arm pll loop div
    anatop.pll_arm.clr = BITNVAL(ANATOP_PLL_ARM_BITS_DIV_SELECT, ANATOP_PLL_ARM_BITMASK_DIV_SELECT);
    anatop.pll_arm.set = BITNVAL(ANATOP_PLL_ARM_BITS_DIV_SELECT, div_select);
    while (!(anatop.pll_arm.dr & BITN(ANATOP_PLL_ARM_BITS_LOCK))) {};

    // set arm, ahb & ipg podf
    ccm.cacrr = (arm_podf - 1);
    ccm.cbcdr = (((ccm.cbcdr // while it looks good, it might be misleading since we are on ARM
        & ~(BITNVAL(CCM_CBCDR_BITS_AHB_PODF, CCM_CBCDR_BITMASK_AHB_PODF)))
        & ~(BITNVAL(CCM_CBCDR_BITS_IPG_PODF, CCM_CBCDR_BITMASK_IPG_PODF)))
        | BITNVAL(CCM_CBCDR_BITS_AHB_PODF, (ahb_podf - 1))
        | BITNVAL(CCM_CBCDR_BITS_IPG_PODF, (ipg_podf - 1))
    );
    while (ccm.cdhipr & (BITN(CCM_CDHIPR_BITS_AHB_PODF_BUSY) | BITN(CCM_CDHIPR_BITS_ARM_PODF_BUSY))) {};

    // restore arm clock with new params
    anatop.pll_arm.clr = BITN(ANATOP_PLL_ARM_BITS_BYPASS);
    while (!(anatop.pll_arm.dr & BITN(ANATOP_PLL_ARM_BITS_LOCK))) {};

    // drop voltage if possible
    if ((new_voltage > 1150) || (CCM_1150MV_MAX_IPG_FREQ >= (Farm / ipg_podf)))
        dcdc_ctrl_vdd_soc(new_voltage, true, true, false);
    
    return core_clkf;
}

// default: pll3 80Mhz
// uart_clk = sauce_pll_clk / (sauce_div_p2 ^ 2)
void ccm_set_uart_clk(bool sauce_osc_clk_24m, int sauce_div_p2, bool wait) {
    uint32_t reg_data = ccm.cscdr1;
    reg_data &= ~(BITN(CCM_CSCDR1_BITS_UART_CLK_SEL) | BITNVAL(CCM_CSCDR1_BITS_UART_CLK_PODF, CCM_CSCDR1_BITMASK_UART_CLK_PODF));
    reg_data |= BITNVAL(CCM_CSCDR1_BITS_UART_CLK_SEL, sauce_osc_clk_24m);
    reg_data |= BITNVAL(CCM_CSCDR1_BITS_UART_CLK_PODF, sauce_div_p2);
    // maybe disable interrupts between fetch-update, and what about auto-switches and pendings?
    ccm.cscdr1 = reg_data;
    while (wait && (
        (ccm.cscdr1 & (BITN(CCM_CSCDR1_BITS_UART_CLK_SEL) | BITNVAL(CCM_CSCDR1_BITS_UART_CLK_PODF, CCM_CSCDR1_BITMASK_UART_CLK_PODF)))
        != (BITNVAL(CCM_CSCDR1_BITS_UART_CLK_SEL, sauce_osc_clk_24m) | BITNVAL(CCM_CSCDR1_BITS_UART_CLK_PODF, sauce_div_p2))
        )) {
    };
}