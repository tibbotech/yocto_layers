/*
 * Sunplus timer test driver
 *
 * Copyright (C) 2018 Sunplus Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/irq.h>
#include <linux/of_irq.h>

#define WATCHDOG_CMD_CNT_WR_UNLOCK	0xAB00
#define WATCHDOG_CMD_CNT_WR_LOCK	0xAB01
#define WATCHDOG_CMD_CNT_WR_MAX		0xDEAF
#define WATCHDOG_CMD_PAUSE		0x3877
#define WATCHDOG_CMD_RESUME		0x4A4B
#define WATCHDOG_CMD_INTR_CLR		0x7482

struct stc_reg {
	volatile unsigned int stc_15_0;
	volatile unsigned int stc_31_16;
	volatile unsigned int stc_64;
	volatile unsigned int stc_divisor;
	volatile unsigned int rtc_15_0;
	volatile unsigned int rtc_23_16;
	volatile unsigned int rtc_divisor;
	volatile unsigned int stc_config;
	volatile unsigned int timer0_ctrl;
	volatile unsigned int timer0_cnt;
	volatile unsigned int timer1_ctrl;
	volatile unsigned int timer1_cnt;
	volatile unsigned int timerw_ctrl;	/* Only STCs @ 0x9C000600 and 0x9C003000 */
	volatile unsigned int timerw_cnt;	/* Only STCs @ 0x9C000600 and 0x9C003000 */
	volatile unsigned int stc_47_32;
	volatile unsigned int stc_63_48;
	volatile unsigned int timer2_ctrl;
	volatile unsigned int timer2_divisor;
	volatile unsigned int timer2_reload;
	volatile unsigned int timer2_cnt;
	volatile unsigned int timer3_ctrl;
	volatile unsigned int timer3_divisor;
	volatile unsigned int timer3_reload;
	volatile unsigned int timer3_cnt;
	volatile unsigned int stcl_0;
	volatile unsigned int stcl_1;
	volatile unsigned int stcl_2;
	volatile unsigned int atc_0;
	volatile unsigned int atc_1;
	volatile unsigned int atc_2;
	volatile unsigned int timer0_reload;
	volatile unsigned int timer1_reload;
};

#define NUM_STC		4
#define NUM_IRQ		5	/* max. number of interrupts for each STC */

struct stc_info_s {
	char irq_name[NUM_IRQ][32];
	int irq[NUM_IRQ];
	u32 interrupt_cnt[NUM_IRQ];
};
static struct stc_info_s stc_info[NUM_STC];

static const struct platform_device_id sp_tmr_tst_devtypes[] = {
	{
		.name = "sp_tmr_tst",
	}, {
		/* sentinel */
	}
};

static const struct of_device_id sp_tmr_tst_dt_ids[] = {
	{
		.compatible = "sunplus,sp-tmr-tst",
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, sp_tmr_tst_dt_ids);

static irqreturn_t sp_tmr_tst_irq(int irq, void *args)
{
	int i, j;
	struct stc_info_s *ptr;

	ptr = (struct stc_info_s *)(args);
	for (i = 0; i < NUM_STC; i++) {
		if (ptr == &stc_info[i]) {
			for (j = 0; j < NUM_IRQ; j++) {
				if (irq == stc_info[i].irq[j]) {
					stc_info[i].interrupt_cnt[j]++;
					if (j == (NUM_IRQ - 1)) {
						panic("Watchdog timeout, die here\n");
						while(1);
					} else  if ((stc_info[i].interrupt_cnt[j] & 0x003F) == 0) {	/* limits output messages */
						printk(KERN_INFO "%s, %s, %u\n", __func__, stc_info[i].irq_name[j], stc_info[i].interrupt_cnt[j]);
					}
				}
			}
			break;
		}
	}

	return IRQ_HANDLED;
}

static int sp_tmr_tst_probe(struct platform_device *pdev)
{
	struct resource *res_mem;
	const struct of_device_id *match;
	int ret, num_irq;
	int i;
	u32 val;
	static int idx_stc = 0;
	struct stc_reg *stc_ptr;
	void __iomem *membase;
	u32 counter_src;

	if (idx_stc >= NUM_STC) {
		printk(KERN_ERR "Error: %s, %d\n", __func__, __LINE__);
		return -EINVAL;
	}

	printk(KERN_INFO "%s, %d\n", __func__, __LINE__);

	memset(&stc_info[idx_stc], 0, sizeof(stc_info[0]));
	if (pdev->dev.of_node) {
		match = of_match_node(sp_tmr_tst_dt_ids, pdev->dev.of_node);
		if (match == NULL) {
			printk(KERN_ERR "Error: %s, %d\n", __func__, __LINE__);
			return -ENODEV;
		}
		num_irq = of_irq_count(pdev->dev.of_node);
		if (num_irq > NUM_IRQ) {
			printk(KERN_ERR "Error: %s, %d\n", __func__, __LINE__);
		}
	}

	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (IS_ERR(res_mem)) {
		printk(KERN_ERR "Error: %s, %d\n", __func__, __LINE__);
		return PTR_ERR(res_mem);
	}

	membase = devm_ioremap_resource(&pdev->dev, res_mem);
	if (IS_ERR(membase)) {
		printk(KERN_ERR "Error: %s, %d\n", __func__, __LINE__);
		return PTR_ERR(membase);
	}

	stc_ptr = (struct stc_reg *)(membase);
	writel((0x1000 - 1), &(stc_ptr->stc_divisor));
	writel(0, &(stc_ptr->stc_64));		/* reset STC */

	writel((0x0002 - 1), &(stc_ptr->rtc_divisor));
	writel(0, &(stc_ptr->rtc_23_16));	/* reset RTC */

	counter_src = 1;	/* 0: system clock, 1: STC, 2: RTC, 3: ... */

	/* timer0: repeat mode, start */
	val = 0x0100 - 1;
	writel(val, &(stc_ptr->timer0_cnt));
	writel(val, &(stc_ptr->timer0_reload));
	writel((counter_src << 14) | (1 << 13) | (1 << 11), &(stc_ptr->timer0_ctrl));

	/* timer1: repeat mode, start */
	val = 0x0100 - 1;
	writel(val, &(stc_ptr->timer1_cnt));
	writel(val, &(stc_ptr->timer1_reload));
	writel((counter_src << 14) | (1 << 13) | (1 << 11), &(stc_ptr->timer1_ctrl));

	/* timer2: repeat mode, start */
	writel(0x0002 - 1, &(stc_ptr->timer2_divisor));
	val = 0x0100 - 1;
	writel(val, &(stc_ptr->timer2_cnt));
	writel(val, &(stc_ptr->timer2_reload));
	writel((counter_src << 2) | (1 << 1) | (1 << 0), &(stc_ptr->timer2_ctrl));

	/* timer3: repeat mode, start */
	writel(0x0004 - 1, &(stc_ptr->timer3_divisor));
	val = 0x0100 - 1;
	writel(val, &(stc_ptr->timer3_cnt));
	writel(val, &(stc_ptr->timer3_reload));
	writel((counter_src << 2) | (1 << 1) | (1 << 0), &(stc_ptr->timer3_ctrl));

	for (i = 0; i < num_irq; i++) {
		stc_info[idx_stc].irq[i] = platform_get_irq(pdev, i);
		if (stc_info[idx_stc].irq[i] < 0) {
			printk(KERN_ERR "Error: %s, %d, irq[%d]\n", __func__, __LINE__, i);
			return -ENODEV;
		}
		sprintf(stc_info[idx_stc].irq_name[i], "irq%d_%x", i, (u32)(res_mem->start));
		ret = request_irq(stc_info[idx_stc].irq[i], sp_tmr_tst_irq, 0, stc_info[idx_stc].irq_name[i], &stc_info[idx_stc]);
		if (ret) {
			printk(KERN_ERR "Error: %s, %d, irq[%d]\n", __func__, __LINE__, i);
			return ret;
		}
		printk(KERN_INFO "%s, %d, irq[%d]: %d, %s\n", __func__, __LINE__, i, stc_info[idx_stc].irq[i], stc_info[idx_stc].irq_name[i]);
	}
#if 0
	/* Watchdog timer */
	/* Need to enable bit 2 and 4 of G(4, 29) to cause chip reset:
	 * If set them in iboot:
	 *     MOON4_REG->misc_ctl_0 = RF_MASK_V((0x1 << 2), (1 << 2));
	 *     MOON4_REG->misc_ctl_0 = RF_MASK_V((0x1 << 4), (1 << 4));
	 * Or enable them in U-Boot's arch_misc_init() in arch/arm/mach-pentagram/cpu.c:
	 *     ptr = (volatile unsigned int *)(PENTAGRAM_BASE_ADDR + (4 << 7) + (29 << 2));
	 *     *ptr = (0x0014 << 16) | (0x0014);
	 */
	writel(WATCHDOG_CMD_CNT_WR_UNLOCK, &(stc_ptr->timerw_ctrl));
	writel(WATCHDOG_CMD_PAUSE, &(stc_ptr->timerw_ctrl));
	writel(0x0800, &(stc_ptr->timerw_cnt));
	writel(WATCHDOG_CMD_RESUME, &(stc_ptr->timerw_ctrl));
	writel(WATCHDOG_CMD_CNT_WR_LOCK, &(stc_ptr->timerw_ctrl));
#endif
	idx_stc++;
	return 0;

}

static struct platform_driver ssc_driver = {
	.driver		= {
		.name		= "sp_tmr_tst",
		.of_match_table	= of_match_ptr(sp_tmr_tst_dt_ids),
	},
	.id_table	= sp_tmr_tst_devtypes,
	.probe		= sp_tmr_tst_probe,
};
module_platform_driver(ssc_driver);

MODULE_DESCRIPTION("Timer test driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:sp_tmr_tst");
