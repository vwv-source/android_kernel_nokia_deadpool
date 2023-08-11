#ifndef __FIH_CPU_H
#define __FIH_CPU_H

/* Refer the 80-P2468-2X_MSM8937 HARDWARE REGISTER DESCRIPTION.pdf
	FIH_MSM_HW_REV_NUM is mean JTAG_ID(0x000A607C) (P.887)
	FIH_MSM_HW_FEATURE_ID is mean QFPROM_CORR_JTAG_ID[bits 27:20](0x000A4120) (P.843)
	FIH_MSM_SERIAL_NO is mean QFPROM_CORR_SERIAL_NUM(0x0005C008) (P.2767)
*/
#define FIH_MSM_HW_REV_NUM  0x000A607C
#define FIH_MSM_HW_FEATURE_ID  0x000A4120
#define FIH_MSM_SERIAL_NO   0x0005C008

/*
  Refer the 80-pf886-4_a_sdm439 DEVICE REVISION GUIDE.pdf
  Refer the 80-pf887-4_a_sdm429 DEVICE REVISION GUIDE.pdf
*/
#define HW_REV_NUM_SDM439	0x000BF0E1
#define HW_REV_NUM_SDM429	0x000BE0E1

#define HW_REV_ES 0x00
#define HW_REV_CS 0x01

#endif /* __FIH_CPU_H */
