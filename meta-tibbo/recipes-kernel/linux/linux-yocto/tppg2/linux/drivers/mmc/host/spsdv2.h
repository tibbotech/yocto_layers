#ifndef _SPSDV2_H_
#define _SPSDV2_H_
////////////////////////////////////////////////////////////////
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/mmc/host.h>
#include <linux/platform_device.h>
#include <linux/mmc/mmc.h>
#include <mach/irqs.h>

/////////////////////////////////////////////////////////////////
#define SPSD_SUCCESS			0x00000000
#define SPSD_FAIL				0x00000001
#define SPSD_RSP_TIMEOUT		0x00000002
#define SPSD_CRC_ERROR			0x00000004
#define SPSD_COUNTER_ERROR		0x00000008
#define SPSD_DMA_FAIL			0x00000010
#define SPSD_TIMEOUT			0x00000020
#define SPSD_CRC_TIMEOUT		0x00000040
#define SPSD_CARD_NOT_EXIST		0x00000080
#define SPSD_COMPLETE_INIT		0x80000000

#define SDCARD_STATUS_NOTEXIST 			0
#define SDCARD_STATUS_EXIST 			1
#define SDCARD_STATUS_TESTNOTEXIST 		2
#define SDCARD_STATUS_TESTEXIST 		3

//1.17[14]
#define CARD_MODE_SD	0
#define CARD_MODE_MMC	1

//to delete
#define SD_1BIT_MODE 			0
#define SD_4BIT_MODE 			1

#define SPRSP_TYPE_NORSP		0
#define SPRSP_TYPE_R1  			1
#define SPRSP_TYPE_R1B 			11
#define SPRSP_TYPE_R2  			2
#define SPRSP_TYPE_R3  			3
#define SPRSP_TYPE_R4  			4
#define SPRSP_TYPE_R5  			5
#define SPRSP_TYPE_R6  			6
#define SPRSP_TYPE_R7 			7

#define SP_DMA_DRAM      		1
#define SP_DMA_FLASH     		2

#define SP_DMA_DST_SHIFT		4
#define SP_DMA_SRT_SHIFT		0

#define SP_BUS_WIDTH_1BIT 0
#define SP_BUS_WIDTH_4BIT 1
#define SP_BUS_WIDTH_8BIT (1<<6)
#define CLOCK_200K	200000
#define CLOCK_300K	300000
#define CLOCK_375K	375000
#define CLOCK_400K	400000
#define CLOCK_1M	1000000
#define CLOCK_5M	5000000
#define CLOCK_10M	10000000
#define CLOCK_12M	12000000
#define CLOCK_15M	15000000
#define CLOCK_20M	20000000
#define CLOCK_25M	25000000
#define CLOCK_27M	27000000
#define CLOCK_30M	30000000
#define CLOCK_35M	35000000
#define CLOCK_45M	45000000
#define CLOCK_48M	48000000
#define CLOCK_50M	50000000
#define CLOCK_52M	52000000

#define CLOCK_240M  240000000
#define CLOCK_270M  270000000
#define CLOCK_202M  202500000

/* kernel set max read timeout to 100ms,
 * mult  to  Improve compatibility for some unstandard card
 */
#define READ_TIMEOUT_MULT     10


/* kernel set max read timeout to 3s,
 * mult  to  Improve compatibility for some unstandard emmc
 */
#define EMMC_WRITE_TIMEOUT_MULT     5

/******************************************************************************
*                          MACRO Function Define
*******************************************************************************/

#ifndef REG
//#define REG(x) (*(volatile unsigned long *)(x))
#define REG(x) (*(volatile unsigned int *)(x))
#endif

#define get_val(addr)		REG(addr)
#define set_val(addr, val)	REG(addr) = (val)

#ifndef set_bit
#define set_bit(addr, val)	set_val((addr), (get_val(addr) | (val)))
#endif

#ifndef clear_bit
#define clear_bit(addr, val)	set_val((addr), (get_val(addr) & ~(val)))
#endif

typedef volatile unsigned int dv_reg;

typedef struct spsd_general_regs {
	/*  group 0 */
	dv_reg mediatype:3;				/* 0. lower bits first, mmc set 6?? */
	dv_reg reserved_card_mediatype:29;
#define SP_MEDIA_NONE			0
#define SP_MEDIA_SMC			1
#define SP_MEDIA_RESERVED1 		2
#define SP_MEDIA_CF				3
#define SP_MEDIA_SPI			4
#define SP_MEDIA_RESERVED2		5
#define SP_MEDIA_SD				6
#define SP_MEDIA_MEMORY_STICK	7

	/* for the register that take whole bits(0-15, ignore 16-31), just define the field name */
	dv_reg reserved0;				/* 1. 32 bits */
	dv_reg cpu_page_cnt;			/* 2. 16 bits */
	dv_reg card_ctl_page_cnt;		/* 3. 16 bits */
	dv_reg sdram_sector_0_size;		/* 4. 16 bits */
	dv_reg ring_buffer_on;			/* 5. 1 bit valid */
	dv_reg CARD_GCLK_DISABLE;		/* 6. 12 bits */
	dv_reg SDRAM_SECTOR_1_ADDR;		/* 7. 32 bits */
	dv_reg SDRAM_SECTOR_1_SIZE;		/* 8. 16 bits */
	dv_reg SDRAM_SECTOR_2_ADDR;		/* 9. 32 bits */
	dv_reg SDRAM_SECTOR_2_SIZE;		/* 10.16 bits */
	dv_reg SDRAM_SECTOR_3_ADDR;		/* 11.32 bits */
	dv_reg SDRAM_SECTOR_3_SIZE;		/* 12.16 bits */
	dv_reg SDRAM_SECTOR_4_ADDR;		/* 13.32 bits */
	dv_reg SDRAM_SECTOR_4_SIZE;		/* 14.16 bits */
	dv_reg SDRAM_SECTOR_5_ADDR;		/* 15.32 bits */
	dv_reg SDRAM_SECTOR_5_SIZE;		/* 16.16 bits */
	dv_reg SDRAM_SECTOR_6_ADDR;		/* 17.32 bits */
	dv_reg SDRAM_SECTOR_6_SIZE;		/* 18.16 bits */
	dv_reg SDRAM_SECTOR_7_ADDR;		/* 19.32 bits */
	dv_reg SDRAM_SECTOR_7_SIZE;		/* 20.16 bits */
	dv_reg sdram_sector_cnt;		/* 21.3 bits */
#define SP_HW_DMA_MEMORY_SECTORS	8 /*  supports up to 8 fragmented memory blocks */
	dv_reg reserved[10];			/* 22-31. */

	/*  group 1 */
	dv_reg reserved_group1[11];			/* 0-10. */

	dv_reg vol_tmr:2;				/* 11. */
	dv_reg sw_set_vol:1;
	dv_reg hw_set_vol:1;
	dv_reg vol_result:2;
	dv_reg Reserved_sd_vol_ctrl:26;

	dv_reg sdcmpen:1;				/* 12. */
	dv_reg sd_cmp:1;
	dv_reg sd_cmp_clr:1;
	dv_reg reserved0_sd_int:1;
	dv_reg sdio_int_en:1;
	dv_reg sdio_int:1;
	dv_reg sdio_int_clr:1;
	dv_reg detect_int_en:1;
	dv_reg detect_int:1;
	dv_reg detect_int_clr:1;
	dv_reg reserved1_sd_int:22;

	dv_reg sd_page_num;				/* 13. 16 bits */

	dv_reg sdpiomode:1;				/* 14. */
	dv_reg sdddrmode:1;
	dv_reg sd_len_mode:1;
#define SP_LENMODE_SEND_STOP 				0
#define SP_LENMODE_NOT_SEND_STOP 			1
	dv_reg reserved0_sd_config0:1;
	dv_reg sd_trans_mode:2;
#define SP_TRANSACTION_MODE_CMD_RSP			0
#define SP_TRANSACTION_MODE_WRITE_DATA		1
#define SP_TRANSACTION_MODE_READ_DATA		2
	dv_reg sdautorsp:1; 				/* sdautorsp should be enabled except CMD0(idle, norsp) and read command(rx data, including acmd51, acmd13, ...) */
#define SP_AUTORSP_NO_RX_RESPONSE 			0
#define SP_AUTORSP_ATUO_RX_RESPONSE 		1
	dv_reg sdcmddummy:1; 				/* sdcmddummy should be enabled except read command(rx data, including acmd51, acmd13, ...) */
	dv_reg sdrspchk_en:1;
	dv_reg reserved1_sd_config0:23;

	dv_reg rwc:1;					/* 15. */
	dv_reg s4mi:1;
	dv_reg resu:1;
	dv_reg sus_req:1;
	dv_reg con_req:1;
	dv_reg sus_data_flag:1;
	dv_reg int_multi_trig:1;
	dv_reg reserved_sdio_ctrl:25;

	dv_reg sdrst:1;					/* 16. */
	dv_reg sdcrcrst:1;
	dv_reg sdiorst:1;
	dv_reg reserved_sd_rst:29;

	dv_reg sdfqsel:12;				/* 17. */
	dv_reg sddatawd:1;				/* bus width:0:1bit   1:4bit */
	dv_reg sdrsptype:1;
#define SP_SD_RSP_TYPE_LEN_48_BITS		0
#define SP_SD_RSP_TYPE_LEN_136_BITS		1
	dv_reg sdrsptmren:1;
	dv_reg sdcrctmren:1;
	dv_reg sdmmcmode:1;
#define SP_SD_MODE   0
#define SP_MMC_MODE  1
	dv_reg selci:1;
	dv_reg mmc8_en:1;
	dv_reg rx4b_en:1;
	dv_reg sdiomode:1;
#define SP_SD_CARD    0
#define SP_SDIO_CARD  1
	dv_reg reserved_sd_config:11;

	dv_reg sdctrl_trigger_cmd:1;	/* 18. */
	dv_reg sdctrl_trigger_dummy:1;
	dv_reg reserved_sd_ctrl:30;

	dv_reg sdstatus:16;				/* 19. */
#define SP_SDSTATUS_DUMMY_READY 					BIT(0)
#define SP_SDSTATUS_RSP_BUF_FULL					BIT(1)
#define SP_SDSTATUS_TX_DATA_BUF_EMPTY				BIT(2)
#define SP_SDSTATUS_RX_DATA_BUF_FULL				BIT(3)
#define SP_SDSTATUS_CMD_PIN_STATUS					BIT(4)
#define SP_SDSTATUS_DAT0_PIN_STATUS					BIT(5)
#define SP_SDSTATUS_WAIT_RSP_TIMEOUT				BIT(6)
#define SP_SDSTATUS_WAIT_CARD_CRC_CHECK_TIMEOUT		BIT(7)
#define SP_SDSTATUS_WAIT_STB_TIMEOUT				BIT(8)
#define SP_SDSTATUS_RSP_CRC7_ERROR					BIT(9)
#define SP_SDSTATUS_CRC_TOKEN_CHECK_ERROR			BIT(10)
#define SP_SDSTATUS_RDATA_CRC16_ERROR				BIT(11)
#define SP_SDSTATUS_SUSPEND_STATE_READY				BIT(12)
#define SP_SDSTATUS_BUSY_CYCLE						BIT(13)
#define SP_SDSTATUS_SD_DATA1						BIT(14)
#define SP_SDSTATUS_SD_SENSE						BIT(15)

	dv_reg reserved_sd_status:16;

	union {
		struct {
			dv_reg sdstate:3;				/* 20. */
			dv_reg reserved0_sd_state:1;
			dv_reg sdcrdcrc:3;
#define WRITE_CRC_TOKEN_CORRECT		2
#define WRITE_CRC_TOKEN_ERROR		5
			dv_reg reserved1_sd_state:1;
			dv_reg sdstate_new:7;
#define SDSTATE_NEW_FINISH_IDLE 		BIT(6)
#define SDSTATE_NEW_ERROR_TIMEOUT 		BIT(5)
			dv_reg reserved2_sd_state:17;
		};
		dv_reg sd_state;
	};

	dv_reg sddatalen:11;			/* 21. */
	dv_reg reserved_sd_blocksize:21;

	dv_reg hw_sd_hcsd_en:1;			/* 22. */
	dv_reg hw_sd_dma_type:2;
#define SP_DMA_TYPE_SINGLEBLOCK_CMD	1
#define SP_DMA_TYPE_MULTIBLOCK_CMD		2
	dv_reg hw_sd_cmd13_en:1;
	dv_reg hwsd_stb_en:1;			/* ?? */
	dv_reg cmd13_rsp_cnt:11;
	dv_reg reserved_sd_hwdma_config:16;

	dv_reg sdrsptmr:11;				/* 23. */
	dv_reg sd_high_speed_en:1;
	dv_reg sd_wr_dly_sel:3;
	dv_reg reserved_sd_timing_config0:17;

	dv_reg sdcrctmr:11;				/* 24. */
	dv_reg reserved_sdcrctmr:2;
	dv_reg sd_rd_dly_sel:3;
	dv_reg reserved_sd_timing_config1:16;

	dv_reg sdpiodatatx;				/* 25. 16 bits, the write buffer always 2 bytes. */
	dv_reg sdpiodatarx;				/* 26. when rx4b_en=1, read 4 bytes from this register. When rx4b_en=0, read 2 bytes. Little endian. */
#define SP_SD_CMDBUF_SIZE 5
	dv_reg sd_cmdbuf[SP_SD_CMDBUF_SIZE];	/* 27-31. 8 bits. the first bit is the start bit. So the command should be cmd|0x40 */
	/* total 5 bytes, and the controller will append the crc7. Overall 6 bytes (48 bits) for the command */

	/*  group 2 */
	union {
		struct {
			dv_reg sdrspbuf3:8;				/* 0. */
			dv_reg sdrspbuf2:8;
			dv_reg sdrspbuf1:8;
			dv_reg sdrspbuf0:8;

			dv_reg sdrspbuf5:8;				/* 1. */
			dv_reg sdrspbuf4:8;
			dv_reg reserved_sd_rspbuf:16;
		};
		dv_reg sd_rspbuf[2];
	};

	dv_reg sdcrc16evenbuf0:16;		/* 2. */
	dv_reg sdcrc16evenbuf4:16;

	dv_reg sdcrc16evenbuf1:16;		/* 3. */
	dv_reg sdcrc16evenbuf5:16;

	dv_reg sdcrc16evenbuf2:16;		/* 4. */
	dv_reg sdcrc16evenbuf6:16;

	dv_reg sdcrc16evenbuf3:16;		/* 5. */
	dv_reg sdcrc16evenbuf7:16;

	dv_reg reserved0_sd_crc7buf:1;	/* 6. */
	dv_reg sdcrc7buf:7;
	dv_reg reserved1_sd_crc7buf:24;

	dv_reg sdcrc16buf0:16;			/* 7. */
	dv_reg sdcrc16buf4:16;

	dv_reg hwsd_sm:10;				/* 8. */
	dv_reg reserved_sd_hw_state:22;

	dv_reg sdcrc16buf1:16;			/* 9. */
	dv_reg sdcrc16buf5:16;

	dv_reg hwsd_cmd13_rca:16;		/* 10. */
	dv_reg reserved_sd_hw_cmd13_rca:16;

	dv_reg sdcrc16buf2:16;			/* 11. */
	dv_reg sdcrc16buf6:16;

	dv_reg tx_dummy_num:9;			/* 12. */
	dv_reg reserved_sd_tx_dummy_num:23;

	dv_reg sdcrc16buf3:16;			/* 13. */
	dv_reg sdcrc16buf7:16;

	dv_reg sd_clk_dly_sel:3;			/* 14. */
#define SP_SD_RXDATTMR_MAX	((1 << 29) - 1)
	dv_reg sdrxdattmr_sel:29;

	dv_reg reserved_reserved_2_15;		/* 15. */

	dv_reg mspiomode:1;				/* 16. */
	dv_reg reserved0_ms_piodmarst:3;
	dv_reg msreset:1;
	dv_reg mscrcrst:1;
	dv_reg msclrerr:1;
	dv_reg reserved1_ms_piodmarst:25;

	/* the following ms register not yet use in sd. */
	dv_reg mscommand:4;					/* 17. */
	dv_reg datasize:4;
	dv_reg reserved_ms_cmd:24;

	dv_reg reserved_reserved_2_18;		/* 18. */

	dv_reg hwms_sm:10;
	dv_reg reserved_hw_hw_state:22;		/* 19. */

	dv_reg msspeed:8;					/* 20. */
	dv_reg msdatwd:1;
	dv_reg msspeed8:1;
	dv_reg mstype:1;
	dv_reg ms_rdy_chk3_en:1;
	dv_reg ms_wdat_sel:3;
	dv_reg reserved_ms_modespeed:17;

	dv_reg ms_busy_rdy_timer:5;			/* 21. */
	dv_reg reserved0_ms_timeout:3;
	dv_reg ms_clk_dly_sel:3;
	dv_reg ms_rdat_dly_sel:3;
	dv_reg reserved_ms_timeout:18;

	dv_reg msstate:8;					/* 22. */
	dv_reg reserved_ms_state:24;

	dv_reg reserved0_ms_status:1;		/* 23. */
	dv_reg mserror_busy_rdy_timeout:1;
	dv_reg msbs:1;
	dv_reg reserved1_ms_status:1;
	dv_reg ms_data_in:4;
	dv_reg reserved2_ms_status:24;

#define MS_RDDATA_SIZE 4
	dv_reg ms_rddata[MS_RDDATA_SIZE];	/* 24-27. 8 bits */

	dv_reg mscrc16buf_low:8;				/* 28. */
	dv_reg reserved_ms_crcbuf0:24;

	dv_reg mscrc16buf_high:8;			/* 29. */
	dv_reg reserved_ms_crcbuf1:24;

	dv_reg mserror:1;					/* 30. */
	dv_reg mscrc16cor:1;
	dv_reg reserved_mccrcerror:30;

	dv_reg mspiordy:1;					/* 31. */
	dv_reg reserved_ms_piordy:31;

	/*  group 3 */
#define SP_MS_WD_DATA_SIZE 16
	dv_reg ms_wd_data[SP_MS_WD_DATA_SIZE];
	dv_reg reserved_ms[16];

	/*  group 4 */
	dv_reg dmadata:16;					/* 0. */
	dv_reg reserved_dma_data:16;

	dv_reg dmasrc:3;					/* 1. */
	dv_reg reserved0_dma_data:1;
	dv_reg dmadst:3;
	dv_reg reserved1_dma_data:25;

	dv_reg dmasize:11;					/* 2. */
	dv_reg reserved_dma_size:21;

	dv_reg stop_dma_flag:1;				/* 3. */
	dv_reg hw_dma_rst:1;
	dv_reg rst_cnad:1;					/* When set this bit to 1, wait it change back to 0 */
	dv_reg reserved_dma_hw_stop_rst:29;
	dv_reg dmaidle:1;					/* 4. */
#define SP_DMA_NORMAL_STATE			0
#define SP_RESET_DMA_OPERATION		1
	dv_reg reserved_dma_ctrl:31;

	dv_reg dma_base_addr_low:16;		/* 5. */
	dv_reg reserved_dma_base_addr_low:16;

	dv_reg dma_base_addr_high:16;		/* 6. */
	dv_reg reserved_dma_base_addr_high:16;

	dv_reg hw_dma_en:1;					/* 7. */
	dv_reg hw_block_mode_en:1;
#define SP_DMA_BLOCK_MODE_PAGE_LEVEL	0
#define SP_DMA_BLOCK_MODE_BLOCK_LEVEL	1

	dv_reg reserved_dma_hw_en:30;


	dv_reg block0_hw_page_addr_low:16;	/* 8. */
	dv_reg reserved_block0_hw_page_addr_low:16;

	dv_reg block0_hw_page_addr_high:16;	/* 9. */
	dv_reg reserved_block0_hw_page_addr_high:16;

	dv_reg block1_hw_page_addr_low:16;	/* 10. */
	dv_reg reserved_block1_hw_page_addr_low:16;

	dv_reg block1_hw_page_addr_high:16;	/* 11. */
	dv_reg reserved_block1_hw_page_addr_high:16;

	dv_reg block2_hw_page_addr_low:16;	/* 12. */
	dv_reg reserved_block2_hw_page_addr_low:16;

	dv_reg block2_hw_page_addr_high:16;	/* 13. */
	dv_reg reserved_block2_hw_page_addr_high:16;

	dv_reg block3_hw_page_addr_low:16;	/* 14. */
	dv_reg reserved_block3_hw_page_addr_low:16;

	dv_reg block3_hw_page_addr_high:16;	/* 15. */
	dv_reg reserved_block3_hw_page_addr_high:16;

#define SP_DMA_HW_PAGE_NUM_SIZE 4
	dv_reg dma_hw_page_num[SP_DMA_HW_PAGE_NUM_SIZE];	/* 16-19. 16 bits the actual page number is x+1 */

	dv_reg hw_block_num:2;				/* 20. */
	dv_reg hw_blcok_cnt:2;
	dv_reg reserved_dma_hw_block_num:28;

	dv_reg dmastart:1;					/* 21. */
	dv_reg reserved_dma_start:31;

	dv_reg hw_page_cnt;					/* 22. 16 bits */

	dv_reg dma_cmp:1;					/* 23. */
	dv_reg reserved_dma_cmp:31;

	dv_reg dmacmpen_interrupt:1;		/* 24. */
	dv_reg reserved_dma_int_en:31;

	dv_reg reserved_reserved_4_25;		/* 25. */
	dv_reg hw_wait_num_low;				/* 26. 16 bits */
	dv_reg hw_wait_num_high;			/* 27. 16 bits */
	dv_reg hw_delay_num;				/* 28. 16 bits */

	dv_reg incnt:11;					/* 29. */
	dv_reg outcnt:11;
	dv_reg lmst_sm:2;
	dv_reg reserved_dma_debug:8;

} SDREG;

#define SD_RST_seq(base) 				do { \
		base->sdrst = 1; \
		base->sdcrcrst = 1; \
		base->stop_dma_flag = 1; \
		base->hw_dma_rst = 1; \
		base->dmaidle = SP_DMA_NORMAL_STATE; \
		base->dmaidle = SP_RESET_DMA_OPERATION; \
		base->dmaidle = SP_DMA_NORMAL_STATE; \
	}while(0)

#define BLOCK0_DMA_PARA_SET(base, pageIdx, nrPages)  do { \
		base->block0_hw_page_addr_low = ((pageIdx) & 0x0000ffff); \
		base->block0_hw_page_addr_high = (((pageIdx) >> 16) & 0x0000ffff); \
		base->dma_hw_page_num[0] = ((nrPages) - 1); \
    }while(0)

#define BLOCK1_DMA_PARA_SET(base, pageIdx, nrPages)  do{ \
		base->block1_hw_page_addr_low = ((pageIdx) & 0x0000ffff); \
		base->block1_hw_page_addr_high = (((pageIdx) >> 16) & 0x0000ffff); \
		base->dma_hw_page_num[1] = ((nrPages) - 1); \
    }while(0)

#define BLOCK2_DMA_PARA_SET(base, pageIdx, nrPages)  do{ \
		base->block2_hw_page_addr_low = ((pageIdx) & 0x0000ffff); \
		base->block2_hw_page_addr_high = (((pageIdx) >> 16) & 0x0000ffff); \
		base->dma_hw_page_num[2] = ((nrPages) - 1); \
    }while(0)

#define BLOCK3_DMA_PARA_SET(base, pageIdx, nrPages)  do{ \
		base->block3_hw_page_addr_low = ((pageIdx) & 0x0000ffff); \
		base->block3_hw_page_addr_high = (((pageIdx) >> 16) & 0x0000ffff); \
		base->dma_hw_page_num[3] = ((nrPages) - 1); \
    }while(0)

#define SET_HW_DMA_BASE_ADDR(base, addr)  do{ \
		base->dma_base_addr_low = ((u32)(addr) & 0xffff); \
		base->dma_base_addr_high =  (((u32)(addr) >> 16) & 0xffff); \
	}while(0)

#define DMA_RESET(base)		do{ \
			base->dmaidle = SP_RESET_DMA_OPERATION; \
			base->dmaidle = SP_DMA_NORMAL_STATE; \
			base->hw_dma_rst = 1; \
			base->dma_hw_page_num[0] = 0; \
			base->dma_hw_page_num[1] = 0; \
			base->dma_hw_page_num[2] = 0; \
			base->dma_hw_page_num[3] = 0; \
			base->hw_block_num = 0; \
		}while(0)

#define DMASIZE_SET(base, x)			(base->dmasize = ((x)-1))
#define SDDATALEN_SET(base, x)			(base->sddatalen = ((x)-1))
#define SDDATALEN_GET(base)				(base->sddatalen + 1)
#define HWSD_TIMEOUT(base)				((base->sdstatus >> 7) & 0x01)

#define SD_PAGE_NUM_SET(base, x)		(base->sd_page_num = ((x)-1))
#define SD_PAGE_NUM_GET(base)			(base->sd_page_num +1)

typedef struct spsdhost {
	volatile struct spsd_general_regs *base;
	uint id;
	char *name;
	struct mmc_host *mmc;
	struct mmc_request *mrq;
	struct platform_device *pdev;
	struct semaphore req_sem;

	int dma_sgcount;   /* Used to store dma_mapped sg count */
	int irq;
	uint cd_gpio; /* gpio used for card sense */
	int cd_state;

	uint wrdly;
	uint rddly;
	int need_tune_cmd_timing;
	int need_tune_dat_timing;
	int power_state;
}SPSDHOST;

typedef struct spsdv2_dridata {
	uint id;
} spsdv2_dridata_t;

#endif //#ifndef _SPSDV2_H_
