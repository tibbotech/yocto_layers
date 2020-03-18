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
 * @file disp_dmix.c
 * @brief
 * @author PoChou Chen
 */
/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
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
static volatile DISP_DMIX_REG_t *pDMIXReg;
extern DISPLAY_WORKMEM gDispWorkMem;

#ifdef DEBUG_MSG
	char *LayerNameStr[] = {"BG", "L1", "L2", "L3", "L4", "L5", "L6"};
	char *LayerModeStr[] = {"AlphaBlend", "Transparent", "Opacity"};
	char *SelStr[] = {"VPP0", "VPP1", "VPP2", "OSD0", "OSD1", "OSD2", "OSD3", "PTG"};
#endif

/**************************************************************************
 *             F U N C T I O N    I M P L E M E N T A T I O N S           *
 **************************************************************************/
void DRV_DMIX_Init(void *pInReg)
{
	pDMIXReg = (DISP_DMIX_REG_t *)pInReg;

	pDMIXReg->dmix_pix_en_sel = 0x0006;

	// BG: pattern gen
	// L1: VPP0
	// L6: OSD0
	pDMIXReg->dmix_config0 = 0x0070;
	pDMIXReg->dmix_config1 = 0x8156;
	pDMIXReg->dmix_config2 = 0x3000;

	//DMIX PTG(default border pixel len=0 (BGC))
	pDMIXReg->dmix_ptg_config = 0x2000;
	//default BGC color
	pDMIXReg->dmix_ptg_config_4 = (0<<8) | (0x10 & 0xff);
	pDMIXReg->dmix_ptg_config_5 = (0<<8) | (0x80 & 0xff);
	pDMIXReg->dmix_ptg_config_6 = (0<<8) | (0x80 & 0xff);

	//DMIX YC adjust
	pDMIXReg->dmix_yc_adjust = 0x0;
}

DRV_Status_e DRV_DMIX_PTG_ColorBar(DRV_DMIX_TPG_e tpg_sel, int bg_color_yuv, int border_len)
{
	if (tpg_sel >= DRV_DMIX_TPG_MAX)
		return DRV_ERR_INVALID_PARAM;

	switch(tpg_sel)
	{
		default:
		case DRV_DMIX_TPG_MAX:
		case DRV_DMIX_TPG_BGC:
			pDMIXReg->dmix_ptg_config = (1 << 13);
			DRV_DMIX_PTG_Color_Set_YCbCr(ENABLE, (bg_color_yuv>>16) & 0xff, (bg_color_yuv>>8) & 0xff, (bg_color_yuv>>0) & 0xff);
			break;
		case DRV_DMIX_TPG_V_COLORBAR:
			pDMIXReg->dmix_ptg_config = (0 << 15) | (0 << 14) | (0 << 13);
			break;
		case DRV_DMIX_TPG_H_COLORBAR:
			pDMIXReg->dmix_ptg_config = (1 << 15) | (0 << 14) | (0 << 13);
			break;
		case DRV_DMIX_TPG_BORDER:
			pDMIXReg->dmix_ptg_config = (0 << 15) | (0 << 14) | (1 << 13);
			DRV_DMIX_PTG_Color_Set_YCbCr(DISABLE, 0, 0, 0);
			break;
		case DRV_DMIX_TPG_SNOW:
			pDMIXReg->dmix_ptg_config = (0 << 15) | (1 << 14) | (0 << 13);
			DRV_DMIX_PTG_Color_Set_YCbCr(DISABLE, 0, 0, 0);
			break;
	}

	return DRV_SUCCESS;
}

void DRV_DMIX_PTG_Color_Set(UINT32 color)
{
	UINT16 Y, Cb, Cr;
	UINT16 R, G, B;

	R = (color >> 16) & 0xff;
	G = (color >> 8) & 0xff;
	B = color & 0xff;

	Y = (R * 76 / 255) + (G * 150 / 255) + (B * 29 / 255);
	Cb = -(R * 43 / 255) - (G * 85 / 255) + (B * 128 / 255) + 128;
	Cr = (R * 128 / 255) - (G * 107 / 255) - (B * 21 / 255) + 128;

	if (Cb > 255)
		Cb = 255;
	if (Cr > 255)
		Cr = 255;

	DRV_DMIX_PTG_Color_Set_YCbCr(ENABLE, (Y & 0xff), (Cb & 0xff), (Cr & 0xff));
}

void DRV_DMIX_PTG_Color_Set_YCbCr(UINT8 enable, UINT8 Y, UINT8 Cb, UINT8 Cr)
{
	if (enable)
	{
		pDMIXReg->dmix_ptg_config_4 = (1 << 8) | (Y & 0xff);
		pDMIXReg->dmix_ptg_config_5 = (1 << 8) | (Cb & 0xff);
		pDMIXReg->dmix_ptg_config_6 = (1 << 8) | (Cr & 0xff);
	}
	else
	{
		pDMIXReg->dmix_ptg_config_4 &= 0xff;
		pDMIXReg->dmix_ptg_config_5 &= 0xff;
		pDMIXReg->dmix_ptg_config_6 &= 0xff;
	}
}

DRV_Status_e DRV_DMIX_Layer_Init(DRV_DMIX_LayerId_e Layer, DRV_DMIX_LayerMode_e LayerMode, DRV_DMIX_InputSel_e FG_Sel)
{
	UINT32 tmp, tmp1, tmp2;

	if ((((int)Layer >= DRV_DMIX_L2) && ((int)Layer <= DRV_DMIX_L5)) ||
			((int)LayerMode < DRV_DMIX_AlphaBlend) || ((int)LayerMode > DRV_DMIX_Opacity) ||
			(((int)FG_Sel != DRV_DMIX_VPP0) && ((int)FG_Sel != DRV_DMIX_OSD0) && ((int)FG_Sel != DRV_DMIX_PTG)))
	{
		ERRDISP("Layer %d, LayerMode %d, InSel %d\n", Layer, LayerMode, FG_Sel);
		return DRV_ERR_INVALID_PARAM;
	}

	MSG("Layer %s, LayerMode %s, InSel %s\n", LayerNameStr[Layer], LayerModeStr[LayerMode], SelStr[FG_Sel]);

	tmp  = pDMIXReg->dmix_config0;
	tmp1 = pDMIXReg->dmix_config1;
	tmp2 = pDMIXReg->dmix_config2;

	//Set layer mode
	if (Layer != DRV_DMIX_BG)
	{
		//Clear layer mode bit
		tmp1 &= ~(0x3 << ((Layer - 1) << 1));
		tmp1 |= (LayerMode << ((Layer - 1) << 1));
	}

	//L1 and L2 in set in config0, other layer set in config2
	if (Layer <= DRV_DMIX_L2)
		tmp = (tmp & ~(0X7 << ((Layer * 4) + 4))) | (FG_Sel << ((Layer * 4) + 4));
	else
		tmp2 = (tmp2 & ~(0x7 << ((Layer - DRV_DMIX_L3) * 4))) | (FG_Sel << ((Layer - DRV_DMIX_L3) * 4));

	//Finish set amix layer information
	pDMIXReg->dmix_config0 = tmp;
	pDMIXReg->dmix_config1 = tmp1;
	pDMIXReg->dmix_config2 = tmp2;

	{
		DRV_TGEN_Input_e input;
		switch(FG_Sel)
		{
			case DRV_DMIX_VPP0:
				input = DRV_TGEN_VPP0;
				break;
			case DRV_DMIX_OSD0:
				input = DRV_TGEN_OSD0;
				break;
			case DRV_DMIX_PTG:
				input = DRV_TGEN_PTG;
				break;
			default:
				input = DRV_TGEN_ALL;
				break;
		}
		if (input != DRV_TGEN_ALL)
		{
			if (input == DRV_TGEN_PTG)
				DRV_TGEN_Adjust(input, 0x10);
			else
				DRV_TGEN_Adjust(input, 0x10 - ((Layer - DRV_DMIX_L1) << 1));
		}
	}

	return DRV_SUCCESS;
}

DRV_Status_e DRV_DMIX_Layer_Set(DRV_DMIX_LayerMode_e LayerMode, DRV_DMIX_InputSel_e FG_Sel)
{
	UINT32 tmp;
	DRV_DMIX_LayerId_e Layer;

	if (((int)LayerMode < DRV_DMIX_AlphaBlend) || ((int)LayerMode > DRV_DMIX_Opacity) ||
			(((int)FG_Sel != DRV_DMIX_VPP0) && ((int)FG_Sel != DRV_DMIX_OSD0) && ((int)FG_Sel != DRV_DMIX_PTG)))
	{
		ERRDISP("DMIX%d, Layer %d, LayerMode %d, InSel %d\n", Layer, LayerMode, FG_Sel);
		return DRV_ERR_INVALID_PARAM;
	}

	switch (FG_Sel)
	{
		case DRV_DMIX_VPP0:
			Layer = DRV_DMIX_L1;
			break;
		case DRV_DMIX_OSD0:
			Layer = DRV_DMIX_L6;
			break;
		case DRV_DMIX_PTG:
			Layer = DRV_DMIX_BG;
			break;
		default:
			goto ERROR;
			break;
	}
	MSG("Layer %s, LayerMode %s, InSel %s\n", LayerNameStr[Layer], LayerModeStr[LayerMode], SelStr[FG_Sel]);

	tmp = pDMIXReg->dmix_config1;

	//Set layer mode
	if (Layer != DRV_DMIX_BG)
	{
		tmp &= ~(0x3 << ((Layer - 1) << 1));
		tmp |= (LayerMode << ((Layer - 1) << 1));
	}

	//Finish set amix layer information
	pDMIXReg->dmix_config1 = tmp;

ERROR:
	return DRV_SUCCESS;
}

void DRV_DMIX_Layer_Get(DRV_DMIX_Layer_Set_t* pLayerInfo)
{
	UINT32 tmp, tmp1, tmp2, test;

	tmp  = pDMIXReg->dmix_config0;
	tmp1 = pDMIXReg->dmix_config1;
	tmp2 = pDMIXReg->dmix_config2;

	pLayerInfo->LayerMode = (tmp1 >> ((pLayerInfo->Layer - 1) << 1) & 0x3);

	if (pLayerInfo->Layer <= DRV_DMIX_L2)
		test = (tmp >> ((pLayerInfo->Layer * 4) + 4)) & 0x7;
	else
		test = (tmp2 >> ((pLayerInfo->Layer - DRV_DMIX_L3) * 4)) & 0x7;

	pLayerInfo->FG_Sel = test;

	MSG("Layer %s, LayerMode %s, InSel %s\n", LayerNameStr[pLayerInfo->Layer], LayerModeStr[pLayerInfo->LayerMode], SelStr[pLayerInfo->FG_Sel]);
}

DRV_Status_e DRV_DMIX_Plane_Alpha_Set(DRV_DMIX_PlaneAlpha_t *PlaneAlphaInfo)
{
	UINT32 tmp1;
	UINT32 tmp3;
	int ret = DRV_SUCCESS;

	INFO("Layer%d: En%d, Fix%d, 0x%x\n", PlaneAlphaInfo->Layer, PlaneAlphaInfo->EnPlaneAlpha, PlaneAlphaInfo->EnFixAlpha, PlaneAlphaInfo->AlphaValue);

	tmp1 = pDMIXReg->dmix_plane_alpha;
	tmp3 = pDMIXReg->dmix_plane_alpha3;

	switch(PlaneAlphaInfo->Layer)
	{
		case DRV_DMIX_L1:
			tmp1 = (tmp1 & ~0xFF00) |
					((PlaneAlphaInfo->EnPlaneAlpha & 0x1) << 15) |
					((PlaneAlphaInfo->EnFixAlpha & 0x1) << 14) |
					((PlaneAlphaInfo->AlphaValue & 0x3F) << 8);
			break;
		case DRV_DMIX_L6:
			tmp3 = (tmp3 & ~0x00FF) |
					((PlaneAlphaInfo->EnPlaneAlpha & 0x1) << 7) |
					((PlaneAlphaInfo->EnFixAlpha & 0x1) << 6) |
					(PlaneAlphaInfo->AlphaValue & 0x3F);
			break;
		default:
			ERRDISP("Invalid Layer %d\n", PlaneAlphaInfo->Layer);
			ret = DRV_ERR_INVALID_PARAM;
			break;
	}

	pDMIXReg->dmix_plane_alpha = tmp1;
	pDMIXReg->dmix_plane_alpha3 = tmp3;

	return ret;
}

void DRV_DMIX_PQ_OnOff(int OutId, int enable)
{
	if (enable)
		pDMIXReg->dmix_yc_adjust |= 0x3;
	else
		pDMIXReg->dmix_yc_adjust = 0;
}

#if 0
void DRV_DMIX_Luma_Adjust_Set(DRV_DMIX_Luma_Adj_t *LumaAdjInfo)
{
	//DISPLAY_WORKMEM *pDispWorkMem = &gDispWorkMem;
	// Y = (X - 127) * A + 127 + B
	// => Y = A * X + C
	// => C = B - 127 * A + 127
	// => A is contrast, B is brightness
	int i = 0;
	int x[5] = {0, 50, 100, 200, 255};
	int y[5] = {0, 50, 100, 200, 255};
	UINT16 slope[4] = {256, 256, 256, 256};
	int brightness_indx = 0, contrast_indx = 0;
	int brightness = 0;
	double contrast = 0.00;
	int brightness_min = DMIX_LUMA_OFFSET_MIN, brightness_max = DMIX_LUMA_OFFSET_MAX;
	double contrast_min = DMIX_LUMA_SLOPE_MIN, contrast_max = DMIX_LUMA_SLOPE_MAX;

	// save software setting
	//pDispWorkMem->dmix_adj[LumaAdjInfo->OutId].brightness = LumaAdjInfo->brightness;
	//pDispWorkMem->dmix_adj[LumaAdjInfo->OutId].contrast = LumaAdjInfo->contrast;

	// calculate A and C
	brightness_indx = (LumaAdjInfo->brightness > 24) ? 24 : LumaAdjInfo->brightness;
	contrast_indx = (LumaAdjInfo->contrast > 24) ? 24 : LumaAdjInfo->contrast;

	if (brightness_indx <= 12)
	{
		brightness = (double)(0 - brightness_min) * (double)brightness_indx / 12 + brightness_min;
	}
	else
	{
		brightness = (double)(brightness_max - 0) * (double)(brightness_indx - 12) / 12;
	}

	if (contrast_indx <= 12)
	{
		contrast = (double)(1 - contrast_min) * (double)contrast_indx / 12 + contrast_min;
	}
	else
	{
		contrast = (double)(contrast_max - 1.00) * (double)(contrast_indx - 12) / 12 + 1.00;
	}

	brightness += (127 - (double)127 * contrast);

	// calculate point
	y[0] = contrast * (double) x[0] + brightness;
	y[4] = contrast * (double) x[4] + brightness;

	if (y[0] > 0)
	{
		x[1] = x[0];
		y[1] = y[0];
	}
	else if (y[0] == 0)
	{
		x[1] = 50;
		y[1] = contrast * (double) x[1] + brightness;
	}
	else
	{
		x[1] = (double)((-1) * brightness) / contrast;
		y[1] = 0;
	}

	if (y[4] > 255)
	{
		y[3] = 255;
		x[3] = (double)(y[3] - brightness) / contrast;
	}
	else
	{
		x[3] = 200;
		y[3] = contrast * (double) x[3] + brightness;
	}

	x[2] = (x[1] + x[3]) / 2;
	y[2] = contrast * (double) x[2] + brightness;

	for (i = 0; i < 5; i++)
	{
		if (y[i] < 0)
			y[i] = 0;
		else if (y[i] > 255)
			y[i] = 255;
	}

	// calculate slope
	for (i = 0; i < 4; i++) {
		if (x[i+1] == x[i])
			slope[i] = 256;
		else
			slope[i] = ((double)(y[i+1] - y[i]) / (double)(x[i+1] - x[i])) * 256;
	}

	{
		pDMIXReg->dmix_yc_adjust = (pDMIXReg->dmix_yc_adjust & (~0x0002)) | ((LumaAdjInfo->enable & 0x1) << 1);
		pDMIXReg->dmix_luma_cp1 = ((y[1] & 0xFF) << 8) | (x[1] & 0xFF);
		pDMIXReg->dmix_luma_cp2 = ((y[2] & 0xFF) << 8) | (x[2] & 0xFF);
		pDMIXReg->dmix_luma_cp3 = ((y[3] & 0xFF) << 8) | (x[3] & 0xFF);
		pDMIXReg->dmix_luma_slope0 = (slope[0] & 0x07FF);
		pDMIXReg->dmix_luma_slope1 = (slope[1] & 0x07FF);
		pDMIXReg->dmix_luma_slope2 = (slope[2] & 0x07FF);
		pDMIXReg->dmix_luma_slope3 = (slope[3] & 0x07FF);
	}
}
#endif

void DRV_DMIX_Luma_Adjust_Get(DRV_DMIX_Luma_Adj_t *LumaAdjInfo)
{
	//DISPLAY_WORKMEM *pDispWorkMem = &gDispWorkMem;

	//LumaAdjInfo->brightness = pDispWorkMem->dmix_adj[LumaAdjInfo->OutId].brightness;
	//LumaAdjInfo->contrast = pDispWorkMem->dmix_adj[LumaAdjInfo->OutId].contrast;

	LumaAdjInfo->enable = (pDMIXReg->dmix_yc_adjust & 0x0002) >> 1;
	LumaAdjInfo->CP1_Src = pDMIXReg->dmix_luma_cp1 & 0xFF;
	LumaAdjInfo->CP1_Dst = (pDMIXReg->dmix_luma_cp1 >> 8) & 0xFF;
	LumaAdjInfo->CP2_Src = pDMIXReg->dmix_luma_cp2 & 0xFF;
	LumaAdjInfo->CP2_Dst = (pDMIXReg->dmix_luma_cp2 >> 8) & 0xFF;
	LumaAdjInfo->CP3_Src = pDMIXReg->dmix_luma_cp3 & 0xFF;
	LumaAdjInfo->CP3_Dst = (pDMIXReg->dmix_luma_cp3 >> 8) & 0xFF;
	LumaAdjInfo->Slope0 = pDMIXReg->dmix_luma_slope0 & 0x07FF;
	LumaAdjInfo->Slope1 = pDMIXReg->dmix_luma_slope1 & 0x07FF;
	LumaAdjInfo->Slope2 = pDMIXReg->dmix_luma_slope2 & 0x07FF;
	LumaAdjInfo->Slope3 = pDMIXReg->dmix_luma_slope3 & 0x07FF;
}

void DRV_DMIX_Chroma_Adjust_Set(DRV_DMIX_Chroma_Adj_t *ChromaAdjInfo)
{
	//Chroma Adjust work on L1-L6
	//INFO("en %d, cos 0x%x, sin 0x%x\n",
	//	ChromaAdjInfo->enable,
	//	ChromaAdjInfo->satcos,
	//	ChromaAdjInfo->satsin);

	pDMIXReg->dmix_yc_adjust = (pDMIXReg->dmix_yc_adjust & (~0x1))
		| (ChromaAdjInfo->enable & 0x1);
	pDMIXReg->dmix_chroma_satcos = (ChromaAdjInfo->satcos & 0x3FF);
	pDMIXReg->dmix_chroma_satsin = (ChromaAdjInfo->satsin & 0x3FF);
}

void DRV_DMIX_Chroma_Adjust_Get(DRV_DMIX_Chroma_Adj_t *ChromaAdjInfo)
{
	ChromaAdjInfo->enable = (pDMIXReg->dmix_yc_adjust & 0x1);
	ChromaAdjInfo->satcos = pDMIXReg->dmix_chroma_satcos;
	ChromaAdjInfo->satsin = pDMIXReg->dmix_chroma_satsin;
}

