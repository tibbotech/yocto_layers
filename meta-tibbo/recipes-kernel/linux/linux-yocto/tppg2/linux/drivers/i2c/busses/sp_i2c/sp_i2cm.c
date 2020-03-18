#include <linux/delay.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/rtc.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include "hal_i2c.h"
#include "sp_i2c.h"


//#define I2C_FUNC_DEBUG
#define I2C_DBG_INFO
#define I2C_DBG_ERR

#ifdef I2C_FUNC_DEBUG
	#define FUNC_DEBUG()    printk(KERN_INFO "[I2C] Debug: %s(%d)\n", __FUNCTION__, __LINE__)
#else
	#define FUNC_DEBUG()
#endif

#ifdef I2C_DBG_INFO
	#define DBG_INFO(fmt, args ...)    printk(KERN_INFO "[I2C] Info: " fmt, ## args)
#else
	#define DBG_INFO(fmt, args ...)
#endif

#ifdef I2C_DBG_ERR
	#define DBG_ERR(fmt, args ...)    printk(KERN_ERR "[I2C] Error: " fmt, ## args)
#else
	#define DBG_ERR(fmt, args ...)
#endif

#define I2C_FREQ             100
#define I2C_SLEEP_TIMEOUT    200
#define I2C_SCL_DELAY        0  //SCl dalay xT

#define I2CM_REG_NAME        "i2cm"
#define MOON0_REG_NAME       "moon0"
#define MOON3_REG_NAME       "moon3"

#define DEVICE_NAME          "sp_i2cm"

//burst write use
#define I2C_EMPTY_THRESHOLD_VALUE    4

//burst read use
#define I2C_IS_READ16BYTE

#ifdef I2C_IS_READ16BYTE
#define I2C_BURST_RDATA_BYTES        16
#define I2C_BURST_RDATA_FLAG         0x80008000
#else
#define I2C_BURST_RDATA_BYTES        4
#define I2C_BURST_RDATA_FLAG         0x88888888
#endif

#define I2C_BURST_RDATA_ALL_FLAG     0xFFFFFFFF


typedef struct SpI2C_If_t_ {
	struct i2c_msg *msgs;  /* messages currently handled */
	struct i2c_adapter adap;
	I2C_Cmd_t stCmdInfo;
	void __iomem *i2c_regs;
	int irq;
} SpI2C_If_t;

typedef struct Moon_RegBase_t_ {
	void __iomem *moon0_regs;
	void __iomem *moon3_regs;
} Moon_RegBase_t;

static Moon_RegBase_t stMoonRegBase;
static SpI2C_If_t stSpI2CInfo[I2C_MASTER_NUM];
static I2C_Irq_Event_t stIrqEvent[I2C_MASTER_NUM];
wait_queue_head_t i2cm_event_wait[I2C_MASTER_NUM];


static void _sp_i2cm_intflag_check(unsigned int device_id, I2C_Irq_Event_t *pstIrqEvent)
{
	unsigned int int_flag = 0;
	unsigned int overflow_flag = 0;

	hal_i2cm_status_get(device_id, &int_flag);
	//printk("I2C int_flag = 0x%x !!\n", int_flag);
	if (int_flag & I2C_INT_DONE_FLAG) {
		DBG_INFO("I2C is done !!\n");
		pstIrqEvent->stIrqFlag.bActiveDone = 1;
	} else {
		pstIrqEvent->stIrqFlag.bActiveDone = 0;
	}

	if (int_flag & I2C_INT_ADDRESS_NACK_FLAG) {
		DBG_INFO("I2C slave address NACK !!\n");
		pstIrqEvent->stIrqFlag.bAddrNack = 1;
	} else {
		pstIrqEvent->stIrqFlag.bAddrNack = 0;
	}

	if (int_flag & I2C_INT_DATA_NACK_FLAG) {
		DBG_INFO("I2C slave data NACK !!\n");
		pstIrqEvent->stIrqFlag.bDataNack = 1;
	} else {
		pstIrqEvent->stIrqFlag.bDataNack = 0;
	}

	// write use
	if (int_flag & I2C_INT_EMPTY_THRESHOLD_FLAG) {
		//DBG_INFO("I2C empty threshold occur !!\n");
		pstIrqEvent->stIrqFlag.bEmptyThreshold = 1;
	} else {
		pstIrqEvent->stIrqFlag.bEmptyThreshold = 0;
	}

	// write use
	if (int_flag & I2C_INT_EMPTY_FLAG) {
		DBG_INFO("I2C FIFO empty occur !!\n");
		pstIrqEvent->stIrqFlag.bFiFoEmpty = 1;
	} else {
		pstIrqEvent->stIrqFlag.bFiFoEmpty = 0;
	}

	// write use (for debug)
	if (int_flag & I2C_INT_FULL_FLAG) {
		DBG_INFO("I2C FIFO full occur !!\n");
		pstIrqEvent->stIrqFlag.bFiFoFull = 1;
	} else {
		pstIrqEvent->stIrqFlag.bFiFoFull = 0;
	}

	if (int_flag & I2C_INT_SCL_HOLD_TOO_LONG_FLAG) {
		DBG_INFO("I2C SCL hold too long occur !!\n");
		pstIrqEvent->stIrqFlag.bSCLHoldTooLong = 1;
	} else {
		pstIrqEvent->stIrqFlag.bSCLHoldTooLong = 0;
	}
	hal_i2cm_status_clear(device_id, I2C_CTL1_ALL_CLR);

	// read use
	hal_i2cm_roverflow_flag_get(device_id, &overflow_flag);
	if (overflow_flag) {
		DBG_ERR("I2C burst read data overflow !! overflow_flag = 0x%x\n", overflow_flag);
		pstIrqEvent->stIrqFlag.bRdOverflow = 1;
	} else {
		pstIrqEvent->stIrqFlag.bRdOverflow = 0;
	}
}

static irqreturn_t _sp_i2cm_irqevent_handler(int irq, void *args)
{
	I2C_Irq_Event_t *pstIrqEvent = (I2C_Irq_Event_t *)args;
	unsigned int device_id = 0;
	unsigned char w_data[32] = {0};
	unsigned char r_data[I2C_BURST_RDATA_BYTES] = {0};
	unsigned int rdata_flag = 0;
	unsigned int bit_index = 0;
	int i = 0, j = 0, k = 0;

	device_id = pstIrqEvent->dDevId;
	_sp_i2cm_intflag_check(device_id, pstIrqEvent);

	switch (pstIrqEvent->eRWState) {
		case I2C_WRITE_STATE:
			if (pstIrqEvent->stIrqFlag.bActiveDone) {
				DBG_INFO("I2C write success !!\n");
				pstIrqEvent->bRet = I2C_SUCCESS;
				wake_up(&i2cm_event_wait[device_id]);
			} else if (pstIrqEvent->stIrqFlag.bAddrNack || pstIrqEvent->stIrqFlag.bDataNack) {
				DBG_ERR("I2C reveive NACK !!\n");
				pstIrqEvent->bRet = I2C_ERR_RECEIVE_NACK;
				pstIrqEvent->stIrqFlag.bActiveDone = 1;
				wake_up(&i2cm_event_wait[device_id]);
			} else if (pstIrqEvent->stIrqFlag.bSCLHoldTooLong) {
				DBG_ERR("I2C SCL hold too long !!\n");
				pstIrqEvent->bRet = I2C_ERR_SCL_HOLD_TOO_LONG;
				pstIrqEvent->stIrqFlag.bActiveDone = 1;
				wake_up(&i2cm_event_wait[device_id]);
			} else if (pstIrqEvent->stIrqFlag.bFiFoEmpty) {
				DBG_ERR("I2C FIFO empty !!\n");
				pstIrqEvent->bRet = I2C_ERR_FIFO_EMPTY;
				pstIrqEvent->stIrqFlag.bActiveDone = 1;
				wake_up(&i2cm_event_wait[device_id]);
			} else if (pstIrqEvent->dBurstCount > 0) {
				if (pstIrqEvent->stIrqFlag.bEmptyThreshold) {
					for (i = 0; i < I2C_EMPTY_THRESHOLD_VALUE; i++) {
						for (j = 0; j < 4; j++) {
							if (pstIrqEvent->dDataIndex >= pstIrqEvent->dDataTotalLen) {
								w_data[j] = 0;
							} else {
								w_data[j] = pstIrqEvent->pDataBuf[pstIrqEvent->dDataIndex];
							}
							pstIrqEvent->dDataIndex++;
						}
						hal_i2cm_data0_set(device_id, (unsigned int *)w_data);
						pstIrqEvent->dBurstCount--;
						
						if (pstIrqEvent->dBurstCount == 0) {
							hal_i2cm_int_en0_disable(device_id, (I2C_EN0_EMPTY_THRESHOLD_INT | I2C_EN0_EMPTY_INT));
							break;
						}
					}
					hal_i2cm_status_clear(device_id, I2C_CTL1_EMPTY_THRESHOLD_CLR);
				}
			}
			break;

		case I2C_READ_STATE:
			if (pstIrqEvent->stIrqFlag.bAddrNack || pstIrqEvent->stIrqFlag.bDataNack) {
				DBG_ERR("I2C reveive NACK !!\n");
				pstIrqEvent->bRet = I2C_ERR_RECEIVE_NACK;
				pstIrqEvent->stIrqFlag.bActiveDone = 1;
				wake_up(&i2cm_event_wait[device_id]);
			} else if (pstIrqEvent->stIrqFlag.bSCLHoldTooLong) {
				DBG_ERR("I2C SCL hold too long !!\n");
				pstIrqEvent->bRet = I2C_ERR_SCL_HOLD_TOO_LONG;
				pstIrqEvent->stIrqFlag.bActiveDone = 1;
				wake_up(&i2cm_event_wait[device_id]);
			} else if (pstIrqEvent->stIrqFlag.bRdOverflow) {
				DBG_ERR("I2C read data overflow !!\n");
				pstIrqEvent->bRet = I2C_ERR_RDATA_OVERFLOW;
				pstIrqEvent->stIrqFlag.bActiveDone = 1;
				wake_up(&i2cm_event_wait[device_id]);
			} else {
				if (pstIrqEvent->dBurstCount > 0) {
					hal_i2cm_rdata_flag_get(device_id, &rdata_flag);
					for (i = 0; i < (32 / I2C_BURST_RDATA_BYTES); i++) {
						bit_index = (I2C_BURST_RDATA_BYTES - 1) + (I2C_BURST_RDATA_BYTES * i);
						if (rdata_flag & (1 << bit_index)) {
							for (j = 0; j < (I2C_BURST_RDATA_BYTES / 4); j++) {
								k = pstIrqEvent->dRegDataIndex + j;
								if (k >= 8) {
									k -= 8;
								}

								hal_i2cm_data_get(device_id, k, (unsigned int *)(&pstIrqEvent->pDataBuf[pstIrqEvent->dDataIndex]));
								pstIrqEvent->dDataIndex += 4;
							}
							hal_i2cm_rdata_flag_clear(device_id, (((1 << I2C_BURST_RDATA_BYTES) - 1) << (I2C_BURST_RDATA_BYTES * i)));
							pstIrqEvent->dRegDataIndex += (I2C_BURST_RDATA_BYTES / 4);
							if (pstIrqEvent->dRegDataIndex >= 8) {
								pstIrqEvent->dRegDataIndex -= 8;
							}
							pstIrqEvent->dBurstCount --;
						}
					}
				}

				if (pstIrqEvent->stIrqFlag.bActiveDone) {
					if (pstIrqEvent->dBurstRemainder) {
						j = 0;
						for (i = 0; i < (I2C_BURST_RDATA_BYTES / 4); i++) {
							k = pstIrqEvent->dRegDataIndex + i;
							if (k >= 8) {
								k -= 8;
							}

							hal_i2cm_data_get(device_id, k, (unsigned int *)(&r_data[j]));
							j += 4;
						}

						for (i = 0; i < pstIrqEvent->dBurstRemainder; i++) {
							pstIrqEvent->pDataBuf[pstIrqEvent->dDataIndex + i] = r_data[i];
						}
					}

					DBG_INFO("I2C read success !!\n");
					pstIrqEvent->bRet = I2C_SUCCESS;
					wake_up(&i2cm_event_wait[device_id]);
				}
			}
			break;

		default:
			break;
	}//switch case

	return IRQ_HANDLED;
}

static void _sp_i2cm_init_irqevent(unsigned int device_id)
{
	I2C_Irq_Event_t *pstIrqEvent = NULL;

	FUNC_DEBUG();

	pstIrqEvent = &stIrqEvent[device_id];
	memset(pstIrqEvent, 0, sizeof(I2C_Irq_Event_t));

	switch(device_id) {
		case 0:
			pstIrqEvent->dDevId = 0;
			break;

		case 1:
			pstIrqEvent->dDevId = 1;
			break;

		default:
			break;
	}
}

static int _sp_i2cm_init(unsigned int device_id, SpI2C_If_t *pstSpI2CInfo)
{
	Moon_RegBase_t *pstMoonRegBase = &stMoonRegBase;

	FUNC_DEBUG();

	if (device_id >= I2C_MASTER_NUM)
	{
		DBG_ERR("I2C device id is not correct !! device_id=%d\n", device_id);
		return I2C_ERR_INVALID_DEVID;
	}

	hal_i2cm_pinmux_set(device_id, pstMoonRegBase->moon3_regs);
	hal_i2cm_enable(device_id, pstMoonRegBase->moon0_regs);
	hal_i2cm_base_set(device_id, pstSpI2CInfo->i2c_regs);
	hal_i2cm_reset(device_id);

	_sp_i2cm_init_irqevent(device_id);
	init_waitqueue_head(&i2cm_event_wait[device_id]);

	return I2C_SUCCESS;
}

static int _sp_i2cm_get_irq(struct platform_device *pdev, SpI2C_If_t *pstSpI2CInfo)
{
	int irq;

	FUNC_DEBUG();

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		DBG_ERR("[I2C adapter] get irq number fail, irq = %d\n", irq);
		return -ENODEV;
	}

	pstSpI2CInfo->irq = irq;
	return I2C_SUCCESS;
}

static int _sp_i2cm_get_register_base(struct platform_device *pdev, unsigned int *membase, const char *res_name)
{
	struct resource *r;
	void __iomem *p;

	FUNC_DEBUG();
	DBG_INFO("[I2C adapter] register name  : %s!!\n", res_name);

	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, res_name);
	if(r == NULL) {
		DBG_INFO("[I2C adapter] platform_get_resource_byname fail\n");
		return -ENODEV;
	}

	p = devm_ioremap_resource(&pdev->dev, r);
	if (IS_ERR(p)) {
		DBG_ERR("[I2C adapter] ioremap fail\n");
		return PTR_ERR(p);
	}

	DBG_INFO("[I2C adapter] ioremap addr : 0x%x!!\n", (unsigned int)p);
	*membase = (unsigned int)p;

	return I2C_SUCCESS;
}

static int _sp_i2cm_get_moon_resources(struct platform_device *pdev, Moon_RegBase_t *pstMoonRegBase)
{
	int ret;
	unsigned int membase = 0;

	FUNC_DEBUG();

	ret = _sp_i2cm_get_register_base(pdev, &membase, MOON0_REG_NAME);
	if (ret) {
		DBG_ERR("[I2C adapter] %s (%d) ret = %d\n", __FUNCTION__, __LINE__, ret);
		return ret;
	} else {
		pstMoonRegBase->moon0_regs = (void __iomem *)membase;
	}

	ret = _sp_i2cm_get_register_base(pdev, &membase, MOON3_REG_NAME);
	if (ret) {
		DBG_ERR("[I2C adapter] %s (%d) ret = %d\n", __FUNCTION__, __LINE__, ret);
		return ret;
	} else {
		pstMoonRegBase->moon3_regs = (void __iomem *)membase;
	}

	return I2C_SUCCESS;
}

static int _sp_i2cm_get_resources(struct platform_device *pdev, SpI2C_If_t *pstSpI2CInfo)
{
	int ret;
	unsigned int membase = 0;

	FUNC_DEBUG();

	ret = _sp_i2cm_get_register_base(pdev, &membase, I2CM_REG_NAME);
	if (ret) {
		DBG_ERR("[I2C adapter] %s (%d) ret = %d\n", __FUNCTION__, __LINE__, ret);
		return ret;
	}
	else {
		pstSpI2CInfo->i2c_regs = (void __iomem *)membase;
	}

	ret = _sp_i2cm_get_irq(pdev, pstSpI2CInfo);
	if (ret) {
		DBG_ERR("[I2C adapter] %s (%d) ret = %d\n", __FUNCTION__, __LINE__, ret);
		return ret;
	}

	return I2C_SUCCESS;
}

int sp_i2cm_read(I2C_Cmd_t *pstCmdInfo)
{
	I2C_Irq_Event_t *pstIrqEvent = NULL;
	unsigned char w_data[32] = {0};
	unsigned int read_cnt = 0;
	unsigned int write_cnt = 0;
	unsigned int burst_cnt = 0, burst_r = 0;
	unsigned int int0 = 0, int1 = 0, int2 = 0;
	int ret = I2C_SUCCESS;
	int i = 0;

	FUNC_DEBUG();

	if (pstCmdInfo->dDevId < I2C_MASTER_NUM) {
		pstIrqEvent = &stIrqEvent[pstCmdInfo->dDevId];
	} else {
		return I2C_ERR_INVALID_DEVID;
	}

	if (pstIrqEvent->bI2CBusy) {
		DBG_ERR("I2C is busy !!\n");
		return I2C_ERR_I2C_BUSY;
	}
	_sp_i2cm_init_irqevent(pstCmdInfo->dDevId);
	pstIrqEvent->bI2CBusy = 1;

	write_cnt = pstCmdInfo->dWrDataCnt;
	read_cnt = pstCmdInfo->dRdDataCnt;

	if (pstCmdInfo->dRestartEn)
	{
		if ((write_cnt > 32) || (write_cnt == 0)) {
			pstIrqEvent->bI2CBusy = 0;
			DBG_ERR("I2C write count is invalid !! write count=%d\n", write_cnt);
			return I2C_ERR_INVALID_CNT;
		}
	}

	if ((read_cnt > 0xFFFF)  || (read_cnt == 0)) {
		pstIrqEvent->bI2CBusy = 0;
		DBG_ERR("I2C read count is invalid !! read count=%d\n", read_cnt);
		return I2C_ERR_INVALID_CNT;
	}

	burst_cnt = read_cnt / I2C_BURST_RDATA_BYTES;
	burst_r = read_cnt % I2C_BURST_RDATA_BYTES;
	DBG_INFO("write_cnt = %d, read_cnt = %d, burst_cnt = %d, burst_r = %d\n",
			write_cnt, read_cnt, burst_cnt, burst_r);

	int0 = (I2C_EN0_SCL_HOLD_TOO_LONG_INT | I2C_EN0_EMPTY_INT | I2C_EN0_DATA_NACK_INT
			| I2C_EN0_ADDRESS_NACK_INT | I2C_EN0_DONE_INT );
	if (burst_cnt) {
		int1 = I2C_BURST_RDATA_FLAG;
		int2 = I2C_BURST_RDATA_ALL_FLAG;
	}

	pstIrqEvent->eRWState = I2C_READ_STATE;
	pstIrqEvent->dBurstCount = burst_cnt;
	pstIrqEvent->dBurstRemainder = burst_r;
	pstIrqEvent->dDataIndex = 0;
	pstIrqEvent->dRegDataIndex = 0;
	pstIrqEvent->dDataTotalLen = read_cnt;
	pstIrqEvent->pDataBuf = pstCmdInfo->pRdData;

	hal_i2cm_reset(pstCmdInfo->dDevId);
	hal_i2cm_clock_freq_set(pstCmdInfo->dDevId, pstCmdInfo->dFreq);
	hal_i2cm_slave_addr_set(pstCmdInfo->dDevId, pstCmdInfo->dSlaveAddr);
	hal_i2cm_scl_delay_set(pstCmdInfo->dDevId, I2C_SCL_DELAY);
	hal_i2cm_trans_cnt_set(pstCmdInfo->dDevId, write_cnt, read_cnt);
	hal_i2cm_active_mode_set(pstCmdInfo->dDevId, I2C_TRIGGER);

	if (pstCmdInfo->dRestartEn) {
		DBG_INFO("I2C_RESTART_MODE\n");
		for (i = 0; i < write_cnt; i++) {
			w_data[i] = pstCmdInfo->pWrData[i];
		}
		hal_i2cm_data_set(pstCmdInfo->dDevId, (unsigned int *)w_data);
		hal_i2cm_rw_mode_set(pstCmdInfo->dDevId, I2C_RESTART_MODE);
	} else {
		DBG_INFO("I2C_READ_MODE\n");
		hal_i2cm_rw_mode_set(pstCmdInfo->dDevId, I2C_READ_MODE);
	}

	hal_i2cm_int_en0_set(pstCmdInfo->dDevId, int0);
	hal_i2cm_int_en1_set(pstCmdInfo->dDevId, int1);
	hal_i2cm_int_en2_set(pstCmdInfo->dDevId, int2);
	hal_i2cm_manual_trigger(pstCmdInfo->dDevId);	//start send data

	ret = wait_event_timeout(i2cm_event_wait[pstCmdInfo->dDevId], pstIrqEvent->stIrqFlag.bActiveDone, (I2C_SLEEP_TIMEOUT * HZ) / 1000);
	if (ret == 0) {
		DBG_ERR("I2C read timeout !!\n");
		ret = I2C_ERR_TIMEOUT_OUT;
	} else {
		ret = pstIrqEvent->bRet;
	}

	pstIrqEvent->eRWState = I2C_IDLE_STATE;
	pstIrqEvent->bI2CBusy = 0;

	return ret;
}

int sp_i2cm_write(I2C_Cmd_t *pstCmdInfo)
{
	I2C_Irq_Event_t *pstIrqEvent = NULL;
	unsigned char w_data[32] = {0};
	unsigned int write_cnt = 0;
	unsigned int burst_cnt = 0;
	unsigned int int0 = 0;
	int ret = I2C_SUCCESS;
	int i = 0;

	FUNC_DEBUG();

	if (pstCmdInfo->dDevId < I2C_MASTER_NUM) {
		pstIrqEvent = &stIrqEvent[pstCmdInfo->dDevId];
	} else {
		return I2C_ERR_INVALID_DEVID;
	}

	if (pstIrqEvent->bI2CBusy) {
		DBG_ERR("I2C is busy !!\n");
		return I2C_ERR_I2C_BUSY;
	}
	_sp_i2cm_init_irqevent(pstCmdInfo->dDevId);
	pstIrqEvent->bI2CBusy = 1;

	write_cnt = pstCmdInfo->dWrDataCnt;

	if ((write_cnt > 0xFFFF) || (write_cnt == 0)) {
		pstIrqEvent->bI2CBusy = 0;
		DBG_ERR("I2C write count is invalid !! write count=%d\n", write_cnt);
		return I2C_ERR_INVALID_CNT;
	}

	if (write_cnt > 32) {
		burst_cnt = (write_cnt - 32) / 4;
		if ((write_cnt - 32) % 4) {
			burst_cnt += 1;
		}

		for (i = 0; i < 32; i++) {
			w_data[i] = pstCmdInfo->pWrData[i];
		}
	} else {
		for(i = 0; i < write_cnt; i++){
			w_data[i] = pstCmdInfo->pWrData[i];
		}
	}
	DBG_INFO("write_cnt = %d, burst_cnt = %d\n", write_cnt, burst_cnt);

	int0 = (I2C_EN0_SCL_HOLD_TOO_LONG_INT | I2C_EN0_EMPTY_INT | I2C_EN0_DATA_NACK_INT
			| I2C_EN0_ADDRESS_NACK_INT | I2C_EN0_DONE_INT );

	if (burst_cnt)
		int0 |= I2C_EN0_EMPTY_THRESHOLD_INT;

	pstIrqEvent->eRWState = I2C_WRITE_STATE;
	pstIrqEvent->dBurstCount = burst_cnt;
	pstIrqEvent->dDataIndex = i;
	pstIrqEvent->dDataTotalLen = write_cnt;
	pstIrqEvent->pDataBuf = pstCmdInfo->pWrData;

	hal_i2cm_reset(pstCmdInfo->dDevId);
	hal_i2cm_clock_freq_set(pstCmdInfo->dDevId, pstCmdInfo->dFreq);
	hal_i2cm_slave_addr_set(pstCmdInfo->dDevId, pstCmdInfo->dSlaveAddr);
	hal_i2cm_scl_delay_set(pstCmdInfo->dDevId, I2C_SCL_DELAY);
	hal_i2cm_trans_cnt_set(pstCmdInfo->dDevId, write_cnt, 0);
	hal_i2cm_active_mode_set(pstCmdInfo->dDevId, I2C_TRIGGER);
	hal_i2cm_rw_mode_set(pstCmdInfo->dDevId, I2C_WRITE_MODE);
	hal_i2cm_data_set(pstCmdInfo->dDevId, (unsigned int *)w_data);

	if (burst_cnt)
		hal_i2cm_int_en0_with_thershold_set(pstCmdInfo->dDevId, int0, I2C_EMPTY_THRESHOLD_VALUE);
	else
	hal_i2cm_int_en0_set(pstCmdInfo->dDevId, int0);

	hal_i2cm_manual_trigger(pstCmdInfo->dDevId);	//start send data

	ret = wait_event_timeout(i2cm_event_wait[pstCmdInfo->dDevId], pstIrqEvent->stIrqFlag.bActiveDone, (I2C_SLEEP_TIMEOUT * HZ) / 20);
	if (ret == 0) {
		DBG_ERR("I2C write timeout !!\n");
		ret = I2C_ERR_TIMEOUT_OUT;
	} else {
		ret = pstIrqEvent->bRet;
	}

	pstIrqEvent->eRWState = I2C_IDLE_STATE;
	pstIrqEvent->bI2CBusy = 0;

	return ret;
}

static int sp_master_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	SpI2C_If_t *pstSpI2CInfo = (SpI2C_If_t *)adap->algo_data;
	I2C_Cmd_t *pstCmdInfo = &(pstSpI2CInfo->stCmdInfo);
	int ret = I2C_SUCCESS;
	int i = 0;

	FUNC_DEBUG();

	if (num == 0) {
		return -EINVAL;
	}

	memset(pstCmdInfo, 0, sizeof(I2C_Cmd_t));
	pstCmdInfo->dDevId = adap->nr;
	pstCmdInfo->dFreq = I2C_FREQ;

	for (i = 0; i < num; i++) {
		if(msgs[i].flags & I2C_M_TEN)
			return -EINVAL;

		pstCmdInfo->dSlaveAddr = msgs[i].addr;

		if(msgs[i].flags & I2C_M_NOSTART){
			pstCmdInfo->dWrDataCnt = msgs[i].len;
			pstCmdInfo->pWrData = msgs[i].buf;
			pstCmdInfo->dRestartEn = 1;
			continue;
		}

		if ( msgs[i].flags & I2C_M_RD) {
			pstCmdInfo->dRdDataCnt = msgs[i].len;
			pstCmdInfo->pRdData = msgs[i].buf;
			ret = sp_i2cm_read(pstCmdInfo);
		} else {
			pstCmdInfo->dWrDataCnt = msgs[i].len;
			pstCmdInfo->pWrData = msgs[i].buf;
			ret = sp_i2cm_write(pstCmdInfo);
		}

		if (ret != I2C_SUCCESS) {
			return -EIO;
		}
	}

	return num;
}

static u32 sp_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm sp_algorithm = {
	.master_xfer	= sp_master_xfer,
	.functionality	= sp_functionality,
};

static int sp_i2c_probe(struct platform_device *pdev)
{
	Moon_RegBase_t *pstMoonRegBase = &stMoonRegBase;
	SpI2C_If_t *pstSpI2CInfo = NULL;
	I2C_Irq_Event_t *pstIrqEvent = NULL;
	struct i2c_adapter *p_adap;
	int device_id = 0;
	int ret = I2C_SUCCESS;

	FUNC_DEBUG();

	if (pdev->dev.of_node) {
		pdev->id = of_alias_get_id(pdev->dev.of_node, "i2c");
		DBG_INFO("[I2C adapter] pdev->id=%d\n", pdev->id);
		device_id = pdev->id;
	}

	if(pdev->id == 0) {
		ret = _sp_i2cm_get_moon_resources(pdev, pstMoonRegBase);
		if (ret) {
			DBG_ERR("[I2C adapter] get moon resources fail !\n");
			return ret;
		}
	}

	pstSpI2CInfo = &stSpI2CInfo[device_id];
	memset(pstSpI2CInfo, 0, sizeof(SpI2C_If_t));

	ret = _sp_i2cm_get_resources(pdev, pstSpI2CInfo);
	if (ret != I2C_SUCCESS) {
		DBG_ERR("[I2C adapter] get resources fail !\n");
		return ret;
	}

	ret = _sp_i2cm_init(device_id, pstSpI2CInfo);
	if (ret != 0) {
		DBG_ERR("[I2C adapter] i2c master %d init error\n", device_id);
		return ret;
	}

	p_adap = &pstSpI2CInfo->adap;
	sprintf(p_adap->name, "%s%d", DEVICE_NAME, device_id);
	p_adap->algo = &sp_algorithm;
	p_adap->algo_data = pstSpI2CInfo;
	p_adap->nr = device_id;
	p_adap->class = 0;
	p_adap->retries = 5;
	p_adap->dev.parent = &pdev->dev;

	ret = i2c_add_numbered_adapter(p_adap);
	if (ret < 0) {
		DBG_ERR("[I2C adapter] error add adapter %s\n", p_adap->name);
	} else {
		DBG_INFO("[I2C adapter] add adapter %s success\n", p_adap->name);
		platform_set_drvdata(pdev, pstSpI2CInfo);
	}

	pstIrqEvent = &stIrqEvent[device_id];
	ret = request_irq(pstSpI2CInfo->irq, _sp_i2cm_irqevent_handler, IRQF_TRIGGER_HIGH, p_adap->name, pstIrqEvent);
	if (ret) {
		DBG_ERR("request irq fail !!\n");
		return I2C_ERR_REQUESET_IRQ;
	}

	return ret;
}

static int sp_i2c_remove(struct platform_device *pdev)
{
	Moon_RegBase_t *pstMoonRegBase = &stMoonRegBase;
	SpI2C_If_t *pstSpI2CInfo = platform_get_drvdata(pdev);
	struct i2c_adapter *p_adap = &pstSpI2CInfo->adap;

	FUNC_DEBUG();

	i2c_del_adapter(p_adap);
	if (p_adap->nr < I2C_MASTER_NUM) {
		hal_i2cm_disable(p_adap->nr, pstMoonRegBase->moon0_regs);
		free_irq(pstSpI2CInfo->irq, NULL);
	}

	return 0;
}

static int sp_i2c_suspend(struct platform_device *pdev, pm_message_t state)
{
	Moon_RegBase_t *pstMoonRegBase = &stMoonRegBase;
	SpI2C_If_t *pstSpI2CInfo = platform_get_drvdata(pdev);
	struct i2c_adapter *p_adap = &pstSpI2CInfo->adap;

	FUNC_DEBUG();

	if (p_adap->nr < I2C_MASTER_NUM) {
		hal_i2cm_disable(p_adap->nr, pstMoonRegBase->moon0_regs);
	}

	return 0;
}

static int sp_i2c_resume(struct platform_device *pdev)
{
	Moon_RegBase_t *pstMoonRegBase = &stMoonRegBase;
	SpI2C_If_t *pstSpI2CInfo = platform_get_drvdata(pdev);
	struct i2c_adapter *p_adap = &pstSpI2CInfo->adap;

	FUNC_DEBUG();

	if (p_adap->nr < I2C_MASTER_NUM) {
		hal_i2cm_enable(p_adap->nr, pstMoonRegBase->moon0_regs);
	}

	return 0;
}

static const struct of_device_id sp_i2c_of_match[] = {
	{ .compatible = "sunplus,sp_i2cm" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, sp_i2c_of_match);

static struct platform_driver sp_i2c_driver = {
	.probe		= sp_i2c_probe,
	.remove		= sp_i2c_remove,
	.suspend	= sp_i2c_suspend,
	.resume		= sp_i2c_resume,
	.driver		= {
		.owner		= THIS_MODULE,
		.name		= DEVICE_NAME,
		.of_match_table = sp_i2c_of_match,
	},
};

static int __init sp_i2c_adap_init(void)
{
	return platform_driver_register(&sp_i2c_driver);
}
module_init(sp_i2c_adap_init);

static void __exit sp_i2c_adap_exit(void)
{
	platform_driver_unregister(&sp_i2c_driver);
}
module_exit(sp_i2c_adap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunplus Technology");
MODULE_DESCRIPTION("Sunplus I2C Master Driver");
