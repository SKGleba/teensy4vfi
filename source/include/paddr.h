#ifndef __PADDR_H__
#define __PADDR_H__

#define ITCM_OFFSET             0x00000000
#define ITCM_SIZE                           0x00080000
#define DTCM_OFFSET             0x20000000
#define DTCM_SIZE                           0x00080000
#define OCRAM2_OFFSET           0x20200000
#define OCRAM2_SIZE                         0x00080000
#define OCRAM_FR_OFFSET         0x20280000
#define OCRAM_FR_SIZE                       0x00080000
#define AIPS_1_OFFSET           0x40000000
#define AIPS_1_SIZE                         0x00100000
#define AIPS_2_OFFSET           0x40100000
#define AIPS_2_SIZE                         0x00100000
#define AIPS_3_OFFSET           0x40200000
#define AIPS_3_SIZE                         0x00100000
#define AIPS_4_OFFSET           0x40300000
#define AIPS_4_SIZE                         0x00100000
#define MAIN_CFG_PORT_OFFSET    0x41000000
#define MAIN_CFG_PORT_SIZE                  0x00100000
#define M_CFG_PORT_OFFSET       0x41100000
#define M_CFG_PORT_SIZE                     0x00100000
#define CPU_CFG_PORT_OFFSET     0x41400000
#define CPU_CFG_PORT_SIZE                   0x00100000
#define AIPS_5_OFFSET           0x42000000
#define AIPS_5_SIZE                         0x00100000
#define FLEXSPI_CT_OFFSET       0x60000000
#define FLEXSPI_CT_SIZE                     0x10000000
#define FLEXSPI2_CT_OFFSET      0x70000000
#define FLEXSPI2_CT_SIZE                    0x0F000000
#define FLEXSPI2_TXFIFO_OFFSET  0x7F000000
#define FLEXSPI2_TXFIFO_SIZE                0x00400000
#define FLEXSPI2_RXFIFO_OFFSET  0x7F400000
#define FLEXSPI2_RXFIFO_SIZE                0x00400000
#define FLEXSPI_TXFIFO_OFFSET   0x7F800000
#define FLEXSPI_TXFIFO_SIZE                 0x00400000
#define FLEXSPI_RXFIFO_OFFSET   0x7FC00000
#define FLEXSPI_RXFIFO_SIZE                 0x00400000
#define SEMC_SHARED_OFFSET      0x80000000
#define SEMC_SHARED_SIZE                    0x60000000
#define CM7_PPB_OFFSET          0xE0000000
#define CM7_PPB_SIZE                        0x00100000

#define PADDR_AFTER_ENTRY(entry) (entry ## _OFFSET + entry ## _SIZE)
#define PADDR_ENTRY_SEPARATION(prev,next) (next ## _OFFSET - PADDR_AFTER_ENTRY(prev))

#endif