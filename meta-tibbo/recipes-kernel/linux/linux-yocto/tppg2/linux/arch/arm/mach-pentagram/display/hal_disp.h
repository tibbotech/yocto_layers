#ifndef __HAL_DISP_H__
#define __HAL_DISP_H__
/**
 * @file hal_disp.h
 * @brief 
 * @author PoChou Chen
 */
#include "disp_dmix.h"
#include "disp_tgen.h"
#include "disp_dve.h"
#include "disp_osd.h"
#include "disp_vpp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define REG_START_Q628_B			(0x9c000000)
#define REG_START_Q628_A			(0x9ec00000)

#define ALIGNED(x, n)				((x) & (~(n - 1)))
#define EXTENDED_ALIGNED(x, n)		(((x) + ((n) - 1)) & (~(n - 1)))

#define SWAP32(x)	((((UINT32)(x)) & 0x000000ff) << 24 \
					| (((UINT32)(x)) & 0x0000ff00) << 8 \
					| (((UINT32)(x)) & 0x00ff0000) >> 8 \
					| (((UINT32)(x)) & 0xff000000) >> 24)
#define SWAP16(x)	(((x) & 0x00ff) << 8 | ((x) >> 8))

#ifndef ENABLE
	#define ENABLE			1
#endif

#ifndef DISABLE
	#define DISABLE			0
#endif

#define diag_printf printk
extern int printk(const char *fmt, ...);

#define SUPPORT_DEBUG_MON

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct _display_size_t {
	UINT16 width;
	UINT16 height;
} display_size_t;

typedef struct {
	void *pHWRegBase;

	//void *aio;
	void *bio;
#if 0
	GL_Semaphore_t field_end[DRV_Disp_Out_MAX];
	GL_Semaphore_t field_start[DRV_Disp_Out_MAX];
	UINT32 field_end_protect;
	UINT32 field_start_protect;

	DRV_DISP_SYS_MODE_e SysMode;

	DRV_DISP_Scan_Info_t OutputScanInfo[4];// 0 and 1 : original scan info,    2 : NTSC cvbs out,  3:PAL cvbs out
#endif
	display_size_t		panelRes;
#if 0
	display_size_t		UIRes;
	DRV_FrameRate_e		curr_panel_fps;
	DRV_DISP_SYS_ASPECT_RATIO_e AspectRatio[2];

	UINT8 rcs_cvbs_black;
	DRV_Sys_OutMode_Info_t DispOutMode;
	UINT8 DispOutModeUpdated;

	//TGEN start
	UINT8 bTGEN_SYS_SyncMode;
	//TGEN end

	//DMIX start
	UINT16 TCON_BP;
	UINT32 display_mode_bgc[DRV_Disp_Out_MAX];
	UINT8 display_mode[DRV_Disp_Out_MAX];
	//UINT32 display_dmix_backup[DRV_Disp_Out_MAX];
	//DMIX end

	//PQ start
	UINT32 ScenNum; //used for PQ tool. Number of scenario
	UINT32 OrgScene;
	UINT32 PQ_Flag[2];
	UINT32 ScenSelect;
	UINT8 *PQBuff;
	void *PQPackage[BIN_NUM_MAX]; //PQPackage[0] is reserved
	UINT8 PQ_Version_ID;
	DRV_VPP_Favorite_Color VideoFCA[DRV_VPP_MODULE_NUM];
	UINT8 pq_scene_chg_idx;
	DRV_DMIX_PQ_Adj_t dmix_adj[2];
	//PQ end

	//VPP start
	DRV_VPP_OutputWin_t VPP1_output;
	DRV_VPP_Param_t* pgVppInfo[4];
	//UINT32 RCS_BufCount;
	//UINT32 IsRearView;
	DRV_VPP_Luma_Adj_t VPP_Luma[2];
	UINT8 acfg_overscan_by_scaling;
	int acfg_overscan_ntsc;
	int acfg_overscan_pal;
	UINT32 VideoSrcToDispCurr[2];
	UINT32 VideoSrcToDispId[Video_Source_Num];
	UINT32 VppRcsId;
	UINT8 vpp_aspect_spec;
	//VPP end
#endif
} DISPLAY_WORKMEM;

#endif	//__HAL_DISP_H__

