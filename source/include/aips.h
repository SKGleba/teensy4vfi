/*
    Copyright (C) 2023 skgleba
    This software may be modified and distributed under the terms
    of the MIT license. See the LICENSE file for details.
*/

/*
    aips.h : ARM IP BUS 1-5 C-struct for i.MX RT106X
        by skgleba

    Written based on the i.MX RT1060 Processor Reference Manual
        Rev. 3, 07/2021
            https://www.nxp.com/docs/en/reference-manual/IMXRT1060RM.pdf
*/

#ifndef __AIPS_H__
#define __AIPS_H__

#include "types.h"

#define reg8 volatile uint8_t
#define reg16 volatile uint16_t
#define reg32 volatile uint32_t

#define reg reg32

#define AIPS_ENTRY_SIZE 0x4000

/*
    ------------------------------------
    Shared AIPS devices, buses & structs
    ------------------------------------
*/
// common
struct _aips_dr_manip_def {
    reg dr;     // RW data register
    reg set;    // WO data register bit set
    reg clr;    // WO data register bit clear
    reg tog;    // WO data register bit toggle
};
typedef struct _aips_dr_manip_def aips_dr_manip_def;

// AIPSnTZ
struct _aips_aipstz {
    reg mpr;
    reg unseen4[(0x40 - 4) / 4];
    reg opacr0;
    reg opacr1;
    reg opacr2;
    reg opacr3;
    reg opacr4;
};
typedef struct _aips_aipstz aips_aipstz;

// AIPSn_GPIOn
struct _aips_gpio {
    reg dr;
    reg gdir;
    reg psr;
    reg icr1;
    reg icr2;
    reg imr;
    reg isr;
    reg edge_sel;
    reg unseen20[(0x84 - 0x20) / 4];
    reg dr_set;
    reg dr_clear;
    reg dr_toggle;
};
typedef struct _aips_gpio aips_gpio;


/*
    -------------------------------
    AIPS-1 devices, buses & structs
    -------------------------------
*/
// AIPS1_ADCn
struct _aips_adc {
    reg hc[8];
    reg hs;
    reg r[8];
    reg cfg;
    reg gc;
    reg gs;
    reg cv;
    reg ofs;
    reg cal;
};
typedef struct _aips_adc aips_adc;

// AIPS1_DCDC
struct _aips_dcdc {
    reg reg_0;
    reg reg_1;
    reg reg_2;
    reg reg_3;
};
typedef struct _aips_dcdc aips_dcdc;

// AIPS1_PIT
struct _aips_pit {
    reg mcr;
    reg unseen4[(0xE0 - 4) / 4];
    reg ltmr64h;
    reg ltmr64l;
    reg unseenE8[(0x100 - 0xE8) / 4];
    struct {
        reg ldval;
        reg cval;
        reg tctrl;
        reg tflg;
    } tcfg[4];
};
typedef struct _aips_pit aips_pit;

// AIPS1_ACMP
struct _aips_acmp {
    struct {
        reg8 cr0;
        reg8 cr1;
        reg8 fpr;
        reg8 scr;
        reg8 daccr;
        reg8 muxcr;
        reg8 unused6[2];
    } cmp[4]; // call by this[desired-1]
};
typedef struct _aips_acmp aips_acmp;

// AIPS1_IOMUXC_SNVS_GPR
struct _aips_iomuxc_snvs_gpr {
    reg gpr[4];
};
typedef struct _aips_iomuxc_snvs_gpr aips_iomuxc_snvs_gpr;

// AIPS1 IOMUXC_SNVS
struct _aips_iomuxc_snvs {
    reg sw_mux_ctl_pad_wakeup;
    reg sw_mux_ctl_pad_pmic_on_req;
    reg sw_mux_ctl_pad_pmic_stby_req;
    reg sw_pad_ctl_pad_test_mode;
    reg sw_pad_ctl_pad_por_b;
    reg sw_pad_ctl_pad_onoff;
    reg sw_pad_ctl_pad_wakeup;
    reg sw_pad_ctl_pad_pmic_on_req;
    reg sw_pad_ctl_pad_pmic_stby_req;
};
typedef struct _aips_iomuxc_snvs aips_iomuxc_snvs;

// AIPS1_IOMUXC_GPR
struct _aips_iomuxc_gpr {
    reg gpr[35];
};
typedef struct _aips_iomuxc_gpr aips_iomuxc_gpr;

// AIPS1_CM7_MXRT
struct _aips_cm7_mxrt {
    reg tcm_ctrl;
    reg unseen4[3];
    reg int_status;
    reg int_stat_en;
    reg int_sig_en;
};
typedef struct _aips_cm7_mxrt aips_cm7_mxrt;

// AIPS1_EWM
struct _aips_ewm {
    reg8 ctrl;
    reg8 serv;
    reg8 cmpl;
    reg8 cmph;
    reg8 clkctrl;
    reg8 clkprescaler;
};
typedef struct _aips_ewm aips_ewm;

// AIPS1_WDOG16n
struct _aips_wdog16 {
    reg16 wcr;
    reg16 wsr;
    reg16 wrsr;
    reg16 wicr;
    reg16 wmcr;
};
typedef struct _aips_wdog16 aips_wdog16;

// AIPS1_WDOG32n
struct _aips_wdog32 {
    reg cs;
    reg cnt;
    reg toval;
    reg win;
};
typedef struct _aips_wdog32 aips_wdog32;

// AIPS1_SNVS_HP
struct _aips_snvs_hp {
    reg unseen0;
    reg hpcomr;
    reg hpcr;
    reg unseenC[2];
    reg hpsr;
    reg unseen18[3];
    reg hprtcmr;
    reg hprtclr;
    reg hptamr;
    reg hptalr;
    reg lplr;
    reg lpcr;
    reg unseen3C[4];
    reg lpsr;
    reg unseen50[3];
    reg lpsmcmr;
    reg lpsmclr;
    reg lplvdr;
    reg lpgpr0_legacy;
    reg unseen6C[(0x90 - 0x6C) / 4];
    reg lpgpr_alias[4];
    reg unseenA0[(0x100 - 0xA0) / 4];
    reg lpgpr[8];
    reg unused120[(0xB00 - 0x120) / 4];
    reg unseenB00[(0xBF8 - 0xB00) / 4];
    reg hpvidr1;
    reg hpvidr2;
};
typedef struct _aips_snvs_hp aips_snvs_hp;

// AIPS1_ANALOG
struct _aips_analog {
    aips_dr_manip_def pll_arm;
    aips_dr_manip_def pll_usb1;
    aips_dr_manip_def pll_usb2;
    aips_dr_manip_def pll_sys;
    reg pll_sys_ss;
    reg unseen44[3];
    reg pll_sys_num;
    reg unseen54[3];
    reg pll_sys_denom;
    reg unseen64[3];
    aips_dr_manip_def pll_audio;
    reg pll_audio_num;
    reg unseen84[3];
    reg pll_audio_denom;
    reg unseen94[3];
    aips_dr_manip_def pll_video;
    reg pll_video_num;
    reg unseenB4[3];
    reg pll_video_denom;
    reg unseenC4[7];
    aips_dr_manip_def pll_enet;
    aips_dr_manip_def pll_pfd_480;
    aips_dr_manip_def pll_pfd_528;
    reg unseen110[0x10];
    aips_dr_manip_def misc[3];
};
typedef struct _aips_analog aips_analog;

// AIPS1_TSC
struct _aips_tsc {
    reg basic_setting;
    reg unseen4[3];
    reg pre_charge_time;
    reg unseen14[3];
    reg flow_control;
    reg unseen24[3];
    reg measure_value;
    reg unseen34[3];
    reg int_en;
    reg unseen44[3];
    reg int_sig_en;
    reg unseen54[3];
    reg int_status;
    reg unseen64[3];
    reg debug_mode;
    reg unseen74[3];
    reg debug_mode2;
};
typedef struct _aips_tsc aips_tsc;

// AIPS1_EDMA
struct _aips_edma {
    reg cr;
    reg es;
    reg unseen8;
    reg erq;
    reg unseen10;
    reg eei;
    reg8 ceei;
    reg8 seei;
    reg8 cerq;
    reg8 serq;
    reg8 cdne;
    reg8 ssrt;
    reg8 cerr;
    reg8 cint;
    reg unseen20;
    reg intreq;
    reg unseen28;
    reg err;
    reg unseen30;
    reg hrs;
    reg unseen38[3];
    reg ears;
    reg unused48[(0x100 - 0x48) / 4];
    reg8 dchpri_Nrsh2_DCBA[8][4]; // dchpri_N_data = dchpri_Nrsh2_DCBA[dchpri_N >> 2][3 - (dchpri_N - ((dchpri_N >> 2) << 2))]  xD
    reg32 unused120[(0x1000 - 0x120) / 4];
    struct {
        reg saddr;
        reg16 soff;
        reg16 attr;
        union {
            reg nbytes_mlno;
            reg nbytes_mloffno;
            reg nbytes_mloffyes;
        };
        reg slast;
        reg daddr;
        reg16 doff;
        union {
            reg16 citer_elinkno;
            reg16 citer_elinkyes;
        };
        reg32 dlastsga;
        reg16 csr;
        union {
            reg16 biter_elinkno;
            reg16 biter_elinkyes;
        };
    } tcd[32];
};
typedef struct _aips_edma aips_edma;

// AIPS1_DMA_CH_MUX / DMAMUX
struct _aips_dma_ch_mux {
    reg chcfg[32];
};
typedef struct _aips_dma_ch_mux aips_dma_ch_mux;

// AIPS1_GPC
struct _aips_gpc {
    reg cntr;
    reg unseen4;
    reg imr1;
    reg imr2;
    reg imr3;
    reg imr4;
    reg isr1;
    reg isr2;
    reg isr3;
    reg isr4;
    reg unseen28[3];
    reg imr5;
    reg isr5;
};
typedef struct _aips_gpc aips_gpc;

// AIPS1_SRC
struct _aips_src {
    reg scr;
    reg sbmr1;
    reg srsr;
    reg unseenC[4];
    reg sbmr2;
    reg gpr[10]; // call by this[desired-1]
};
typedef struct _aips_src aips_src;

// AIPS1_CCM
struct _aips_ccm {
    reg ccr;
    reg unseen4;
    reg csr;
    reg ccsr;
    reg cacrr;
    reg cbcdr;
    reg cbcmr;
    reg cscmr1;
    reg cscmr2;
    reg cscdr1;
    reg cs1cdr;
    reg cs2cdr;
    reg cdcdr;
    reg unseen34;
    reg cscdr2;
    reg cscdr3;
    reg unseen40[2];
    reg cdhipr;
    reg unseen4C[2];
    reg clpcr;
    reg cisr;
    reg cimr;
    reg ccosr;
    reg cgpr;
    reg ccgr[8];
    reg cmeor;
};
typedef struct _aips_ccm aips_ccm;

// AIPS 1
struct _aips_1_s {
    reg reserved0[0x7c000 / 4];

    // AIPS Configuration
    union {
        aips_aipstz aipstz;
        reg aipstz_raw[AIPS_ENTRY_SIZE / 4];
    };

    // DCDC
    union {
        aips_dcdc dcdc;
        reg dcdc_raw[AIPS_ENTRY_SIZE / 4];
    };

    // PIT
    union {
        aips_pit pit;
        reg pit_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reserved88000[(AIPS_ENTRY_SIZE * 3) / 4];

    // ACMP
    union {
        aips_acmp acmp;
        reg8 acmp_raw[AIPS_ENTRY_SIZE];
    };

    reg reserved98000[(AIPS_ENTRY_SIZE * 3) / 4];

    // IOMUXC_SNVS_GPR
    union {
        aips_iomuxc_snvs_gpr iomuxc_snvs_gpr;
        reg iomuxc_snvs_gpr_raw[AIPS_ENTRY_SIZE / 4];
    };

    // IOMUXC_SNVS
    union {
        aips_iomuxc_snvs iomuxc_snvs;
        reg iomuxc_snvs_raw[AIPS_ENTRY_SIZE / 4];
    };

    // IOMUXC_GPR
    union {
        aips_iomuxc_gpr iomuxc_gpr;
        reg iomuxc_gpr_raw[AIPS_ENTRY_SIZE / 4];
    };

    // CM7_MXRT (FLEXRAM)
    union {
        aips_cm7_mxrt cm7_mxrt;
        reg cm7_mxrt_raw[AIPS_ENTRY_SIZE / 4];
    };

    // EWM
    union {
        aips_ewm ewm;
        reg8 ewm_raw[AIPS_ENTRY_SIZE];
    };

    // WDOG1
    union {
        aips_wdog16 wdog1;
        reg16 wdog1_raw[AIPS_ENTRY_SIZE / 2];
    };

    // WDOG3
    union {
        aips_wdog32 wdog3;
        reg wdog3_raw[AIPS_ENTRY_SIZE / 4];
    };

    // GPIO5
    union {
        aips_gpio gpio5;
        reg gpio5_raw[AIPS_ENTRY_SIZE / 4];
    };

    // ADC1
    union {
        aips_adc adc1;
        reg adc1_raw[AIPS_ENTRY_SIZE / 4];
    };

    // ADC2
    union {
        aips_adc adc2;
        reg adc2_raw[AIPS_ENTRY_SIZE / 4];
    };

    // TRNG
    reg trng_raw[AIPS_ENTRY_SIZE / 4]; // need docs

    // WDOG2
    union {
        aips_wdog16 wdog2;
        reg16 wdog2_raw[AIPS_ENTRY_SIZE / 2];
    };

    // SNVS_HP (memmap)
    union {
        aips_snvs_hp snvs_hp;
        reg snvs_hp_raw[AIPS_ENTRY_SIZE / 4];
    };

    // ANALOG
    union {
        aips_analog anatop;
        reg anatop_raw[AIPS_ENTRY_SIZE / 4];
    };

    // CSU
    reg csu_raw[AIPS_ENTRY_SIZE / 4]; // need docs

    // TSC
    union {
        aips_tsc tsc;
        reg tsc_raw[AIPS_ENTRY_SIZE / 4];
    };

    // SJC
    reg sjc_raw[AIPS_ENTRY_SIZE / 4]; // need docs

    // EDMA
    union {
        aips_edma edma;
        reg edma_raw[AIPS_ENTRY_SIZE / 4];
    };

    // DMAMUX
    union {
        aips_dma_ch_mux dmamux;
        reg dmamux_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedF0000[AIPS_ENTRY_SIZE / 4];

    // GPC
    union {
        aips_gpc gpc;
        reg gpc_raw[AIPS_ENTRY_SIZE / 4];
    };

    // SRC
    union {
        aips_src src;
        reg src_raw[AIPS_ENTRY_SIZE / 4];
    };

    // CCM
    union {
        aips_ccm ccm;
        reg ccm_raw[AIPS_ENTRY_SIZE / 4];
    };
};
typedef struct _aips_1_s aips_1_s;


/*
    -------------------------------
    AIPS-2 devices, buses & structs
    -------------------------------
*/
// AIPS2_LPUARTn
struct _aips_lpuart {
    reg verid;
    reg param;
    reg global;
    reg pincfg;
    reg baud;
    reg stat;
    reg ctrl;
    reg data;
    reg match;
    reg modir;
    reg fifo;
    reg water; // lul
};
typedef struct _aips_lpuart aips_lpuart;

// AIPS2_FLEXIOn
struct _aips_flexio {
    reg verid;
    reg param;
    reg ctrl;
    reg pin;
    reg shiftstat;
    reg shifterr;
    reg timstat;
    reg unseen1C;
    reg shiftsien;
    reg shifteien;
    reg timien;
    reg unseen2C;
    reg shiftsden;
    reg unseen34[3];
    reg shiftstate;
    reg unused44[(0x80 - 0x44) / 4];
    reg shiftctl[8];
    reg unusedA0[(0x100 - 0xA0) / 4];
    reg shiftcfg[8];
    reg unused120[(0x200 - 0x120) / 4];
    reg shiftbuf[8];
    reg unused220[(0x280 - 0x220) / 4];
    reg shiftbufbis[8];
    reg unused2A0[(0x300 - 0x2A0) / 4];
    reg shiftbufbys[8];
    reg unused320[(0x380 - 0x320) / 4];
    reg shiftbusbbs[8];
    reg unused3A0[(0x400 - 0x3A0) / 4];
    reg timctl[8];
    reg unused420[(0x480 - 0x420) / 4];
    reg timcfg[8];
    reg unused4A0[(0x500 - 0x4A0) / 4];
    reg timcmp[8];
    reg unused520[(0x680 - 0x520) / 4];
    reg shiftbufnbs[8];
    reg unused6A0[(0x700 - 0x6A0) / 4];
    reg shiftbufhws[8];
    reg unused720[(0x780 - 0x720) / 4];
    reg shiftbufnis[8];
};
typedef struct _aips_flexio aips_flexio;

// AIPS2_FLEXCANn
struct _aips_flexcan {
    reg mcr;
    reg ctrl1;
    reg timer;
    reg unseenC;
    reg rxmgmask;
    reg rx14mask;
    reg rx15mask;
    reg ecr;
    reg esr1;
    reg imask2;
    reg imask1;
    reg iflag2;
    reg iflag1;
    reg ctrl2;
    reg esr2;
    reg unseen3C[2];
    reg crcr;
    reg rxfgmask;
    reg rxfir;
    reg unseen50[2];
    reg dbg1;
    reg dbg2;
    reg unused60[(0x880 - 0x60) / 4];
    reg rximr[64];
    reg unused980[(0x9E0 - 0x980) / 4];
    reg gfwr;
};
typedef struct _aips_flexcan aips_flexcan;

// AIPS2_CANFD (FlexCAN3)
struct _aips_canfd {
    reg mcr;
    reg ctrl1;
    reg timer;
    reg unseenC;
    reg rxmgmask;
    reg rx14mask;
    reg rx15mask;
    reg ecr;
    reg esr1;
    reg imask2;
    reg imask1;
    reg iflag2;
    reg iflag1;
    reg ctrl2;
    reg esr2;
    reg unseen3C[2];
    reg crcr;
    reg rxfgmask;
    reg rxfir;
    reg cbt; // nice
    reg unused54[(0x880 - 0x54) / 4];
    reg rximr[64];
    reg unused980[(0xc00 - 0x980) / 4];
    reg fdctrl;
    reg fdcbt;
    reg fdcrc;
};
typedef struct _aips_canfd aips_canfd;

// AIPS2_QTIMERn
struct _aips_qtimer {
    struct {
        reg16 comp[2];
        reg16 capt;
        reg16 load;
        reg16 hold;
        reg16 cntr;
        reg16 ctrl;
        reg16 sctrl;
        reg16 cmpld[2];
        reg16 csctrl;
        reg16 filt;
        reg16 dma;
        reg16 unseen1A[2];
        union {
            reg16 unseen1E;
            reg16 enbl; // only for subchannel1
        };
    } subchannel[4];
};
typedef struct _aips_qtimer aips_qtimer;

// AIPS2_GPTn
struct _aips_gpt {
    reg cr;
    reg pr;
    reg sr;
    reg ir;
    reg ocr[3];
    reg icr[2];
    reg cnt;
};
typedef struct _aips_gpt aips_gpt;

// AIPS2_OCOPT
struct _aips_ocotp {
    aips_dr_manip_def ctrl;
    reg timing;
    reg unused14[3];
    reg data;
    reg unused24[3];
    reg read_ctrl;
    reg unused34[3];
    reg read_fuse_data;
    reg unused44[7];
    aips_dr_manip_def scs;
    reg unused70[8];
    reg version;
    reg unused94[(0x100 - 0x94) / 4];
    reg timing2;
    reg unused104[(0x400 - 0x104) / 4];
    struct {
        struct {
            union {
                reg word;
                reg raw[4];
            };
        } otp_bank_words[8];
    } otp_banks[8];
};
typedef struct _aips_ocotp aips_ocotp;

// AIPS2_IOMUXC
struct _aips_iomuxc {
    reg unseen[5];

    struct {
        reg emc[42];
        reg ad_b0[16];
        reg ad_b1[16];
        reg b0[16];
        reg b1[16];
        reg sd_b0[6];
        reg sd_b1[12];
    } sw_mux_ctl_pad_gpio;

    struct {
        reg emc[42];
        reg ad_b0[16];
        reg ad_b1[16];
        reg b0[16];
        reg b1[16];
        reg sd_b0[6];
        reg sd_b1[12];
    } sw_pad_ctl_pad_gpio;

    struct {
        reg anatop_usb_otg1_id;
        reg anatop_usb_otg2_id;
        
        reg ccm_pmic_ready;

        reg csi_data02plus[8]; // hehe
        reg csi_hsync;
        reg csi_pixclk;
        reg csi_vsync;

        reg enet_ipg_clk_rmii;
        reg enet_mdio;
        reg enet0_rxdata;
        reg enet1_rxdata;
        reg enet_rxen;
        reg enet_rxerr;
        reg enet0_timer;
        reg enet_txclk;

        reg flexcan1_rx;
        reg flexcan2_rx;

        reg flexpwm1_pwm_A3;
        reg flexpwm1_pwm_A0;
        reg flexpwm1_pwm_A1;
        reg flexpwm1_pwm_A2;
        reg flexpwm1_pwm_B3;
        reg flexpwm1_pwm_B0;
        reg flexpwm1_pwm_B1;
        reg flexpwm1_pwm_B2;

        reg flexpwm2_pwm_A3;
        reg flexpwm2_pwm_A0;
        reg flexpwm2_pwm_A1;
        reg flexpwm2_pwm_A2;
        reg flexpwm2_pwm_B3;
        reg flexpwm2_pwm_B0;
        reg flexpwm2_pwm_B1;
        reg flexpwm2_pwm_B2;

        reg flexpwm4_pwm_A0;
        reg flexpwm4_pwm_A1;
        reg flexpwm4_pwm_A2;
        reg flexpwm4_pwm_A3;

        reg flexspi_A_dqs;
        reg flexspi_A_data[4];
        reg flexspi_B_data[4];
        reg flexspi_A_sck;

        struct {
            reg scl;
            reg sda;
        } lpi2c[4]; // n - 1

        struct {
            reg pcs0;
            reg sck;
            reg sdi;
            reg sdo;
        } lpspi[4]; // n - 1

        reg lpuart2_rx;
        reg lpuart2_tx;
        reg lpuart3_cts_b; // why dis in the middle, cmon, could have a cute array
        reg lpuart3_rx;
        reg lpuart3_tx;
        reg lpuart4_rx;
        reg lpuart4_tx;
        reg lpuart5_rx;
        reg lpuart5_tx;
        reg lpuart6_rx;
        reg lpuart6_tx;
        reg lpuart7_rx;
        reg lpuart7_tx;
        reg lpuart8_rx;
        reg lpuart8_tx;

        reg nmi_glue_nmi;

        reg qtimer2_timer[4];
        reg qtimer3_timer[4];

        reg sai_1_mclk2;
        reg sai_1_rx_bclk;
        reg sai_1_rx_data[4];
        reg sai_1_rx_sync;
        reg sai_1_tx_bclk;
        reg sai_1_tx_sync;

        reg sai_2_mclk2;
        reg sai_2_rx_bclk;
        reg sai_2_rx_data0;
        reg sai_2_rx_sync;
        reg sai_2_tx_bclk;
        reg sai_2_tx_sync;

        reg spdif_in;

        reg usb_otg2_oc;
        reg usb_otg1_oc;

        reg usdhc_1_cd_b;
        reg usdhc_1_wp;

        reg usdhc_2_clk;
        reg usdhc_2_cd_b;
        reg usdhc_2_cmd;
        reg usdhc_2_data[8];
        reg usdhc_2_wp;

        reg xbar1_in02;
        reg xbar1_in03;
        reg xbar1_in04;
        reg xbar1_in05;
        reg xbar1_in06;
        reg xbar1_in07;
        reg xbar1_in08;
        reg xbar1_in09;
        reg xbar1_in17;
        reg xbar1_in18;
        reg xbar1_in20;
        reg xbar1_in22;
        reg xbar1_in23;
        reg xbar1_in24;
        reg xbar1_in14;
        reg xbar1_in15;
        reg xbar1_in16;
        reg xbar1_in25;
        reg xbar1_in19;
        reg xbar1_in21;

        reg unseen65C[(0x70C - 0x65C) / 4];

        reg enet2_ipg_clk_rmii;
        reg enet2_ipp_ind_mac0_mdio;
        reg enet2_ipp_ind_mac0_rxdata[2];
        reg enet2_ipp_ind_mac0_rxen;
        reg enet2_ipp_ind_mac0_rxerr;
        reg enet2_ipp_ind_mac0_timer;
        reg enet2_ipp_ind_mac0_txclk;
        
        reg flexspi2_ipp_ind_dqs_fa;
        reg flexspi2_ipp_ind_io_fa_bit[4];
        reg flexspi2_ipp_ind_io_fb_bit[4];
        reg flexspi2_ipp_ind_sck_fa;
        reg flexspi2_ipp_ind_sck_fb;

        struct {
            reg capin1;
            reg capin2;
            reg clkin;
        } gpt_ipp_ind[2]; // n - 1

        reg sai_3_ipg_clk_sai_mclk;
        reg sai_3_ipp_ind_sai_rx_bclk;
        reg sai_3_ipp_ind_sai_rx_data0;
        reg sai_3_ipp_ind_sai_rx_sync;
        reg sai_3_ipp_ind_sai_tx_bclk;
        reg sai_3_ipp_ind_sai_tx_sync;

        reg semc_i_ipp_ind_dqs4;
        
        reg canfd_ipp_ind_canrx;
    } input_select;
};
typedef struct _aips_iomuxc aips_iomuxc;

// AIPS2_KPP
struct _aips_kpp {
    reg16 cr;
    reg16 sr;
    reg16 ddr;
    reg16 dr;
};
typedef struct _aips_kpp aips_kpp;

// AIPS 2
struct _aips_2_s {
    reg reserved0[0x7c000 / 4];

    // AIPS Configuration
    union {
        aips_aipstz aipstz;
        reg aipstz_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reserved80000[AIPS_ENTRY_SIZE / 4];

    // LPUARTs
    union {
        aips_lpuart lpuart1;
        reg lpuart1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpuart lpuart2;
        reg lpuart2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpuart lpuart3;
        reg lpuart3_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpuart lpuart4;
        reg lpuart4_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpuart lpuart5;
        reg lpuart5_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpuart lpuart6;
        reg lpuart6_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpuart lpuart7;
        reg lpuart7_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpuart lpuart8;
        reg lpuart8_raw[AIPS_ENTRY_SIZE / 4];
    };


    reg reservedA4000[(AIPS_ENTRY_SIZE * 2) / 4];

    // FLEXIOs
    union {
        aips_flexio flexio1;
        reg flexio1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_flexio flexio2;
        reg flexio2_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedB4000[AIPS_ENTRY_SIZE / 4];

    // GPIOs
    union {
        aips_gpio gpio1;
        reg gpio1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_gpio gpio2;
        reg gpio2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_gpio gpio3;
        reg gpio3_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_gpio gpio4;
        reg gpio4_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedC8000[(AIPS_ENTRY_SIZE * 2) / 4];

    // FLEXCANs
    union {
        aips_flexcan flexcan1;
        reg flexcan1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_flexcan flexcan2;
        reg flexcan2_raw[AIPS_ENTRY_SIZE / 4];
    };

    // CANFD / FLEXCAN3
    union {
        aips_canfd canfd;
        reg canfd_raw[AIPS_ENTRY_SIZE / 4];
    };

    // QTIMERs
    union {
        aips_qtimer qtimer1;
        reg qtimer1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_qtimer qtimer2;
        reg qtimer2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_qtimer qtimer3;
        reg qtimer3_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_qtimer qtimer4;
        reg qtimer4_raw[AIPS_ENTRY_SIZE / 4];
    };

    // GPTs
    union {
        aips_gpt gpt1;
        reg gpt1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_gpt gpt2;
        reg gpt2_raw[AIPS_ENTRY_SIZE / 4];
    };

    // OCOTP
    union {
        aips_ocotp ocotp;
        reg ocotp_raw[AIPS_ENTRY_SIZE / 4];
    };

    // IOMUXC
    union {
        aips_iomuxc iomuxc;
        reg iomuxc_raw[AIPS_ENTRY_SIZE / 4];
    };

    // KPP
    union {
        aips_kpp kpp;
        reg kpp_raw[AIPS_ENTRY_SIZE / 4];
    };

};
typedef struct _aips_2_s aips_2_s;


/*
    -------------------------------
    AIPS-3 devices, buses & structs
    -------------------------------
*/
// AIPS3_FLEXSPIn
struct _aips_flexspi {
    reg mcr[3];
    reg ahbcr;
    reg inten;
    reg intr;
    reg lutkey;
    reg lutcr;
    reg ahbrxbufcr0[4];
    reg unseen30[(0x60 - 0x30) / 4];
    reg flsha1cr0;
    reg flsha2cr0;
    reg flshb1cr0;
    reg flshb2cr0;
    reg flsha1cr1;
    reg flsha2cr1;
    reg flshb1cr1;
    reg flshb2cr1;
    reg flsha1cr2;
    reg flsha2cr2;
    reg flshb1cr2;
    reg flshb2cr2;
    reg unseen90;
    reg flshcr4;
    reg unseen98[2];
    reg ipcr[2];
    reg unseenA8[2];
    reg ipcmd;
    reg unseenB4;
    reg iprxfcr;
    reg iptxfcr;
    reg dllacr;
    reg dllbcr;
    reg unseenC8[(0xE0 - 0xC8) / 4];
    reg sts[3];
    reg ahbspndsts;
    reg iprxfsts;
    reg iptxfsts;
    reg unseenF8[2];
    reg rfdr[32];
    reg tfdr[32];
    reg lut[64];
};
typedef struct _aips_flexspi aips_flexspi;

// AIPS3_PXP
struct _aips_pxp {
    aips_dr_manip_def ctrl;
    aips_dr_manip_def stat;
    aips_dr_manip_def out_ctrl;
    reg out_buf;
    reg unused34[3];
    reg out_buf2;
    reg unused44[3];
    reg out_pitch;
    reg unused54[3];
    reg out_lrc;
    reg unused64[3];
    reg out_ps_ulc;
    reg unused74[3];
    reg out_ps_lrc;
    reg unused84[3];
    reg out_as_ulc;
    reg unused94[3];
    reg out_as_lrc;
    reg unusedA4[3];
    aips_dr_manip_def ps_ctrl;
    reg ps_buf;
    reg unusedC4[3];
    reg ps_ubuf;
    reg unusedD4[3];
    reg ps_vbuf;
    reg unusedE4[3];
    reg ps_pitch;
    reg unusedF4[3];
    reg ps_background;
    reg unused104[3];
    reg ps_scale;
    reg unused114[3];
    reg ps_offset;
    reg unused124[3];
    reg ps_clrkeylo;
    reg unused134[3];
    reg ps_clrkeyhi;
    reg unused144[3];
    reg as_ctrl;
    reg unused154[3];
    reg as_buf;
    reg unused164[3];
    reg as_pitch;
    reg unused174[3];
    reg as_clrkeylo;
    reg unused184[3];
    reg as_clrkeyhi;
    reg unused194[3];
    reg csc1_coef0;
    reg unused1A4[3];
    reg csc1_coef1;
    reg unused1B4[3];
    reg csc1_coef2;
    reg unused1C4[(0x320 - 0x1C4) / 4];
    reg power;
    reg unused324[(0x400 - 0x324) / 4];
    reg next;
    reg unused404[(0x440 - 0x404) / 4];
    reg porter_duff_ctrl;
};
typedef struct _aips_pxp aips_pxp;

// AIPS3_LCDIF
struct _aips_lcdif {
    aips_dr_manip_def ctrl[3];
    reg transfer_count;
    reg unused34[3];
    reg cur_buf;
    reg unused44[3];
    reg next_buf;
    reg unused54[7];
    aips_dr_manip_def vdctrl0;
    reg vdctrl1;
    reg unused84[3];
    reg vdctrl2;
    reg unused94[3];
    reg vdctrl3;
    reg unusedA4[3];
    reg vdctrl4;
    reg unusedB4[(0x190 - 0xB4) / 4];
    reg bm_error_stat;
    reg unused194[3];
    reg crc_stat;
    reg unused1A4[3];
    reg stat;
    reg unused1B4[(0x380 - 0x1B4) / 4];
    aips_dr_manip_def pigeonctrl[3];
    reg unused3B0[(0x800 - 0x3b0) / 4];
    struct {
        reg p0;
        reg unused4[3];
        reg p1;
        reg unused14[3];
        reg p2;
        reg unseen24[7];
    } pigeon[12];
    reg lut_ctrl;
    reg unusedB04[3];
    struct {
        reg addr;
        reg unused4[3];
        reg data;
        reg unused14[3];
    } lut_addr_data[2];
};
typedef struct _aips_lcdif aips_lcdif;

// AIPS3_CSI
struct _aips_csi {
    reg cr[3]; // n-1
    reg statinfo;
    reg rinfo;
    reg rxcnt;
    reg sr;
    reg unseen1C;
    reg dmasa_statfifo;
    reg dmats_statfifo;
    reg dmasa_fb1;
    reg dmasa_fb2;
    reg fbuf_para;
    reg imag_para;
    reg unseen38[4];
    reg cr18;
    reg cr19;
};
typedef struct _aips_csi aips_csi;

// AIPS3_USDHCn
struct _aips_usdhc {
    reg ds_addr;
    reg blk_att;
    reg cmd_arg;
    reg cmd_xfr_typ;
    reg cmd_rsp[4];
    reg data_buff_acc_port;
    reg pres_state;
    reg prot_ctrl;
    reg sys_ctrl;
    reg int_status;
    reg int_status_en;
    reg int_signal_en;
    reg autocmd12_err_status;
    reg host_ctrl_cap;
    reg wtmk_lvl;
    reg mix_ctrl;
    reg unseen4C;
    reg force_event;
    reg adma_err_status;
    reg adma_sys_addr;
    reg unseen5C;
    reg dll_ctrl;
    reg dll_status;
    reg clk_tune_ctrl_status;
    reg unseen6C[(0xC0 - 0x6C) / 4];
    reg vend_spec;
    reg mmc_boot;
    reg vend_spec2;
    reg tuning_ctrl;
};
typedef struct _aips_usdhc aips_usdhc;

// AIPS3_ENETn
struct _aips_enet {
    reg unseen0;
    reg eir;
    reg eimr;
    reg unseenC;
    reg rdar;
    reg tdar;
    reg unseen18[3];
    reg ecr;
    reg unseen28[(0x40 - 0x28) / 4];
    reg mmfr;
    reg mscr;
    reg unseen48[7];
    reg mibc;
    reg unseen68[7];
    reg rcr;
    reg unseen88[(0xC4 - 0x88) / 4];
    reg tcr;
    reg unseenC8[7];
    reg palr;
    reg paur;
    reg opd;
    reg txic0;
    reg unseenF4[3];
    reg rxic0;
    reg unseen104[5];
    reg iaur;
    reg ialr;
    reg gaur;
    reg galr;
    reg unseen128[7];
    reg tfwr;
    reg unseen148[(0x180 - 0x148) / 4];
    reg rdsr;
    reg tdsr;
    reg mrbr;
    reg unseen18C;
    reg rfsl;
    reg rsem;
    reg raem;
    reg rafl;
    reg tsem;
    reg taem;
    reg tafl;
    reg tipg;
    reg ftrl;
    reg unseen1b4[3];
    reg tacc;
    reg racc;
    reg unseen1c8[(0x204 - 0x1c8) / 4];
    reg rmon_t_packets;
    reg rmon_t_bc_pkt;
    reg rmon_t_mc_pkt;
    reg rmon_t_crc_align;
    reg rmon_t_undersize;
    reg rmon_t_oversize;
    reg rmon_t_frag;
    reg rmon_t_jab;
    reg rmon_t_col;
    reg rmon_t_p64;
    reg rmon_t_p65to127;
    reg rmon_t_p128to255;
    reg rmon_t_p256to511;
    reg rmon_t_p512to1023;
    reg rmon_t_p1024to2047;
    reg rmon_t_p_gte2048;
    reg rmon_t_octets;
    reg unseen248;
    reg ieee_t_frame_ok;
    reg ieee_t_1col;
    reg ieee_t_mcol;
    reg ieee_t_def;
    reg ieee_t_lcol;
    reg ieee_t_excol;
    reg ieee_t_macerr;
    reg ieee_t_cserr;
    reg ieee_t_sqe;
    reg ieee_t_fdxfc;
    reg ieee_t_octets_ok;
    reg unseen278[3];
    reg rmon_r_packets;
    reg rmon_r_bc_pkt;
    reg rmon_r_mc_pkt;
    reg rmon_r_crc_align;
    reg rmon_r_undersize;
    reg rmon_r_oversize;
    reg rmon_r_frag;
    reg rmon_r_jab;
    reg unseen2a4;
    reg rmon_r_p64;
    reg rmon_r_p65to127;
    reg rmon_r_p128to255;
    reg rmon_r_p256to511;
    reg rmon_r_p512to1023;
    reg rmon_r_p1024to2047;
    reg rmon_r_p_gte2048;
    reg rmon_r_octets;
    reg ieee_r_drop;
    reg ieee_r_frame_ok;
    reg ieee_r_crc;
    reg ieee_r_align;
    reg ieee_r_macerr;
    reg ieee_r_fdxfc;
    reg ieee_r_octets_ok;
    reg unseen2E4[(0x400 - 0x2e4) / 4];
    reg atcr;
    reg atvr;
    reg atoff;
    reg atper;
    reg atcor;
    reg atinc;
    reg atstmp;
    reg unseen41C[(0x604 - 0x41c) / 4];
    reg tgsr;
    struct {
        reg tcsr;
        reg tccr;
    } timers[4];
};
typedef struct _aips_enet aips_enet;

// AIPS3_USBCORE
struct _aips_usb_core {
    struct {
        reg id;
        reg hwgeneral;
        reg hwhost;
        reg hwdevice;
        reg hwtxbuf;
        reg hwrxbuf;
        reg unused18[(0x80 - 0x18) / 4];
        struct {
            reg ld;
            reg ctrl;
        } gptimer[2];
        reg sbuscfg;
        reg unused94[(0x100 - 0x94) / 4];
        reg8 caplength;
        reg8 unused101;
        reg16 hciver;
        reg hcsparam;
        reg hccparam;
        reg unseen10C[5];
        reg16 dciver;
        reg16 unused122;
        reg dccparam;
        reg unseen128[6];
        reg usbcmd;
        reg usbsts;
        reg usbintr;
        reg frindex;
        reg unseen150;
        union {
            reg periodiclistbase;
            reg deviceaddr;
        };
        union {
            reg asynclistaddr;
            reg endplistaddr;
        };
        reg unseen15C;
        reg burstsize;
        reg txfilltuning;
        reg unseen168[4];
        reg endptnak;
        reg endptnaken;
        reg configflag;
        reg portsc1;
        reg unseen188[7];
        reg otgsc;
        reg usbmode;
        reg endptsetupstat;
        reg endptprime;
        reg endptflush;
        reg endptstat;
        reg encptcomplete;
        reg endptctrl[8];
        reg unseen1E0[8];
    } uog[2]; // n-1
};
typedef struct _aips_usb_core aips_usb_core;

// AIPS3_SEMC
struct _aips_semc {
    reg mcr;
    reg iocr;
    reg bmcr[2];
    reg br[9];
    reg dllcr;
    reg inten;
    reg intr;
    reg sdramcr[4];
    reg nandcr[4];
    reg norcr[4];
    reg sramcr[4];
    reg dbicr[2];
    reg unseen88[2];
    reg ipcr[3];
    reg ipcmd;
    reg iptxdat;
    reg unseena4[3];
    reg iprxdat;
    reg unseenb4[3];
    reg sts[16];
};
typedef struct _aips_semc aips_semc;

// AIPS 3
struct _aips_3_s {
    reg reserved0[0x7c000 / 4];

    // AIPS Configuration
    union {
        aips_aipstz aipstz;
        reg aipstz_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reserved80000[(AIPS_ENTRY_SIZE * 9) / 4];

    // FLEXSPIs
    union {
        aips_flexspi flexspi2;
        reg flexspi2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_flexspi flexspi;
        reg flexspi_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedAC000[(AIPS_ENTRY_SIZE * 2) / 4];

    // PXP
    union {
        aips_pxp pxp;
        reg pxp_raw[AIPS_ENTRY_SIZE / 4];
    };

    // LCDIF
    union {
        aips_lcdif lcdif;
        reg lcdif_raw[AIPS_ENTRY_SIZE / 4];
    };

    // CSI
    union {
        aips_csi csi;
        reg csi_raw[AIPS_ENTRY_SIZE / 4];
    };

    // USDHCs
    union {
        aips_usdhc usdhc1;
        reg usdhc1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_usdhc usdhc2;
        reg usdhc2_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedC8000[(AIPS_ENTRY_SIZE * 3) / 4];

    // ENETs
    union {
        aips_enet enet2;
        reg enet2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_enet enet;
        reg enet_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedDC000[AIPS_ENTRY_SIZE / 4];

    // USB(CORE)
    union {
        aips_usb_core usb_core;
        reg usb_raw[AIPS_ENTRY_SIZE / 4]; // TODO: include NC stuff for teensy?
    };

    reg reservedE4000[(AIPS_ENTRY_SIZE * 3) / 4];

    // SEMC
    union {
        aips_semc semc;
        reg semc_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedF4000[(AIPS_ENTRY_SIZE * 2) / 4];

    // DCP
    reg dcp_raw[AIPS_ENTRY_SIZE / 4]; // gib docs

};
typedef struct _aips_3_s aips_3_s;


/*
    -------------------------------
    AIPS-4 devices, buses & structs
    -------------------------------
*/
// AIPS4_SPDIF
struct _aips_spdif {
    reg scr;
    reg srcd;
    reg srpc;
    reg sie;
    union {
        reg sic;
        reg sis;
    };
    reg srl;
    reg srr;
    reg srcsh;
    reg srcsl;
    reg sru;
    reg srq;
    reg stl;
    reg str;
    reg stcsch;
    reg stcscl;
    reg unseen3C[2];
    reg srfm;
    reg unseen48[2];
    reg stc;
};
typedef struct _aips_spdif aips_spdif;

// AIPS4_SAIn
struct _aips_sai {
    reg verid;
    reg param;
    reg tcsr;
    reg tcr[5]; // n-1
    reg tdr[4];
    reg unseen30[4];
    reg tfr[4];
    reg unseen50[4];
    reg tmr;
    reg unseen64[9];
    reg rcsr;
    reg rcr[5]; // n-1
    reg rdr[4];
    reg unseenB0[4];
    reg rfr[4];
    reg unseenD0[4];
    reg rmr;
};
typedef struct _aips_sai aips_sai;

// AIPS4_LPSPIn
struct _aips_lpspi {
    reg verid;
    reg param;
    reg unseen8[2];
    reg cr;
    reg sr;
    reg ier;
    reg der;
    reg cfgr[2];
    reg unseen28[2];
    reg dmr[2];
    reg unseen38[2];
    reg ccr;
    reg unseen44[5];
    reg fcr;
    reg fsr;
    reg tcr;
    reg tdr;
    reg unseen68[2];
    reg rsr;
    reg rdr;
};
typedef struct _aips_lpspi aips_lpspi;

// AIPS4_ADC_ETC
struct _aips_adc_etc {
    reg ctrl;
    reg done0_1_irq;
    reg done2_3_err_irq;
    reg dma_ctrl;
    struct {
        reg ctrl;
        reg counter;
        reg chain_1_0; // soo tempting to just array it reg16, since the cpu will anyways sniff up by 32
        reg chain_3_2;
        reg chain_5_4;
        reg chain_7_6;
        reg result_1_0;
        reg result_3_2;
        reg result_5_4;
        reg result_7_6;
    } trig[8];
};
typedef struct _aips_adc_etc aips_adc_etc;

// AIPS4_AOI
struct _aips_aoi {
    struct {
        reg16 r01;
        reg16 r23;
    } bfcrt__n[4];
};
typedef struct _aips_aoi aips_aoi;

// AIPS4_XBAR1
struct _aips_xbar1 {
    reg16 sel[66];
    reg16 ctrl[2];
};
typedef struct _aips_xbar1 aips_xbar1;

// AIPS4_XBAR23
struct _aips_xbar23 {
    reg16 sel[8];
};
typedef struct _aips_xbar23 aips_xbar23;

// AIPS4_QDCn
struct _aips_qdc {
    reg16 ctrl;
    reg16 filt;
    reg16 wtr;
    reg16 posd;
    reg16 posdh;
    reg16 rev;
    reg16 revh;
    reg16 upos;
    reg16 lpos;
    reg16 uposh;
    reg16 lposh;
    reg16 uinit;
    reg16 linit;
    reg16 imr;
    reg16 tst;
    reg16 ctrl2;
    reg16 umod;
    reg16 lmod;
    reg16 ucomp;
    reg16 lcomp;
};
typedef struct _aips_qdc aips_qdc;

// AIPS4_FLEXPWMn
struct _aips_flexpwm {
    struct {
        reg16 cnt;
        reg16 init;
        reg16 ctrl2;
        reg16 ctrl;
        reg16 unseen8;
        reg16 val0;
        reg16 fracval1;
        reg16 val1;
        reg16 fracval2;
        reg16 val2;
        reg16 fracval3;
        reg16 val3;
        reg16 fracval4;
        reg16 val4;
        reg16 fracval5;
        reg16 val5;
        reg16 frctrl;
        reg16 octrl;
        reg16 sts;
        reg16 inten;
        reg16 dmaen;
        reg16 tctrl;
        reg16 dismap0;
        reg16 unseen2E;
        reg16 dtcnt[2];
        reg16 captctrla;
        reg16 captcompa;
        reg16 captctrlb;
        reg16 captcompb;
        reg16 captctrlx;
        reg16 captcompx;
        struct {
            reg16 cval;
            reg16 cval_cyc;
        } cvals[6];
        reg16 unused58[4];
    } sm[4];
    reg16 outen;
    reg16 mask;
    reg16 swcout;
    reg16 dtsrcsel;
    reg16 mctrl;
    reg16 mctrl2;
    reg16 fctrl0;
    reg16 fsts0;
    reg16 ffilt0;
    reg16 ftst0;
    reg16 fctrl20;
};
typedef struct _aips_flexpwm aips_flexpwm;

// AIPS4_LPI2Cn
struct _aips_lpi2c {
    reg verid;
    reg param;
    reg unseen8[2];
    reg mcr;
    reg msr;
    reg mier;
    reg mder;
    reg mcfgr[4];
    reg unseen30[4];
    reg mdmr;
    reg unseen44;
    reg mccr0;
    reg unseen4C;
    reg mccr1;
    reg unseen54;
    reg mfcr;
    reg mfsr;
    reg mtdr;
    reg unseen64[3];
    reg mrdr;
    reg unseen74[(0x110 - 0x74) / 4];
    reg scr;
    reg ssr;
    reg sier;
    reg sder;
    reg unseen120;
    reg scfgr1;
    reg scfgr2;
    reg unseen12C[5];
    reg samr;
    reg unseen144[3];
    reg sasr;
    reg star;
    reg unseen158[2];
    reg stdr;
    reg unseen164[3];
    reg srdr;
};
typedef struct _aips_lpi2c aips_lpi2c;

// AIPS 4
struct _aips_4_s {
    reg reserved0[0x7c000 / 4];

    // AIPS Configuration
    union {
        aips_aipstz aipstz;
        reg aipstz_raw[AIPS_ENTRY_SIZE / 4];
    };

    // SPDIF
    union {
        aips_spdif spdif;
        reg spdif_raw[AIPS_ENTRY_SIZE / 4];
    };

    // SAIs
    union {
        aips_sai sai1;
        reg sai1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_sai sai2;
        reg sai2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_sai sai3;
        reg sai3_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reserved90000[AIPS_ENTRY_SIZE / 4];

    // LPSPIs
    union {
        aips_lpspi lpspi1;
        reg lpspi1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpspi lpspi2;
        reg lpspi2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpspi lpspi3;
        reg lpspi3_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpspi lpspi4;
        reg lpspi4_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedA4000[(AIPS_ENTRY_SIZE * 3) / 4];

    // ADC_ETC
    union {
        aips_adc_etc adc_etc;
        reg adc_etc_raw[AIPS_ENTRY_SIZE / 4];
    };

    // AOIs
    union {
        aips_aoi aoi1;
        reg aoi1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_aoi aoi2;
        reg aoi2_raw[AIPS_ENTRY_SIZE / 4];
    };

    // XBARs
    union {
        aips_xbar1 xbar1;
        reg xbar1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_xbar23 xbar2;
        reg xbar2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_xbar23 xbar3;
        reg xbar3_raw[AIPS_ENTRY_SIZE / 4];
    };

    // QDCs
    union {
        aips_qdc qdc1;
        reg qdc1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_qdc qdc2;
        reg qdc2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_qdc qdc3;
        reg qdc3_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_qdc qdc4;
        reg qdc4_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reservedD8000[AIPS_ENTRY_SIZE / 4];

    // FLEXPWMs
    union {
        aips_flexpwm flexpwm1;
        reg flexpwm1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_flexpwm flexpwm2;
        reg flexpwm2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_flexpwm flexpwm3;
        reg flexpwm3_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_flexpwm flexpwm4;
        reg flexpwm4_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg bee_raw[AIPS_ENTRY_SIZE / 4]; // gib docs

    // LPI2Cs
    union {
        aips_lpi2c lpi2c1;
        reg lpi2c1_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpi2c lpi2c2;
        reg lpi2c2_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpi2c lpi2c3;
        reg lpi2c3_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_lpi2c lpi2c4;
        reg lpi2c4_raw[AIPS_ENTRY_SIZE / 4];
    };
};
typedef struct _aips_4_s aips_4_s;


/*
    -------------------------------
    AIPS-5 devices, buses & structs
    -------------------------------
*/
// AIPS 5
struct _aips_5_s {
    // GPIOs
    union {
        aips_gpio gpio6;
        reg gpio6_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_gpio gpio7;
        reg gpio7_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_gpio gpio8;
        reg gpio8_raw[AIPS_ENTRY_SIZE / 4];
    };
    union {
        aips_gpio gpio9;
        reg gpio9_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reserved10000[(AIPS_ENTRY_SIZE * 4) / 4];

    // FLEXIO3
    union {
        aips_flexio flexio3;
        reg flexio3_raw[AIPS_ENTRY_SIZE / 4];
    };

    reg reserved24000[(0x00100000 - 0x00024000) / 4];
};
typedef struct _aips_5_s aips_5_s;

#undef reg
#undef reg8
#undef reg16
#undef reg32

#endif