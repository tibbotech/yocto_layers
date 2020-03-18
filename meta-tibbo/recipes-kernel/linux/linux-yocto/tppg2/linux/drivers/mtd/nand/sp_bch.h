#ifndef __SP_BCH_H
#define __SP_BCH_H

struct sp_bch_regs {
	uint32_t cr0;     /* control register */
	uint32_t buf;     /* data base address register */
	uint32_t ecc;     /* parity base address register */
	uint32_t isr;     /* interrupt status register */

	uint32_t srr;     /* software reset register */
	uint32_t ier;     /* interrupt enable register */
	uint32_t sr;      /* status register */
	uint32_t esr;     /* error status register */

	uint32_t fsr;     /* failure status register */
	uint32_t lwa;     /* address of last write operation */
	uint32_t lwd;     /* data of last write operation */
	uint32_t cr1;     /* control register */

	uint32_t revr;    /* revision register */
};

#define CR0_START		BIT(0)
#define CR0_ENCODE		0
#define CR0_DECODE		BIT(4)
#define CR0_CMODE_1024x60	(0 << 8)
#define CR0_CMODE_1024x40	(1 << 8)
#define CR0_CMODE_1024x24	(2 << 8)
#define CR0_CMODE_1024x16	(3 << 8)
#define CR0_CMODE_512x8		(4 << 8)
#define CR0_CMODE_512x4		(5 << 8)
#define CR0_CMODE(n)		(((n) & 7) << 8)
#define CR0_DMODE(n)		((n) ?  BIT(11) : 0)
#define CR0_NBLKS(n)		((((n) - 1) & 0x1f) << 16)
#define CR0_BMODE(n)		(((n) & 7) << 28)

/* 32-bits word aligned */
#define CR1_WORD_ALIGN		BIT(0)
/* Non-buffered write */
#define CR1_NBWR		BIT(1)
/* Non-buffered write for last correction write only */
#define CR1_NBWR_LAST		BIT(2)
/* Threshold for blank check (0x00 or 0xff) */
#define CR1_BLANK_THRES(n)	(((n) & 0x3ff) << 4)

#define ISR_BCH			BIT(0)
#define ISR_BUSY		BIT(4)
#define ISR_CURR_DBLK(x)	(((x) >> 8) & 0x1f)
#define ISR_CURR_CBLK(x)	(((x) >> 16) & 0x1f)

#define IER_DONE		BIT(0)
#define IER_FAIL		BIT(1)

#define SRR_RESET		BIT(0)

#define SR_DONE			BIT(0)
#define SR_FAIL			BIT(4)
#define SR_ERR_BITS(x)		(((x) >> 8) & 0x7ff)
#define SR_ERR_MAX(x)		(((x) >> 20) & 0x7f)
#define SR_BLANK_00		BIT(28)	/* data are all 0x00 */
#define SR_BLANK_FF		BIT(29)	/* data are all 0xff */
#define SP_BCH_IOC1K60ENC	_IOWR('S', 0, struct sp_bch_req)
#define SP_BCH_IOC1K60DEC	_IOWR('S', 1, struct sp_bch_req)

struct sp_bch_chip {
	struct device *dev;
	struct mtd_info *mtd;
	void __iomem *regs;
	int irq;
	int busy;
	uint32_t cr0;
	struct mutex lock;
	wait_queue_head_t wq;
};

struct sp_bch_req {
	uint8_t buf[1024];
	uint8_t ecc[128];
};

int sp_bch_init(struct mtd_info *mtd);
int sp_bch_encode(struct mtd_info *mtd, dma_addr_t buf, dma_addr_t ecc);
int sp_bch_decode(struct mtd_info *mtd, dma_addr_t buf, dma_addr_t ecc);

#endif /* __SP_BCH_H */
