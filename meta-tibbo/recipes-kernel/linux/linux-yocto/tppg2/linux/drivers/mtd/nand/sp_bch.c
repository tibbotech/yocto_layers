#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include "sp_bch.h"
#include <mach/hardware.h>

#define ISP_MODE	 (*(volatile unsigned char *)(SRAM0_BASE + 0x9e36))

static struct sp_bch_chip __this;
unsigned int* sp_bch_ftl_info;
unsigned int* Get_SP_BCH_Info(void)
{
	return sp_bch_ftl_info;
}
EXPORT_SYMBOL(Get_SP_BCH_Info);

static int get_setbits(uint32_t n)
{
	n = n - ((n >> 1) & 0x55555555);
	n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
	return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

static int sp_bch_blank(dma_addr_t ecc, int len)
{
	uint8_t *oob = ioremap(ecc, len);
	int mbe = 4; /* Max. BIT errors */
	int ret = 0;
	int i, n;

	if (oob) {
		ret = 1;
		for (i = 2, n = 0; i < len; ++i) {
			if (oob[i] != 0xff)
				n += get_setbits(oob[i] ^ 0xff);
			if (n > mbe) {
				ret = 0;
				break;
			}
		}
	}

	if (oob)
		iounmap(oob);

	return ret;
}

static int sp_bch_reset(struct sp_bch_chip *chip)
{
	struct sp_bch_regs *regs = chip->regs;
	unsigned long timeout = jiffies + msecs_to_jiffies(50);

	writel(32 << 4, &regs->cr1);
	/* reset controller */
	writel(SRR_RESET, &regs->srr);
	while (jiffies < timeout) {
		if (!(readl(&regs->srr) & SRR_RESET))
			break;
		cpu_relax();
	}
	if (jiffies >= timeout) {
		printk("sp_bch: reset timeout\n");
		return -1;
	}

	/* reset interrupts */
	writel(IER_DONE | IER_FAIL, &regs->ier);
	writel(ISR_BCH, &regs->isr);

	return 0;
}

static int sp_bch_wait(struct sp_bch_chip *chip)
{
	int ret = 0;

	if (ISP_MODE != 0xA5) {
		if (!wait_event_timeout(chip->wq, !chip->busy, HZ/10)) {
			if (chip->busy == 0) {
				printk(KERN_WARNING "sp_bch 1..system irq busy\n");
				return 0;
			}
			ret = -EBUSY;
		}
	} else {
		if (!wait_event_timeout(chip->wq, !chip->busy, HZ)) {
			if (chip->busy == 0) {
				printk(KERN_WARNING "sp_bch 2..system irq busy\n");
				return 0;
			}
			ret = -EBUSY;
		}
	}

	return ret;
}

static irqreturn_t sp_bch_irq(int irq, void *dev)
{
	struct sp_bch_chip *chip = dev;
	struct sp_bch_regs *regs = chip->regs;
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(1);
	writel(ISR_BCH, &regs->isr);
	chip->busy = 0;
	wake_up(&chip->wq);

	if (time_after(jiffies, timeout)) {
		printk(KERN_ERR "more 1ms");
	}

	return IRQ_HANDLED;
}

int sp_bch_init(struct mtd_info *mtd)
{
	struct sp_bch_chip *chip = &__this;
	struct nand_chip *nand;
	struct nand_oobfree *oobfree;
	int i;
	int oobsz;
	int pgsz;
	int rsvd;  /* Reserved bytes for YAFFS2 */
	int size;  /* BCH data length per sector */
	int bits;  /* BCH strength per sector */
	int nrps;  /* BCH parity sector number */
	int pssz;  /* BCH parity sector size */
	int free;  /* BCH free bytes per sector */

	if (!mtd || !chip)
		BUG();

	if (!mtd->priv)
		BUG();

	rsvd = 32;
	oobsz = mtd->oobsize;
	pgsz = mtd->writesize;
	nand = mtd->priv;
	chip->mtd = mtd;

	/* 1024x60 */
	size = 1024;
	bits = 60;
	nrps = pgsz >> 10;
	chip->cr0 = CR0_CMODE_1024x60 | CR0_NBLKS(nrps);
	if (oobsz >= nrps * 128) {
		chip->cr0 |= CR0_DMODE(0) | CR0_BMODE(5);
		pssz = 128;
		free = 23;
	} else if (oobsz >= nrps * 112) {
		chip->cr0 |= CR0_DMODE(1) | CR0_BMODE(1);
		pssz = 112;
		free = 7;
	} else {
		pssz = 0;
		free = 0;
	}
	if (free * nrps >= rsvd)
		goto ecc_detected;

	/* 1024x40 */
	size = 1024;
	bits = 40;
	nrps = pgsz >> 10;
	chip->cr0 = CR0_CMODE_1024x40 | CR0_NBLKS(nrps);
	if (oobsz >= nrps * 96) {
		chip->cr0 |= CR0_DMODE(0) | CR0_BMODE(6);
		pssz = 96;
		free = 26;
	} else if (oobsz >= nrps * 80) {
		chip->cr0 |= CR0_DMODE(1) | CR0_BMODE(2);
		pssz = 80;
		free = 10;
	} else {
		pssz = 0;
		free = 0;
	}
	if (free * nrps >= rsvd)
		goto ecc_detected;

	/* 1024x24 */
	size = 1024;
	bits = 24;
	nrps = pgsz >> 10;
	chip->cr0 = CR0_CMODE_1024x24 | CR0_NBLKS(nrps);
	if (oobsz >= nrps * 64) {
		chip->cr0 |= CR0_DMODE(0) | CR0_BMODE(5);
		pssz = 64;
		free = 22;
	} else if (oobsz >= nrps * 48) {
		chip->cr0 |= CR0_DMODE(1) | CR0_BMODE(1);
		pssz = 48;
		free = 6;
	} else {
		pssz = 0;
		free = 0;
	}
	if (free * nrps >= rsvd)
		goto ecc_detected;

	/* 1024x16 */
	size = 1024;
	bits = 16;
	nrps = pgsz >> 10;
	chip->cr0 = CR0_CMODE_1024x16 | CR0_NBLKS(nrps);
	if (oobsz >= nrps * 64) {
		chip->cr0 |= CR0_DMODE(0) | CR0_BMODE(6);
		pssz = 64;
		free = 28;
	} else if (oobsz >= nrps * 48) {
		chip->cr0 |= CR0_DMODE(1) | CR0_BMODE(4);
		pssz = 48;
		free = 20;
	} else {
		pssz = 0;
		free = 0;
	}
	if (free * nrps >= rsvd)
		goto ecc_detected;

	/* 512x8 */
	size = 512;
	bits = 8;
	nrps = pgsz >> 9;
	chip->cr0 = CR0_CMODE_512x8 | CR0_NBLKS(nrps);
	if (oobsz >= nrps * 32) {
		chip->cr0 |= CR0_DMODE(0) | CR0_BMODE(4);
		pssz = 32;
		free = 18;
	} else if (oobsz >= nrps * 16) {
		chip->cr0 |= CR0_DMODE(1) | CR0_BMODE(0);
		pssz = 16;
		free = 2;
	} else {
		pssz = 0;
		free = 0;
	}
	if (free * nrps >= rsvd)
		goto ecc_detected;

	/* 512x4 */
	size = 512;
	bits = 4;
	nrps = pgsz >> 9;
	chip->cr0 = CR0_CMODE_512x4 | CR0_NBLKS(nrps);
	if (oobsz >= nrps * 32) {
		chip->cr0 |= CR0_DMODE(0) | CR0_BMODE(6);
		pssz = 32;
		free = 25;
	} else {
		chip->cr0 |= CR0_DMODE(1) | CR0_BMODE(2);
		pssz = 16;
		free = 9;
	}

ecc_detected:
	printk("sp_bch: ecc mode=%ux%u, cr0=0x%x\n", size, bits, chip->cr0);
	nand->ecc.size = size;
	nand->ecc.steps = nrps;
	nand->ecc.bytes = ((12 + (size >> 9)) * bits + 7) >> 3;

	/* sanity check */
	if (nand->ecc.steps > MTD_MAX_OOBFREE_ENTRIES_LARGE)
		BUG();

	if (free * nrps < rsvd)
		BUG();

	nand->ecc.layout->oobavail = 0;
	oobfree = nand->ecc.layout->oobfree;
	for (i = 0; i < nand->ecc.steps; ++i) {
		oobfree->offset = i * pssz;
		oobfree->length = free;

		/* reserved bad block + scrambler marker */
		if (i == 0) {
			oobfree->offset += 2;
			oobfree->length -= 2;
		}

		if (oobfree->length) {
			nand->ecc.layout->oobavail += oobfree->length;
			++oobfree;
		}
	}

	printk("sp_bch: oob avail=%u\n", nand->ecc.layout->oobavail);
	sp_bch_ftl_info = (unsigned int *)chip;
	/* printk("sp_bch chip addr:0x%p,0x%p\n", chip,sp_bch_ftl_info); */

#if 0
	for (i = 0; i < nand->ecc.steps; ++i) {
		printk("off=%u, len=%u\n",
			nand->ecc.layout->oobfree[i].offset,
			nand->ecc.layout->oobfree[i].length);
	}
	printk("\n");
#endif

	mutex_lock(&chip->lock);
	sp_bch_reset(chip);
	mutex_unlock(&chip->lock);

	return 0;
}
EXPORT_SYMBOL(sp_bch_init);

/*
 * Calculate BCH ecc code
 */
int sp_bch_encode(struct mtd_info *mtd, dma_addr_t buf, dma_addr_t ecc)
{
	struct sp_bch_chip *chip = &__this;
	struct sp_bch_regs *regs = chip->regs;
	int ret;

	if (!mtd)
		BUG();

	if (chip->mtd != mtd)
		sp_bch_init(mtd);

	mutex_lock(&chip->lock);

	writel(buf, &regs->buf);
	writel(ecc, &regs->ecc);

	chip->busy = 1;
	writel(CR0_START | CR0_ENCODE | chip->cr0, &regs->cr0);
	ret = sp_bch_wait(chip);
	if (ret)
		printk("sp_bch: encode timeout\n");

	mutex_unlock(&chip->lock);

	return ret;
}
EXPORT_SYMBOL(sp_bch_encode);

/*
 * Detect and correct bit errors
 */
int sp_bch_decode(struct mtd_info *mtd, dma_addr_t buf, dma_addr_t ecc)
{
	struct sp_bch_chip *chip = &__this;
	struct sp_bch_regs *regs = chip->regs;
	uint32_t status;
	int ret;

	if (!mtd)
		BUG();

	if (chip->mtd != mtd)
		sp_bch_init(mtd);

	mutex_lock(&chip->lock);

	writel(buf, &regs->buf);
	writel(ecc, &regs->ecc);

	chip->busy = 1;
	writel(CR0_START | CR0_DECODE | chip->cr0, &regs->cr0);
	ret = sp_bch_wait(chip);
	status = readl(&regs->sr);
	if (ret) {
		printk("sp_bch: decode timeout\n");
	} else if (status & SR_FAIL) {
#if 0
		if ((status & SR_BLANK_FF) || (status & SR_BLANK_00)) {
#else
		if (sp_bch_blank(ecc, mtd->oobsize)) {
#endif
			/* printk("sp_bch: decode All FF or 00!\n"); */
			ret = 0;
		} else {
			printk("sp_bch: decode failed.\n");
			mtd->ecc_stats.failed += SR_ERR_BITS(status);
			sp_bch_reset(chip);
			ret = 2;
		}
	} else {
		mtd->ecc_stats.corrected += SR_ERR_BITS(status);
	}

	mutex_unlock(&chip->lock);

	return ret;
}
EXPORT_SYMBOL(sp_bch_decode);

static long sp_bch_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct sp_bch_chip *chip = &__this;
	struct sp_bch_regs *regs = chip->regs;
	void __user *argp = (void __user *)arg;
	unsigned long size;
	long ret = 0;
	void *buf;
	struct sp_bch_req *req;
	dma_addr_t buf_phys, ecc_phys;

	size = (cmd & IOCSIZE_MASK) >> IOCSIZE_SHIFT;
	if (cmd & IOC_IN) {
		if (!access_ok(VERIFY_READ, argp, size))
			return -EFAULT;
	}
	if (cmd & IOC_OUT) {
		if (!access_ok(VERIFY_WRITE, argp, size))
			return -EFAULT;
	}

	buf = kmalloc(sizeof(*req) + 32, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	/* make sure it's 32 bytes aligned */
	req = (void *)(((unsigned)buf + 31) & 0xffffffe0);

	switch (cmd) {
	case SP_BCH_IOC1K60ENC:
		if (copy_from_user(req, argp, sizeof(*req))) {
			ret = -EFAULT;
			break;
		}

		mutex_lock(&chip->lock);

		buf_phys = dma_map_single(chip->dev, req->buf, 1024, DMA_TO_DEVICE);
		ecc_phys = dma_map_single(chip->dev, req->ecc, 128, DMA_BIDIRECTIONAL);

		writel(buf_phys, &regs->buf);
		writel(ecc_phys, &regs->ecc);

		chip->busy = 1;
		writel(CR0_START | CR0_ENCODE | CR0_CMODE_1024x60, &regs->cr0);
		if (sp_bch_wait(chip)) {
			printk("sp_bch: 1k60 encode timeout\n");
			ret = -EFAULT;
		}

		dma_unmap_single(chip->dev, buf_phys, 1024, DMA_TO_DEVICE);
		dma_unmap_single(chip->dev, ecc_phys, 128, DMA_BIDIRECTIONAL);

		mutex_unlock(&chip->lock);

		if (copy_to_user(argp, req, sizeof(*req)))
			ret = -EFAULT;
		break;

	case SP_BCH_IOC1K60DEC:
		if (copy_from_user(req, argp, sizeof(*req))) {
			ret = -EFAULT;
			break;
		}

		mutex_lock(&chip->lock);

		buf_phys = dma_map_single(chip->dev, req->buf, 1024, DMA_BIDIRECTIONAL);
		ecc_phys = dma_map_single(chip->dev, req->ecc, 128, DMA_TO_DEVICE);

		writel(buf_phys, &regs->buf);
		writel(ecc_phys, &regs->ecc);

		chip->busy = 1;
		writel(CR0_START | CR0_DECODE | CR0_CMODE_1024x60, &regs->cr0);
		if (sp_bch_wait(chip)) {
			printk("sp_bch: 1k60 decode timeout\n");
			ret = -EFAULT;
		}

		dma_unmap_single(chip->dev, buf_phys, 1024, DMA_BIDIRECTIONAL);
		dma_unmap_single(chip->dev, ecc_phys, 128, DMA_TO_DEVICE);

		mutex_unlock(&chip->lock);

		if (copy_to_user(argp, req, sizeof(*req)))
			ret = -EFAULT;
		break;

	default:
		ret = -ENOTTY;
		break;
	}

	kfree(buf);

	return ret;
}

static struct file_operations sp_bch_fops = {
	.owner          = THIS_MODULE,
	.llseek         = no_llseek,
	.compat_ioctl   = sp_bch_ioctl,
	.unlocked_ioctl = sp_bch_ioctl,
};

static struct miscdevice sp_bch_dev = {
	.name = "sp_bch",
	.fops = &sp_bch_fops,
};

static int sp_bch_probe(struct platform_device *pdev)
{
	struct sp_bch_chip *chip = &__this;
	struct resource *res;
	int ret = 0;

	/*
	 * use reasonable defaults so platforms don't have to provide these.
	 * with DT probing on ARM, none of these are set.
	 */
	if (!pdev->dev.dma_mask)
		pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask;
	if (!pdev->dev.coherent_dma_mask)
		pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);

	memset(chip, 0, sizeof(*chip));

	mutex_init(&chip->lock);
	init_waitqueue_head(&chip->wq);

	chip->dev = &pdev->dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		ret = -ENXIO;
		return ret;
	}
	if (!request_mem_region(res->start, resource_size(res), "sp_bch")) {
		ret = -EBUSY;
		goto err;
	}
	chip->regs = ioremap(res->start, resource_size(res));
	if (!chip->regs) {
		ret = -ENOMEM;
		goto err;
	}

	chip->irq = platform_get_irq(pdev, 0);
	if (chip->irq <= 0) {
		ret = -ENXIO;
		goto err;
	}

	printk("sp_bch: mmio=0x%p, irq=%d\n", chip->regs, chip->irq);

	if (sp_bch_reset(chip)) {
		ret = -ENXIO;
		goto err;
	}

	if (request_irq(chip->irq, sp_bch_irq, 0, "sp_bch", chip)) {
		printk("sp_bch: unable to register IRQ(%d)\n", chip->irq);
		ret = -EBUSY;
		goto err;
	}

	platform_set_drvdata(pdev, chip);

	ret = misc_register(&sp_bch_dev);

	return ret;

err:
	release_mem_region(res->start, resource_size(res));
	return ret;
}

static int sp_bch_remove(struct platform_device *pdev)
{
	struct sp_bch_chip *chip = platform_get_drvdata(pdev);
	struct resource *res;

	misc_deregister(&sp_bch_dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (res)
		release_mem_region(res->start, resource_size(res));

	if (!chip)
		BUG();

	if (chip->regs)
		iounmap(chip->regs);

	if (chip->irq)
		free_irq(chip->irq, "sp_bch");

	wake_up(&chip->wq);
	mutex_destroy(&chip->lock);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef CONFIG_PM
static int sp_bch_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	return 0;
}

static int sp_bch_resume(struct platform_device *pdev)
{
	return 0;
}
#else
#define sp_bch_suspend	NULL
#define sp_bch_resume	NULL
#endif

static const struct of_device_id of_fun_match[] = {
	{ .compatible = "sunplus,sp_bch" },
	{},
};
MODULE_DEVICE_TABLE(of, of_fun_match);

static struct platform_driver sp_bch_driver = {
	.probe = sp_bch_probe,
	.remove = sp_bch_remove,
	.shutdown = NULL,
	.suspend = sp_bch_suspend,
	.resume = sp_bch_resume,
	.driver = {
			.name = "sunplus,sp_bch",
			.owner = THIS_MODULE,
			.of_match_table = of_fun_match,
		   },
};

module_platform_driver(sp_bch_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Sunplus BCH controller");
