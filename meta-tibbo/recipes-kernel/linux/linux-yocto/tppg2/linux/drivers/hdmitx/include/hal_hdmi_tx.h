// *****************************************************************************
// *    HDMI_TX process                                                           *
// *    Create by Leon                                                        *
// *    2017/12/29                                                            *
// *****************************************************************************
#include <linux/kernel.h>

#ifndef HDMI_TX_H
#define HDMI_TX_H

#ifdef DISABLE
#undef DISABLE
#endif

#ifdef ENABLE
#undef ENABLE
#endif

#define DISABLE (0)
#define ENABLE (1)

// #define HDMI_TX_PTG

#define EXT_FLAG_OFFSET               	0x7e //Extension block num
#define EXT_TAG_OFFSET        			0x80

#define EXT_BLOCK_MAP_TAG        			0xf0

#define EDID_CEA_EXT_OFFSET        		128


//#define hdmi_tx_printf(f, a...)  LOG(CEC,1 , f, ##a)
#define hdmi_tx_printf(f, a...) printk(f, ##a)

// #define hdmi_tx_printf0(f, a...)  LOG0(CEC,1 , f, ##a)
// #define hdmi_tx_printf2(f, a...)  LOG(CEC, 2, f, ##a)
// #define hdmi_tx_printf_E(f, a...) LOG(ERR, 1, "[HDMI_TX][%s][%d]"f, __FUNCTION__, __LINE__, ##a)

#define hdmi_tx_printf0(f, a...)  printk(f, ##a)
#define hdmi_tx_printf2(f, a...)  printk(f, ##a)
#define hdmi_tx_printf_E(f, a...) printk(f, ##a)

//#define get_stc_32_inline()    (regs0->stc_15_0 | (regs0->stc_31_16 << 16))


enum hal_hdmi_tx_timing_e
{
	HDMI_TX_TIMING_480P = 0,
	HDMI_TX_TIMING_576P,
	HDMI_TX_TIMING_720P50,
	HDMI_TX_TIMING_720P60,
	HDMI_TX_TIMING_1080P50,
	HDMI_TX_TIMING_1080P60,
	HDMI_TX_TIMING_MAX
};

enum hal_hdmi_tx_color_depth_e
{
	HDMI_TX_COLOR_DEPTH_24BITS = 0,
	HDMI_TX_COLOR_DEPTH_30BITS,
	HDMI_TX_COLOR_DEPTH_36BITS,
	HDMI_TX_COLOR_DEPTH_48BITS,
	HDMI_TX_COLOR_DEPTH_MAX
};

enum hal_hdmi_tx_color_space_e
{
	HDMI_TX_COLOR_SPACE_LIMITED_RGB = 0,
	HDMI_TX_COLOR_SPACE_LIMITED_YUV,
	HDMI_TX_COLOR_SPACE_FULL_RGB,
	HDMI_TX_COLOR_SPACE_FULL_YUV,
	HDMI_TX_COLOR_SPACE_MAX
};

enum hal_hdmi_tx_pixel_format_e
{
	HDMI_TX_PIXEL_FORMAT_RGB = 0,
	HDMI_TX_PIXEL_FORMAT_YUV444,
	HDMI_TX_PIXEL_FORMAT_YUV422,
	HDMI_TX_PIXEL_FORMAT_MAX
};

struct hal_hdmi_tx_video_attr_t
{
	enum hal_hdmi_tx_timing_e timing;
	enum hal_hdmi_tx_color_depth_e color_depth;
	enum hal_hdmi_tx_color_space_e in_color_space;
	enum hal_hdmi_tx_color_space_e out_color_space;
	enum hal_hdmi_tx_pixel_format_e pixel_fmt;
};

// ===================================================================
// Function interface
// ===================================================================
void hal_hdmi_tx_disable(void);

void hal_hdmi_tx_config_video(struct hal_hdmi_tx_video_attr_t *video_attr);

void hal_hdmi_tx_enable(void);

#endif

