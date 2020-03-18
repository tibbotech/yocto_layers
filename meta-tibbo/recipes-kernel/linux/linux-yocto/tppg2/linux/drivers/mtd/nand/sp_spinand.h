/*
 * Sunplus Technology
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __SP_SPINAND_H
#define __SP_SPINAND_H

/* Q628 spi nand driver */
struct sp_spinand_regs {
unsigned int spi_ctrl;       // 87.0	
unsigned int spi_timing;     // 87.1	
unsigned int spi_page_addr;  // 87.2	
unsigned int spi_data;       // 87.3	
unsigned int spi_status;     // 87.4	
unsigned int spi_auto_cfg;   // 87.5	
unsigned int spi_cfg[3];     // 87.6	
unsigned int spi_data_64;    // 87.9	
unsigned int spi_buf_addr;   // 87.10	
unsigned int spi_statu_2;    // 87.11	
unsigned int spi_err_status; // 87.12
unsigned int mem_data_addr;  // 87.13	
unsigned int mem_parity_addr;// 87.14	
unsigned int spi_col_addr;   // 87.15	
unsigned int spi_bch;        // 87.16	
unsigned int spi_intr_msk;   // 87.17	
unsigned int spi_intr_sts;   // 87.18	
unsigned int spi_page_size;  // 87.19
};

#define SPI_DEVICE_IDLE  	(1<<31)
#define SPI_NAND_ENABLE  	(1<<11)
#define SPI_NAND_CHIP_A	 	(1<<24)	
#define SPI_NAND_AUTO_WEL	(1<<19)
#define SPI_NAND_CLK_32DIV	(0x7<<16)
#define SPI_NAND_DMA_OWNER	(0x1<<17)

#define SPI_CUSTCMD_SHIFT          8
#define SPI_CUSTCMD_FN_SHIFT       7 
#define SPI_CUSTCMD_RW_SHIFT       2 
#define CUSTCMD_BYTECNT_DATA_SHIFT 4
#define CUSTCMD_BYTECNT_ADDR_SHIFT 0
#define SPI_NAND_CTRL_EN       	(1<<SPI_CUSTCMD_FN_SHIFT)
#define SPI_NAND_READ_MDOE      (0<<SPI_CUSTCMD_RW_SHIFT)
#define SPI_NAND_WRITE_MDOE     (1<<SPI_CUSTCMD_RW_SHIFT)
#define SPINAND_CUSTCMD_NO_DATA	(0<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_1_DATA	(1<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_2_DATA	(2<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_3_DATA	(3<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_4_DATA	(4<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_NO_ADDR	(0<<CUSTCMD_BYTECNT_ADDR_SHIFT)
#define SPINAND_CUSTCMD_1_ADDR	(1<<CUSTCMD_BYTECNT_ADDR_SHIFT)
#define SPINAND_CUSTCMD_2_ADDR	(2<<CUSTCMD_BYTECNT_ADDR_SHIFT)
#define SPINAND_CUSTCMD_3_ADDR	(3<<CUSTCMD_BYTECNT_ADDR_SHIFT)

#define SPINAND_CMD_RESET		  0xff	
#define SPINAND_CMD_READID		  0x9f	
#define SPINAND_CMD_GETFEATURES   0x0F
#define SPINAND_CMD_SETFEATURES   0x1F
#define SPINAND_CMD_BLKERASE      0xD8
#define SPINAND_CMD_PAGE2CACHE    0x13
#define SPINAND_CMD_PAGEREAD	  0x3
#define SPINAND_CMD_RDCACHEQUADIO 0xEB
#define SPINAND_CMD_PROLOADx4     0x32
#define SPINAND_CMD_PROEXECUTE    0x10

#define SPINAND_AUTOCFG_CMDEN		(1<<21)
#define SPINAND_AUTOCFG_RDCACHE		(1<<20)
#define SPINAND_AUTOCFG_RDSTATUS	(1<<18)

#define SPINAND_CFG01_DEFAULT	  0x150085	//CMD 1bit DQ0 output, ADDR 1bit DQ0 output, DATA 1bit DQ1 INTPUT
											//cmd 1bit mode, addr 1bit mode, data 1 bit.
#define SPINAND_CFG01_DEFAULT1	  0x150015	//CMD 1bit DQ0 output, ADDR 1bit DQ0 output, DATA 1bit DQ0 OUTPUT
											//cmd 1bit mode, addr 1bit mode, data 1 bit.
#define SPINAND_CFG01_DEFAULT2	  0x50005   //CMD 1bit DQ0 output, ADDR 1bit DQ0 output,
											//cmd 1bit mode, addr 1bit mode, 											
#define SPINAND_CFG01_DEFAULT3	  0x10001   //CMD 1bit DQ0 output,
											//cmd 1bit mode, 
#define SPINAND_CFG02_DEFAULT	  0x8150085 //CMD 1bit DQ0 output, ADDR 1bit DQ0 output, DATA 1bit DQ1 INTPUT,
											//cmd 1bit mode, addr 1bit mode, data 1 bit. 8 dummy cycle

// =============================
/*block erase status */
#define ERASE_STATUS		0x04

/*protect status */
#define PROTECT_STATUS		0x38

/*data mode & ecc mode */
#define QUADIO_EN_ECC 		0x11
#define QUADIO_DIS_ECC 		0x01
#define WB_BUF1_DIS_ECC		0x08
#define WB_BUF1_EN_ECC		0x18
#define WB_BUF0_EN_ECC		0x10

/*data mode & ecc mode */
#define DEVICE_PROTECTION_ADDR	0xA0
#define DEVICE_FEATURE_ADDR	0xB0
#define DEVICE_STATUS_ADDR	0xC0

/* device id */
#define ESMT1G_ID 0x21C8
#define GD_ID 0xC8
#define GD_ID_HIGH 0x2100
#define WB_ID 0xEF
#define MXIC_ID 0xC2

/* read map addr */
#define SPI_NAND_READ_ADDR 0x94000000
#define SPI_NAND_READ_MXIC_ADDR 0x94001000

extern const struct nand_flash_dev sp_nand_ids[];

struct sp_spinand_info {
	struct device *dev;
	struct device *dma;
	struct mtd_info mtd;
	struct nand_chip nand;
	wait_queue_head_t wq;
	void __iomem *regs;
	int irq;
	int cs;
	struct {
		uint32_t idx;
		uint32_t size;
		void *virt;
		dma_addr_t phys;
	} buff;

	struct nand_ecclayout ecc_layout;
	int busy;
	int cmd;		/* current command code */
	int cac;		/* col address cycles */
	int rac;		/* row address cycles */
	int id;
	int row;
	int err;		/* keep err flag */
	struct nand_flash_dev *ids;	/* NAND flash IDs */
	void *hdr;		/* Sunplus Boot Header */

#ifdef CONFIG_MTD_SP_NAND_BKER_COUNTER
	uint16_t *blk_erased;
#endif

	struct mutex lock;
	struct semaphore spnand_sem;
};

#endif /* __SP_SPINAND_H */
