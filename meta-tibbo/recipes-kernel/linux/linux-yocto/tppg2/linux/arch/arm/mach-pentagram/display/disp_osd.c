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
 * @file disp_osd.c
 * @brief
 * @author PoChou Chen
 */
/*******************************************************************************
 *                         H E A D E R   F I L E S
 *******************************************************************************/
#include <asm/io.h>
#include "reg_disp.h"
#include "hal_disp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
// OSD Header config0
#define OSD_HDR_C0_CULT				0x80
#define OSD_HDR_C0_TRANS			0x40
	
// OSD Header config1
#define OSD_HDR_C1_BS				0x10
#define OSD_HDR_C1_BL2				0x04
#define OSD_HDR_C1_BL				0x01
	
// OSD control register
#define OSD_CTRL_COLOR_MODE_YUV		(0 << 10)
#define OSD_CTRL_COLOR_MODE_RGB		(1 << 10)
#define OSD_CTRL_NOT_FETCH_EN		(1 << 8)
#define OSD_CTRL_CLUT_FMT_GBRA		(0 << 7)
#define OSD_CTRL_CLUT_FMT_ARGB		(1 << 7)
#define OSD_CTRL_LATCH_EN			(1 << 5)
#define OSD_CTRL_A32B32_EN			(1 << 4)
#define OSD_CTRL_FIFO_DEPTH			(7 << 0)

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
typedef struct HW_OSD_Header_s   
{
	UINT8 config0;	//config0 includes:
	// [bit 7] cu	: color table update
	// [bit 6] ft	: force transparency
	// [bit 5:4]	: reserved
	// [bit 3:0] md : bitmap format (color mode)

	UINT8 reserved0; // reserved bits.

	UINT8 config1;	//config1 includes:
	// [bit 7:5]	: reserved
	// [bit 4] b_s	: byte swap enable
	// [bit 3] KY	: reserved
	// [bit 2] bl2	: region blend alpha enable (multiplier)
	// [bit 1]		: reserved
	// [bit 0] bl	: region blend alpha enable (replace)

	UINT8 blend_level;	//region blend level value

	UINT16 v_size;		//vertical display region size (line number)
	UINT16 h_size;		//horizontal display region size (pixel number)

	UINT16 disp_start_row;		//region vertical start row (0~(y-1))
	UINT16 disp_start_column;	//region horizontal start column (0~(x-1))

	UINT8 keying_R;
	UINT8 keying_G;
	UINT8 keying_B;
	UINT8 keying_A;

	UINT16 data_start_row;
	UINT16 data_start_column;

	UINT8 reserved2;
	UINT8 csc_mode_sel; //color space converter mode sel
	UINT16 data_width;	//source bitmap crop width

	UINT32 link_next;
	UINT32 link_data;

	UINT32 reserved3[24];	// need 128 bytes for HDR
} HW_OSD_Header_t;
STATIC_ASSERT(sizeof(HW_OSD_Header_t) == 128);

typedef struct _Region_Manager_t_
{
	DRV_OsdWindow_e				WinID;

	//DRV_Region_Info_t			RegionInfo;

	DRV_OsdRegionFormat_e		Format;
	UINT32						Align;
	UINT16						NumBuff;
	UINT16						AlloceInside;
	UINT32						DataPhyAddr;
	UINT8						*DataAddr;
	UINT8						*Hdr_ClutAddr;	//palette addr in osd header
	UINT32						BmpSize;
	UINT32						CurrBufID;
	UINT32						ForceTrans;		//ignore transmode dirtyflag

	// SW latch
	DRV_OsdTransparencyMode_e	TransMode;
	//DRV_Region_Info_t			Src_crop;
	UINT8						EnByteSwap;
	UINT8						EnOSDBlend;
	UINT8						BlendLevel;
	UINT8						DirtyFlag;
	DRV_OsdBlendMethod_e		BlendMethod;
	UINT8						*PaletteAddr;	//other side palette addr, Gearing with swap buffer.

	HW_OSD_Header_t				*pHWRegionHdr;
	struct _Region_Manager_t_	*pNextRegion;
	UINT32 reserved[4]; //For gsl allocate buffer case. The structure size should be 32 alignment.
} Region_Manager_t;
STATIC_ASSERT((sizeof(Region_Manager_t) % 4) == 0);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static DISP_OSD_REG_t *pOSDReg;
static DISP_GPOST_REG_t *pGPOSTReg;

extern DISPLAY_WORKMEM gDispWorkMem;

/**************************************************************************
 *             F U N C T I O N    I M P L E M E N T A T I O N S           *
 **************************************************************************/
void DRV_OSD_Init(void *pInHWReg1, void *pInHWReg2)
{
	pOSDReg = (DISP_OSD_REG_t *)pInHWReg1;
	pGPOSTReg = (DISP_GPOST_REG_t *)pInHWReg2;
}

#define OSD_WIDTH	720
#define OSD_HEIGHT	480

static UINT8 _gOSD_header[128 + 1024 + OSD_WIDTH * OSD_HEIGHT * 4] __attribute__((aligned(32))) = {
//#include "osd_pattern/osd_header_1152.h"
//#include "osd_pattern/ARGB8888_720x480.h"	//0xe
//#include "osd_pattern/RGBA8888_720x480.h"	//0xd
//#include "osd_pattern/ARGB4444_720x480.h"	//0xb
//#include "osd_pattern/RGBA4444_720x480.h"	//0xa
//#include "osd_pattern/ARGB1555_720x480.h"	//0x9
//#include "osd_pattern/RGB565_720x480.h"	//0x8
//#include "osd_pattern/YUY2_720x480.h"	//0x4
//#include "osd_pattern/8BPP_720x480.h"	//0x2
};

void DRV_OSD_InitTest(void)
{
	UINT32 *osd_header = (UINT32 *)_gOSD_header;
	diag_printf("osd_header=0x%x 0x%x\n", (UINT32)_gOSD_header, virt_to_phys(_gOSD_header));

#if 0
	osd_header[0] = SWAP32(0x0e001000);
#else
	osd_header[0] = SWAP32(0x82001000);
#endif
	osd_header[1] = SWAP32((OSD_HEIGHT << 16) | OSD_WIDTH);
	osd_header[2] = 0;
	osd_header[3] = 0;
	osd_header[4] = 0;
	osd_header[5] = SWAP32(0x00010000 | OSD_WIDTH);
	osd_header[6] = SWAP32(0xFFFFFFE0);
	osd_header[7] = SWAP32(virt_to_phys(_gOSD_header) + 128 + 1024);

	//OSD
	pOSDReg->osd_ctrl = OSD_CTRL_COLOR_MODE_RGB | OSD_CTRL_CLUT_FMT_ARGB | OSD_CTRL_LATCH_EN | OSD_CTRL_A32B32_EN | OSD_CTRL_FIFO_DEPTH;
	pOSDReg->osd_base_addr = virt_to_phys(_gOSD_header);
	pOSDReg->osd_hvld_offset = 0;
	pOSDReg->osd_vvld_offset = 0;
	pOSDReg->osd_hvld_width = OSD_WIDTH;
	pOSDReg->osd_vvld_height = OSD_HEIGHT;
	pOSDReg->osd_bist_ctrl = 0x0;
	pOSDReg->osd_3d_h_offset = 0x0;
	pOSDReg->osd_src_decimation_sel = 0x0;

	pOSDReg->osd_en = 1;

	//GPOST bypass
	pGPOSTReg->gpost0_config = 0;
	pGPOSTReg->gpost0_master_en = 0;
	pGPOSTReg->gpost0_bg1 = 0x8010;
	pGPOSTReg->gpost0_bg2 = 0x0080;

	//GPOST PQ disable
	pGPOSTReg->gpost0_contrast_config = 0x0;
}

void DRV_OSD_Info(void)
{
	DRV_OsdWindow_e id;
	UINT32 j;
	//DRV_OsdRegionHandle_t region;

	ERRDISP("Region display-order is as follows:\n");

	for (id = DRV_OSD0; id < DRV_OSD_MAX; ++id)
	{
		//Region_Manager_t *pRegionManager = NULL;
		HW_OSD_Header_t *pOsdHdr = NULL;

		//(void)DRV_OSD_GetMainRegion(id, &region);

		diag_printf("******osd layer:%d******\n", id);

		//if (!region)
		//	continue;

		//pRegionManager = (Region_Manager_t *)region;
		//pOsdHdr = pRegionManager->pHWRegionHdr;
		pOsdHdr = (HW_OSD_Header_t *)_gOSD_header;

		diag_printf("    Check osd output: %d %d, region ouput:%d %d\n",
			pOSDReg[id].osd_hvld_width,
			pOSDReg[id].osd_vvld_height,
			SWAP16(pOsdHdr->h_size),
			SWAP16(pOsdHdr->v_size));

		j = 0;
		//while (pRegionManager != NULL)
		{
#if 0
			pOsdHdr = pRegionManager->pHWRegionHdr;
			diag_printf("Region[%d]: (x, y)=(%d, %d) (w, h)=(%d, %d) (buf_w, buf_h)=(%d, %d)\n",
						j++,
						pRegionManager->RegionInfo.startX, pRegionManager->RegionInfo.startY,
						pRegionManager->RegionInfo.actW, pRegionManager->RegionInfo.actH,
						pRegionManager->RegionInfo.bufW, pRegionManager->RegionInfo.bufH);
			diag_printf("GSCL out: (x, y)=(%d, %d) (w, h)=(%d, %d) (buf_w, buf_h)=(%d, %d)\n",
						pRegionManager->OutInfo.startX, pRegionManager->OutInfo.startY,
						pRegionManager->OutInfo.actW, pRegionManager->OutInfo.actH,
						pRegionManager->OutInfo.bufW, pRegionManager->OutInfo.bufH);
			diag_printf("PalletteAddr: 0x%x\n", (UINT32)pRegionManager->PaletteAddr);
			diag_printf("Region DataAddr: 0x%x, Hdr_ClutAddr: 0x%x, ID: %d, Size: %d\n", (UINT32)pRegionManager->DataAddr, (UINT32)pRegionManager->Hdr_ClutAddr, pRegionManager->CurrBufID, pRegionManager->BmpSize);
			diag_printf("Region Hdl: 0x%x, forcetrans: %d, osd header: 0x%x\n", (UINT32)pRegionManager, pRegionManager->ForceTrans, (UINT32)pRegionManager->pHWRegionHdr);
#endif
			diag_printf("header: (x, y)=(%d, %d) (w, h)=(%d, %d) data(x, y)=(%d, %d) data width=%d\n",
						SWAP16(pOsdHdr->disp_start_column), SWAP16(pOsdHdr->disp_start_row),
						SWAP16(pOsdHdr->h_size), SWAP16(pOsdHdr->v_size),
						SWAP16(pOsdHdr->data_start_column), SWAP16(pOsdHdr->data_start_row), SWAP16(pOsdHdr->data_width));
			diag_printf("cu:%d ft:%d bit format:%d link data:0x%x\n\n", (pOsdHdr->config0 & 0x80)?1:0, (pOsdHdr->config0 & 0x40)?1:0, (pOsdHdr->config0 & 0xf), SWAP32(pOsdHdr->link_data));

			//pRegionManager = pRegionManager->pNextRegion;
		}
	}
}

void DRV_OSD_HDR_Show(void)
{
	int *ptr = (int *)_gOSD_header;
	int i;

	for (i = 0; i < 8; ++i)
		diag_printf("%d: 0x%08x\n", i, *(ptr+i));
}

void DRV_OSD_HDR_Write(int offset, int value)
{
	int *ptr = (int *)_gOSD_header;

	*(ptr+offset) = value;
}

