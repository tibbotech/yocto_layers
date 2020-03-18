#include <common.h>
#include <malloc.h>
//#include <asm/io.h>
#include <nand.h>

#include <linux/mtd/nand.h>
#include <dm.h>
#include <linux/io.h>
#include <linux/ioport.h>

#include "sp_bch.h"
#include "sp_spinand.h"


/**************************************************************************
 *                             M A C R O S                                *
 **************************************************************************/
/* SRAM */
/* #define CFG_BBT_USE_FLASH */
#define CFG_BUFF_MAX		(18 << 10)
#define CONFIG_MTD_SP_NAND_SRAM_BASE	(0x9e800000)
#define QUADIO_DIS_ECC 	(0x01)
#define WB_BUF1_DIS_ECC (0x08)

/* device id */
#define ESMT1G_ID 0x21C8
#define GD_ID 0xC8
#define GD_ID_HIGH 0x2100
#define WB_ID 0xEF
#define MXIC_ID 0xC2


static void wait_spi_idle(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	int retry = 0;
	
	do {
		retry++;
	} while((readl(&regs->spi_ctrl) & SPI_DEVICE_IDLE) && (retry < 100));
}

static int sp_spinand_reset(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	int ret = -1;
	int retry = 0;
	//initial
	do {
		retry++;
	} while((readl(&regs->spi_ctrl) & SPI_DEVICE_IDLE) && (retry < 100));
	
	/* ==== Flash reset ==== */
	value = (SPI_NAND_CHIP_A)|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_RESET<<8)|(SPI_NAND_CTRL_EN)|(SPI_NAND_WRITE_MDOE);
	writel(value, &regs->spi_ctrl);

	value = SPINAND_CFG01_DEFAULT3;
	writel(value, &regs->spi_cfg[1]);	

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value, &regs->spi_auto_cfg);

	wait_spi_idle(info);

	retry = 0;	
	do {
		ret = spi_nand_getfeatures(info, DEVICE_STATUS_ADDR);
		retry++;
	} while ((ret & 0x01) && (retry < 100));

	return ret;
}

static void spi_nand_readid(struct sp_spinand_info *info, uint32_t addr, uint32_t *data)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;

	writel(addr, &regs->spi_page_addr);
	/*read 3 byte cycle same to 8388 */
	value = SPI_NAND_CHIP_A|SPI_NAND_AUTO_WEL|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_READID<<8)|SPI_NAND_CTRL_EN|(SPINAND_CUSTCMD_3_DATA)|(SPINAND_CUSTCMD_1_ADDR);
	writel(value, &regs->spi_ctrl);

	value = SPINAND_CFG01_DEFAULT;
	writel(value ,&regs->spi_cfg[1]);

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);

	value = readl(&regs->spi_data);

	printk("\nReadID:0x%02x,0x%02x,0x%02x,0x%02x\n",
	       (value & 0xFF),
	       ((value >> 8) & 0xFF),
	       ((value >> 16) & 0xFF),
	       ((value >> 24) & 0xFF));

	*data = value;
	
	return ;
}

static int spi_nand_getfeatures(struct sp_spinand_info *info, uint32_t addr)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	
	value = (SPI_NAND_CHIP_A)|(SPI_NAND_AUTO_WEL)|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_GETFEATURES<<8)|(SPI_NAND_CTRL_EN)|(SPINAND_CUSTCMD_1_DATA)|(SPINAND_CUSTCMD_1_ADDR);
	writel(value ,&regs->spi_ctrl);

	writel(addr ,&regs->spi_page_addr);

	value = SPINAND_CFG01_DEFAULT;
	writel(value ,&regs->spi_cfg[1]);

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);

	return (readl(&regs->spi_data) & 0xFF);
}

static void spi_nand_setfeatures(struct sp_spinand_info *info,uint32_t addr, uint32_t data)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;

	value = (SPI_NAND_CHIP_A)|(SPI_NAND_AUTO_WEL)|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_SETFEATURES<<8)|(SPI_NAND_CTRL_EN)|(SPINAND_CUSTCMD_1_DATA)|(SPI_NAND_WRITE_MDOE)|(SPINAND_CUSTCMD_1_ADDR);
	writel(value ,&regs->spi_ctrl);

	writel(addr ,&regs->spi_page_addr);

	writel(data ,&regs->spi_data);

	value = SPINAND_CFG01_DEFAULT1;
	writel(value ,&regs->spi_cfg[1]);

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);
	
	return ;
}

static int spi_nand_blkerase(struct sp_spinand_info *info, uint32_t addr)
{	
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	
	value = (1<<24)|(1<<20)|(1<<19)|(7<<16)|(0xd8<<8)|(1<<7)|(0<<4)|(1<<2)|(3);
	writel(value ,&regs->spi_ctrl);

	writel(addr ,&regs->spi_page_addr);

	value = 0x150095;
	writel(value ,&regs->spi_cfg[1]);

	value = (1<<21);
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);

	value = (1<<21)|(1<<19);
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);
	
	value = readl(&regs->spi_status);

	if (value & ERASE_STATUS) {
		printk("\nErase Fail!\n");
		value = 0x1;
	} else {
		value = 0x0;
	}
	
	return value;
}

static void spi_nanddma_pageread_prep(struct sp_spinand_info *info, uint32_t addr, unsigned int size, uint32_t *pbuf)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	int retry;
	
	while ((readl(&regs->spi_auto_cfg) & SPI_NAND_DMA_OWNER) && (retry < 100)) {
		retry++;
	}

	if (retry >= 100) {
		printk("sp_spinand: dma busy timeout \n");
		info->err |= SPI_NAND_READ_FAIL;
		return ;
	}
	
	value = SPI_NAND_CHIP_A|SPI_NAND_CLK_32DIV|SPI_NAND_CTRL_EN|(2);
	writel(value, &regs->spi_ctrl);

	writel(addr, &regs->spi_page_addr);

	value = 0x08350095; // 4 bit data 8 dummy clock 1bit cmd  1bit addr
	writel(value, &regs->spi_cfg[1]);

	value = readl(&regs->spi_cfg[0]);
	value = value|size; // 1k data len
	writel(value, &regs->spi_cfg[0]);

	if ((addr & 0x40) && (((info->id & 0xFF) == 0xC2)||((info->id & 0xFF) == 0x2C)))		
		value = 0x1000;
	else
		value = 0x0;
	
	writel(value, &regs->spi_col_addr);

	value = (0x40<<4)|(0x1);
	writel(value, &regs->spi_page_size);

	writel((uint32_t)pbuf, &regs->mem_data_addr);

	//config ctrl info	
	//set auto cfg
	value = 0x5;
	writel(value, &regs->spi_intr_msk);
	value = 0x7;
	writel(value, &regs->spi_intr_sts);
	
	value = (0x6b<<24)|(1<<20)|(1<18);
	writel(value, &regs->spi_auto_cfg);
	
	return ;
}

static void spi_nanddma_run(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;

	value = (1<<17);
	writel(value, &regs->spi_auto_cfg);

	return ;
}

static void spi_nanddma_down(struct sp_spinand_info *info)
{
	int ret;
	
	info->busy = 1;
	
	spi_nanddma_run(info);

	ret = sp_spinand_wait(info);
	if (ret) {
		printk("sp_spinand: descriptor timeout, cmd=0x%x\n", info->cmd);
		sp_spinand_reset(info);
	}
	
	return ;
}

#if 0
static void spi_nanddma_pageread(struct sp_spinand_info *info, uint32_t addr, unsigned int size, uint32_t *pbuf)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	int retry;
	
	while ((readl(&regs->spi_auto_cfg) & SPI_NAND_DMA_OWNER) && (retry < 100)) {
		retry++;
	}

	if (retry >= 100) {
		printk("sp_spinand: dma busy timeout \n");
		info->err |= SPI_NAND_READ_FAIL;
		return ;
	}
	
	value = SPI_NAND_CHIP_A|SPI_NAND_CLK_32DIV|SPI_NAND_CTRL_EN|(2);
	writel(value, &regs->spi_ctrl);

	writel(addr, &regs->spi_page_addr);

	value = 0x08350095; // 4 bit data 8 dummy clock 1bit cmd  1bit addr
	writel(value, &regs->spi_cfg[1]);

	value = readl(&regs->spi_cfg[0]);
	value = value|size; // 1k data len
	writel(value, &regs->spi_cfg[0]);

	if ((addr & 0x40) && (((info->id & 0xFF) == 0xC2)||((info->id & 0xFF) == 0x2C)))		
		value = 0x1000;
	else
		value = 0x0;
	
	writel(value, &regs->spi_col_addr);

	value = (0x40<<4)|(0x1);
	writel(value, &regs->spi_page_size);

	writel((uint32_t)pbuf, &regs->mem_data_addr);

	//config ctrl info	
	//set auto cfg
	value = 0x5;
	writel(value, &regs->spi_intr_msk);
	value = 0x7;
	writel(value, &regs->spi_intr_sts);
	
	value = (0x6b<<24)|(1<<20)|(1<18)|(1<<17);
	writel(value, &regs->spi_auto_cfg);

	/* porting  dma ready */
	//while((readl(&regs->spi_intr_sts) & 0x2) == 0x0);

	return ;
}
#endif 

static void spi_nanddma_pageprogram_prep(struct sp_spinand_info *info, uint32_t addr, unsigned int size, uint32_t *pbuf)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;	
	int retry;
	
	/* polling DMA_OWNER == 0 */
	while ((readl(&regs->spi_auto_cfg) & SPI_NAND_DMA_OWNER) && (retry < 100)) {
		retry++;
	}
	
	if (retry >= 100) {
		printk("sp_spinand: dma busy timeout \n");
		info->err |= SPI_NAND_WRITE_FAIL;
		return ;
	}

	value = (1<<24)|(7<<16)|(1<<7)|(1<<2)|(2);
	writel(value, &regs->spi_ctrl);

	writel(addr, &regs->spi_page_addr);

	// config device info
	//set cfg[1]= cmd 1 bit addr 1 bit data 1 bit	
	value = 0x150095;
	writel(value, &regs->spi_cfg[1]);

	//read 2k data
	value = readl(&regs->spi_cfg[0]);
	value |= size; 
	writel(value, &regs->spi_cfg[0]);
	
	// col addr set
	if ((addr & 0x40) && (((info->id & 0xFF) == 0xC2)||((info->id & 0xFF) == 0x2C)))		
		value = 0x1000;
	else
		value = 0x0;
	writel(value, &regs->spi_col_addr);

	//set nand page size
	value = (0x0<<15)|(0x40<<4)|(0x1); 
	writel(value, &regs->spi_page_size);

	//page size 2K
	writel((uint32_t)pbuf, &regs->mem_data_addr);

	//config ctrl info	
	//set auto cfg 	
	value = 0x5;	
	writel((uint32_t)value, &regs->spi_intr_msk);
	value = 0x7;	
	writel((uint32_t)value, &regs->spi_intr_sts);

	value = (1<18)|(0x02<<8)|(1);
	writel(value, &regs->spi_auto_cfg);

	//polling dma operation done bit	
	//while((readl(&regs->spi_intr_sts) & 0x2) == 0x0);

	return;
}

#if 0
static void spi_nanddma_pageprogram(struct sp_spinand_info *info, uint32_t addr, unsigned int size, uint32_t *pbuf)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;	
	int retry;
	
	/* polling DMA_OWNER == 0 */
	while ((readl(&regs->spi_auto_cfg) & SPI_NAND_DMA_OWNER) && (retry < 100)) {
		retry++;
	}
	
	if (retry >= 100) {
		printk("sp_spinand: dma busy timeout \n");
		info->err |= SPI_NAND_WRITE_FAIL;
		return ;
	}

	value = (1<<24)|(7<<16)|(1<<7)|(1<<2)|(2);
	writel(value, &regs->spi_ctrl);

	writel(addr, &regs->spi_page_addr);

	// config device info
	//set cfg[1]= cmd 1 bit addr 1 bit data 1 bit	
	value = 0x150095;
	writel(value, &regs->spi_cfg[1]);

	//read 2k data
	value = readl(&regs->spi_cfg[0]);
	value |= size; 
	writel(value, &regs->spi_cfg[0]);
	
	// col addr set
	if ((addr & 0x40) && (((info->id & 0xFF) == 0xC2)||((info->id & 0xFF) == 0x2C)))		
		value = 0x1000;
	else
		value = 0x0;
	writel(value, &regs->spi_col_addr);

	//set nand page size
	value = (0x0<<15)|(0x40<<4)|(0x1); 
	writel(value, &regs->spi_page_size);

	//page size 2K
	writel((uint32_t)pbuf, &regs->mem_data_addr);

	//config ctrl info	
	//set auto cfg 	
	value = 0x5;	
	writel((uint32_t)value, &regs->spi_intr_msk);
	value = 0x7;	
	writel((uint32_t)value, &regs->spi_intr_sts);

	value = (1<18)|(1<<17)|(0x02<<8)|(1);
	writel(value, &regs->spi_auto_cfg);

	//polling dma operation done bit	
	//while((readl(&regs->spi_intr_sts) & 0x2) == 0x0);

	return;
}
#endif

static irqreturn_t sp_nand_irq(int irq, void *dev)
{
	struct sp_nand_info *info = dev;
	struct sp_nand_regs *regs = info->regs;
	uint32_t st;
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(1);

	/* clear intrrupt flag */	
	st = readl(&regs->spi_intr_sts);
	writel(st, &regs->spi_intr_sts);

	info->busy = 0;
	wake_up(&info->wq);

	if (time_after(jiffies, timeout))
	{
		printk(KERN_ERR "more 1ms");
	}

	return IRQ_HANDLED;
}

static int sp_spinand_wait(struct sp_spinand_info *info)
{
	int ret = 0;

	if (!wait_event_timeout(info->wq, !info->busy, HZ/10)) {
		/*HZ=100, means timeout=100*jiffies, jiffies=every 10ms plus one */
		if (info->busy == 0) {
			printk(KERN_WARNING "sp_nand ..system irq busy?\n");
			return ret;
		}
		ret = -1;
		printk("\n\nERROR!!!! sp_nand_wait timeout\n\n");		
	}	

	return ret;
}

static void sp_spinand_write_page(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t * buf)
{
	struct sp_spinand_info *info = container_of(mtd, struct sp_spinand_info, mtd);

	down(&info->spnand_sem);
	chip->write_buf(mtd, buf, mtd->writesize);
	chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);

	sp_bch_encode(mtd, info->buff.phys, info->buff.phys + mtd->writesize);

	up(&info->spnand_sem);
}

static int sp_spinand_read_page(struct mtd_info *mtd, struct nand_chip *chip, uint8_t * buf, int page)
{
	struct sp_spinand_info *info = container_of(mtd, struct sp_spinand_info, mtd);
	int ret = 0;

	down(&info->spnand_sem);

	ret = sp_spinand_wait(info);
	if (ret) {
		printk("sp_spinand: descriptor timeout, cmd=0x%x\n", info->cmd);
		sp_spinand_reset(info);
		goto exit_pp;
	}
	
	if ((info->cmd == NAND_CMD_READ0)||(info->cmd == NAND_CMD_READOOB)) {
		/*check spi nand control busy/ready status */
		if (info->err&SPI_NAND_READ_FAIL) {
			info->err&=~SPI_NAND_READ_FAIL;
			goto exit_pp;
		}
	}

	ret = sp_bch_decode(mtd, info->buff.phys, info->buff.phys + mtd->writesize);
	if (ret) {
		printk("sp_spinand: bch decode failed at page=%d\n", page);
		goto exit_pp;
	}

	chip->read_buf(mtd, buf, mtd->writesize);

	if (page < 0)
		up(&info->spnand_sem);
	else {
		chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);
		up(&info->spnand_sem);
	}
	return ret;

exit_pp:
	ret = -1;
	up(&info->spnand_sem);
	return ret;
}

static void sp_spinand_write_buf(struct mtd_info *mtd, const u_char * buf, int len)
{
	dma_addr_t src, dst;
	struct sp_spinand_info *info = container_of(mtd, struct sp_spinand_info, mtd);

	if (virt_addr_valid(buf) || virt_addr_valid(buf + len - 1)) {
		printk("buf addr valid \n");
		return;
	}

	memcpy(info->buff.virt + info->buff.idx, buf, len);

	info->buff.idx += len;
}

static void sp_spinand_read_buf(struct mtd_info *mtd, u_char * buf, int len)
{
	dma_addr_t src, dst;

	struct sp_spinand_info *info = container_of(mtd, struct sp_spinand_info, mtd);

	if (virt_addr_valid(buf) || virt_addr_valid(buf + len - 1)) {
		printk("buf addr valid \n");
		return;
	}

	memcpy(buf, info->buff.virt + info->buff.idx, len);

	info->buff.idx += len;
}

static uint8_t sp_spinand_read_byte(struct mtd_info *mtd)
{
	struct sp_spinand_info *info = container_of(mtd, struct sp_spinand_info, mtd);
	uint8_t ret = 0;

	switch (info->cmd) {
		case NAND_CMD_STATUS:
			ret = readb(info->buff.virt);
			break;

		default:
			if (info->buff.idx < info->buff.size) {
				ret = readb(info->buff.virt + info->buff.idx);
				info->buff.idx += 1;
			}
			break;
	}

	return ret;
}

/*
 * Check hardware register for wait status. Returns 1 if device is ready,
 * 0 if it is still busy.
 */
static int sp_spinand_dev_ready(struct mtd_info *mtd)
{
	return 1;
}

static void sp_spinand_desc_prep(struct sp_spinand_info *info, uint8_t cmd, int col, int row)
{
	mutex_lock(&info->lock);
	switch (cmd) {
		case NAND_CMD_READOOB:	/* 0x50 */
			col += info->mtd.writesize;
		/* fall through */

		case NAND_CMD_READ0:	/* 0x00 */
			info->cmd = SPINAND_CMD_PAGE2CACHE;
			info->buff.idx = col;
			info->row = row;
			//spi_nanddma_pageread(info, row, ((info->mtd.writesize) + (info->mtd.oobsize)), (unsigned int *)info->buff.phys);	
			spi_nanddma_pageread_prep(info, row, ((info->mtd.writesize) + (info->mtd.oobsize)), (unsigned int *)info->buff.phys);
			break;

		case NAND_CMD_SEQIN:	/* 0x80 */
			info->cmd = SPINAND_CMD_PROLOADx4;
			info->buff.idx = col;
			spi_nand_setfeatures(info, DEVICE_PROTECTION_ADDR, 0x0);
			spi_nand_wren(info);
			info->row = row;
			break;

		case NAND_CMD_ERASE1:	/* 0x60 */
			info->cmd = SPINAND_CMD_BLKERASE;
			spi_nand_wren(info);	/* for MXIC,ESMT */
			spi_nand_setfeatures(info, DEVICE_PROTECTION_ADDR, 0x0);
			spi_nand_wren(info);
			if (spi_nand_blkerase(info, row)) {
				pr_err("block erase fail! \n");
				info->err |= SPI_NAND_ERASE_FAIL;
			}
			break;

		case NAND_CMD_STATUS:	/* 0x70 */
			info->cmd = SPINAND_CMD_GETFEATURES;
			info->buff.idx = 0;

			/*get protection info*/
			if (spi_nand_getfeatures(info, DEVICE_PROTECTION_ADDR) & PROTECT_STATUS) {
				*(unsigned int *)info->buff.virt = 0x0;	/* protected */
			} else {
				/* not protectd. comply w/ raw NAND */
				*(unsigned int *)info->buff.virt = 0x80;
			}

			/*get erase & program status*/
			if (spi_nand_getfeatures(info, DEVICE_STATUS_ADDR) & (PROGRAM_STATUS|ERASE_STATUS)) {
				*(unsigned int *)info->buff.virt |= 0x01;	/*return status is fail */
			}
			break;

		case NAND_CMD_READID:	/* 0x90 */
		case NAND_CMD_PARAM:	/* 0xEC */
		case NAND_CMD_GET_FEATURES:	/* 0xEE */
		case NAND_CMD_SET_FEATURES:	/* 0xEF */
		case NAND_CMD_RESET:	/* 0xFF */
			break;
		default:
			BUG();
			break;
	}

}
EXPORT_SYMBOL(sp_spinand_desc_prep);

static int sp_spinand_desc_send(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	int ret = 0;
	int retry = 0;	
	
	/*clear status.  start dma. check flag */ 
	if ( (info->cmd == NAND_CMD_READ0) || (info->cmd == NAND_CMD_READOOB) || (info->cmd == NAND_CMD_PAGEPROG)) {
		info->busy = 1;
		spi_nanddma_run(info);
		ret = sp_spinand_wait(info);
		if (ret) {
			printk("sp_spinand: descriptor timeout, cmd=0x%x\n", info->cmd);
			sp_spinand_reset(info);
			/* set err flag */
			ret = 0x01;
			return ret;
		}	
	}

	do {
		retry++;
	} while((readl(&regs->spi_ctrl) & SPI_DEVICE_IDLE) && (retry < 100));

	if (retry >= 100) {
		ret = 0x01;
	}

	mutex_unlock(&info->lock);
	if (ret) {
		pr_info("sp_spinand: timeout, cmd=0x%x\n", info->cmd);
		sp_spinand_reset(info);
	}

	return ret;
}

static void sp_spinand_cmdfunc(struct mtd_info *mtd, unsigned cmd, int col, int row)
{
	struct sp_spinand_info *info = container_of(mtd, struct sp_spinand_info, mtd);
	int ret = 0;

	if (cmd == NAND_CMD_STATUS) {
		if (info->cmd == SPINAND_CMD_PROEXECUTE) {
			/*check spi nand control busy/ready  status */
			if (info->err&SPI_NAND_WRITE_FAIL) {
				info->err &= ~SPI_NAND_WRITE_FAIL;
				*(unsigned int *)info->buff.virt = 0x1;
				return;
			}
		}

		if ((info->cmd == SPINAND_CMD_BLKERASE)||(info->cmd == NAND_CMD_ERASE2)) {
			/*check spi nand control busy/ready  status */
			if (info->err&SPI_NAND_ERASE_FAIL) {
				info->err &= ~SPI_NAND_ERASE_FAIL;
				*(unsigned int *)info->buff.virt = 0x1;
				return;
			}
		}
	}

	info->cmd = cmd;

	switch (cmd) {
		case NAND_CMD_READ0:	/* 0x00 */
			sp_spinand_desc_prep(info, cmd, col, row);
			ret=sp_spinand_desc_send(info);
			if (ret&0x1) {
				info->err |= SPI_NAND_READ_FAIL;
				break;
			}

			if (readb(info->buff.virt + mtd->writesize + 1) == 0xFF)
				break;

		case NAND_CMD_READOOB:	/* 0x50 */
			sp_spinand_desc_prep(info, cmd, col, row);
			ret=sp_spinand_desc_send(info);
			if(ret&0x1)
				info->err |= SPI_NAND_READ_FAIL;
			break;

		case NAND_CMD_SEQIN:	/* 0x80 */
			sp_spinand_desc_prep(info, cmd, col, row);
			break;

		case NAND_CMD_PAGEPROG:	/* 0x10 */
			ret=sp_spinand_desc_send(info);
			if(ret&0x1)
				info->err |= SPI_NAND_WRITE_FAIL;
			break;

		case NAND_CMD_ERASE1:	/* 0x60 */
			sp_spinand_desc_prep(info, cmd, col, row);
			break;

		case NAND_CMD_ERASE2:	/* 0xD0 */
			ret=sp_spinand_desc_send(info);
			if(ret&0x1)
				info->err |= SPI_NAND_ERASE_FAIL;
			break;

		case NAND_CMD_STATUS:	/* 0x70 */
			sp_spinand_desc_prep(info, cmd, -1, -1);
			ret=sp_spinand_desc_send(info);
			if(ret&0x1)
				pr_err("get status fail\n");
			break;

		case NAND_CMD_RESET:	/* 0xFF */
			ret=sp_spinand_reset(info);
			if (ret & 0x1)
				pr_err("reset nand fail\n");
			mdelay(5);
			break;

		case NAND_CMD_READID:	/* 0x90 */
			info->buff.idx = 0;
			spi_nand_readid(info, 0, info->buff.virt);
			break;

		case NAND_CMD_PARAM:	/* 0xEC */
		case NAND_CMD_GET_FEATURES:	/* 0xEE */
			sp_spinand_desc_prep(info, cmd, col, -1);
			ret=sp_spinand_desc_send(info);
			if(ret&0x1)
				pr_err("get feature fail!\n");
			/* mdelay(5); */
			break;

		case NAND_CMD_SET_FEATURES:	/* 0xEF */
			sp_spinand_desc_prep(info, cmd, col, -1);
			ret=sp_spinand_desc_send(info);
			if(ret&0x1)
				pr_err("set feature fail!\n");
			/* mdelay(5); */
			break;

		default:
			break;
	}
}

/*
 * nand_select_chip - control CE line
 * @mtd:	MTD device structure
 * @chipnr:	chipnumber to select, -1 for deselect
 */
static void sp_spinand_select_chip(struct mtd_info *mtd, int chipnr)
{
	struct sp_spinand_info *info = container_of(mtd, struct sp_spinand_info, mtd);

	switch (chipnr) {
		case -1:
		case 0:
		case 1:
		case 2:
		case 3:
			info->cs = chipnr;
			break;

		default:
			BUG();
	}
}

#define LP_OPTIONS  (0x00000004|0x00000008|0x00000010)
#define NAND_LP(name, id, ext_id, pgsz, bksz, size) \
		{ name, id, pgsz, size, bksz, LP_OPTIONS, ext_id }

struct nand_flash_dev spinand_flash_ids2[] = {
/* GigaDevice SPI NAND  */
	NAND_LP("GD5F1GQ4UAYIG", 0xC8D1, 0x0, (2048 + 128), SZ_128K, 128),
	NAND_LP("GD5F2GQ4UAYIG", 0xC8D2, 0x0, (2048 + 128), SZ_128K, 256),

/* MXIC  */
	NAND_LP("MX30LF1G18AC", 0xC2F1, 0x80950200, (2048 + 64), SZ_128K, 128),

/* Winbond SPI NAND  */
	NAND_LP("W25N01GVxx1G", 0xEFAA, 0, (2048 + 64), SZ_128K, 128),
	NAND_LP("W25N02GVxx2G", 0xEFAB, 0, (2048 + 64), SZ_128K, 256),

/* MXIC SPI NAND  */
	NAND_LP("MX35LF1GE4AB", 0xC212, 0, (2048 + 64), SZ_128K, 128),
	NAND_LP("MX35LF2GE4AB", 0xC222, 0, (2048 + 64), SZ_128K, 256),

/* ESMT SPI NAND  */
	NAND_LP("ESMTF50L1G41A", 0xC821, 0, (2048 + 64), SZ_128K, 128)
};
static int sp_spinand_loadids(struct sp_spinand_info *info, const char *fn)
{
	int num, num_spinand_flash_ids;
	struct nand_flash_dev *ptr;

	num_spinand_flash_ids = 0;
	for (ptr = nand_flash_ids; ptr->name; ptr++) {
		num_spinand_flash_ids++;
	}

	num = ARRAY_SIZE(spinand_flash_ids2) + num_spinand_flash_ids;

	info->ids = kzalloc(num * sizeof(struct nand_flash_dev), GFP_KERNEL);
	if (!info->ids) {
		printk("sp_spinand: out of memory\n");
		info->ids = nand_flash_ids;
		printk("sp_spinand: %s\n", __func__);
		printk("\nsp_spinand: %s:%d\t%s return -ENOMEM", __FILE__, __LINE__, __FUNCTION__);
		return -ENOMEM;
	}

	memcpy(info->ids, spinand_flash_ids2, sizeof(spinand_flash_ids2));

	memcpy(&(info->ids[ARRAY_SIZE(spinand_flash_ids2)]), nand_flash_ids,
	       num_spinand_flash_ids * sizeof(struct nand_flash_dev));

	return 0;
}

static int sp_spinand_fixup(struct sp_spinand_info *info)
{
	struct mtd_info *mtd = &info->mtd;
	struct nand_chip *chip = mtd->priv;
	uint64_t nrpg;

	info->cac = 2;

	/* row address */
	nrpg = lldiv(chip->chipsize, mtd->writesize);
	if (nrpg & 0xFF000000)
		info->rac = 4;
	else if (nrpg & 0xFFFF0000)
		info->rac = 3;
	else
		info->rac = 2;

	return 0;
}

static int sp_spinand_probe(struct platform_device *pdev)
{
	int i, ret = 0;
	unsigned int id;
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct sp_spinand_info *info;
	struct mtd_part_parser_data data;

	const char *part_types[] = {
#ifdef CONFIG_MTD_CMDLINE_PARTS
		"cmdlinepart",
#endif
		NULL
	};

	info = devm_kzalloc(dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->dev = dev;	
	
	/* init sema */
	sema_init(&info->spnand_sem, 1);
	
	/* init mutex */
	mutex_init(&info->lock);
	
	/* get module reg addr */	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);	
	if (!res) {
		devm_kfree(info);
		printk("sp_spinand: %s:[%d]\n", __FILE__, __LINE__);
		return -ENXIO;
	}	
	
	info->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(info->regs))
	{
		devm_kfree(info);
		return PTR_ERR(info->regs);	
	}
	
	/* get irp */
	info->irq = platform_get_irq(pdev, 0);
	if (info->irq <= 0) {
		devm_kfree(info);
		printk("get spi nand irq resource fail\n");
		return -EINVAL;
	}else {
		ret = request_irq(info->irq, sp_nand_irq, 0, "sp_spinand", info);
		if (ret) {
			printk("sp_spinand: unable to register IRQ(%d) \n", info->irq);
		}
	}
	
	platform_set_drvdata(pdev, info);
	
	info->hdr = dev->platform_data;
	info->mtd.priv = &info->nand;
	info->mtd.name = dev_name(info->dev);
	info->mtd.owner = THIS_MODULE;
	init_waitqueue_head(&info->wq);
	
	if (sp_spinand_reset(info) < 0) {
		ret = -ENXIO;
		printk("sp_spinand: %s:[%d]\n", __FILE__, __LINE__);
		goto err1;
	}	
	
	info->buff.size = CFG_BUFF_MAX;
	info->buff.phys = CONFIG_MTD_SP_NAND_SRAM_BASE;
	info->buff.virt = ioremap(info->buff.phys, info->buff.size);
	if (!info->buff.virt) {
		ret = -ENOMEM;
		printk("sp_spinand: %s:[%d]\n", __FILE__, __LINE__);
		goto err1;
	}	
	
	printk("sp_spinand: buff=0x%p@0x%08x size=%u\n",
	       info->buff.virt, info->buff.phys, info->buff.size);	
	
	info->nand.select_chip = sp_spinand_select_chip;
	//info->nand.cmd_ctrl = sp_spinand_cmd_ctrl;
	info->nand.cmdfunc = sp_spinand_cmdfunc;
	info->nand.dev_ready = sp_spinand_dev_ready;
	info->nand.chip_delay = 0;

	info->nand.read_byte = sp_spinand_read_byte;
	info->nand.read_buf = sp_spinand_read_buf;
	info->nand.write_buf = sp_spinand_write_buf;
	//info->nand.verify_buf = sp_spinand_verify_buf;

	info->nand.ecc.read_page = sp_spinand_read_page;
	info->nand.ecc.write_page = sp_spinand_write_page;
	info->nand.ecc.layout = &info->ecc_layout;
	info->nand.ecc.mode = NAND_ECC_HW;

	/* Read ID */
	spi_nand_readid(info, 0, &id);	
	
	if ((id & 0xFF) == WB_ID) {
		info->id = (id & 0xff);
		/* disable ecc , enable BUF mode */
		spi_nand_setfeatures(info, DEVICE_FEATURE_ADDR, WB_BUF1_DIS_ECC);
	} else if(((id & 0xFF) == GD_ID)||((id & 0xFF) == MXIC_ID)||((id & 0xFFFF) == ESMT1G_ID)) {
		info->id = (id & 0xff);
		/* disable ecc , enable QE mode */
		spi_nand_setfeatures(info, DEVICE_FEATURE_ADDR, QUADIO_DIS_ECC);		
	} else {
		info->id = (id & 0xff);
		/* disable ecc */
		spi_nand_setfeatures(info, DEVICE_FEATURE_ADDR, 0);			
	}
	
	spi_nand_setfeatures(info, DEVICE_PROTECTION_ADDR, 0x0);
	
	ret = sp_spinand_loadids(info, "name");	
	if (ret < 0) {
		goto err1;
	}
	
	if (nand_scan_ident(&info->mtd, CONFIG_MTD_SP_NAND_CSNR, info->ids)) {
		ret = -ENXIO;
		printk("sp_spinand: %s:[%d]\n", __FILE__, __LINE__);
		goto err1;
	}

	if (sp_spinand_fixup(info) < 0) {
		ret = -ENXIO;
		printk("sp_spinand: %s:[%d]\n", __FILE__, __LINE__);
		goto err1;
	}

	if (sp_bch_init(&info->mtd) < 0) {
		ret = -ENXIO;
		printk("sp_spinand: %s:[%d]\n", __FILE__, __LINE__);
		goto err1;
	}

	if (nand_scan_tail(&info->mtd)) {
		ret = -ENXIO;
		printk("sp_spinand: %s:[%d]\n", __FILE__, __LINE__);
		goto err1;
	}	
	
	data.origin = (unsigned long)info->hdr;
	ret = mtd_device_parse_register(&info->mtd, part_types, &data, NULL, 0);	
	
	return ret;
err1:
	if (info->regs)
		iounmap(info->regs);

	if (info->buff.virt)
		iounmap(info->buff.virt);
	
	devm_kfree(info);
	
	return ret;
}


#define sp_spinand_suspend	NULL
#define sp_spinand_resume	NULL

static const struct of_device_id of_fun_match[] = {
	{ .compatible = "sunplus,sp_spinand" },
	{},
};
MODULE_DEVICE_TABLE(of, of_fun_match);

static struct platform_driver sp_spinand_driver = {
	.probe = sp_spinand_probe,
	.remove = sp_spinand_remove,
	.shutdown = NULL,
	.suspend = sp_spinand_suspend,
	.resume = sp_spinand_resume,
	.driver = {
			.name = "sunplus,sp_spinand",
			.owner = THIS_MODULE,
			.of_match_table = of_fun_match,
		   },
};

module_platform_driver(sp_spinand_driver);

MODULE_ALIAS("platform:sp_spinand");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Sunplus SPINAND flash controller");


