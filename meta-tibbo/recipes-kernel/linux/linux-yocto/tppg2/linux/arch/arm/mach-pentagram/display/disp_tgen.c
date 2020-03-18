/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2018 by Sunplus Inc.                             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Inc. All rights are reserved by Sunplus Inc.                          *
 *  This software may only be used in accordance with the                 *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Inc. reserves the right to modify this software               *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus Inc.                                                          *
 *  19, Innovation First Road, Hsinchu Science Park                       *
 *  Hsinchu City 30078, Taiwan, R.O.C.                                    *
 *                                                                        *
 **************************************************************************/
/**
 * @file disp_tgen.c
 * @brief 
 * @author PoChou Chen
 */
/*******************************************************************************
 *                         H E A D E R   F I L E S
 *******************************************************************************/
#include "reg_disp.h"
#include "hal_disp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef DEBUG_MSG
	#define DEBUG(fmt, arg...) diag_printf("[%s:%d] "fmt, __FUNCTION__, __LINE__, ##arg)
	#define MSG(fmt, arg...) diag_printf("[%s:%d] "fmt, __FUNCTION__, __LINE__, ##arg)
#else
	#define DEBUG(fmt, arg...)
	#define MSG(fmt, arg...)
#endif
#define ERRDISP(fmt, arg...) diag_printf("[Disp][%s:%d] "fmt, __FUNCTION__, __LINE__, ##arg)
#define WARNING(fmt, arg...) diag_printf("[Disp][%s:%d] "fmt, __FUNCTION__, __LINE__, ##arg)
#define INFO(fmt, arg...) diag_printf("[Disp][%s:%d] "fmt, __FUNCTION__, __LINE__, ##arg)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static volatile DISP_TGEN_REG_t *pTGENReg;
extern DISPLAY_WORKMEM gDispWorkMem;

#ifdef DEBUG_MSG
	char *StrFmt[] = {"480P", "576P", "720P", "1080P","User Mode"};
	char *StrFps[] = {"60Hz", "50Hz", "24Hz"};
#endif

/**************************************************************************
 *             F U N C T I O N    I M P L E M E N T A T I O N S           *
 **************************************************************************/
void DRV_TGEN_Init(void *pInHWReg)
{
	pTGENReg = (DISP_TGEN_REG_t *)pInHWReg;

	pTGENReg->tgen_config = 0x0007;		// latch mode on
	pTGENReg->tgen_source_sel = 0x0;
	pTGENReg->tgen_user_int2_config = 400;
}

void DRV_TGEN_GetFmtFps(DRV_VideoFormat_e *fmt, DRV_FrameRate_e *fps)
{
	UINT32 tmp_dtg_config = 0;

	tmp_dtg_config = pTGENReg->tgen_dtg_config;

	if (tmp_dtg_config & 0x1)
	{
		*fmt = DRV_FMT_USER_MODE;
		*fps = DRV_FrameRate_MAX;	//unknown
	}
	else
	{
		*fmt = (tmp_dtg_config >> 8) & 0x7;
		*fps = (tmp_dtg_config >> 4) & 0x3;
	}
}

unsigned int DRV_TGEN_GetLineCntNow(void)
{
	return (pTGENReg->tgen_dtg_status1 & 0xfff);
}

void DRV_TGEN_SetUserInt1(UINT32 count)
{
	pTGENReg->tgen_user_int1_config = count & 0xfff;
}

void DRV_TGEN_SetUserInt2(UINT32 count)
{
	pTGENReg->tgen_user_int2_config = count & 0xfff;
}

DRV_Status_e DRV_TGEN_Set(DRV_SetTGEN_t *SetTGEN)
{
//	DISPLAY_WORKMEM *pDispWorkMem = &gDispWorkMem;

	if (SetTGEN->fmt >= DRV_FMT_MAX)
	{
		ERRDISP("Timing format:%d error\n", SetTGEN->fmt);
		return DRV_ERR_INVALID_PARAM;
	}

	MSG("%s, %s\n", StrFmt[SetTGEN->fmt], StrFps[SetTGEN->fps]);

	if (SetTGEN->fmt == DRV_FMT_USER_MODE)
	{
		// [TODO] get clock value

		pTGENReg->tgen_dtg_config = 0x0001;

		pTGENReg->tgen_dtg_total_pixel = SetTGEN->htt;		// total pixel
		pTGENReg->tgen_dtg_total_line = SetTGEN->vtt;
		pTGENReg->tgen_dtg_start_line = SetTGEN->v_bp;
		pTGENReg->tgen_dtg_ds_line_start_cd_point = SetTGEN->hactive;
		pTGENReg->tgen_dtg_field_end_line = SetTGEN->vactive + SetTGEN->v_bp + 1;
		ERRDISP("htt:%d, vtt:%d, h:%d, v:%d, bp:%d\n", SetTGEN->htt, SetTGEN->vtt, SetTGEN->hactive, SetTGEN->vactive, SetTGEN->v_bp);
#if 0
		pDispWorkMem->TCON_BP = backproch;

		DRV_OSD_SetBackPorch(DRV_OSD0, backproch);
		DRV_OSD_SetBackPorch(DRV_OSD2, backproch);
		DRV_DMIX_Set_BP(gMAIN_DISP, backproch);
		DRV_VPP_SetBP(DRV_VPP_MODULE_0 ,backproch);
#endif

		//pDispWorkMem->curr_panel_fps = SetTGEN->fps;
	}
	else
	{
		// [Todo] Moon register setting for pll
		pTGENReg->tgen_dtg_config = ((SetTGEN->fmt & 0x7) << 8) | ((SetTGEN->fps & 0x3) << 4);

#if 0
		pDispWorkMem->DispOutMode.fps = SetTGEN->fps;
		if (SetTGEN->fps == DRV_FrameRate_50Hz)
		{
			DRV_OSD_SetBackPorch(DRV_OSD1, 44-1);
			DRV_VPP_SetBP(DRV_VPP_MODULE_1, 44-1);
		}
		else
		{
			DRV_OSD_SetBackPorch(DRV_OSD1, 39-1);
			DRV_VPP_SetBP(DRV_VPP_MODULE_1, 39-1);
		}
#endif
	}

	return DRV_SUCCESS;
}

void DRV_TGEN_Get(DRV_SetTGEN_t *GetTGEN)
{
	UINT32 tmp;

	tmp = pTGENReg->tgen_dtg_config;

	GetTGEN->fps = (tmp >> 4) & 0x3;
	GetTGEN->fmt = (tmp & 0x1) ? DRV_FMT_USER_MODE:(tmp >> 8) & 0x7;

	DEBUG("%s %s\n", StrFmt[GetTGEN->fmt], StrFps[GetTGEN->fps]);
}

void DRV_TGEN_Reset(void)
{
	pTGENReg->tgen_reset |= 0x1;
}

DRV_Status_e DRV_TGEN_Adjust(DRV_TGEN_Input_e Input, UINT32 Adjust)
{
	switch(Input)
	{
		case DRV_TGEN_VPP0:
			pTGENReg->tgen_dtg_adjust1 = (pTGENReg->tgen_dtg_adjust1 & ~(0x3F<<8)) | ((Adjust & 0x3F) << 8);
			break;
		case DRV_TGEN_OSD0:
			pTGENReg->tgen_dtg_adjust3 = (pTGENReg->tgen_dtg_adjust3 & ~(0x3F<<0)) | ((Adjust & 0x3F) << 0);
			break;
		case DRV_TGEN_PTG:
			pTGENReg->tgen_dtg_adjust4 = (pTGENReg->tgen_dtg_adjust4 & ~(0x3F<<8)) | ((Adjust & 0x3F) << 8);
			break;
		default:
			ERRDISP("Invalidate Input %d \n", Input);
			return DRV_ERR_INVALID_PARAM;
	}

	return DRV_SUCCESS;
}

