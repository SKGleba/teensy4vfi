#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/debug.h"

#include "include/dcdc.h"

int dcdc_ctrl_vdd_soc(int mv, bool step, bool wait, bool onlyUp) {
    if (!mv)
        return DCDC_TRG_TO_VDD_SOC(dcdc.reg_3 & DCDC_REG3_BITMASK_TRG);
    
    int trg = DCDC_VDD_SOC_TO_TRG(mv);
    if (trg > DCDC_REG3_BITMASK_TRG)
        return -1;

    int dcdc_r3 = dcdc.reg_3;
    if ((dcdc_r3 & BITNVAL(DCDC_REG3_BITS_TRG, DCDC_REG3_BITMASK_TRG)) == BITNVAL(DCDC_REG3_BITS_TRG, trg))
        return 0;

    if (onlyUp && ((dcdc_r3 & BITNVAL(DCDC_REG3_BITS_TRG, DCDC_REG3_BITMASK_TRG)) > BITNVAL(DCDC_REG3_BITS_TRG, trg)))
        return 0;

    dcdc_r3 &= ~(BITNVAL(DCDC_REG3_BITS_TRG, DCDC_REG3_BITMASK_TRG) | BITN(DCDC_REG3_BITS_DISABLE_STEP));
    dcdc_r3 |= BITNVAL(DCDC_REG3_BITS_TRG, trg);
    if (!step)
        dcdc_r3 |= BITN(DCDC_REG3_BITS_DISABLE_STEP);

    printf("VDD_SOC %X => %X [%s]\n", dcdc_get_vdd_soc(), mv, (step) ? "STEP" : "JUMP");
    dcdc.reg_3 = dcdc_r3;
    if (wait)
        while (!(dcdc.reg_0 & BITN(DCDC_REG0_BITS_STS_DC_OK))) {};

    return DCDC_TRG_TO_VDD_SOC(dcdc.reg_3 & DCDC_REG3_BITMASK_TRG);
}