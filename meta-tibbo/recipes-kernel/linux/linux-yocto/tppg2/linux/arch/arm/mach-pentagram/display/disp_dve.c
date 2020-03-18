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
 * @file disp_dve.c
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
static DISP_DVE_REG_t *pDVEReg;
extern DISPLAY_WORKMEM gDispWorkMem;

/**************************************************************************
 *             F U N C T I O N    I M P L E M E N T A T I O N S           *
 **************************************************************************/
void DRV_DVE_Init(void *pInHWReg)
{
	pDVEReg = (DISP_DVE_REG_t *)pInHWReg;

	pDVEReg->color_bar_mode = 0;
	pDVEReg->dve_hdmi_mode_1 = 0x3;
	pDVEReg->dve_hdmi_mode_0 = 0x41;// latch mode on
}

void DRV_DVE_SetMode(int mode)
{
	int colorbarmode = pDVEReg->color_bar_mode & ~0xfe;
	int hdmi_mode = pDVEReg->dve_hdmi_mode_0 & ~0x1f80;

	INFO("dve mode: %d\n", mode);

	switch (mode)
	{
		default:
		case 0:	//480P
			pDVEReg->dve_hdmi_mode_0 = hdmi_mode | (0 << 12) | (0 << 7) | (1 << 8) | (0 << 9);
			pDVEReg->color_bar_mode = colorbarmode | (0 << 3);
			break;
		case 1:	//576P
			pDVEReg->dve_hdmi_mode_0 = hdmi_mode | (0 << 12) | (0 << 7) | (1 << 8) | (1 << 9);
			pDVEReg->color_bar_mode = colorbarmode | (1 << 3);
			break;
		case 2:	//720P60
			pDVEReg->dve_hdmi_mode_0 = hdmi_mode | (0 << 12) | (0 << 7) | (1 << 8) | (6 << 9);
			pDVEReg->color_bar_mode = colorbarmode | (2 << 3);
			break;
		case 3:	//720P50
			pDVEReg->dve_hdmi_mode_0 = hdmi_mode | (0 << 12) | (0 << 7) | (1 << 8) | (7 << 9);
			pDVEReg->color_bar_mode = colorbarmode | (3 << 3);
			break;
		case 4:	//1080P50
			pDVEReg->dve_hdmi_mode_0 = hdmi_mode | (0 << 12) | (0 << 7) | (1 << 8) | (3 << 9);
			pDVEReg->color_bar_mode = colorbarmode | (4 << 3);
			break;
		case 5:	//1080P60
			pDVEReg->dve_hdmi_mode_0 = hdmi_mode | (0 << 12) | (0 << 7) | (1 << 8) | (2 << 9);
			pDVEReg->color_bar_mode = colorbarmode | (5 << 3);
			break;
		case 6:	//1080P24
			pDVEReg->dve_hdmi_mode_0 = hdmi_mode | (1 << 12) | (0 << 7) | (1 << 8) | (2 << 9);
			pDVEReg->color_bar_mode = colorbarmode | (6 << 3);
			break;
		case 7:	//user mode
			pDVEReg->color_bar_mode = colorbarmode | (12 << 3);
			break;
	}
}

void DRV_DVE_SetColorbar(int enable)
{
	pDVEReg->color_bar_mode |= (1 << 0);
}

