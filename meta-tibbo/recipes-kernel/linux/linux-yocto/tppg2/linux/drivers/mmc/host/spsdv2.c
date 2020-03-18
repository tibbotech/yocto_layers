/******************************************************************************
*                          Include File
*******************************************************************************/
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/pm_runtime.h>
#include <asm/uaccess.h>

#include <linux/mmc/sd.h>
#include <linux/mmc/core.h>
#include <linux/mmc/card.h>

#include <linux/miscdevice.h>
#include <asm/cacheflush.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>

#include <asm-generic/io.h>
#include <asm/bitops.h>
#include "mach/gpio_drv.h" /* for card sense */
#include "spsdv2.h"

/******************************************************************************
*                          MACRO Function Define
*******************************************************************************/
#define SPSD_MAX_CLOCK  CLOCK_52M     /* Max supported SD Card frequency */
#define SPSD_CLK_SOURCE CLOCK_202M    /* Host controller's clk source */
#define SPSDV2_SDC_NAME "sunplus,sp-cardx"
#define MAX_SDDEVICES   2
#define SPSD_DEVICE_MASK 1
#define SPSD_READ_DELAY  0		/* delay for sampling data */
#define SPSD_WRITE_DELAY 0		/* delay for output data   */
#define SPEMMC_READ_DELAY  0		/* delay for sampling data */
#define SPEMMC_WRITE_DELAY 2		/* delay for output data   */
#define DUMMY_COCKS_AFTER_DATA     8

#define MAX_DLY_CLK     7 		/* max  delay clocks */
#ifdef CONFIG_SP_SDEMMCV2
#define SP_EMMCSLOT_ID  0
#else
#define SP_EMMCSLOT_ID  (~0)
#endif
#define SP_SDIO_SLOT_ID  4
#define SP_SDCARD_SENSE_WITH_GPIO

/* Disabled fatal error messages temporarily */
static u32 loglevel = 0x002;
/* static u32 loglevel = 0x001; */
/* static u32 loglevel = 0x033; */
/* static u32 loglevel = 0xefff; */
/* static u32 loglevel = 0xffff; */


#define MMC_LOGLEVEL_FATAL		0x01
#define MMC_LOGLEVEL_ERROR		0x02
#define MMC_LOGLEVEL_DEBUG		0x04


#define MMC_LOGLEVEL_IF 		0x10
#define MMC_LOGLEVEL_PK 		0x20

#define MMC_LOGLEVEL_COUNTER	0x100
#define MMC_LOGLEVEL_WAITTIME	0x200

#define MMC_LOGLEVEL_DUMPREG	0x1000
#define MMC_LOGLEVEL_MINI		0x2000

#if 1

#define FATAL(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_FATAL)) \
		printk(KERN_ERR "[SD FATAL]: %s: " fmt, __func__ , ## args)

#define EPRINTK(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_ERROR)) \
		printk(KERN_ERR "[SD ERROR]: %s: " fmt, __func__ , ## args)

#define DPRINTK(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_DEBUG)) \
		printk(KERN_INFO "[SD DBG]: %s: " fmt, __func__ , ## args)

#define IFPRINTK(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_IF)) \
		printk(KERN_NOTICE "[SD IF]: %s:" fmt, __func__, ## args)

#define pk(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_PK)) \
		printk(KERN_NOTICE "[SD PK]: %s: " fmt, __func__ , ## args)

#define CPRINTK(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_COUNTER)) \
		printk(KERN_INFO "[SD COUNTER]: %s: " fmt, __func__ , ## args)

#define WPRINTK(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_WAITTIME)) \
		printk(KERN_INFO "[SD WAITTIME]: %s: " fmt, __func__ , ## args)

#define REGPRINTK(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_DUMPREG)) \
		printk(KERN_INFO "[SD REG]: %s: " fmt, __func__ , ## args)

#define MPRINTK(fmt, args...) if(unlikely(loglevel & MMC_LOGLEVEL_MINI)) \
		printk(KERN_INFO "[SD]: %s: " fmt, __func__ , ## args)

#else

#define FATAL(fmt, args...)
#define EPRINTK(fmt, args...)
#define DPRINTK(fmt, args...)

#define IFPRINTK(fmt, args...)
#define pk(fmt, args...)

#define CPRINTK(fmt, args...)
#define WPRINTK(fmt, args...)


#define REGPRINTK(fmt, args...)
#define MPRINTK(fmt, args...)

#endif

#define IS_DMA_ADDR_2BYTE_ALIGNED(x)  (!((x) & 0x1))



/******************************************************************************
*                          Global Variables
*******************************************************************************/

const static spsdv2_dridata_t spsdv2_driv_data[] = {
	{.id = 0,},
	{.id = 1,},
	{.id = 4,},
};

static const struct of_device_id spsdv2_of_id[] = {
	{
		.compatible = "sunplus,sp-card0",
	  	.data = &spsdv2_driv_data[0],
	},
	{
		.compatible = "sunplus,sp-card1",
		.data = &spsdv2_driv_data[1],
	},
	{
		.compatible = "sunplus,sp-sdio",
		.data = &spsdv2_driv_data[2],
	},
	{}

};
MODULE_DEVICE_TABLE(of, spsdv2_of_id);



/******************************************************************************
*                         Function Prototype
*******************************************************************************/
#define REG_BASE					0x9c000000
#define RF_GRP(_grp, _reg)			((((_grp) * 32 + (_reg)) * 4) + REG_BASE)
#define RF_MASK_V(_mask, _val)      (((_mask) << 16) | (_val))
#define RF_MASK_V_SET(_mask)        (((_mask) << 16) | (_mask))
#define RF_MASK_V_CLR(_mask)        (((_mask) << 16) | 0)
#define REGn(base, n)	((base) + 4 * (n))

static int spsdv2_get_dma_dir(SPSDHOST *, struct mmc_data *);
static void sphe_mmc_finish_request(SPSDHOST *, struct mmc_request *);
static void spsdv2_set_data_info(SPSDHOST *, struct mmc_data *);
static void spsdv2_prepare_cmd_rsp(SPSDHOST *, struct mmc_command *);

static inline bool is_crc_token_valid(SPSDHOST *host)
{
	return (host->base->sdcrdcrc == 0x2 || host->base->sdcrdcrc == 0x5);
}

static int pinmux_enable(void *host)
{
	volatile void __iomem  *reg  = ioremap_nocache(RF_GRP(1, 1), 4);
	if (reg) {
		writel(RF_MASK_V(1 << 6, 1 << 6), reg);
	}
	else {
		printk("ioremap fail\n");
		return -ENOMEM;
	}
	iounmap(reg);
	/* fully pin-mux configuration */
	if (SP_SDIO_SLOT_ID == ((SPSDHOST *)host)->id) {
		reg = ioremap_nocache(RF_GRP(2, 0), 128);
		if (!reg) {
			printk("trying to set sdio pinmux failed at ioremap!\n");
			return -1;
		}
		writel(0x7f << 16 | 14, REGn(reg, 11)); /* CLK */
		writel(0x7f << 24 | 16 << 8, REGn(reg, 11)); /* CMD */
		writel(0x7f << 16 | 12, REGn(reg, 12)); /* DAT0 */
		writel(0x7f << 24 | 10 << 8, REGn(reg, 12)); /* DAT1 */
		writel(0x7f << 16 | 20, REGn(reg, 13)); /* DAT2 */
		writel(0x7f << 24 | 18 << 8, REGn(reg, 13)); /* DAT3 */
		iounmap(reg);
	}
	return 0;
}

static int ctlr_clk_enable(void)
{
	volatile void __iomem  *reg  = ioremap_nocache(RF_GRP(0, 4), 4);
	if (reg) {
		writel(RF_MASK_V_SET(1 << 15), reg);
	}	else {
		printk("ioremap fail\n");
		return -ENOMEM;
	}
	iounmap(reg);
	return 0;
}

static int ctlr_clk_disable(void)
{
	volatile void __iomem  *reg  = ioremap_nocache(RF_GRP(0, 4), 4);
	if (reg) {
		writel(RF_MASK_V_CLR(1 << 15), reg);
	}	else {
		printk("ioremap fail\n");
		return -ENOMEM;
	}
	iounmap(reg);
	return 0;
}

static int sd_set_in_clock(void *host)
{
	return 0;
}

static int sd_get_in_clock(void *host)
{
	return SPSD_CLK_SOURCE;
}

void tx_dummy(SPSDHOST *host, u32 rounds)
{
	host->base->tx_dummy_num = rounds;
	host->base->sdctrl_trigger_dummy = 1;
	/* wait  dummy done  */
	while (host->base->sdstate);
}


static int Sd_Bus_Reset_Channel(SPSDHOST *host)
{
	int counter = 0;

	host->base->rst_cnad = 1;	/*reset Central FIFO*/
	/* Wait for channel reset to complete */
	while (host->base->rst_cnad == 1) {
	}

	CPRINTK("%s, counter:%d\n", __func__, counter);
	return SPSD_SUCCESS;
}

static int Reset_Controller(SPSDHOST *host)
{
	DPRINTK("controller reset\n");
	SD_RST_seq(host->base);
	return Sd_Bus_Reset_Channel(host);
}

static void spsdv2_controller_init(SPSDHOST *host)
{
	host->base->sdddrmode = 0;
	host->base->sdpiomode = 1;
	host->base->rx4b_en = 1;
	if (SP_SDIO_SLOT_ID == host->id)
		host->base->sdiomode = SP_SDIO_CARD;
	else
		host->base->sdiomode = SP_SD_CARD;

	host->base->sdrsptmr = 0x7ff;
	host->base->sdrsptmren = 1;
	host->base->sdcrctmr = 0x7ff;
	host->base->sdcrctmren = 1;
	if (SP_EMMCSLOT_ID == host->id)
		host->base->sdmmcmode = SP_MMC_MODE;
	else
		host->base->sdmmcmode = SP_SD_MODE;
	host->base->sdrxdattmr_sel = SP_SD_RXDATTMR_MAX;
	host->base->mediatype = 6;
}

static void spsdv2_set_power_mode(SPSDHOST *host, struct mmc_ios *ios)
{
	if (host->power_state == ios->power_mode)
		return;

	switch (ios->power_mode) {
		/* power off->up->on */
	case MMC_POWER_ON:
		DPRINTK("set MMC_POWER_ON\n");
		spsdv2_controller_init(host);
		//pm_runtime_get_sync(&pdev->dev);
		break;
	case MMC_POWER_UP:
		/* Set default control register */
		DPRINTK("set MMC_POWER_UP\n");
		Reset_Controller(host);
		break;
	case MMC_POWER_OFF:
		Reset_Controller(host);
		DPRINTK("set MMC_POWER_OFF\n");
		//pm_runtime_put(&pdev->dev);
		break;
	}
	host->power_state = ios->power_mode;
}

/*
 * Set SD card clock divider value based on the required clock in HZ
 * TODO: Linux passes clock as 0, look into it
 */
static void spsdv2_set_clock(SPSDHOST *host, struct mmc_ios *ios)
{
	struct mmc_host *mmc = host->mmc;
	uint clkrt, sys_clk, act_clock;
	uint rd_dly = SPSD_READ_DELAY, wr_dly = SPSD_WRITE_DELAY;
	uint clock = ios->clock;
	/* Check if requested frequency is above/below supported range */
	if (clock < mmc->f_min)
		clock = mmc->f_min;
	else if (clock > mmc->f_max)
		clock = mmc->f_max;

	sys_clk = sd_get_in_clock(host);
	clkrt = (sys_clk / clock) - 1;

	/* Calculate the actual clock for the divider used */
	act_clock = sys_clk / (clkrt + 1);
	/*
	if (act_clock > clock)
		clkrt++;
	*/
	/* printk("sys_clk =%u, act_clock=%u, clkrt = %u\n", sys_clk, act_clock, clkrt); */
	/* check clock divider boundary and correct it */
	if (clkrt > 0xFFF)
		clkrt = 0xFFF;

	host->base->sdfqsel = clkrt;
	/* Delay 4 msecs for now (wait till clk stabilizes?) */
	mdelay(4);

	/*
	*Host adjusts the data sampling edge and send edge depending on the speed mode used.
	*sd:
	* read delay:
	* default speed controller sample data at fall edge, card send data at fall edge
	* high speed controller sample data at rise edge, card send data at rise edge
	* tunel (clkrt + 1)/2 clks to ensure controller sample In the middle of the data.
	* write delay:
	*  default speed controller send data at fall edge, card sample data at rise edge
	* high speed controller send data at rise edge, card sample data at rise edge
	* so we need to tunel write delay (clkrt + 1)/2  clks at high speed to ensure card sample right data
	*mmc:
	* mmc card host and device sample data at rising edge in sdr mode,
	*  so we need to  tunel write and read delay (clkrt + 1)/2
	*/
	rd_dly = (clkrt + 1)/2 > MAX_DLY_CLK ? MAX_DLY_CLK:(clkrt + 1)/2;
	if (ios->timing != MMC_TIMING_LEGACY) {
		host->base->sd_high_speed_en = 1;
		wr_dly = rd_dly;
	} else {
		host->base->sd_high_speed_en = 0;
	}

	if (SP_EMMCSLOT_ID == host->id)
	{
		wr_dly = host->wrdly;
		rd_dly = host->rddly;
	}
	/* Write delay: Controls CMD, DATA signals timing to SD Card */
	host->base->sd_wr_dly_sel = wr_dly;
	/* Read delay: Controls timing to sample SD card's CMD, DATA signals */
	host->base->sd_rd_dly_sel = rd_dly;

	return;
}

/* Sets bus width accordingly */
static void spsdv2_set_bus_width(SPSDHOST *host, u32 bus_width)
{
	switch (bus_width) {
	case MMC_BUS_WIDTH_8:
		host->base->sddatawd = 0;
		host->base->mmc8_en = 1;
		break;
	case MMC_BUS_WIDTH_4:
		host->base->sddatawd = 1;
		host->base->mmc8_en = 0;
		if (SP_SDIO_SLOT_ID == host->id) {
			host->base->s4mi = 1;
			host->base->int_multi_trig = 1;
		}
		break;
	case MMC_BUS_WIDTH_1:
		host->base->sddatawd = 0;
		host->base->mmc8_en = 0;
		if (SP_SDIO_SLOT_ID == host->id) {
			host->base->s4mi = 0;
			host->base->int_multi_trig = 1;
		}
		break;
	default:
		EPRINTK("unknown bus wide %d\n", bus_width);
		break;
	}

	return;
}

static void spsdv2_trigger_sdstate(SPSDHOST *host)
{
	host->base->sdctrl_trigger_cmd = 1;   /* Start transaction */
}


#define SP_SD_WAIT_RSP_BUFF_FULL(host) \
	do { \
		while (1) { \
			if ((host)->base->sdstatus & SP_SDSTATUS_RSP_BUF_FULL) \
			break; \
			if ((host)->base->sdstate_new & SDSTATE_NEW_ERROR_TIMEOUT) \
			return; \
		} \
	} while (0)


/*
 * Receive 136 bits response, and pass it back to Linux
 * Used for cmd+rsp and normal dma requests
 * If error occurs, stop receiving response and return
 * Note: Host doesn't support Response R2 CRC error check
 */
static void spsdv2_get_rsp_136(SPSDHOST *host, struct mmc_command *cmd)
{
	unsigned int rspNum;
	unsigned char rspBuf[18] = {0}; /* Used to store 17 bytes(136 bits) or 6 bytes(48 bits) response */

	/*  read R2 response in 3 times, each time read 6 bytes */
	for (rspNum = 0; rspNum + 6 < 18; ) {
		SP_SD_WAIT_RSP_BUFF_FULL(host);
		/*
		 * Store received response buffer data.
		 * Function runs to here only if no error occurs
		 */
		rspBuf[rspNum++] = host->base->sdrspbuf0;
		rspBuf[rspNum++] = host->base->sdrspbuf1;
		rspBuf[rspNum++] = host->base->sdrspbuf2;
		rspBuf[rspNum++] = host->base->sdrspbuf3;
		rspBuf[rspNum++] = host->base->sdrspbuf4;
		rspBuf[rspNum++] = host->base->sdrspbuf5;
	}

	/*
	 * Wait till host controller becomes idle or error occurs
	 * The host may be busy sending 8 clk cycles for the end of a request
	 */
	while (1) {
		if (host->base->sdstate_new & SDSTATE_NEW_FINISH_IDLE)
			break;
		if (host->base->sdstate_new & SDSTATE_NEW_ERROR_TIMEOUT)
			return;
	}

	/*
	 * Pass response back to Linux
	 * Function runs to here only if no error occurs
	 */
	cmd->resp[0] = (rspBuf[1] << 24) | (rspBuf[2] << 16) | (rspBuf[3] << 8) | rspBuf[4];
	cmd->resp[1] = (rspBuf[5] << 24) | (rspBuf[6] << 16) | (rspBuf[7] << 8) | rspBuf[8];
	cmd->resp[2] = (rspBuf[9] << 24) | (rspBuf[10] << 16) | (rspBuf[11] << 8) | rspBuf[12];
	cmd->resp[3] = (rspBuf[13] << 24) | (rspBuf[14] << 16) | (rspBuf[15] << 8) | rspBuf[16];

	return;
}

/*
 * Receive 48 bits response, and pass it back to Linux
 * Used for cmd+rsp and normal dma requests
 * If error occurs, stop receiving response and return
 */
static void spsdv2_get_rsp_48(SPSDHOST *host, struct mmc_command *cmd)
{
	unsigned char rspBuf[6] = {0}; /* Used to store 6 bytes(48 bits) response */

	/* Wait till host controller becomes idle or error occurs */
	while (1) {
		if (host->base->sdstate_new & SDSTATE_NEW_FINISH_IDLE)
			break;
		if (host->base->sdstate_new & SDSTATE_NEW_ERROR_TIMEOUT)
			return;
	}
	/*
	 * Store received response buffer data
	 * Function runs to here only if no error occurs
	 */
	rspBuf[0] = host->base->sdrspbuf0;
	rspBuf[1] = host->base->sdrspbuf1;
	rspBuf[2] = host->base->sdrspbuf2;
	rspBuf[3] = host->base->sdrspbuf3;
	rspBuf[4] = host->base->sdrspbuf4;
	rspBuf[5] = host->base->sdrspbuf5;

	/* Pass response back to Linux */
	cmd->resp[0] = (rspBuf[1] << 24) | (rspBuf[2] << 16) | (rspBuf[3] << 8) | rspBuf[4];
	cmd->resp[1] = rspBuf[5] << 24;

	return;
}

/*
 * Retrieve response for cmd+rsp and normal dma request
 * This function makes sure host returns to it's sdstate_new idle or error state
 * Note: Error handling should be performed afterwards
 */
static void spsdv2_get_rsp(SPSDHOST *host, struct mmc_command *cmd)
{
	if (cmd->flags & MMC_RSP_136)
		spsdv2_get_rsp_136(host, cmd);
	else
		spsdv2_get_rsp_48(host, cmd);
	return;
}

#if 0
ETIMEDOUT    Card took too long to respond
EILSEQ       Basic format problem with the received or sent data
(e.g. CRC check failed, incorrect opcode in response or bad end bit)
#endif

static void spsdv2_check_sdstatus_errors(SPSDHOST *host, struct mmc_command *cmd, struct mmc_data *data)
{
	if (host->base->sdstate_new & SDSTATE_NEW_ERROR_TIMEOUT) {
		DPRINTK("cmd %d error with sdstate = %x, sdstatus = %x\n",
			cmd->opcode, host->base->sd_state, host->base->sdstatus);
		/* Response related errors */
		if (host->base->sdstatus & SP_SDSTATUS_WAIT_RSP_TIMEOUT)
			cmd->error = -ETIMEDOUT;
		if (host->base->sdstatus & SP_SDSTATUS_RSP_CRC7_ERROR)
			cmd->error = -EILSEQ;

		/* Only check the below error flags if data transaction is involved */
		if(data) {
			/* Data transaction related errors */
			if (host->base->sdstatus & SP_SDSTATUS_WAIT_STB_TIMEOUT)
				data->error = -ETIMEDOUT;
			if (host->base->sdstatus & SP_SDSTATUS_WAIT_CARD_CRC_CHECK_TIMEOUT)
				data->error = -ETIMEDOUT;

			if (host->base->sdstatus & SP_SDSTATUS_CRC_TOKEN_CHECK_ERROR)
				data->error = -EILSEQ;
			if (host->base->sdstatus & SP_SDSTATUS_RDATA_CRC16_ERROR)
				data->error = -EILSEQ;

			/* Reset PBus channel */
			Sd_Bus_Reset_Channel(host);
		}
	}

	return;
}

/*
 * Receive 48 bits response, and pass it back to kernel
 * Used for interrupt transactions (don't need to wait sdstate_new to become idle)
 */
static void spsdv2_get_response_48(SPSDHOST *host)
{
	unsigned char rspBuf[6] = {0}; /* Used to store 6 bytes(48 bits) response */

	/* Store received response buffer data */
	rspBuf[0] = host->base->sdrspbuf0;
	rspBuf[1] = host->base->sdrspbuf1;
	rspBuf[2] = host->base->sdrspbuf2;
	rspBuf[3] = host->base->sdrspbuf3;
	rspBuf[4] = host->base->sdrspbuf4;
	rspBuf[5] = host->base->sdrspbuf5;

	/* Pass response back to kernel */
	host->mrq->cmd->resp[0] = (rspBuf[1] << 24) | (rspBuf[2] << 16) | (rspBuf[3] << 8) | rspBuf[4];
	host->mrq->cmd->resp[1] = rspBuf[5] << 24;

	return;
}

void dump_all_regs(SPSDHOST *host)
{
	volatile unsigned int *reg = (volatile unsigned int *)host->base;
	int i, j;
	for (i =  0; i < 5; i++){
		for (j =  0; j < 32; j++){
			printk("g%d.%d = 0x%08x\n", i, j, *reg);
			reg++;
		}
	}
}

static void spsdv2_irq_normDMA(SPSDHOST *host)
{
	struct mmc_data *data = host->mrq->data;
	struct mmc_command *cmd = host->mrq->cmd;

	/* Get response */
	spsdv2_get_response_48(host);
	/* Check error flags */
	spsdv2_check_sdstatus_errors(host, cmd, data);

	if (host->base->sdstate_new & SDSTATE_NEW_FINISH_IDLE) {
		data->bytes_xfered = data->blocks * data->blksz;
	} else {
		EPRINTK("normal DMA error!\n");
		Sd_Bus_Reset_Channel(host);
		data->bytes_xfered = 0;
	}

	host->base->sd_cmp_clr = 1;

	return;
}

static void spsdv2_irq_cmd_rsp(SPSDHOST *host)
{
	struct mmc_data *data = host->mrq->data;
	struct mmc_command *cmd = host->mrq->cmd;

	/* Get response */
	spsdv2_get_response_48(host);
	/* Check error flags */
	spsdv2_check_sdstatus_errors(host, cmd, data);

	host->base->sd_cmp_clr = 1;

	return;
}

/* Interrupt Handler */
irqreturn_t spsdv2_irq(int irq, void *dev_id)
{
	struct mmc_host *mmc = dev_id;

	SPSDHOST *host = (SPSDHOST *)mmc_priv(mmc);

	/*ignore unexpected irq */
	if (SP_SDIO_SLOT_ID == host->id) {
		if (host->base->sd_cmp) {
			if (host->mrq->data != NULL)
				spsdv2_irq_normDMA(host);
			else
				spsdv2_irq_cmd_rsp(host);
			sphe_mmc_finish_request(host, host->mrq);
		}

		if (host->base->sdio_int) {
			host->base->sdio_int_clr = 1;
			mmc_signal_sdio_irq(mmc);
		}
	} else {
		if(!host->base->sd_cmp &&
				!host->base->hw_dma_en)
		{
			printk("!!!!!spsdv2_irq:unknow int\n");
			return IRQ_HANDLED;
		}

		if (host->mrq->data != NULL) {
			spsdv2_irq_normDMA(host);
		} else { /* Cmd + Rsp(48 bits) IRQ */
			spsdv2_irq_cmd_rsp(host);
		}
		/* disbale interrupt to workaround unexpected irq*/
		host->base->dmacmpen_interrupt = 0;
		host->base->sdcmpen = 0;
		sphe_mmc_finish_request(host, host->mrq);
	}
	return IRQ_HANDLED;
}

/*
 * 1. Releases semaphore for mmc_request
 * 2. Notifies kernel that mmc_request is done
 */
static void sphe_mmc_finish_request(SPSDHOST *host, struct mmc_request *mrq)
{
	if (mrq->data) {
		/*
		 * The original sg_len may differ after dma_map_sg function callback.
		 * When executing dma_unmap_sg, the memory segment count value returned
		 * by dma_map_sg should be used instead (value is stored in host->dma_sgcount)
		 */
		dma_unmap_sg(mmc_dev(host->mmc), mrq->data->sg,
					host->dma_sgcount,
					spsdv2_get_dma_dir(host, mrq->data));
		if(mrq->data->error && -EINVAL != mrq->data->error) {
			/* tune next data request timing */
			host->need_tune_dat_timing = 1;
			EPRINTK("data err(%d)\n", mrq->data->error);
		}
	}
	if(mrq->cmd->error) {
		/* tune next cmd request timing */
		host->need_tune_cmd_timing = 1;
		EPRINTK("cmd err(%d)\n",mrq->cmd->error);
	}

	host->mrq = NULL;

	up(&host->req_sem);
	mmc_request_done(host->mmc, mrq);
}

static int spsdv2_get_dma_dir(SPSDHOST *host, struct mmc_data *data)
{
	if (data->flags & MMC_DATA_WRITE)
		return DMA_TO_DEVICE;
	else
		return DMA_FROM_DEVICE;
}

/* Get timeout_ns from kernel, and set it to HW DMA's register */
static inline void spsdv2_set_data_timeout(SPSDHOST *host)
{
	unsigned int timeout_clks, cycle_ns;

	cycle_ns = 1000000000 / sd_get_in_clock(host);
	timeout_clks = host->mrq->data->timeout_ns / cycle_ns;
	timeout_clks +=  host->mrq->data->timeout_clks;

	/*  kernel set max read timeout to for SDHC 100ms, */
	/*  mult 10 to  Improve compatibility for some unstandard card */
	if (host->mrq->data->flags & MMC_DATA_READ)
		timeout_clks *= READ_TIMEOUT_MULT;
	else {
		if (SP_EMMCSLOT_ID == host->id) {
			timeout_clks *= EMMC_WRITE_TIMEOUT_MULT;
		}
	}

	host->base->hw_wait_num_low = (u16)(timeout_clks & 0x0000ffff);
	host->base->hw_wait_num_high = (u16)((timeout_clks >> 16) & 0x0000ffff);
}

static void spsdv2_set_data_info(SPSDHOST *host, struct mmc_data *data)
{
	int i, count = dma_map_sg(mmc_dev(host->mmc), data->sg,
							  data->sg_len,
							  spsdv2_get_dma_dir(host, data));
	struct scatterlist *sg;
	struct mmc_request *mrq = host->mrq;
	unsigned int hw_address[SP_HW_DMA_MEMORY_SECTORS] = {0}, hw_len[SP_HW_DMA_MEMORY_SECTORS] = {0};

	/* Store the dma_mapped memory segment count, it will be used when calling dma_unmap_sg */
	host->dma_sgcount = count;

	/* retreive physical memory address & size of the fragmented memory blocks */
	for_each_sg(data->sg, sg, count, i) {
		hw_address[i] = sg_dma_address(sg);
		hw_len[i] = sg_dma_len(sg);
		if(unlikely(!IS_DMA_ADDR_2BYTE_ALIGNED(hw_address[i]))) {
			printk("[sd err]dma addr is not 2 bytes aligned\n");
			data->error = -EINVAL;
			return;
		}
	}

	/* Due to host limitations, normal DMA transfer mode only supports 1 consecutive physical memory area */
	if (count == 1) {
		/* Reset */
		Reset_Controller(host);

		/* Configure Group SD Registers */
		host->base->sd_cmdbuf[0] = (u8)(mrq->cmd->opcode | 0x40);	/* add start bit, according to spec, command format */
		host->base->sd_cmdbuf[1] = (u8)((mrq->cmd->arg >> 24) & 0x000000ff);
		host->base->sd_cmdbuf[2] = (u8)((mrq->cmd->arg >> 16) & 0x000000ff);
		host->base->sd_cmdbuf[3] = (u8)((mrq->cmd->arg >>  8) & 0x000000ff);
		host->base->sd_cmdbuf[4] = (u8)((mrq->cmd->arg >>  0) & 0x000000ff);

		SD_PAGE_NUM_SET(host->base, data->blocks);
		if (mrq->cmd->flags & MMC_RSP_CRC && !(mrq->cmd->flags & MMC_RSP_136))
			host->base->sdrspchk_en = 0x1;
		else
			host->base->sdrspchk_en = 0x0;

		if (data->flags & MMC_DATA_READ) {
			host->base->sdcmddummy = 0;
			host->base->sdautorsp = 0;
			host->base->sd_trans_mode = 2;
		} else {
			host->base->sdcmddummy = 1;
			host->base->sdautorsp = 1;
			host->base->sd_trans_mode = 1;
		}
		if (mrq->stop)
			host->base->sd_len_mode = 0;
		else
			host->base->sd_len_mode = 1;

		host->base->sdpiomode = 0;
		host->base->sdcrctmren = 1;
		host->base->sdrsptmren = 1;
		host->base->hw_dma_en = 0;
		/* Set response type */
		if(mrq->cmd->flags & MMC_RSP_136)
			host->base->sdrsptype = 0x1;
		else
			host->base->sdrsptype = 0x0;

		SDDATALEN_SET(host->base, data->blksz);

		/* Configure Group DMA Registers */
		if (data->flags & MMC_DATA_WRITE) {
			host->base->dmadst = 0x2;
			host->base->dmasrc = 0x1;
		} else {
			host->base->dmadst = 0x1;
			host->base->dmasrc = 0x2;
		}
		DMASIZE_SET(host->base, data->blksz);
		SET_HW_DMA_BASE_ADDR(host->base, hw_address[0]);
	} else {
		EPRINTK("SD Card DMA memory segment > 1, not supported!\n");
		data->error = -EINVAL;
	}
	return;
}

/*
 * DMA transfer mode, used for all other data transfer commands besides read/write block commands (cmd17, 18, 24, 25)
 * Due to host limitations, this kind of DMA transfer mode only supports 1 consecutive memory area
 */
static void spsdv2_proc_normDMA(SPSDHOST *host)
{
	struct mmc_request *mrq = host->mrq;
	struct mmc_data *data = mrq->data;
	spsdv2_prepare_cmd_rsp(host, mrq->cmd);
	spsdv2_set_data_info(host, data);
	if (!data->error) {
		/* Configure SD INT reg */
		/* Disable HW DMA data transfer complete interrupt (when using sdcmpen) */
		host->base->dmacmpen_interrupt = 0;
		host->base->sdcmpen = 0x1;
		/* Start Transaction */
		host->base->sdctrl_trigger_cmd = 1;
	}
	return;
}

static void spsdv2_prepare_cmd_rsp(SPSDHOST *host, struct mmc_command *cmd)
{
	struct mmc_request *mrq = host->mrq;

	/* Reset */
	Reset_Controller(host);

	/* Configure Group SD Registers */
	host->base->sd_cmdbuf[0] = (u8)(cmd->opcode | 0x40);	/* add start bit, according to spec, command format */
	host->base->sd_cmdbuf[1] = (u8)((cmd->arg >> 24) & 0x000000ff);
	host->base->sd_cmdbuf[2] = (u8)((cmd->arg >> 16) & 0x000000ff);
	host->base->sd_cmdbuf[3] = (u8)((cmd->arg >>  8) & 0x000000ff);
	host->base->sd_cmdbuf[4] = (u8)((cmd->arg >>  0) & 0x000000ff);

	host->base->sd_trans_mode = 0x0;
	if (cmd->flags & MMC_RSP_PRESENT)
		host->base->sdautorsp = 1;
	else
		host->base->sdautorsp = 0;
	host->base->sdcmddummy = 1;

	/*
	 * Currently, host is not capable of checking Response R2's CRC7
	 * Because of this, enable response crc7 check only for 48 bit response commands
	 */
	if (cmd->flags & MMC_RSP_CRC && !(mrq->cmd->flags & MMC_RSP_136))
		host->base->sdrspchk_en = 0x1;
	else
		host->base->sdrspchk_en = 0x0;

	if (cmd->flags & MMC_RSP_136)
		host->base->sdrsptype = 0x1;
	else
		host->base->sdrsptype = 0x0;

	/* Configure SD INT reg (Disable them) */
	host->base->dmacmpen_interrupt = 0;
	host->base->sdcmpen = 0x0;

	return;
}

/* Process Command + Response commands (with no data) , interrupt mode */
static void spsdv2_mmc_proc_cmd_rsp_intr(SPSDHOST *host)
{
	struct mmc_request *mrq = host->mrq;
	spsdv2_prepare_cmd_rsp(host, mrq->cmd);

	/* Configure SD INT reg */
	/* Disable HW DMA data transfer complete interrupt (when using sdcmpen) */
	host->base->dmacmpen_interrupt = 0;
	host->base->sdcmpen = 0x1;

	/* Start Transaction */
	host->base->sdctrl_trigger_cmd = 1;

	return;
}

/* Process Command + No Response commands (with no data) */
static void spsdv2_mmc_proc_cmd(SPSDHOST *host)
{
	struct mmc_request *mrq = host->mrq;
	spsdv2_prepare_cmd_rsp(host, mrq->cmd);

	/* Configure SD INT reg */
	/* Disable HW DMA data transfer complete interrupt (when using sdcmpen) */
	host->base->dmacmpen_interrupt = 0;
	host->base->sdcmpen = 0x0;

	/* Start Transaction */
	host->base->sdctrl_trigger_cmd = 1;

	while((host->base->sdstate_new & SDSTATE_NEW_FINISH_IDLE) != 0x40) {
		/* printk("Waiting! sd_hw_state : 0x%x   LMST_SM:0x%x   Data In Counter :%u Data Out Counter: %u\n", host->base->hwsd_sm, host->base->lmst_sm, host->base->incnt, host->base->outcnt); */
		/* printk("sd status:0x%x, state:0x%x, state new:0x%x\n", host->base->sdstatus, host->base->sdstate, host->base->sdstate_new); */
		/* printk("Waiting\n"); */
	}

	sphe_mmc_finish_request(host, host->mrq);
}

/* For mmc_requests
 * ================ Data transfer requests ===========================
 * 1. Data transfer requests : use interrupt mode
 * ================= Non data transfer requests =======================
 * 1. Command + Response (48 bit response) requests : use interrupt mode
 * 2. Command + Response (136 bit response) requests : use polling mode
 * 3. Command (no response) requests : use polling mode
 */
void spsdv2_mmc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	SPSDHOST *host = mmc_priv(mmc);
	int retry_count = (host->need_tune_cmd_timing || host->need_tune_dat_timing)
		? MAX_DLY_CLK : 0;

	DPRINTK("\n<----- mrq:0x%x, cmd:%d, arg:0x%08x, data len:%d, stop:0x%x\n",
		(unsigned int)mrq, mrq->cmd->opcode&0xfff, mrq->cmd->arg,
		(mrq->data)?(mrq->data->blocks*mrq->data->blksz):0, (unsigned int)mrq->stop);

	/*
	 * The below semaphore is released when "sphe_mmc_finish_request" is called
	 * TODO: Observe if the below semaphore is necessary
	 */
	down(&host->req_sem);

	host->mrq = mrq;
	if (mrq->data == NULL) {
		if (mrq->cmd->flags & MMC_RSP_PRESENT) {
			if (unlikely(host->need_tune_cmd_timing || mrq->cmd->flags & MMC_RSP_136)) {
				do {
					mrq->cmd->error = 0;
					spsdv2_prepare_cmd_rsp(host, mrq->cmd);
					spsdv2_trigger_sdstate(host);
					spsdv2_get_rsp(host, mrq->cmd); /* Makes sure host returns to an idle or error state */
					spsdv2_check_sdstatus_errors(host, mrq->cmd, mrq->data);
					if (-EILSEQ == mrq->cmd->error) {
						host->base->sd_rd_dly_sel++;
					} else if (-ETIMEDOUT == mrq->cmd->error) {
						host->base->sd_wr_dly_sel++;
					} else {
						break;
					}
				} while(retry_count--);
				if (!mrq->cmd->error)
					host->need_tune_cmd_timing = 0;
				sphe_mmc_finish_request(host, host->mrq);
			} else {
				spsdv2_mmc_proc_cmd_rsp_intr(host);
			}
		} else {
			spsdv2_mmc_proc_cmd(host);
		}
	} else {
		if (unlikely(host->need_tune_dat_timing)) {
			do {
				mrq->data->error = 0;
				spsdv2_prepare_cmd_rsp(host, mrq->cmd);
				spsdv2_set_data_info(host, mrq->data);
				if(-EINVAL == mrq->data->error) {
					break;
				}
				spsdv2_trigger_sdstate(host);
				spsdv2_get_rsp(host, mrq->cmd); /* Makes sure host returns to an idle or error state */
				spsdv2_check_sdstatus_errors(host, mrq->cmd, mrq->data);
				if (-EILSEQ == mrq->data->error) {
					if (mrq->data->flags & MMC_DATA_WRITE) {
						if (is_crc_token_valid(host))
							host->base->sd_wr_dly_sel++;
						else
							host->base->sd_rd_dly_sel++;
					} else {
						host->base->sd_rd_dly_sel++;
					}
				} else if (-ETIMEDOUT == mrq->data->error) {
					host->base->sd_wr_dly_sel++;
				} else {
					break;
				}
				if (mrq->stop) {
					spsdv2_prepare_cmd_rsp(host, mrq->stop);
					spsdv2_trigger_sdstate(host);
					spsdv2_get_rsp(host, mrq->stop); /* Makes sure host returns to an idle or error state */
					spsdv2_check_sdstatus_errors(host, mrq->stop, NULL);
				}
			} while(retry_count--);
			if (!mrq->data->error)
				host->need_tune_dat_timing = 0;
			sphe_mmc_finish_request(host, host->mrq);
		} else {
			spsdv2_proc_normDMA(host);
			if(-EINVAL == host->mrq->data->error) { /*  para is not correct return */
				sphe_mmc_finish_request(host, host->mrq);
			}
		}
	}
}

/* set_ios -
 * 1) Set/Disable clock
 * 2) Power on/off to offer SD card or not
 * 3) Set SD Card Bus width to 1 or 4
 */
void spsdv2_mmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	SPSDHOST *host = (SPSDHOST *)mmc_priv(mmc);

	IFPRINTK("\n<----- sd%d: clk:%d, buswidth:%d(2^n), bus_mode:%d, powermode:%d(0:off, 1:up, 2:on), timing:%d\n",
		host->id, ios->clock, ios->bus_width, ios->bus_mode, ios->power_mode, ios->timing);

	down(&host->req_sem);

	spsdv2_set_power_mode(host, ios);
	spsdv2_set_clock(host, ios);
	spsdv2_set_bus_width(host, ios->bus_width);
	host->need_tune_cmd_timing = 0;
	host->need_tune_dat_timing = 0;

	up(&host->req_sem);
	IFPRINTK("----- \n\n");

	return;
}

int spsdv2_mmc_read_only(struct mmc_host *mmc)
{
	/* return  > 0 :support read only */
	/*         < 0 :not support RO */
	/*         = 0 :no action */
	return -ENOSYS;
}


/*
 * Return values for the get_cd callback should be:
 *   0 for a absent card
 *   1 for a present card
 *   -ENOSYS when not supported (equal to NULL callback)
 *   or a negative errno value when something bad happened
 */
int spsdv2_mmc_card_detect(struct mmc_host *mmc)
{
	SPSDHOST *host = mmc_priv(mmc);

	int ret = 0;
	#ifdef SP_SDCARD_SENSE_WITH_GPIO
	ret = !GPIO_I_GET(host->cd_gpio);
	#endif
	host->cd_state = ret;
	return ret;
}

static void spsdv2_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	SPSDHOST *host = mmc_priv(mmc);
	if (enable) {
		host->base->sdio_int_en = 1;
	} else {
		host->base->sdio_int_en = 0;
	}
}

const struct mmc_host_ops spsdv2_sdc_ops = {
	.request = spsdv2_mmc_request,
	.set_ios = spsdv2_mmc_set_ios,
	.get_ro = spsdv2_mmc_read_only,
	.get_cd = spsdv2_mmc_card_detect,
	.enable_sdio_irq = spsdv2_enable_sdio_irq,
};


static uint get_max_sd_freq(SPSDHOST *host)
{
	uint max_freq = SPSD_MAX_CLOCK;

	/* fix me read from device tree */
	max_freq = SPSD_MAX_CLOCK;

	printk("[SDCard] Slot %d, actually use max_freq #%d M\n", host->id, max_freq/CLOCK_1M);
	return max_freq;
}

int spsdv2_drv_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct mmc_host *mmc;
	struct resource *resource;
	SPSDHOST *host;
	spsdv2_dridata_t *priv = NULL;

	if (pdev->dev.of_node) {
		const struct of_device_id *match;
		match = of_match_node(spsdv2_of_id, pdev->dev.of_node);
		if (match)
			priv = (spsdv2_dridata_t *)match->data;
	}

	mmc = mmc_alloc_host(sizeof(SPSDHOST), &pdev->dev);
	if (!mmc) {
		ret = -ENOMEM;
		goto probe_free_host;
	}

	host = (SPSDHOST *)mmc_priv(mmc);
	host->mmc = mmc;
	host->pdev = pdev;
	host->power_state = MMC_POWER_UNDEFINED;

	if (priv)
		host->id  = priv->id;

	printk("sd slot id:%d\n", host->id);

	/*sd controller register*/
	resource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (IS_ERR(resource)) {
		EPRINTK("get sd %d register resource fail\n", host->id);
		ret = PTR_ERR(resource);
		goto probe_free_host;
	}

	if ((resource->end - resource->start + 1) < sizeof(SDREG)) {
		EPRINTK("register size not right e:%d:r:%d\n",
			resource->end - resource->start + 1, sizeof(SDREG));
		ret = -EINVAL;
		goto probe_free_host;
	}

	host->base = devm_ioremap_resource(&pdev->dev, resource);
	if (IS_ERR((void *)host->base)) {
		EPRINTK("devm_ioremap_resource fail\n");
		ret = PTR_ERR((void *)host->base);
		goto probe_free_host;
	}
	DPRINTK("SD card driver probe, sd %d, base:0x%x, host size:%d\n", host->id, resource->start, resource->end - resource->start);

	/* host->irq = of_irq_get(pdev->dev.of_node, 0); */
	host->irq = platform_get_irq(pdev, 0);
	if (host->irq <= 0) {
		EPRINTK("get sd %d irq resource fail\n", host->id);
		ret = -EINVAL;
		goto probe_free_host;
	}
	DPRINTK("irq  %d\n",	host->irq);

	/*
	 * fix me read from device tree after clock pinmux device tree ok
	 */
	pinmux_enable(host);

	sd_set_in_clock(host);

	if(SP_EMMCSLOT_ID == host->id) {
		host->wrdly = host->base->sd_wr_dly_sel;
		host->rddly = host->base->sd_rd_dly_sel;
	} else if(SP_SDIO_SLOT_ID == host->id) {
		host->wrdly = 1;
		host->rddly = 0;
	}

	if (request_irq(host->irq, spsdv2_irq, IRQF_SHARED, dev_name(&pdev->dev), mmc)) {
		printk("\nFailed to request sd card interrupt.\n");
		ret = -ENOENT;
		goto probe_free_host;
	}

	DPRINTK("[SDCard] Slot %d driver probe, host:0x%x, base:0x%x\n", host->id, (unsigned int)host,(unsigned int)host->base);


	sema_init(&host->req_sem, 1);

	mmc->ops = &spsdv2_sdc_ops;

	/*
	 * freq_divisor[11:10] = sdfreq[1:0]
	 * freq_divisor[9:0] = sdfqsel[9:0]
	 */
	mmc->f_min = (SPSD_CLK_SOURCE / (0x0FFF + 1));
	mmc->f_max = get_max_sd_freq(host);

	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;

	mmc->max_seg_size = 65536 * 512;            /* Size per segment is limited via host controller's
	                                               ((sdram_sector_#_size max value) * 512) */
	/* mmc->max_segs = SP_HW_DMA_MEMORY_SECTORS; */
	/* Host controller supports up to "SP_HW_DMA_MEMORY_SECTORS", a.k.a. max scattered memory segments per request */
	mmc->max_segs = 1;
	mmc->max_req_size = 65536 * 512;			/* Decided by hw_page_num0 * SDHC's blk size */
	mmc->max_blk_size = 512;                   /* Limited by host's dma_size & data_length max value, set it to 512 bytes for now */
	mmc->max_blk_count = 65536;                 /* Limited by sdram_sector_#_size max value */
	if(SP_EMMCSLOT_ID == host->id) {
		mmc->caps =  MMC_CAP_MMC_HIGHSPEED | MMC_CAP_4_BIT_DATA | MMC_CAP_8_BIT_DATA | MMC_CAP_NONREMOVABLE;
		mmc->caps2 = MMC_CAP2_NO_SDIO | MMC_CAP2_NO_SD;
	} else if (SP_SDIO_SLOT_ID == host->id) {
		mmc->caps =  MMC_CAP_SD_HIGHSPEED | MMC_CAP_4_BIT_DATA | MMC_CAP_SDIO_IRQ | MMC_CAP_NONREMOVABLE;
		mmc->caps2 = MMC_CAP2_NO_SD | MMC_CAP2_NO_MMC;
	} else {
		mmc->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_SD_HIGHSPEED | MMC_CAP_NEEDS_POLL;
		mmc->caps2 = MMC_CAP2_NO_SDIO | MMC_CAP2_NO_MMC;
		#ifdef SP_SDCARD_SENSE_WITH_GPIO
		if (of_property_read_u32(pdev->dev.of_node, "sense-gpio", &host->cd_gpio)) {
			printk(KERN_ERR "Failed to get card detect gpio pin configuration!\n");
			ret = -ENOENT;
			goto probe_free_host;
		}
		GPIO_F_SET(host->cd_gpio, 1);
		GPIO_M_SET(host->cd_gpio, 1);
		GPIO_E_SET(host->cd_gpio, 0);
		#endif
	}
	dev_set_drvdata(&pdev->dev, mmc);

	mmc_add_host(mmc);
	pm_runtime_set_active(&pdev->dev);
	pm_runtime_enable(&pdev->dev);
	return 0;

probe_free_host:
	if (mmc)
		mmc_free_host(mmc);

	return ret;
}

int spsdv2_drv_remove(struct platform_device *dev)
{
	struct mmc_host *mmc;
	SPSDHOST *host;

	DPRINTK("Remove sd card\n");
	mmc = platform_get_drvdata(dev);
	if (!mmc)
		return -EINVAL;

	host = (SPSDHOST *)mmc_priv(mmc);
	devm_iounmap(&dev->dev, (void *)host->base);
	platform_set_drvdata(dev, NULL);

	mmc_remove_host(mmc);
	free_irq(host->irq, mmc);
	mmc_free_host(mmc);

	return 0;
}


int spsdv2_drv_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host *mmc;
	SPSDHOST *host;

	mmc = platform_get_drvdata(dev);
	if (!mmc)
		return -EINVAL;

	host = (SPSDHOST *)mmc_priv(mmc);

	printk("Sunplus SD %d driver suspend.\n", host->id);
	down(&host->req_sem);
	up(&host->req_sem);

	return 0;
}

int spsdv2_drv_resume(struct platform_device *dev)
{

#ifdef CONFIG_PM
	struct mmc_host *mmc;
	SPSDHOST *host;

	mmc = platform_get_drvdata(dev);
	if (!mmc)
		return -EINVAL;

	host = (SPSDHOST *)mmc_priv(mmc);


	printk("Sunplus SD%d driver resume.\n", host->id);
#endif

	return 0;
}
#ifdef CONFIG_PM

static int spsdv2_pm_suspend(struct device *dev)
{
	pm_runtime_force_suspend(dev);
	return 0;
}

static int spsdv2_pm_resume(struct device *dev)
{
	pm_runtime_force_resume(dev);
	return 0;
}

static int spsdv2_pm_runtime_suspend(struct device *dev)
{
	#if 0
	int qctl_val, ret = 0, retry = 1000;
	struct mmc_host *mmc;
	SPSDHOST *host;
	volatile void __iomem *reg;
	mmc = platform_get_drvdata(to_platform_device(dev));
	if (!mmc)
		return -EINVAL;
	host = (SPSDHOST *)mmc_priv(mmc);
	down(&host->req_sem);
	if (ctlr_clk_disable()) {
		EPRINTK("fail to disable card controller clock!\n");
		ret = -EIO;
		goto out;
	}
	reg = ioremap_nocache(RF_GRP(30, 1), 4);
	if (!reg) {
		EPRINTK("map Q-channel register failed!\n");
		ret = -EIO;
		goto out;
	}
	writel(RF_MASK_V_CLR(1 << 11), reg);
	qctl_val = readl(reg);
	while((qctl_val & (1 << 8)) && retry--)
		qctl_val = readl(reg);
	if ((qctl_val & (1 << 10)) || !retry) {
		EPRINTK("[SD]q-channel unable to enter quiescence state\n");
		writel(RF_MASK_V_SET(1 << 11), reg);
		ret = -EIO;
	}
	iounmap(reg);
out:
	up(&host->req_sem);
	return ret;
	#endif
	return 0;
}

static int spsdv2_pm_runtime_resume(struct device *dev)
{
	#if 0
	int qctl_val, ret = 0, retry = 1000;
	struct mmc_host *mmc;
	SPSDHOST *host;
	volatile void __iomem *reg;
	mmc = platform_get_drvdata(to_platform_device(dev));
	if (!mmc)
		return -EINVAL;
	host = (SPSDHOST *)mmc_priv(mmc);
	reg = ioremap_nocache(RF_GRP(30, 1), 4);
	if (!reg) {
		EPRINTK("map Q-channel register failed!\n");
		ret = -EIO;
		goto out;
	}
	writel(RF_MASK_V_SET(1 << 11), reg);
	qctl_val = readl(reg);
	while(!(qctl_val & (1 << 8)) && retry--)
		qctl_val = readl(reg);
	iounmap(reg);
	if (!retry) {
		EPRINTK("[SD]q-channel unable to exit quiescence state\n");
		ret = -EIO;
		goto out;
	}
	if (ctlr_clk_enable()) {
		EPRINTK("fail to enable card controller clock!\n");
		ret = -EIO;
	}
out:
	return ret;
	#endif
	return 0;
}

const struct dev_pm_ops sphe_mmc2_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(spsdv2_pm_suspend, spsdv2_pm_resume)
	SET_RUNTIME_PM_OPS(spsdv2_pm_runtime_suspend, spsdv2_pm_runtime_resume, NULL)
};
#endif



struct platform_driver spsdv2_driver_sdc = {
	.probe = spsdv2_drv_probe,
	.remove = spsdv2_drv_remove,
	.shutdown = NULL,
	.suspend = spsdv2_drv_suspend,
	.resume = spsdv2_drv_resume,
	.driver = {
		.name = SPSDV2_SDC_NAME,
		.owner = THIS_MODULE,
		.of_match_table = spsdv2_of_id,
#ifdef CONFIG_PM
		.pm= &sphe_mmc2_pm_ops,
#endif
	}
};

module_platform_driver(spsdv2_driver_sdc);

MODULE_AUTHOR("SPHE B1");
MODULE_DESCRIPTION("SPHE MMC/SD Card Interface Driver");
MODULE_LICENSE("GPL v2");
