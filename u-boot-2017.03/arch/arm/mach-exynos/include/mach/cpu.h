/*
 * (C) Copyright 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _EXYNOS4_CPU_H
#define _EXYNOS4_CPU_H

#define DEVICE_NOT_AVAILABLE		0

#define EXYNOS_CPU_NAME			"Exynos"
#define EXYNOS4_ADDR_BASE		0x10000000

/* EXYNOS4 Common*/
#define EXYNOS4_I2C_SPACING		0x10000

#define EXYNOS4_GPIO_PART3_BASE		0x03860000
#define EXYNOS4_PRO_ID			0x10000000
#define EXYNOS4_SYSREG_BASE		0x10010000
#define EXYNOS4_POWER_BASE		0x10020000
#define EXYNOS4_SWRESET			0x10020400
#define EXYNOS4_CLOCK_BASE		0x10030000
#define EXYNOS4_SYSTIMER_BASE		0x10050000
#define EXYNOS4_WATCHDOG_BASE		0x10060000
#define EXYNOS4_TZPC_BASE		0x10110000
#define EXYNOS4_DMC_CTRL_BASE		0x10400000
#define EXYNOS4_MIU_BASE		0x10600000
#define EXYNOS4_ACE_SFR_BASE		0x10830000
#define EXYNOS4_GPIO_PART2_BASE		0x11000000
#define EXYNOS4_GPIO_PART2_0		0x11000000 /* GPJ0 */
#define EXYNOS4_GPIO_PART2_1		0x11000c00 /* GPX0 */
#define EXYNOS4_GPIO_PART1_BASE		0x11400000
#define EXYNOS4_FIMD_BASE		0x11C00000
#define EXYNOS4_MIPI_DSIM_BASE		0x11C80000
#define EXYNOS4_USBOTG_BASE		0x12480000
#define EXYNOS4_MMC_BASE		0x12510000
#define EXYNOS4_SROMC_BASE		0x12570000
#define EXYNOS4_USB_HOST_EHCI_BASE	0x12580000
#define EXYNOS4_USBPHY_BASE		0x125B0000
#define EXYNOS4_UART_BASE		0x13800000
#define EXYNOS4_I2C_BASE		0x13860000
#define EXYNOS4_ADC_BASE		0x13910000
#define EXYNOS4_SPI_BASE		0x13920000
#define EXYNOS4_PWMTIMER_BASE		0x139D0000
#define EXYNOS4_MODEM_BASE		0x13A00000
#define EXYNOS4_USBPHY_CONTROL		0x10020704
#define EXYNOS4_I2S_BASE		0xE2100000

#define EXYNOS4_GPIO_PART4_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4_DP_BASE			DEVICE_NOT_AVAILABLE
#define EXYNOS4_SPI_ISP_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4_DMC_PHY_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4_AUDIOSS_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4_USB_HOST_XHCI_BASE	DEVICE_NOT_AVAILABLE
#define EXYNOS4_USB3PHY_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4_DMC_TZASC_BASE		DEVICE_NOT_AVAILABLE

/* EXYNOS4X12 */
#define EXYNOS4X12_GPIO_PART3_BASE	0x03860000
#define EXYNOS4X12_PRO_ID		0x10000000
#define EXYNOS4X12_SYSREG_BASE		0x10010000
#define EXYNOS4X12_POWER_BASE		0x10020000
#define EXYNOS4X12_SWRESET		0x10020400
#define EXYNOS4X12_USBPHY_CONTROL	0x10020704
#define EXYNOS4X12_CLOCK_BASE		0x10030000
#define EXYNOS4X12_SYSTIMER_BASE	0x10050000
#define EXYNOS4X12_WATCHDOG_BASE	0x10060000
#define EXYNOS4X12_TZPC_BASE		0x10110000
#define EXYNOS4X12_DMC_CTRL_BASE	0x10600000
#define EXYNOS4X12_GPIO_PART4_BASE	0x106E0000
#define EXYNOS4X12_DMC_TZASC_BASE	0x10700000 /* add by Flinn */
#define EXYNOS4X12_ACE_SFR_BASE		0x10830000
#define EXYNOS4X12_GPIO_PART2_BASE	0x11000000
#define EXYNOS4X12_GPIO_PART2_0		0x11000000
#define EXYNOS4X12_GPIO_PART2_1		0x11000040 /* GPK0 */
#define EXYNOS4X12_GPIO_PART2_2		0x11000260 /* GPM0 */
#define EXYNOS4X12_GPIO_PART2_3		0x11000c00 /* GPX0 */
#define EXYNOS4X12_GPIO_PART1_BASE	0x11400000
#define EXYNOS4X12_GPIO_PART1_0		0x11400000 /* GPA0 */
#define EXYNOS4X12_GPIO_PART1_1		0x11400180 /* GPF0 */
#define EXYNOS4X12_GPIO_PART1_2		0x11400240 /* GPJ0 */
#define EXYNOS4X12_FIMD_BASE		0x11C00000
#define EXYNOS4X12_MIPI_DSIM_BASE	0x11C80000
#define EXYNOS4X12_USBOTG_BASE		0x12480000
#define EXYNOS4X12_MMC_BASE		0x12510000
#define EXYNOS4X12_SROMC_BASE		0x12570000
#define EXYNOS4X12_USB_HOST_EHCI_BASE	0x12580000
#define EXYNOS4X12_USBPHY_BASE		0x125B0000
#define EXYNOS4X12_UART_BASE		0x13800000
#define EXYNOS4X12_I2C_BASE		0x13860000
#define EXYNOS4X12_PWMTIMER_BASE	0x139D0000

#define EXYNOS4X12_ADC_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_DP_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_MODEM_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_I2S_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_SPI_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_SPI_ISP_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_DMC_PHY_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_AUDIOSS_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_USB_HOST_XHCI_BASE	DEVICE_NOT_AVAILABLE
#define EXYNOS4X12_USB3PHY_BASE		DEVICE_NOT_AVAILABLE
//#define EXYNOS4X12_DMC_TZASC_BASE	DEVICE_NOT_AVAILABLE

/* EXYNOS5 */
#define EXYNOS5_I2C_SPACING		0x10000

#define EXYNOS5_AUDIOSS_BASE		0x03810000
#define EXYNOS5_GPIO_PART8_BASE		0x03860000
#define EXYNOS5_PRO_ID			0x10000000
#define EXYNOS5_CLOCK_BASE		0x10010000
#define EXYNOS5_POWER_BASE		0x10040000
#define EXYNOS5_SWRESET			0x10040400
#define EXYNOS5_SYSREG_BASE		0x10050000
#define EXYNOS5_TZPC_BASE		0x10100000
#define EXYNOS5_WATCHDOG_BASE		0x101D0000
#define EXYNOS5_ACE_SFR_BASE		0x10830000
#define EXYNOS5_DMC_PHY_BASE		0x10C00000
#define EXYNOS5_GPIO_PART5_BASE		0x10D10000
#define EXYNOS5_GPIO_PART6_BASE		0x10D10060
#define EXYNOS5_GPIO_PART7_BASE		0x10D100C0
#define EXYNOS5_DMC_CTRL_BASE		0x10DD0000
#define EXYNOS5_GPIO_PART1_BASE		0x11400000
#define EXYNOS5_GPIO_PART2_BASE		0x114002E0
#define EXYNOS5_GPIO_PART3_BASE		0x11400C00
#define EXYNOS5_MIPI_DSIM_BASE		0x11D00000
#define EXYNOS5_USB_HOST_XHCI_BASE	0x12000000
#define EXYNOS5_USB3PHY_BASE		0x12100000
#define EXYNOS5_USB_HOST_EHCI_BASE	0x12110000
#define EXYNOS5_USBPHY_BASE		0x12130000
#define EXYNOS5_USBOTG_BASE		0x12140000
#define EXYNOS5_MMC_BASE		0x12200000
#define EXYNOS5_SROMC_BASE		0x12250000
#define EXYNOS5_UART_BASE		0x12C00000
#define EXYNOS5_I2C_BASE		0x12C60000
#define EXYNOS5_SPI_BASE		0x12D20000
#define EXYNOS5_I2S_BASE		0x12D60000
#define EXYNOS5_PWMTIMER_BASE		0x12DD0000
#define EXYNOS5_SPI_ISP_BASE		0x131A0000
#define EXYNOS5_GPIO_PART4_BASE		0x13400000
#define EXYNOS5_FIMD_BASE		0x14400000
#define EXYNOS5_DP_BASE			0x145B0000

#define EXYNOS5_ADC_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5_MODEM_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5_DMC_TZASC_BASE		DEVICE_NOT_AVAILABLE

/* EXYNOS5420 */
#define EXYNOS5420_AUDIOSS_BASE		0x03810000
#define EXYNOS5420_GPIO_PART6_BASE	0x03860000
#define EXYNOS5420_PRO_ID		0x10000000
#define EXYNOS5420_CLOCK_BASE		0x10010000
#define EXYNOS5420_POWER_BASE		0x10040000
#define EXYNOS5420_SWRESET		0x10040400
#define EXYNOS5420_INFORM_BASE		0x10040800
#define EXYNOS5420_SPARE_BASE		0x10040900
#define EXYNOS5420_CPU_CONFIG_BASE	0x10042000
#define EXYNOS5420_CPU_STATUS_BASE	0x10042004
#define EXYNOS5420_SYSREG_BASE		0x10050000
#define EXYNOS5420_TZPC_BASE		0x100E0000
#define EXYNOS5420_WATCHDOG_BASE	0x101D0000
#define EXYNOS5420_ACE_SFR_BASE		0x10830000
#define EXYNOS5420_DMC_PHY_BASE		0x10C00000
#define EXYNOS5420_DMC_CTRL_BASE	0x10C20000
#define EXYNOS5420_DMC_TZASC_BASE	0x10D40000
#define EXYNOS5420_USB_HOST_EHCI_BASE	0x12110000
#define EXYNOS5420_MMC_BASE		0x12200000
#define EXYNOS5420_SROMC_BASE		0x12250000
#define EXYNOS5420_USB3PHY_BASE	0x12500000
#define EXYNOS5420_UART_BASE		0x12C00000
#define EXYNOS5420_I2C_BASE		0x12C60000
#define EXYNOS5420_I2C_8910_BASE	0x12E00000
#define EXYNOS5420_SPI_BASE		0x12D20000
#define EXYNOS5420_I2S_BASE		0x12D60000
#define EXYNOS5420_PWMTIMER_BASE	0x12DD0000
#define EXYNOS5420_SPI_ISP_BASE		0x131A0000
#define EXYNOS5420_GPIO_PART2_BASE	0x13400000
#define EXYNOS5420_GPIO_PART3_BASE	0x13400C00
#define EXYNOS5420_GPIO_PART4_BASE	0x13410000
#define EXYNOS5420_GPIO_PART5_BASE	0x14000000
#define EXYNOS5420_GPIO_PART1_BASE	0x14010000
#define EXYNOS5420_MIPI_DSIM_BASE	0x14500000
#define EXYNOS5420_DP_BASE		0x145B0000

#define EXYNOS5420_USBPHY_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5420_USBOTG_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5420_FIMD_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5420_ADC_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5420_MODEM_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5420_USB_HOST_XHCI_BASE	DEVICE_NOT_AVAILABLE


#ifndef __ASSEMBLY__
#include <asm/io.h>
/* CPU detection macros */
extern unsigned int s5p_cpu_id;
extern unsigned int s5p_cpu_rev;

static inline int s5p_get_cpu_rev(void)
{
	return s5p_cpu_rev;
}

static inline void s5p_set_cpu_id(void)
{
	unsigned int pro_id = readl(EXYNOS4_PRO_ID);
	unsigned int cpu_id = (pro_id & 0x00FFF000) >> 12;
	unsigned int cpu_rev = pro_id & 0x000000FF;

	switch (cpu_id) {
	case 0x200:
		/* Exynos4210 EVT0 */
		s5p_cpu_id = 0x4210;
		s5p_cpu_rev = 0;
		break;
	case 0x210:
		/* Exynos4210 EVT1 */
		s5p_cpu_id = 0x4210;
		s5p_cpu_rev = cpu_rev;
		break;
	case 0x412:
		/* Exynos4412 */
		s5p_cpu_id = 0x4412;
		s5p_cpu_rev = cpu_rev;
		break;
	case 0x520:
		/* Exynos5250 */
		s5p_cpu_id = 0x5250;
		break;
	case 0x420:
		/* Exynos5420 */
		s5p_cpu_id = 0x5420;
		break;
	case 0x422:
		/*
		 * Exynos5800 is a variant of Exynos5420
		 * and has product id 0x5422
		 */
		s5p_cpu_id = 0x5422;
		break;
	}
}

static inline char *s5p_get_cpu_name(void)
{
	return EXYNOS_CPU_NAME;
}

#define IS_SAMSUNG_TYPE(type, id)			\
static inline int __attribute__((no_instrument_function)) cpu_is_##type(void) \
{							\
	return (s5p_cpu_id >> 12) == id;		\
}

IS_SAMSUNG_TYPE(exynos4, 0x4)
IS_SAMSUNG_TYPE(exynos5, 0x5)

#define IS_EXYNOS_TYPE(type, id)			\
static inline int __attribute__((no_instrument_function)) \
	proid_is_##type(void)				\
{							\
	return s5p_cpu_id == id;			\
}

IS_EXYNOS_TYPE(exynos4210, 0x4210)
IS_EXYNOS_TYPE(exynos4412, 0x4412)
IS_EXYNOS_TYPE(exynos5250, 0x5250)
IS_EXYNOS_TYPE(exynos5420, 0x5420)
IS_EXYNOS_TYPE(exynos5422, 0x5422)

#define SAMSUNG_BASE(device, base)				\
static inline unsigned long __attribute__((no_instrument_function)) \
	samsung_get_base_##device(void) \
{								\
	if (cpu_is_exynos4()) {				\
		if (proid_is_exynos4412())			\
			return EXYNOS4X12_##base;		\
		return EXYNOS4_##base;				\
	} else if (cpu_is_exynos5()) {				\
		if (proid_is_exynos5420() || proid_is_exynos5422())	\
			return EXYNOS5420_##base;		\
		return EXYNOS5_##base;				\
	}							\
	return 0;						\
}

SAMSUNG_BASE(adc, ADC_BASE)
SAMSUNG_BASE(clock, CLOCK_BASE)
SAMSUNG_BASE(ace_sfr, ACE_SFR_BASE)
SAMSUNG_BASE(sysreg, SYSREG_BASE)
SAMSUNG_BASE(i2c, I2C_BASE)
SAMSUNG_BASE(i2s, I2S_BASE)
SAMSUNG_BASE(mipi_dsim, MIPI_DSIM_BASE)
SAMSUNG_BASE(gpio_part1, GPIO_PART1_BASE)
SAMSUNG_BASE(gpio_part2, GPIO_PART2_BASE)
SAMSUNG_BASE(gpio_part3, GPIO_PART3_BASE)
SAMSUNG_BASE(gpio_part4, GPIO_PART4_BASE)
SAMSUNG_BASE(pro_id, PRO_ID)
SAMSUNG_BASE(mmc, MMC_BASE)
SAMSUNG_BASE(modem, MODEM_BASE)
SAMSUNG_BASE(sromc, SROMC_BASE)
SAMSUNG_BASE(swreset, SWRESET)
SAMSUNG_BASE(timer, PWMTIMER_BASE)
SAMSUNG_BASE(uart, UART_BASE)
SAMSUNG_BASE(usb_phy, USBPHY_BASE)
SAMSUNG_BASE(usb3_phy, USB3PHY_BASE)
SAMSUNG_BASE(usb_ehci, USB_HOST_EHCI_BASE)
SAMSUNG_BASE(usb_xhci, USB_HOST_XHCI_BASE)
SAMSUNG_BASE(usb_otg, USBOTG_BASE)
SAMSUNG_BASE(watchdog, WATCHDOG_BASE)
SAMSUNG_BASE(power, POWER_BASE)
SAMSUNG_BASE(spi, SPI_BASE)
SAMSUNG_BASE(spi_isp, SPI_ISP_BASE)
SAMSUNG_BASE(tzpc, TZPC_BASE)
SAMSUNG_BASE(dmc_ctrl, DMC_CTRL_BASE)
SAMSUNG_BASE(dmc_phy, DMC_PHY_BASE)
SAMSUNG_BASE(dmc_tzasc, DMC_TZASC_BASE)
SAMSUNG_BASE(audio_ass, AUDIOSS_BASE)
#endif

#endif	/* _EXYNOS4_CPU_H */
