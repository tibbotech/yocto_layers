#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/completion.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/spi-nor.h>
#include <linux/mutex.h>
#include <linux/sizes.h>
#include <linux/time.h>

//#define SPHE8388
#ifdef SPHE8388
#define SPI_DATA64_EN (1 << 10)
#define SPI_WRITE_TO (1 << 5)
#define SPI_READ_FROM (~(1 << 5))
#define SPI_CUST_CMD_ENA (1 << 7)
#define SPI_DATA64_MAX_LEN 256
#define SPI_SRAM_FULL (0x4000)
#define SPI_SRAM_EMPTY (0x2000)
#define SPI_TIMEOUT (500*1000)
#define XFER_RW
typedef struct {
	// Group 022 : SPI_FLASH
	unsigned int spi_ctrl;
	unsigned int spi_wait;
	unsigned int spi_cust_cmd;
	unsigned int spi_addr_low;
	unsigned int spi_addr_high;
	unsigned int spi_data_low;
	unsigned int spi_data_high;
	unsigned int spi_status;
	unsigned int spi_cfg0;
	unsigned int spi_cfg1;
	unsigned int spi_cfg2;
	unsigned int spi_cfg3;
	unsigned int spi_cfg4;
	unsigned int spi_cfg5;
	unsigned int spi_cfg6;
	unsigned int spi_cfg7;
	unsigned int spi_cfg8;
	unsigned int spi_cust_cmd_2;
	unsigned int spi_data_64;
	unsigned int spi_buf_addr;
	unsigned int spi_status_2;
	unsigned int spi_status_3;
	unsigned int spi_mode_status;
	unsigned int spi_err_status;
	unsigned int grp22_reserved[8];
}SPI_NOR_REG;
#else
#define PIO_TRIGGER (1<<21)
#define SPI_CTRL_BUSY (1<<31)
#define SPI_DUMMY_CYC(x) (x<<24)
#define CUST_CMD(x) (x<<8)
#define CLEAR_CUST_CMD (~0xffff)
#define AXI_DEFINED_READ(x) (x<<24)
#define CLEAR_AXI_DEFINED_READ (0xffffff)
#define ENABLE_AXI_DEFINED_READ (1<<20)
#define CLEAR_DATA_LEN (~0xff)
#define DMA_TRIGGER (1<<17)
#define ENHANCE_DATA(x) (x<<24)
#define CLEAR_ENHANCE_DATA (~(0xff<<24))
#define DATA64_EN (1<<20)
#define DATA64_DIS ~(1<<20)
#define DATA64_READ_ADDR(x) (x<<16)
#define DATA64_WRITE_ADDR(x) (x)
#define SPI_SRAM_ST (0x7<<13)
#define DUMMY_CYCLE(x) (x)
#define USER_DEFINED_READ(x) (x<<24)
#define USER_DEFINED_READ_EN (1<<20)
#define PREFETCH_ENABLE (1<<22)
#define PREFETCH_DISABLE (~(1<<22))
#define PAGE_ACCESS_ENABLE (1<<29)
#define PAGE_ACCESS_DISABLE (~(1<<29))
#define AUTO_RDSR_EN (1<<18)
#define CLEAR_DATA64_LEN (~0xffff)

#define SPI_DATA64_MAX_LEN ((1<<16)-1)
#define CMD_FAST_READ (0xb)
#define CMD_READ_STATUS (0x5)
#define SPI_TIMEOUT 450

typedef struct{
	unsigned char enhance_en;
	unsigned char enhance_bit;
	unsigned char enhance_bit_mode;
	unsigned char enhance_data;
}SPI_ENHANCE;
enum ERASE_TYPE
{
	SECTOR = 0,
	BLOCK = 1,
	CHIP = 2
};
enum SPI_CLOCK_SEL
{
	SPI_CLK_D_2 = (1)<<16,
	SPI_CLK_D_4 = (2)<<16,
	SPI_CLK_D_6 = (3)<<16,
	SPI_CLK_D_8 = (4)<<16,
	SPI_CLK_D_16 = (5)<<16,
	SPI_CLK_D_24 = (6)<<16,
	SPI_CLK_D_32 = (7)<<16
};
enum SPI_USEABLE_DQ
{
	DQ0 = 1<<20,
	DQ1 = 1<<21,
	DQ2 = 1<<22,
	DQ3 = 1<<23
};
enum SPI_CHIP_SEL
{
	A_CHIP = 1<<24,
	B_CHIP = 1<<25
};
enum SPI_PIO_ADDRESS_BYTE
{
	ADDR_0B = 0,
	ADDR_1B = 1,
	ADDR_2B = 2,
	ADDR_3B = 3
};
enum SPI_PIO_CMD
{
	READ_CMD = 0<<2,
	WRITE_CMD = 1<<2
};
enum SPI_PIO_DATA_BYTE
{
	BYTE_0 = 0<<4,
	BYTE_1 = 1<<4,
	BYTE_2 = 2<<4,
	BYTE_3 = 3<<4,
	BYTE_4 = 4<<4
};
enum SPI_CMD_BIT
{
	SPI_CMD_NO = (0)<<16,
	SPI_CMD_1b = (1)<<16,
	SPI_CMD_2b = (2)<<16,
	SPI_CMD_4b = (3)<<16
};
enum SPI_ADDR_BIT
{
	SPI_ADDR_NO = (0)<<18,
	SPI_ADDR_1b = (1)<<18,
	SPI_ADDR_2b = (2)<<18,
	SPI_ADDR_4b = (3)<<18
};
enum SPI_DATA_BIT
{
	SPI_DATA_NO = (0)<<20,
	SPI_DATA_1b = (1)<<20,
	SPI_DATA_2b = (2)<<20,
	SPI_DATA_4b = (3)<<20
};
enum SPI_ENHANCE_BIT
{
	SPI_ENHANCE_NO = (0)<<22,
	SPI_ENHANCE_1b = (1)<<22,
	SPI_ENHANCE_2b = (2)<<22,
	SPI_ENHANCE_4b = (3)<<22
};
enum SPI_CMD_OEN_BIT
{
	SPI_CMD_OEN_NO = 0,
	SPI_CMD_OEN_1b = 1,
	SPI_CMD_OEN_2b = 2,
	SPI_CMD_OEN_4b = 3
};
enum SPI_ADDR_OEN_BIT
{
	SPI_ADDR_OEN_NO = (0)<<2,
	SPI_ADDR_OEN_1b = (1)<<2,
	SPI_ADDR_OEN_2b = (2)<<2,
	SPI_ADDR_OEN_4b = (3)<<2
};
enum SPI_DATA_OEN_BIT
{
	SPI_DATA_OEN_NO = (0)<<4,
	SPI_DATA_OEN_1b = (1)<<4,
	SPI_DATA_OEN_2b = (2)<<4,
	SPI_DATA_OEN_4b = (3)<<4
};
enum SPI_DATA_IEN_BIT
{
	SPI_DATA_IEN_NO = (0)<<6,
	SPI_DATA_IEN_DQ0 = (1)<<6,
	SPI_DATA_IEN_DQ1 = (2)<<6
};
enum SPI_CMD_IO_BIT
{
	CMD_0 = 0,
	CMD_1 = 1,
	CMD_2 = 2,
	CMD_4 = 4
};
enum SPI_ADDR_IO_BIT
{
	ADDR_0 = 0,
	ADDR_1 = 1,
	ADDR_2 = 2,
	ADDR_4 = 4
};
enum SPI_DATA_IO_BIT
{
	DATA_0 = 0,
	DATA_1 = 1,
	DATA_2 = 2,
	DATA_4 = 4
};
enum SPI_DMA_MODE
{
	DMA_OFF = 0,
	DMA_ON = 1
};
enum SPI_SRAM_STATUS
{
	SRAM_CONFLICT = 0,
	SRAM_EMPTY = 1,
	SRAM_FULL = 2
};
enum SPI_INTR_STATUS
{
	BUFFER_ENOUGH = 1,
	DMA_DONE = 2,
	PIO_DONE = 4
};
typedef struct{
	// Group 022 : SPI_FLASH
	unsigned int  spi_ctrl							; 
	unsigned int  spi_timing						; 
	unsigned int  spi_page_addr						; 
	unsigned int  spi_data							; 
	unsigned int  spi_status						; 
	unsigned int  spi_auto_cfg						; 
	unsigned int  spi_cfg0							; 
	unsigned int  spi_cfg1							; 
	unsigned int  spi_cfg2							; 
	unsigned int  spi_data64						; 
	unsigned int  spi_buf_addr						; 
	unsigned int  spi_status_2						; 
	unsigned int  spi_err_status					; 
	unsigned int  spi_mem_data_addr					; 
	unsigned int  spi_mem_parity_addr				; 
	unsigned int  spi_col_addr						; 
	unsigned int  spi_bch							; 
	unsigned int  spi_intr_msk						; 
	unsigned int  spi_intr_sts						; 
	unsigned int  spi_page_size						; 
	unsigned int  G22_RESERVED[12]					; 
}SPI_NOR_REG;
#endif

struct pentagram_spi_nor 
{
	struct spi_nor nor;
	struct device *dev;
	void __iomem *io_base;
	struct clk *ctrl_clk;
	struct clk *nor_clk;
	u32 clk_rate;
	u32 clk_src;
	u32 read_mode;
	u32 nor_size;
	struct mutex lock;
};

#ifdef SPHE8388
static int pentagram_spi_nor_init(SPI_NOR_REG* spi_reg)
{
	printk("%s\n",__FUNCTION__);
	return 0;
}
static unsigned char pentagram_spi_nor_rdsr(SPI_NOR_REG* reg_base)
{
	unsigned char data;
	SPI_NOR_REG* spi_reg = (SPI_NOR_REG*)reg_base;

	writel(0,&spi_reg->spi_data_low);
	writel(0,&spi_reg->spi_data_high);
	writel(0x0584,&spi_reg->spi_cust_cmd);

	while (readl(&spi_reg->spi_cust_cmd) & 0x80);

	data = readl(&spi_reg->spi_status) & 0xff;
	return data;
}

static int pentagram_spi_nor_erase(struct spi_nor *nor, loff_t offs)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s \n",__FUNCTION__);
	SPI_NOR_REG* spi_reg = (SPI_NOR_REG *)pspi->io_base;
	unsigned char opcode = nor->erase_opcode;
	unsigned char addr_len = 3;
	unsigned int temp_reg = 0;
	unsigned int offset = (unsigned int)offs;
	unsigned int data_count = 0;

	while (readl(&spi_reg->spi_ctrl) & 0x8000);
	writel(0, &spi_reg->spi_cust_cmd);
	writel(0, &spi_reg->spi_cust_cmd_2);
	writel(0, &spi_reg->spi_buf_addr);
	writel(0, &spi_reg->spi_addr_low);
	writel(0, &spi_reg->spi_addr_high);
	temp_reg = readl(&spi_reg->spi_cust_cmd_2);
	temp_reg |= SPI_DATA64_EN | data_count;
	dev_dbg(pspi->dev,"cust cmd 2 0x%x\n",temp_reg);
	writel(temp_reg, &spi_reg->spi_cust_cmd_2);
	temp_reg = readl(&spi_reg->spi_cust_cmd);
	temp_reg |= addr_len;
	writel(offset & 0xffff, &spi_reg->spi_addr_low);
	writel((offset & 0xff0000)>>16, &spi_reg->spi_addr_high);
	dev_dbg(pspi->dev,"addr 0x%x\n",offset);
	temp_reg |= (opcode << 8);
	temp_reg |= SPI_WRITE_TO;
	temp_reg |= SPI_CUST_CMD_ENA;
	dev_dbg(pspi->dev,"cust cmd 0x%x\n",temp_reg);
	writel(temp_reg, &spi_reg->spi_cust_cmd);
	while (readl(&spi_reg->spi_ctrl) & 0x8000)
	{
		dev_dbg(pspi->dev,"wait spi ctrl busy\n");
	};
	while ((pentagram_spi_nor_rdsr(spi_reg) & 0x01) != 0)
	{
		dev_dbg(pspi->dev,"wait spi device busy\n");
	};
	return 0;
}
static int pentagram_spi_nor_xfer_write(struct spi_nor *nor, u8 opcode, u32 addr, u8 addr_len,
				u8 *buf, size_t len)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	SPI_NOR_REG* spi_reg = (SPI_NOR_REG *)pspi->io_base;
	int ret;
	int total_count = len;
	int data_count = 0;
	unsigned int temp_reg = 0;
	unsigned int offset = (unsigned int)addr;
	unsigned int addr_offset = 0;
	unsigned int addr_temp = 0;
	unsigned short data_temp = 0;
	unsigned char * data_in = buf;
	unsigned char cmd = opcode;
	struct timeval time;
	struct timeval time_out;

	dev_dbg(pspi->dev,"to 0x%x, len 0x%x\n",offset,len);
	dev_dbg(pspi->dev,"opcode 0x%x\n",opcode);
	mutex_lock(&pspi->lock);
	if(total_count == 0)
	{
		dev_dbg(pspi->dev,"total_count = 0\n");
		while (readl(&spi_reg->spi_ctrl) & 0x8000);
		writel(0, &spi_reg->spi_cust_cmd);
		writel(0, &spi_reg->spi_cust_cmd_2);
		writel(0, &spi_reg->spi_buf_addr);
		writel(0, &spi_reg->spi_addr_low);
		writel(0, &spi_reg->spi_addr_high);
		temp_reg = readl(&spi_reg->spi_cust_cmd_2);
		temp_reg |= SPI_DATA64_EN | data_count;
		dev_dbg(pspi->dev,"cust cmd 2 0x%x\n",temp_reg);
		writel(temp_reg, &spi_reg->spi_cust_cmd_2);
		temp_reg = readl(&spi_reg->spi_cust_cmd);
		temp_reg |= addr_len;
		writel(offset & 0xffff, &spi_reg->spi_addr_low);
		writel((offset & 0xff0000)>>16, &spi_reg->spi_addr_high);
		dev_dbg(pspi->dev,"addr 0x%x\n",offset);
		temp_reg |= (cmd << 8);
		temp_reg |= SPI_WRITE_TO;
		temp_reg |= SPI_CUST_CMD_ENA;
		dev_dbg(pspi->dev,"cust cmd 0x%x\n",temp_reg);
		writel(temp_reg, &spi_reg->spi_cust_cmd);
		while (readl(&spi_reg->spi_ctrl) & 0x8000)
		{
			dev_dbg(pspi->dev,"wait spi ctrl busy\n");
		};
		while ((pentagram_spi_nor_rdsr(spi_reg) & 0x01) != 0)
		{
			dev_dbg(pspi->dev,"wait spi device busy\n");
		};
	}
	while (total_count > 0)
	{
		dev_dbg(pspi->dev,"total_count > 0\n");
		if (total_count > SPI_DATA64_MAX_LEN)
		{
			total_count = total_count - SPI_DATA64_MAX_LEN;
			data_count = SPI_DATA64_MAX_LEN;
		}else
		{
			data_count = total_count;
			total_count = 0;
		}
		while (readl(&spi_reg->spi_ctrl) & 0x8000);
		writel(0, &spi_reg->spi_cust_cmd);
		writel(0, &spi_reg->spi_cust_cmd_2);
		writel(0, &spi_reg->spi_buf_addr);
		if(addr_len > 0)
		{
			addr_temp = offset +  addr_offset* SPI_DATA64_MAX_LEN;
			dev_dbg(pspi->dev,"addr 0x%x\n",addr_temp);
			writel(addr_temp&0xffff, &spi_reg->spi_addr_low);
			writel((addr_temp&0xff0000)>>16, &spi_reg->spi_addr_high);
		}
		temp_reg = readl(&spi_reg->spi_cust_cmd_2);
		temp_reg |= SPI_DATA64_EN | data_count;
		dev_dbg(pspi->dev,"cust cmd 2 0x%x\n",temp_reg);
		writel(temp_reg, &spi_reg->spi_cust_cmd_2);
		temp_reg = readl(&spi_reg->spi_cust_cmd);
		temp_reg |= (cmd << 8) | 0x3;
		temp_reg |= SPI_WRITE_TO;
		temp_reg |= SPI_CUST_CMD_ENA;
		dev_dbg(pspi->dev,"cust cmd 0x%x\n",temp_reg);
		writel(temp_reg, &spi_reg->spi_cust_cmd);

		while (data_count > 0)
		{
			if ((data_count / 2) > 0)
			{
				if ((readl(&spi_reg->spi_status_2) & SPI_SRAM_FULL) == SPI_SRAM_FULL)
				{
					do_gettimeofday(&time);
					while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_EMPTY) != SPI_SRAM_EMPTY)
					{
						do_gettimeofday(&time_out);
						if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
						{
							dev_dbg(pspi->dev,"timeout \n");
							break;
						}
					};
				}
				data_temp = (data_in[1] << 8) | data_in[0];
				dev_dbg(pspi->dev,"data 0x%x\n",data_temp);
				writel(data_temp, &spi_reg->spi_data_64);
				data_in = data_in + 2;
				data_count = data_count - 2;
			}else 
			{
				if ((readl(&spi_reg->spi_status_2) & SPI_SRAM_FULL) == SPI_SRAM_FULL)
				{
					do_gettimeofday(&time);
					while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_EMPTY) != SPI_SRAM_EMPTY)
					{
						do_gettimeofday(&time_out);
						if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
						{
							dev_dbg(pspi->dev,"timeout \n");
							break;
						}
					};
				}
				data_temp = data_in[0] & 0xff;
				dev_dbg(pspi->dev,"data 0x%x\n",data_temp);
				writel(data_temp, &spi_reg->spi_data_64);
				data_count = data_count - 1;
			}
		}
		while (readl(&spi_reg->spi_ctrl) & 0x8000)
		{
			dev_dbg(pspi->dev,"wait spi ctrl busy\n");
		};
		while ((pentagram_spi_nor_rdsr(spi_reg) & 0x01) != 0)
		{
			dev_dbg(pspi->dev,"wait spi device busy\n");
		};
		addr_offset = addr_offset + 1;
	}
	mutex_unlock(&pspi->lock);
	return 0;
}
static int pentagram_spi_nor_xfer_read(struct spi_nor *nor, u8 opcode, u32 addr, u8 addr_len,
				u8 *buf, size_t len)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	SPI_NOR_REG* spi_reg = (SPI_NOR_REG *)pspi->io_base;
	int ret;
	int total_count = len;
	int data_count = 0;
	unsigned int temp_reg = 0;
	unsigned int offset = (unsigned int)addr;
	unsigned int addr_offset = 0;
	unsigned int addr_temp = 0;
	unsigned short data_temp = 0;
	unsigned char * data_in = buf;
	unsigned char cmd = opcode;
	struct timeval time;
	struct timeval time_out;

	dev_dbg(pspi->dev,"from 0x%x, len 0x%x\n",offset,len);
	dev_dbg(pspi->dev,"opcode 0x%x, len 0x%x\n",opcode);
	mutex_lock(&pspi->lock);
	while (total_count > 0)
	{
		if (total_count > SPI_DATA64_MAX_LEN)
		{
			total_count = total_count - SPI_DATA64_MAX_LEN;
			data_count = SPI_DATA64_MAX_LEN;
		}else
		{
			data_count = total_count;
			total_count = 0;
		}
		while (readl(&spi_reg->spi_ctrl) & 0x8000);
		writel(0, &spi_reg->spi_cust_cmd);
		writel(0, &spi_reg->spi_cust_cmd_2);
		writel(0, &spi_reg->spi_buf_addr);
		if(addr_len > 0)
		{
			addr_temp = offset +  addr_offset* SPI_DATA64_MAX_LEN;
			dev_dbg(pspi->dev,"addr_offset 0x%x\n",addr_offset);
			dev_dbg(pspi->dev,"offset 0x%x\n",offset);
			dev_dbg(pspi->dev,"addr 0x%x\n",addr_temp);
			writel(addr_temp&0xffff, &spi_reg->spi_addr_low);
			writel((addr_temp&0xff0000)>>16, &spi_reg->spi_addr_high);
		}
		temp_reg = readl(&spi_reg->spi_cust_cmd_2);
		temp_reg |= SPI_DATA64_EN | data_count;
		dev_dbg(pspi->dev,"cust cmd 2 0x%x\n",temp_reg);
		writel(temp_reg, &spi_reg->spi_cust_cmd_2);
		temp_reg = readl(&spi_reg->spi_cust_cmd);
		temp_reg |= (opcode << 8) | 0x3;
		temp_reg &= SPI_READ_FROM;
		temp_reg |= SPI_CUST_CMD_ENA;
		dev_dbg(pspi->dev,"cust cmd 0x%x\n",temp_reg);
		writel(temp_reg, &spi_reg->spi_cust_cmd);
		if (opcode == SPINOR_OP_RDSR)
		{
			data_in[0] = readl(&spi_reg->spi_status)&0xff;
			data_count = 0;
		}
		while (data_count > 0)
		{
			if ((data_count / 2) > 0)
			{
				do_gettimeofday(&time);
				while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_EMPTY) == SPI_SRAM_EMPTY)
				{
					do_gettimeofday(&time_out);
					if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
					{
						dev_dbg(pspi->dev,"timeout \n");
						break;
					}
				};
				data_temp = readl(&spi_reg->spi_data_64);
				dev_dbg(pspi->dev,"data 0x%x\n",data_temp);
				data_in[0] = data_temp & 0xff;
				data_in[1] = ((data_temp & 0xff00) >> 8);
				data_in = data_in + 2;
				data_count = data_count - 2;
			}else 
			{
				do_gettimeofday(&time);
				while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_EMPTY) == SPI_SRAM_EMPTY)
				{
					do_gettimeofday(&time_out);
					if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
					{
						dev_dbg(pspi->dev,"timeout \n");
						break;
					}
				};
				data_temp = readl(&spi_reg->spi_data_64);
				dev_dbg(pspi->dev,"data 0x%x\n",data_temp);
				data_in[0] = data_temp & 0xff;
				data_count = data_count - 1;
			}
		}
		while (readl(&spi_reg->spi_ctrl) & 0x8000)
		{
			dev_dbg(pspi->dev,"wait spi ctrl busy\n");
		};
		while ((pentagram_spi_nor_rdsr(spi_reg) & 0x01) != 0)
		{
			dev_dbg(pspi->dev,"wait spi device busy\n");
		};
		addr_offset = addr_offset + 1;
	}
	mutex_unlock(&pspi->lock);
	return 0;
}
#else
static int pentagram_spi_nor_init(SPI_NOR_REG* spi_reg)
{
	unsigned int reg_temp;
	writel(A_CHIP | SPI_CLK_D_32,&spi_reg->spi_ctrl);
	reg_temp = readl(&spi_reg->spi_ctrl);
	writel(SPI_CMD_OEN_1b | SPI_ADDR_OEN_1b | SPI_DATA_OEN_1b | SPI_CMD_1b | SPI_ADDR_1b
		| SPI_DATA_1b | SPI_ENHANCE_NO | SPI_DUMMY_CYC(0) | SPI_DATA_IEN_DQ1,&spi_reg->spi_cfg1);
	reg_temp = readl(&spi_reg->spi_auto_cfg);
	reg_temp &= ~(AUTO_RDSR_EN);
	writel(reg_temp,&spi_reg->spi_auto_cfg);
	return 0;
}
static unsigned char pentagram_spi_nor_rdsr(SPI_NOR_REG* reg_base)
{
	unsigned char data;
	SPI_NOR_REG* spi_reg = (SPI_NOR_REG*)reg_base;
	unsigned int reg_temp;
	reg_temp = readl(&spi_reg->spi_ctrl) & CLEAR_CUST_CMD;
	reg_temp = reg_temp | READ_CMD | BYTE_0 | ADDR_0B | CUST_CMD(0x05);
	while((readl(&spi_reg->spi_ctrl) & SPI_CTRL_BUSY)!=0);

	writel(0,&spi_reg->spi_data);
	writel(reg_temp,&spi_reg->spi_ctrl);
	reg_temp = readl(&spi_reg->spi_auto_cfg);
	reg_temp |= PIO_TRIGGER;
	writel(reg_temp,&spi_reg->spi_auto_cfg);
	while((readl(&spi_reg->spi_auto_cfg) & PIO_TRIGGER)!=0);
	data = readl(&spi_reg->spi_status) & 0xff;
	return data;
}
static int pentagram_spi_nor_xfer_write(struct spi_nor *nor, u8 opcode, u32 addr, u8 addr_len,
				u8 *buf, size_t len)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	SPI_NOR_REG* spi_reg = (SPI_NOR_REG *)pspi->io_base;
	int ret;
	int total_count = len;
	int data_count = 0;
	unsigned int temp_reg = 0;
	unsigned int offset = (unsigned int)addr;
	unsigned int addr_offset = 0;
	unsigned int addr_temp = 0;
	unsigned int reg_temp = 0;
	unsigned int cfg0 = 0;
	unsigned int data_temp = 0;
	unsigned char * data_in = buf;
	unsigned char cmd = opcode;
	struct timeval time;
	struct timeval time_out;

	dev_dbg(pspi->dev,"to 0x%x, len 0x%x\n",offset,len);
	dev_dbg(pspi->dev,"opcode 0x%x\n",opcode);
	mutex_lock(&pspi->lock);
	if (total_count == 0) 
	{
		while((readl(&spi_reg->spi_ctrl) & SPI_CTRL_BUSY)!=0)
		{
			dev_dbg(pspi->dev,"wait ctrl busy\n");
		};
		reg_temp = readl(&spi_reg->spi_ctrl) & CLEAR_CUST_CMD;
		reg_temp = reg_temp | WRITE_CMD | BYTE_0 | ADDR_0B | CUST_CMD(cmd);
		cfg0 = readl(&spi_reg->spi_cfg0);
		cfg0 = (cfg0 & CLEAR_DATA64_LEN) | data_count | DATA64_EN;
		writel(cfg0,&spi_reg->spi_cfg0);
		writel(DATA64_READ_ADDR(0) | DATA64_WRITE_ADDR(0),&spi_reg->spi_buf_addr);
		if (addr_len > 0)
		{
			writel(addr,&spi_reg->spi_page_addr);
			reg_temp = reg_temp | ADDR_3B ;
			dev_dbg(pspi->dev,"addr 0x%x\n", spi_reg->spi_page_addr);
		}
		writel(0,&spi_reg->spi_data);
		writel(reg_temp,&spi_reg->spi_ctrl);
		reg_temp = readl(&spi_reg->spi_auto_cfg);
		reg_temp |= PIO_TRIGGER;
		writel(reg_temp,&spi_reg->spi_auto_cfg);
	}
	while (total_count > 0) 
	{
		if (total_count > SPI_DATA64_MAX_LEN) {
			total_count = total_count - SPI_DATA64_MAX_LEN;
			data_count = SPI_DATA64_MAX_LEN;
		} else {
			data_count = total_count;
			total_count = 0;
		}
		while((readl(&spi_reg->spi_ctrl) & SPI_CTRL_BUSY)!=0)
		{
			dev_dbg(pspi->dev,"wait ctrl busy\n");
		};
		reg_temp = readl(&spi_reg->spi_ctrl) & CLEAR_CUST_CMD;
		reg_temp = reg_temp | WRITE_CMD | BYTE_0 | ADDR_0B | CUST_CMD(cmd);
		cfg0 = readl(&spi_reg->spi_cfg0);
		cfg0 = (cfg0 & CLEAR_DATA64_LEN) | data_count | DATA64_EN;
		writel(cfg0,&spi_reg->spi_cfg0);
		writel(DATA64_READ_ADDR(0) | DATA64_WRITE_ADDR(0),&spi_reg->spi_buf_addr);
		if (addr_len > 0) 
		{
			addr_temp = offset +  addr_offset* SPI_DATA64_MAX_LEN;
			dev_dbg(pspi->dev,"addr_offset 0x%x\n",addr_offset);
			dev_dbg(pspi->dev,"offset 0x%x\n",offset);
			dev_dbg(pspi->dev,"addr 0x%x\n",addr_temp);
			writel(addr_temp,&spi_reg->spi_page_addr);
			reg_temp = reg_temp | ADDR_3B ;
		}
		writel(0,&spi_reg->spi_data);
		writel(reg_temp,&spi_reg->spi_ctrl);
		reg_temp = readl(&spi_reg->spi_auto_cfg);
		reg_temp |= PIO_TRIGGER;
		writel(reg_temp,&spi_reg->spi_auto_cfg);

		while (data_count > 0) 
		{
			if ((data_count / 2) > 0) {
				if ((readl(&spi_reg->spi_status_2) & SPI_SRAM_ST) == SRAM_FULL) {
					do_gettimeofday(&time);
					while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_ST) != SRAM_EMPTY)
					{
						do_gettimeofday(&time_out);
						if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
						{
							dev_dbg(pspi->dev,"timeout \n");
							break;
						}
					};
				}
				data_temp = (data_in[3] << 24) | (data_in[2] << 16) | (data_in[1] << 8) | data_in[0];
				writel(data_temp,&spi_reg->spi_data64);
				data_in = data_in + 4;
				data_count = data_count - 4;
			} else {
				if ((readl(&spi_reg->spi_status_2) & SPI_SRAM_ST) == SRAM_FULL) {
					do_gettimeofday(&time);
					while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_ST) != SRAM_EMPTY)
					{
						do_gettimeofday(&time_out);
						if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
						{
							dev_dbg(pspi->dev,"timeout \n");
							break;
						}
					};
				}
				if(data_count%4 == 3)
				{
					data_temp = (data_in[2] << 16) | (data_in[1] << 8) | data_in[0];
					data_count = data_count-3; 
				}else if(data_count%4 == 2)
				{
					data_temp =  (data_in[1] << 8) | data_in[0];
					data_count = data_count-2; 
				}else if (data_count%4 == 1)
				{
					data_temp = data_in[0];
					data_count = data_count-1; 
				}
				writel(data_temp,&spi_reg->spi_data64);
			}
		}
		addr_offset = addr_offset + 1;
		while ((pentagram_spi_nor_rdsr(spi_reg) & 0x01) != 0)
		{
			dev_dbg(pspi->dev,"wait DEVICE busy \n");
		};
	}
	while((readl(&spi_reg->spi_auto_cfg) & PIO_TRIGGER)!=0)
	{
		dev_dbg(pspi->dev,"wait PIO_TRIGGER\n");
	};
	while((readl(&spi_reg->spi_ctrl) & SPI_CTRL_BUSY)!=0)
	{
		dev_dbg(pspi->dev,"wait ctrl busy\n");
	};
	cfg0 = readl(&spi_reg->spi_cfg0);
	cfg0 &= DATA64_DIS;
	writel(cfg0,&spi_reg->spi_cfg0);
	mutex_unlock(&pspi->lock);
	return 0;
}
static int pentagram_spi_nor_xfer_read(struct spi_nor *nor, u8 opcode, u32 addr, u8 addr_len,
				u8 *buf, size_t len)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	SPI_NOR_REG* spi_reg = (SPI_NOR_REG *)pspi->io_base;
	int ret;
	int total_count = len;
	int data_count = 0;
	unsigned int temp_reg = 0;
	unsigned int offset = (unsigned int)addr;
	unsigned int addr_offset = 0;
	unsigned int addr_temp = 0;
	unsigned int reg_temp = 0;
	unsigned int cfg0 = 0; 
	unsigned int data_temp = 0;
	unsigned char * data_in = buf;
	unsigned char cmd = opcode;
	struct timeval time;
	struct timeval time_out;

	dev_dbg(pspi->dev,"from 0x%x, len 0x%x\n",offset,len);
	dev_dbg(pspi->dev,"opcode 0x%x, len 0x%x\n",opcode);
	mutex_lock(&pspi->lock);

	while (total_count > 0)
	{
		if (total_count > SPI_DATA64_MAX_LEN)
		{
			total_count = total_count - SPI_DATA64_MAX_LEN;
			data_count = SPI_DATA64_MAX_LEN;
		} else
		{
			data_count = total_count;
			total_count = 0;
		}
		while((readl(&spi_reg->spi_ctrl) & SPI_CTRL_BUSY)!=0)
		{
			dev_dbg(pspi->dev,"wait ctrl busy\n");
		};
		reg_temp = readl(&spi_reg->spi_ctrl) & CLEAR_CUST_CMD;
		reg_temp = reg_temp | READ_CMD | BYTE_0 | ADDR_0B | CUST_CMD(cmd);
		cfg0 = readl(&spi_reg->spi_cfg0);
		cfg0 = (cfg0 & CLEAR_DATA64_LEN) | data_count | DATA64_EN;
		dev_dbg(pspi->dev,"spi_cfg0 0x%x\n",cfg0);
		writel(cfg0,&spi_reg->spi_cfg0);
		writel(0,&spi_reg->spi_page_addr);
		writel(DATA64_READ_ADDR(0) | DATA64_WRITE_ADDR(0),&spi_reg->spi_buf_addr);
		if(addr_len > 0)
		{
			addr_temp = offset +  addr_offset* SPI_DATA64_MAX_LEN;
			dev_dbg(pspi->dev,"addr_offset 0x%x\n",addr_offset);
			dev_dbg(pspi->dev,"offset 0x%x\n",offset);
			dev_dbg(pspi->dev,"addr 0x%x\n",addr_temp);
			writel(addr_temp,&spi_reg->spi_page_addr);
			reg_temp = reg_temp | ADDR_3B ;
		}
		writel(0,&spi_reg->spi_data);
		dev_dbg(pspi->dev,"spi_ctrl 0x%x\n",reg_temp);
		writel(reg_temp,&spi_reg->spi_ctrl);
		reg_temp = readl(&spi_reg->spi_auto_cfg);
		reg_temp |= PIO_TRIGGER;
		writel(reg_temp,&spi_reg->spi_auto_cfg);
		if (opcode == SPINOR_OP_RDSR)
		{
			data_in[0] = readl(&spi_reg->spi_status)&0xff;
			data_count = 0;
		}
		dev_dbg(pspi->dev,"cfg1 0x%x, len 0x%x\n",readl(&spi_reg->spi_cfg1));
		while (data_count > 0)
		{
			if ((data_count / 4) > 0)
			{
				do_gettimeofday(&time);
				while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_ST) == SRAM_EMPTY)
				{
					do_gettimeofday(&time_out);
					if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
					{
						dev_dbg(pspi->dev,"timeout \n");
						break;
					}
				};
				data_temp = readl(&spi_reg->spi_data64);
				dev_dbg(pspi->dev,"data_temp 0x%x\n",data_temp);
				data_in[0] = data_temp & 0xff;
				data_in[1] = ((data_temp & 0xff00) >> 8);
				data_in[2] = ((data_temp & 0xff0000) >> 16);
				data_in[3] = ((data_temp & 0xff000000) >> 24);
				data_in = data_in + 4;
				data_count = data_count - 4;
			} else {
				do_gettimeofday(&time);
				while ((readl(&spi_reg->spi_status_2) & SPI_SRAM_ST) == SRAM_EMPTY)
				{
					do_gettimeofday(&time_out);
					if ((time_out.tv_usec - time.tv_usec) > SPI_TIMEOUT)
					{
						dev_dbg(pspi->dev,"timeout \n");
						break;
					}
				};
				data_temp = readl(&spi_reg->spi_data64);
				dev_dbg(pspi->dev,"data_temp 0x%x\n",data_temp);
				if(data_count%4 == 3)
				{
					data_in[0] = data_temp & 0xff;
					data_in[1] = ((data_temp & 0xff00) >> 8);
					data_in[2] = ((data_temp & 0xff0000) >> 16);
					data_count = data_count-3; 
				}else if(data_count%4 == 2)
				{
					data_in[0] = data_temp & 0xff;
					data_in[1] = ((data_temp & 0xff00) >> 8);
					data_count = data_count-2; 
				}else if (data_count%4 == 1)
				{
					data_in[0] = data_temp & 0xff;
					data_count = data_count-1; 
				}
			}
		}
		addr_offset = addr_offset + 1;
		while ((pentagram_spi_nor_rdsr(spi_reg) & 0x01) != 0)
		{
			dev_dbg(pspi->dev,"wait DEVICE busy \n");
		};
	}
	while((readl(&spi_reg->spi_auto_cfg) & PIO_TRIGGER)!=0)
	{
		dev_dbg(pspi->dev,"wait PIO_TRIGGER\n");
	};
	while((readl(&spi_reg->spi_ctrl) & SPI_CTRL_BUSY)!=0)
	{
		dev_dbg(pspi->dev,"wait ctrl busy\n");
	};
	cfg0 = readl(&spi_reg->spi_cfg0);
	cfg0 &= DATA64_DIS;
	writel(cfg0,&spi_reg->spi_cfg0);
	mutex_unlock(&pspi->lock);
	return 0;
}
#endif
static ssize_t pentagram_spi_nor_read(struct spi_nor *nor, loff_t from, size_t len, u_char *buf)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	unsigned char opcode = nor->read_opcode;
	unsigned int offset = (unsigned int)from;
	pentagram_spi_nor_xfer_read(nor, opcode, offset, 0x3, buf, len);
	return len;
}
static ssize_t pentagram_spi_nor_write(struct spi_nor *nor, loff_t to,
				size_t len, const u_char *buf)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	unsigned char opcode = nor->program_opcode;
	unsigned int offset = (unsigned int)to;
	pentagram_spi_nor_xfer_write(nor, opcode, offset, 0x3, buf, len);
	return len;
}
static int pentagram_spi_nor_read_reg(struct spi_nor *nor, u8 opcode, u8 *buf, int len)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	pentagram_spi_nor_xfer_read(nor, opcode, 0, 0, buf, len);
	return 0;
}

static int pentagram_spi_nor_write_reg(struct spi_nor *nor, u8 opcode, u8 *buf, int len)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	unsigned char * data_in = buf;
	unsigned int addr = 0;
	int addr_len = 0;
	int data_len = 0;
	switch(opcode)
	{
		case SPINOR_OP_BE_4K:
			addr_len = len;
			data_len = 0;
			addr= data_in[2]<<16 | data_in[1]<<8 | data_in[0];
			break;
		default:
			addr_len = 0;
			data_len = len;
			break;
	};
	pentagram_spi_nor_xfer_write(nor, opcode, addr, addr_len, buf, data_len);
	return 0;
}

static int pentagram_spi_nor_prep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	switch(ops)
	{
		case 0:
			dev_dbg(pspi->dev,"SPI_NOR_OPS_READ \n");
			break;
		case 1:
			dev_dbg(pspi->dev,"SPI_NOR_OPS_WRITE \n");
			break;
		case 2:
			dev_dbg(pspi->dev,"SPI_NOR_OPS_ERASE \n");
			break;
		case 3:
			dev_dbg(pspi->dev,"SPI_NOR_OPS_LOCK \n");
			break;
		case 4:
			dev_dbg(pspi->dev,"SPI_NOR_OPS_UNLOCK \n");
			break;
		default:
			dev_dbg(pspi->dev,"Unknow ops \n");
			break;
	};
	return 0;
}
static void pentagram_spi_nor_unprep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	struct pentagram_spi_nor *pspi = nor->priv;
	dev_dbg(pspi->dev,"%s\n",__FUNCTION__);
	return ;
}
static int pentagram_spi_nor_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct pentagram_spi_nor *pspi ;
	struct resource *res;
	struct spi_nor *nor;
	struct mtd_info *mtd;
	SPI_NOR_REG* spi_reg;
	int ret;

	pspi = devm_kzalloc(dev, sizeof(*pspi), GFP_KERNEL);
	if (!pspi)
		return -ENOMEM;

	pspi->dev = dev;

	platform_set_drvdata(pdev, pspi);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pspi->io_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(pspi->io_base))
		return PTR_ERR(pspi->io_base);

	#if 0
	/* find the clocks */
	pspi->ctrl_clk = devm_clk_get(dev, "sysslow");
	if (IS_ERR(pspi->ctrl_clk))
		return PTR_ERR(pspi->ctrl_clk);

	pspi->nor_clk = devm_clk_get(dev, "sysslow");
	if (IS_ERR(pspi->nor_clk))
		return PTR_ERR(pspi->nor_clk);

	ret = clk_prepare_enable(pspi->ctrl_clk);
	if (ret)
	{
		dev_err(dev, "can not enable the clock\n");
		goto clk_failed;
	}

	ret = clk_prepare_enable(pspi->nor_clk);
	if (ret)
	{
		goto clk_failed_nor;
	}
	#endif
	
	/* find the irq */
	#if 0
	ret = platform_get_irq(pdev, 0);
	if (ret < 0) 
	{
		dev_err(dev, "failed to get the irq: %d\n", ret);
		goto irq_failed;
	}

	ret = devm_request_irq(dev, ret,fsl_qspi_irq_handler, 0, pdev->name, pspi);
	if (ret) 
	{
		dev_err(dev, "failed to request irq: %d\n", ret);
		goto irq_failed;
	}
	#endif

	nor = &pspi->nor;
	mtd = &nor->mtd;

	nor->dev = dev;
	spi_nor_set_flash_node(nor, np);
	nor->priv = pspi;

	mutex_init(&pspi->lock);
	/* fill the hooks */
	nor->read_reg = pentagram_spi_nor_read_reg;
	nor->write_reg = pentagram_spi_nor_write_reg;
	nor->read = pentagram_spi_nor_read;
	nor->write = pentagram_spi_nor_write;
	//nor->erase = pentagram_spi_nor_erase;

	nor->prepare = pentagram_spi_nor_prep;
	nor->unprepare = pentagram_spi_nor_unprep;

	ret = of_property_read_u32(np, "spi-max-frequency",&pspi->clk_rate);
	if (ret < 0)
		goto mutex_failed;

	spi_reg = (SPI_NOR_REG *)pspi->io_base;
	pentagram_spi_nor_init(spi_reg);
	
	ret = spi_nor_scan(nor, NULL, SPI_NOR_NORMAL);
	if (ret)
		goto mutex_failed;

	ret = mtd_device_register(mtd, NULL, 0);
	if (ret)
		goto mutex_failed;

	if (pspi->nor_size == 0)
	{
		pspi->nor_size = mtd->size;
	}

mutex_failed:
	mutex_destroy(&pspi->lock);
clk_failed:

clk_failed_nor:
	clk_disable_unprepare(pspi->nor_clk);
	clk_disable_unprepare(pspi->ctrl_clk);

irq_failed:

	return 0;
}
static int pentagram_spi_nor_remove(struct platform_device *pdev)
{
	struct pentagram_spi_nor *pspi = platform_get_drvdata(pdev);

	mtd_device_unregister(&pspi->nor.mtd);

	mutex_destroy(&pspi->lock);

	clk_disable_unprepare(pspi->ctrl_clk);
	clk_disable_unprepare(pspi->nor_clk);
	
	return 0;
}

static const struct of_device_id pentagram_spi_nor_ids[] = {
	{.compatible = "sunplus,pentagram-spi-nor"},
	{}
};
MODULE_DEVICE_TABLE(of, pentagram_spi_nor_ids);

static struct platform_driver pentagram_spi_nor_driver = {
	.probe	= pentagram_spi_nor_probe,
	.remove	= pentagram_spi_nor_remove,
	.driver	= {
		.name = "pentagram-spi-nor",
		.of_match_table = pentagram_spi_nor_ids,
	},
};
module_platform_driver(pentagram_spi_nor_driver);

MODULE_AUTHOR("Sunplus");
MODULE_DESCRIPTION("Sunplus spi nor driver");
MODULE_LICENSE("GPL");
