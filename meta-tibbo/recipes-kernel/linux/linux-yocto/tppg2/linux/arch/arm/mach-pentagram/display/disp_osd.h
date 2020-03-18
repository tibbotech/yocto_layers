#ifndef __DISP_OSD_H__
#define __DISP_OSD_H__

typedef enum {
	DRV_OSD0 = 0,
	DRV_OSD_MAX
} DRV_OsdWindow_e;

typedef enum {
	/* 8 bit/pixel with CLUT */
	DRV_OSD_REGION_FORMAT_8BPP			= 0x2,
	/* 16 bit/pixel RGB 5:6:5 */
	DRV_OSD_REGION_FORMAT_RGB_565		= 0x8,
	/* 16 bit/pixel ARGB 1:5:5:5 */
	DRV_OSD_REGION_FORMAT_ARGB_1555		= 0x9,
	/* 16 bit/pixel RGBA 4:4:4:4 */
	DRV_OSD_REGION_FORMAT_RGBA_4444		= 0xA,
	/* 16 bit/pixel ARGB 4:4:4:4 */
	DRV_OSD_REGION_FORMAT_ARGB_4444		= 0xB,
	/* 32 bit/pixel RGBA 8:8:8:8 */
	DRV_OSD_REGION_FORMAT_RGBA_8888		= 0xD,
	/* 32 bit/pixel ARGB 8:8:8:8 */
	DRV_OSD_REGION_FORMAT_ARGB_8888		= 0xE,
} DRV_OsdRegionFormat_e;

typedef enum
{
	DRV_OSD_TRANSPARENCY_DISABLED = 0,	/*!< transparency is disabled */
	DRV_OSD_TRANSPARENCY_ALL			/*!< the whole region is transparent */
} DRV_OsdTransparencyMode_e;

typedef UINT32 DRV_OsdRegionHandle_t;

typedef enum
{
	DRV_OSD_BLEND_REPLACE,		/*!< OSD blend method is region alpha replace */
	DRV_OSD_BLEND_MULTIPLIER,	/*!< OSD blend method is region alpha multiplier */
	MAX_BLEND_METHOD,
} DRV_OsdBlendMethod_e;

void DRV_OSD_Init(void *pInHWReg1, void *pInHWReg2);
void DRV_OSD_Info(void);

void DRV_OSD_HDR_Show(void);
void DRV_OSD_HDR_Write(int offset, int value);

#endif	//__DISP_OSD_H__

