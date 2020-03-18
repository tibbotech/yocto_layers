#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/usb/phy.h>
#include <linux/usb/sp_usb.h>

static struct resource *uphy1_res_mem;

static void uphy1_init(void)
{
	u32 val, set;
	void __iomem *usb_otp_reg;
	void __iomem *regs = (void __iomem *)B_SYSTEM_BASE;

	usb_otp_reg = ioremap_nocache(USB_OTP_REG, 1);

	/* 1. Default value modification */
	writel(RF_MASK_V(0xffff, 0x4002), regs + UPHY1_CTL0_OFFSET);
	writel(RF_MASK_V(0xffff, 0x8747), regs + UPHY1_CTL1_OFFSET);

	/* 2. PLL power off/on twice */
	writel(RF_MASK_V(0xffff, 0x88), regs + UPHY1_CTL3_OFFSET);
	mdelay(1);
	writel(RF_MASK_V(0xffff, 0x80), regs + UPHY1_CTL3_OFFSET);
	mdelay(1);
	writel(RF_MASK_V(0xffff, 0x88), regs + UPHY1_CTL3_OFFSET);
	mdelay(1);
	writel(RF_MASK_V(0xffff, 0x80), regs + UPHY1_CTL3_OFFSET);
	mdelay(1);
	writel(RF_MASK_V(0xffff, 0x00), regs + UPHY1_CTL3_OFFSET);


	/* 3. reset UPHY 1 */
	writel(RF_MASK_V_SET(1 << 14), regs + USB_RESET_OFFSET);
	writel(RF_MASK_V_CLR(1 << 14), regs + USB_RESET_OFFSET);
	mdelay(1);

	/* 4.b board uphy 1 internal register modification for tid certification */
	val = readl(usb_otp_reg);
	set = (val >> UPHY1_OTP_DISC_LEVEL_OFFSET) & OTP_DISC_LEVEL_BIT;
	if (!set || set >= OTP_DISC_LEVEL_BIT) {
		set = DISC_LEVEL_DEFAULT;
	}

	val = readl(uphy1_base_addr + DISC_LEVEL_OFFSET);
	val = (val & ~OTP_DISC_LEVEL_BIT) | (OTP_DISC_LEVEL_TEMP - set);
	writel(val, uphy1_base_addr + DISC_LEVEL_OFFSET);
	
	val = readl(uphy1_base_addr + ECO_PATH_OFFSET);
	val &= ~(ECO_PATH_SET);
	writel(val, uphy1_base_addr + ECO_PATH_OFFSET);
	val = readl(uphy1_base_addr + POWER_SAVING_OFFSET);
	val &= ~(POWER_SAVING_SET);
	writel(val, uphy1_base_addr + POWER_SAVING_OFFSET);

	/* 5. USBC 1 reset */
	writel(RF_MASK_V_SET(1 << 11), regs + USB_RESET_OFFSET);
	writel(RF_MASK_V_CLR(1 << 11), regs + USB_RESET_OFFSET);

	/* port 1 uphy clk fix */
	writel(RF_MASK_V_SET(1 << 6), regs + UPHY1_CTL2_OFFSET);

	/* 6. switch to host */
	writel(RF_MASK_V_SET(3 << 12), regs + USBC_CTL_OFFSET);

#ifdef CONFIG_USB_SUNPLUS_OTG
	writel(RF_MASK_V_CLR(1 << 12), regs + USBC_CTL_OFFSET);
	mdelay(1);
#endif

	iounmap(usb_otp_reg);
}

static int sunplus_usb_phy1_probe(struct platform_device *pdev)
{
	uphy1_irq_num = platform_get_irq(pdev, 0);
	if (uphy1_irq_num < 0) {
		printk(KERN_NOTICE "no irq provieded,ret:%d\n",uphy1_irq_num);
		return uphy1_irq_num;
	}
	printk(KERN_NOTICE "uphy0_irq:%d\n",uphy1_irq_num);

	uphy1_res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!uphy1_res_mem) {
		printk(KERN_NOTICE "no memory recourse provieded");
		return -ENXIO;
	}

	if (!request_mem_region(uphy1_res_mem->start, resource_size(uphy1_res_mem), "uphy0")) {
		printk(KERN_NOTICE "hw already in use");
		return -EBUSY;
	}

	uphy1_base_addr = ioremap_nocache(uphy1_res_mem->start, resource_size(uphy1_res_mem));
	if (!uphy1_base_addr){
		release_mem_region(uphy1_res_mem->start, resource_size(uphy1_res_mem));
		 return -EFAULT;
	}
	
	uphy1_init();

	return 0;
}

static int sunplus_usb_phy1_remove(struct platform_device *pdev)
{
	iounmap(uphy1_base_addr);
	release_mem_region(uphy1_res_mem->start, resource_size(uphy1_res_mem));

	return 0;
}

static const struct of_device_id phy1_sunplus_dt_ids[] = {
	{ .compatible = "sunplus,sunplus-usb-phy1" },
	{ }
};

MODULE_DEVICE_TABLE(of, phy1_sunplus_dt_ids);

static struct platform_driver sunplus_usb_phy1_driver = {
	.probe		= sunplus_usb_phy1_probe,
	.remove		= sunplus_usb_phy1_remove,
	.driver		= {
		.name	= "sunplus-usb-phy1",
		.of_match_table = phy1_sunplus_dt_ids,
	},
};


static int __init usb_phy1_sunplus_init(void)
{
	if (sp_port_enabled & PORT1_ENABLED) {
		printk(KERN_NOTICE "register sunplus_usb_phy1_driver\n");
		return platform_driver_register(&sunplus_usb_phy1_driver);	
	} else {
		printk(KERN_NOTICE "uphy1 not enabled\n");
		return 0;
	}
}
subsys_initcall(usb_phy1_sunplus_init);

static void __exit usb_phy1_sunplus_exit(void)
{
	if (sp_port_enabled & PORT1_ENABLED) {
		printk(KERN_NOTICE "unregister sunplus_usb_phy1_driver\n");
		platform_driver_unregister(&sunplus_usb_phy1_driver);
	} else {
		printk(KERN_NOTICE "uphy1 not enabled\n");
		return;
	}
}
module_exit(usb_phy1_sunplus_exit);


MODULE_ALIAS("sunplus_usb_phy1");
MODULE_AUTHOR("qiang.deng");
MODULE_LICENSE("GPL");

