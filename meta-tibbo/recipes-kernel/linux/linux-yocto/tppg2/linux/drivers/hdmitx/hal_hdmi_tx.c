/******************************************************************************

	Copyright (c) 2017 Sunplus technology co. Ltd.All right reserved!

			HDMI_TX_MAIN process
			Create by Leon
			2017/12/29


******************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/

// #include <string.h>
// #include <stdio.h>
// #include "common.h"
// #include "regmap.h"
#include <linux/string.h>
#include "include/hal_hdmi_tx.h"
#include "include/hal_hdmi_tx_reg.h"
#include "include/hdmi_pub.h"
// ===================================================================
// Local Variable
// ===================================================================

#define TP_480P  0
#define TP_720P  1
#define TP_1080P 2
#define TP_1080i 4
#define TP_576P  3

#define DC_8B  0x4
#define DC_10B 0x5
#define DC_12B 0x6
#define DC_16B 0x7

#define HDMI_MODE 1
#define DVI_MODE  0

unsigned char HDMI_ouput_mode;
unsigned char HDMI_tv_mode;
#define STR_OS_TV_SYS           111

#define STR_OS_PAL              132
#define STR_OS_NTSC             133

#define Mode_480I   10
#define Mode_576I   9
#define Mode_1080P    8
#define Mode_VGA    7
#define Mode_SVGA   6
#define Mode_XGA    5
#define Mode_SXGA   4
#define Mode_480P   3
#define Mode_576P   2
#define Mode_720P60   1
#define Mode_720P50   1
#define Mode_1080i    0

unsigned char RGB_YCbCr;

//#define SUPPORT_PREFER_COLOR_SPACE
//BYTE edid_data[512];
//static BYTE temp_385_0;
#define acr_MCLK_over_Fs    2
#define acr_sw_CTS_en     1
#define acr_pkt_en          0

#define auinfo_pkt_rpt_spa          12
#define auinfo_pkt_tx             10
#define auinfo_pkt_rpt            9
#define auinfo_pkt_en             8
#define aviinfo_pkt_rpt_spa         4
#define aviinfo_pkt_tx            2
#define aviinfo_pkt_rpt           1
#define aviinfo_pkt_en            0

enum pixel_clk_mode_e
{
	PIXEL_CLK_MODE_27_MHZ = 0,
	PIXEL_CLK_MODE_74P25_MHZ,
	PIXEL_CLK_MODE_148P5_MHZ,
	PIXEL_CLK_MODE_297_MHZ,
	PIXEL_CLK_MODE_23P75_MHZ,
	PIXEL_CLK_MODE_38P25_MHZ,
	PIXEL_CLK_MODE_63P5_MHZ,
	PIXEL_CLK_MODE_109_MHZ,
	PIXEL_CLK_MODE_19P75_MHZ,
	PIXEL_CLK_MODE_30P75_MHZ,
	PIXEL_CLK_MODE_52_MHZ,
	PIXEL_CLK_MODE_88P5_MHZ,
	PIXEL_CLK_MODE_MAX
};

struct pll_tv_param_t
{
	unsigned char bypass;
	unsigned char r;
	unsigned char m;
	unsigned char n;
};

struct hal_hdmit_tx_csc_param_t
{
	unsigned short on_off;
	unsigned short manual;
	unsigned short ycc_range; // 0:limit, 1:full
	unsigned short rgb_range; // 0:limit, 1:full
	unsigned short conversion_type; // 0:r2y, 1:y2r
	unsigned short colorimetry; // 0:601, 1:709
};

static struct pll_tv_param_t g_pll_tv_cfg[PIXEL_CLK_MODE_MAX] =
{
	// PIXEL_CLK_MODE_27_MHZ
	{.bypass = ENABLE, .r = 0x0, .m = 0x0, .n = 0x0},

	// PIXEL_CLK_MODE_74P25_MHZ
	{.bypass = DISABLE, .r = 0x1, .m = 0x1, .n = 0xa},

	// PIXEL_CLK_MODE_148P5_MHZ
	{.bypass = DISABLE, .r = 0x0, .m = 0x1, .n = 0xa},

	// PIXEL_CLK_MODE_297_MHZ
	{.bypass = DISABLE, .r = 0x0, .m = 0x9, .n = 0x2a},

	// PIXEL_CLK_MODE_23P75_MHZ
	{.bypass = DISABLE, .r = 0x3, .m = 0x42, .n = 0xbc},

	// PIXEL_CLK_MODE_38P25_MHZ
	{.bypass = DISABLE, .r = 0x2, .m = 0x42, .n = 0x97},

	// PIXEL_CLK_MODE_63P5_MHZ
	{.bypass = DISABLE, .r = 0x1, .m = 0x42, .n = 0x7d},

	// PIXEL_CLK_MODE_109_MHZ
	{.bypass = DISABLE, .r = 0x1, .m = 0x42, .n = 0xd8},

	// PIXEL_CLK_MODE_19P75_MHZ
	{.bypass = DISABLE, .r = 0x3, .m = 0x42, .n = 0x9c},

	// PIXEL_CLK_MODE_30P75_MHZ
	{.bypass = DISABLE, .r = 0x2, .m = 0x42, .n = 0x79},

	// PIXEL_CLK_MODE_52_MHZ
	{.bypass = DISABLE, .r = 0x1, .m = 0x42, .n = 0x66},

	// PIXEL_CLK_MODE_88P5_MHZ
	{.bypass = DISABLE, .r = 0x1, .m = 0x42, .n = 0xaf},
};

static struct hal_hdmit_tx_csc_param_t g_csc_cfg[HDMI_TX_COLOR_SPACE_MAX][HDMI_TX_COLOR_SPACE_MAX] =
{
	// on_off, manual, ycc_range, rgb_range, conversion_type, colorimetry
	//
	{
		// RGB limited to RGB limited
		{0, 0, 0, 0, 0, 0},

		// RGB limited to YC limited
		{1, 0, 0, 0, 0, 0},

		// RGB limited to RGB full
		{1, 1, 0, 0, 0, 0},

		// RGB limited to YC full
		{1, 0, 1, 0, 0, 0},
	},

	{
		// YC limited to RGB limited
		{1, 0, 0, 0, 1, 0},

		// YC limited to YC limited
		{0, 0, 0, 0, 0, 0},

		// YC limited to RGB full
		{1, 0, 0, 1, 1, 0},

		// YC limited to YC full
		{1, 1, 1, 0, 0, 0},
	},

	{
		// RGB full to RGB limited
		{1, 1, 0, 0, 0, 0},

		// RGB full to YC limited
		{1, 0, 0, 1, 0, 0},

		// RGB full to RGB full
		{0, 0, 0, 0, 0, 0},

		// RGB full to YC full
		{1, 0, 1, 1, 0, 0},
	},

	{
		// YC full to RGB limited
		{1, 0, 1, 0, 1, 0},

		// YC full to YC limited
		{1, 1, 0, 1, 0, 0},

		// YC full to RGB full
		{1, 0, 1, 1, 1, 0},

		// YC full to YC full
		{0, 0, 0, 0, 0, 0},
	}
};

static struct hal_hdmi_tx_video_attr_t g_video_attr =
{
	.timing          = HDMI_TX_TIMING_480P,
	.color_depth     = HDMI_TX_COLOR_DEPTH_24BITS,
	.in_color_space  = HDMI_TX_COLOR_SPACE_LIMITED_RGB,
	.out_color_space = HDMI_TX_COLOR_SPACE_LIMITED_RGB,
	.pixel_fmt       = HDMI_TX_PIXEL_FORMAT_RGB,
};

// ===================================================================
// Function interface
// ===================================================================
void HDMI_TX_480P(unsigned int color_depth,
					enum hal_hdmi_tx_color_space_e in_cs,
					enum hal_hdmi_tx_color_space_e out_cs,
					enum hal_hdmi_tx_pixel_format_e pixel_fmt);
void DVI_TX_480P(void);
void HDMI_TX_576P(unsigned int color_depth);
void DVI_TX_576P(void);
void HDMI_TX_DISABLE(void);
// return HDMI mode (DVI/HDMI)
// return -1, if error
int HDMI_TX_MODE(unsigned char* pEdidData);
int GET_TV_RES(unsigned char* pEdidData);

void hdmitx_clk (int video_mode)
{
	enum pixel_clk_mode_e clk_mode = PIXEL_CLK_MODE_27_MHZ;

	switch (video_mode)
	{
		case TP_720P:
			clk_mode = PIXEL_CLK_MODE_74P25_MHZ;
			hdmi_tx_printf("[hdmitx] Set CLK - 720P\n");
			break;
		case TP_1080P:
			clk_mode = PIXEL_CLK_MODE_148P5_MHZ;
			hdmi_tx_printf("[hdmitx] Set CLK - 1080P\n");
			break;
		case TP_480P:
		case TP_576P:
		default:
			clk_mode = PIXEL_CLK_MODE_27_MHZ;
			hdmi_tx_printf("[hdmitx] Set CLK - 480P\n");
			break;
	}

	if (g_pll_tv_cfg[clk_mode].bypass == ENABLE)
	{
		MOON4_REG->plltv_ctl[0] = 0x80000000 | (0x1 << 15);
	}
	else
	{
		MOON4_REG->plltv_ctl[0] = 0x80000000;
		MOON4_REG->plltv_ctl[1] = 0x01800000 | (g_pll_tv_cfg[clk_mode].r << 7);
		MOON4_REG->plltv_ctl[2] = 0x7fff0000 | (g_pll_tv_cfg[clk_mode].m << 8) | (g_pll_tv_cfg[clk_mode].n);
	}

	 hdmi_tx_printf("[hdmitx] hdmitx_clk finished\n");

}

void hdmitx_sys_init(void)
{
	//regs2->G380_HDMI_TX[5] = 0x141f;
	hdmi_tx_regs->hdmi_pwr_ctrl=0x141f;
	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+0, 0x05, 0x1f); // remove all power down bit
	//regs2->G380_HDMI_TX[6] = 0x0000;
	hdmi_tx_regs->hdmi_sw_reset=0x0000;
	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+0, 0x06, 0x00);
	//regs2->G380_HDMI_TX[6] = 0x00ff;
	hdmi_tx_regs->hdmi_sw_reset=0x00ff;
	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+0, 0x06, 0xff); // issue SW reset
	hdmi_tx_printf("[hdmitx] System initialized\n");
}

void hdmitx_tp_init (int interlace, int video_mode, int GrayStep, int HD_SD, int YC444_422, int RGB_YC, int VHSync_inv)
{
		//unsigned short read_data;
		unsigned int w_data;
		w_data = (interlace<<11 | video_mode<<8 | GrayStep<<6 | HD_SD<<5 | YC444_422<<4 | RGB_YC<<3 | VHSync_inv<<1 | 1);
		//regs2->G381_HDMI_TX[20] = w_data;
	hdmi_tx_regs->hdmi_video_pat_gen1=w_data;
		//read_data = regs2->G381_HDMI_TX[20] ;
		//diag_printf("[hdmitx] CARBON Video Setting :%x\n", w_data);

	 // HDMITX_W_HB(HDMITX_RGST_GRP_BASE+1, 0x14, (interlace<<3)|video_mode);  // vg_inter_mode, pat_res
		switch (video_mode) {
		case TP_480P:
				 hdmi_tx_printf("[hdmitx] Set Internal Pattern Gen - 480P\n");
				 break;
		case TP_720P:
				 hdmi_tx_printf("[hdmitx] Set Internal Pattern Gen - 720P\n");
				 break;
		case TP_1080P:
				 hdmi_tx_printf("[hdmitx] Set Internal Pattern Gen - 1080P\n");
				 break;
		default:
				 hdmi_tx_printf("[hdmit] Set Internal Pattern Gen - None\n");
				 break;
		}
	 //HDMITX_W_LB(381, 0x14, GrayStep<<6|HD_SD<<5|YC444_422<<4|RGB_YC<<3|VHSync_inv<<1|1);
	 // hdmi_tx_printf("[hdmitx] Enable Internal Pattern Gen");

}

void hdmitx_video_cfg(int DC_Mode, int HVSyncOutNeg, int HDMI_Mode)
{
				// REG: 0x4c
				//  8bits ColorDepth = 4'b0000
				// 10bits ColorDepth = 4'b0101
				// 12bits ColorDepth = 4'b0110
				// 16bits ColorDepth = 4'b0111
	unsigned int w_data;
	w_data = (DC_Mode<<4 | HVSyncOutNeg);
	//regs2->G382_HDMI_TX[12] = w_data;
	hdmi_tx_regs->hdmi_video_format= w_data;
	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+2, 0x0C, DC_Mode<<4|HVSyncOutNeg);
	w_data = (1<<12 | HDMI_Mode);
	//regs2->G380_HDMI_TX[8] = w_data;
	hdmi_tx_regs->hdmi_system_ctrl1= w_data;
	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+0, 0x08, HDMI_Mode);


	// CSC setting
	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+1,0x10,CSC_Mode<<4|Pix_Rep);
	//HDMITX_W_HB(HDMITX_RGST_GRP_BASE+1,0x10,DN_SMP<<4|UP_SMP<<3|CSC_Man<<1|CSC_EN );
//  GRP(HDMITX_RGST_GRP_BASE+1, 0x10) = (DN_SMP<<12|UP_SMP<<11|CSC_Man<<9|CSC_EN<<8|CSC_Mode<<4|Pix_Rep);

	// Set GCP
	w_data = 0x1013;
	//regs2->G385_HDMI_TX[1] = w_data;
	hdmi_tx_regs->hdmi_infoframe_ctrl2=w_data;
	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+5,0x01,0x13);
	hdmi_tx_printf("[hdmitx] Video initialized");
}

void hdmitx_audio_cfg(int I2S_SPDIF, int AudioLayout, int AudioSP, int MCLK, int AudioEn, int AudioMute, int audio_n)
{
	unsigned short read_data;
	unsigned int w_data;
	int I2S_SD_EN;
	unsigned int SPDIF_EN;
	unsigned int SPDIF_SP;

	// audio source
	if (AudioLayout)
		I2S_SD_EN = I2S_SPDIF?AudioSP:0x0;
	else
		I2S_SD_EN = I2S_SPDIF?0x1:0xf;


	w_data = (1<<15 | 1<<14 | 1<<13 | 1<<10 | I2S_SD_EN<<4|AudioMute<<2|AudioLayout<<1|AudioEn);
	//regs2->G382_HDMI_TX[16] = w_data ;
	hdmi_tx_regs->hdmi_audio_ctrl1=w_data;

	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+2, 0x10, I2S_SD_EN<<4|AudioMute<<2|AudioLayout<<1|AudioEn);

	//regs2->G382_HDMI_TX[17] = 0x01b5 ;
	hdmi_tx_regs->hdmi_audio_ctrl2=0x01b5;

	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+2, 0x11, 0xb5);
	//HDMITX_W_HB(HDMITX_RGST_GRP_BASE+2, 0x11, 0x01);

	//regs2->G382_HDMI_TX[24] = 0x0200 ;
	hdmi_tx_regs->hdmi_audio_chnl_sts2=0x0200;//48khz
	//hdmi_tx_regs->hdmi_audio_chnl_sts2=0x0300;//32khz
	//hdmi_tx_regs->hdmi_audio_chnl_sts2=0x0000;//44.1khz
	//hdmi_tx_regs->hdmi_audio_chnl_sts2=0x0800;//88.2khz
	//hdmi_tx_regs->hdmi_audio_chnl_sts2=0x0a00;//96khz
	//hdmi_tx_regs->hdmi_audio_chnl_sts2=0x0c00;//176.4khz
	//hdmi_tx_regs->hdmi_audio_chnl_sts2=0x0e00;//192khz


	//HDMITX_W_HB(HDMITX_RGST_GRP_BASE+2, 0x18, 0x02);  // 48KHz




	SPDIF_EN = !I2S_SPDIF;
	SPDIF_SP = 0x3f;  // in our design, SPDIF only support 4 samples each audio packet for 96khz~32khz
	//SPDIF_SP = 0x00;  //for 176.4khz & 192khz

	// SPDIF
	//regs2->G382_HDMI_TX[18] = (SPDIF_SP<<1|SPDIF_EN) ;

	hdmi_tx_regs->hdmi_audio_spdif_ctrl= (SPDIF_SP<<1|SPDIF_EN) ;

	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+2, 0x12, SPDIF_SP<<1|SPDIF_EN);



	// ACR
	w_data = (MCLK<<2|0x1);
	//regs2->G382_HDMI_TX[26] = w_data ;
	hdmi_tx_regs->hdmi_arc_config1=w_data; //for all
	//hdmi_tx_regs->hdmi_arc_config1=0xd; //for 176.4khz

	//HDMITX_W_LB(HDMITX_RGST_GRP_BASE+2, 0x1A, MCLK<<2|0x1);  // enable ACR
	// N = 0x1800
	//regs2->G382_HDMI_TX[27] = 0x1800 ;
	hdmi_tx_regs->hdmi_arc_n_value1=audio_n; //for 48khz
	//hdmi_tx_regs->hdmi_arc_n_value1=0x1000; //for 32khz
	//hdmi_tx_regs->hdmi_arc_n_value1=0x1880; //for 44.1khz
	//hdmi_tx_regs->hdmi_arc_n_value1=0x3100; //for 88.2khz
	//hdmi_tx_regs->hdmi_arc_n_value1=0x3000; //for 96khz
	//hdmi_tx_regs->hdmi_arc_n_value1=0x6200; //for 176.4khz
	//hdmi_tx_regs->hdmi_arc_n_value1=0x6000; //for 192khz



	//HDMITX_W_HB(HDMITX_RGST_GRP_BASE+2, 0x1B,0x18);

	//read_data = regs2->G382_HDMI_TX[27] ;

	read_data = hdmi_tx_regs->hdmi_arc_n_value1;
	//diag_printf("[hdmitx] CARBON N :%x\n", read_data);

	hdmi_tx_printf("[hdmitx] CARBON N :%x CTS:%x\n", read_data,hdmi_tx_regs->hdmi_arc_hw_cts1);
}




int Get_InfoTab(void)
{
	if( HDMI_tv_mode == STR_OS_NTSC){
		 if( HDMI_ouput_mode == Mode_480P) //for (NTSC) AVI infoTab
					return 0;
		 else if( HDMI_ouput_mode == Mode_720P60)
					return 1;
		 else if( HDMI_ouput_mode == Mode_1080i)
					return 2;
		 else if( (HDMI_ouput_mode == Mode_SXGA)||(HDMI_ouput_mode == Mode_480I)) //480i
					return 3;
		 else if( HDMI_ouput_mode ==Mode_1080P)
					return 4;
		 else
					return 0;
	 }

	 if(HDMI_tv_mode == STR_OS_PAL){
		 if( HDMI_ouput_mode == Mode_576P) //for (PAL)AVI infoTab
					return 5;
		 else if( HDMI_ouput_mode == Mode_720P60)
					return 6;
		 else if( HDMI_ouput_mode == Mode_1080i)
					return 7;
		 else if( HDMI_ouput_mode == Mode_1080P)
					return 8;
		 else if( HDMI_ouput_mode == Mode_576I)//eric lin,2006-06-07,solve 576i protocol fail bug.
					return 9;            //eric lin,2006-06-07,solve 576i protocol fail bug.
		 else
					return 5;
	 }
	 return -1;                     // nothing match just return -1
}

/*------------ helm add infoframe table 050721 --------------

	Dev[0x7a] Reg[0x40~0x50], 17 bytes,
	reg[0x40~0x43]: header
	reg[0x44~0x50]: data

detail AVI packet information is as following:
{type,ver , len, crc, ycbcr,aspect,uninf, VIC ,  VPR, end_top   , str_bottom, end_left , str_right},
*/
const unsigned char hdmi_avi_buf[19][17] = {
 //480p --> 480p
 {0x82, 0x02, 0x0d, 0x08, 0x1c, 0x58, 0x00, 0x02, 0x00, 0x24, 0x00, 0x04, 0x02, 0x7a, 0x00, 0x4a, 0x03}, //gray bar rgb 444
 {0x82, 0x02, 0x0d, 0x08, 0x3c, 0x58, 0x00, 0x02, 0x00, 0x24, 0x00, 0x04, 0x02, 0x7a, 0x00, 0x4a, 0x03}, //Leon gray bar ycbcr 422
 {0x82, 0x02, 0x0d, 0x08, 0x5c, 0x58, 0x00, 0x02, 0x00, 0x24, 0x00, 0x04, 0x02, 0x7a, 0x00, 0x4a, 0x03}, //Leon gray bar ycbcr 444

 {0x82, 0x02, 0x0d, 0x08, 0x1c, 0x98, 0x00, 0x02, 0x00, 0x24, 0x00, 0x04, 0x02, 0x7a, 0x00, 0x4a, 0x03}, //Leon color bar rgb 444
 {0x82, 0x02, 0x0d, 0x08, 0x3c, 0x98, 0x00, 0x02, 0x00, 0x24, 0x00, 0x04, 0x02, 0x7a, 0x00, 0x4a, 0x03}, //Leon color bar ycbcr 422
 {0x82, 0x02, 0x0d, 0x08, 0x5c, 0x98, 0x00, 0x02, 0x00, 0x24, 0x00, 0x04, 0x02, 0x7a, 0x00, 0x4a, 0x03}, //Leon color bar ycbcr 444

// {0x82, 0x02, 0x0d, 0x08, 0x10, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

 //480p --> 720p rgb
 {0x82, 0x02, 0x0d, 0x94, 0x1c, 0xa8, 0x00, 0x04, 0x00, 0x19, 0x00, 0xe9, 0x02, 0x04, 0x01, 0x04, 0x06},
 //480p --> 1080i
 {0x82, 0x02, 0x0d, 0x3a, 0x1c, 0xa8, 0x00, 0x05, 0x00, 0x14, 0x00, 0x4c, 0x04, 0xc0, 0x00, 0x40, 0x08},
 //480p --> 480i
 {0x82, 0x02, 0x0d, 0x2e, 0x1c, 0x58, 0x00, 0x06, 0x01, 0x12, 0x00, 0xf2, 0x01, 0x77, 0x00, 0x47, 0x03},
 //480p --> 1080P
 {0x82, 0x02, 0x0d, 0x04, 0x1d, 0xa8, 0x00, 0x10, 0x00, 0x29, 0x00, 0x61, 0x04, 0xc0, 0x00, 0x40, 0x08},

 //576p --> 576p
 {0x82, 0x02, 0x0d, 0x75, 0x1c, 0x58, 0x00, 0x11, 0x00, 0x2c, 0x00, 0x6c, 0x02, 0x84, 0x00, 0x54, 0x03},
 //576p --> 720p
 {0x82, 0x02, 0x0d, 0x8a, 0x1c, 0xa8, 0x00, 0x13, 0x00, 0x19, 0x00, 0xe9, 0x02, 0x04, 0x01, 0x00, 0x05},
 //576p --> 1080i
 {0x82, 0x02, 0x0d, 0x49, 0x1c, 0xa8, 0x00, 0x14, 0x00, 0x14, 0x00, 0x30, 0x02, 0xc0, 0x00, 0x40, 0x08},
 //576p --> 1080p rgb
 {0x82, 0x02, 0x0d, 0xf5, 0x1d, 0xa8, 0x00, 0x1f, 0x00, 0x29, 0x00, 0x61, 0x04, 0xc0, 0x00, 0x40, 0x08},
 //576p --> 576i,eric lin,2006-06-07,solve 576i protocol fail bug.
 {0x82, 0x02, 0x0d, 0xc8, 0x1d, 0x58, 0x00, 0x15, 0x01, 0x16, 0x00, 0x56, 0x02, 0x84, 0x00, 0x24, 0x06},

 //720P60 ycbcr 422
 {0x82, 0x02, 0x0d, 0x94, 0x3c, 0xa8, 0x00, 0x04, 0x00, 0x19, 0x00, 0xe9, 0x02, 0x04, 0x01, 0x04, 0x06},

 //720P60 ycbcr 444
 {0x82, 0x02, 0x0d, 0x94, 0x5c, 0xa8, 0x00, 0x04, 0x00, 0x19, 0x00, 0xe9, 0x02, 0x04, 0x01, 0x04, 0x06},

 // 1080P60 ycbcr 422
 {0x82, 0x02, 0x0d, 0xf5, 0x3d, 0xa8, 0x00, 0x1f, 0x00, 0x29, 0x00, 0x61, 0x04, 0xc0, 0x00, 0x40, 0x08},

 // 1080P60 ycbcr 444
 {0x82, 0x02, 0x0d, 0xf5, 0x5d, 0xa8, 0x00, 0x1f, 0x00, 0x29, 0x00, 0x61, 0x04, 0xc0, 0x00, 0x40, 0x08},
};

void SendAVIInfoFrame(void)
{
		unsigned char CRC;//, RegVal, TXAddr, EEAddr,i;
		int InfoTab, i; //helm for infoframe table index 050721

		//hdmi_tx_regs->hdmi_infoframe_ctrl1|=0x0013; //set AVI enable

		//InfoTab = Get_InfoTab();
		InfoTab=0;//480p gray bar rgb 444
		//InfoTab=1;//480p gray bar ycbcr 422
		//InfoTab=2;//480p gray bar ycbcr 444
		//InfoTab=3;//480p color bar rgb 444
		//InfoTab=4;//480p color bar ycbcr 422
		//InfoTab=5;//480p color bar ycbcr 444

		hdmi_tx_printf("<SendAVIInfoFrame>\n");
		hdmi_tx_printf("InfoTab= %x \n",InfoTab);

		CRC = 0x82 + 0x02 + 13;     //helm: CRC plus header 050727

#ifdef SUPPORT_PREFER_COLOR_SPACE
		/* RGB/YCbCr4:4:4 */
		if (RGB_YCbCr)
				CRC += 0x5c;
		else
				CRC += 0x1c;

		for(i = 5; i < 17; i++ ) //helm for InfoTab 050721
				CRC += hdmi_avi_buf[InfoTab][i];
#else
	#if 0
	if((edid_data[8]==0x42)&&(edid_data[9]==0x49))
	{ //spike 20080619 SUPPORT_PRIMA_TV_1080P
		if (RGB_YCbCr)
			CRC += 0x5c;
		else
			CRC += 0x1c;

		for(i = 5; i < 17; i++ ) //helm for InfoTab 050721
			CRC += hdmi_avi_buf[InfoTab][i];

	}
	else //NOT PRIMA_TV
	#endif
	{
		for(i = 4; i < 17; i++ ) //helm for InfoTab 050721
			CRC += hdmi_avi_buf[InfoTab][i];
	}

#endif

		CRC = 0x100 - CRC;

		//temp_9190=read_hdmi13_reg(SUB_9190_2,INF_CTRL1);
		//temp_9190 &= (~BIT_AVI_REPEAT);
		//write_hdmi13_reg(SUB_9190_2, INF_CTRL1, temp_9190);

	//temp_385_0=regs0->hdmi_infoframe_ctrl1;
		//temp_385_0 |= (1<<aviinfo_pkt_en);
		//temp_385_0 &= ~(1<<aviinfo_pkt_rpt);
	//hdmi_tx_regs->hdmi_infoframe_ctrl1=temp_385_0;

#ifdef SUPPORT_PREFER_COLOR_SPACE
		//for (i=0 ; i<3 ; i++) //helm for AVI header 050721
		//     write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+i, hdmi_avi_buf[InfoTab][i]);
		///* Write CRC */
		//write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+3, CRC);

	//regs0->hdmi_avi_infoframe01=hdmi_avi_buf[InfoTab][0]+hdmi_avi_buf[InfoTab][1]<<8;
	//regs0->hdmi_avi_infoframe23=hdmi_avi_buf[InfoTab][2]+hdmi_avi_buf[InfoTab][3]<<8;

	//for (i=0 ; i<2 ; i++)
	//  hdmi_tx_regs->hdmi_avi_infoframe01+4*i=hdmi_avi_buf[InfoTab][i*2]+hdmi_avi_buf[InfoTab][i*2+1]<<8;


	//hdmi_tx_regs->hdmi_universal_infoframe_hb01=hdmi_avi_buf[InfoTab][0]+(hdmi_avi_buf[InfoTab][1]<<8);
	//hdmi_tx_regs->hdmi_universal_infoframe_hb2=hdmi_avi_buf[InfoTab][2];

	/* Write CRC */
	hdmi_tx_regs->hdmi_avi_infoframe01=CRC;

#else
		#if 0
		if((edid_data[8]==0x42)&&(edid_data[9]==0x49))
	{ //spike 20080619 SUPPORT_PRIMA_TV_1080P
			//for (i=0 ; i<3 ; i++) //helm for AVI header 050721
			//     write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+i, hdmi_avi_buf[InfoTab][i]);
			///* Write CRC */
			//write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+3, CRC);
		//for (i=0 ; i<2 ; i++)
		//  hdmi_tx_regs->hdmi_avi_infoframe01+4*i=hdmi_avi_buf[InfoTab][i*2]+hdmi_avi_buf[InfoTab][i*2+1]<<8;

		//hdmi_tx_regs->hdmi_universal_infoframe_hb01=hdmi_avi_buf[InfoTab][0]+(hdmi_avi_buf[InfoTab][1]<<8);
		//hdmi_tx_regs->hdmi_universal_infoframe_hb2=hdmi_avi_buf[InfoTab][2];

		/* Write CRC */
		hdmi_tx_regs->hdmi_avi_infoframe01=CRC;
		}
		else //NOT PRIMA_TV
		#endif
		{
			//for (i=0 ; i<4 ; i++) //helm for AVI header 050721
				//  write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+i, hdmi_avi_buf[InfoTab][i]);
		//for (i=0 ; i<2 ; i++)
		//  hdmi_tx_regs->hdmi_avi_infoframe01+4*i=hdmi_avi_buf[InfoTab][i*2]+hdmi_avi_buf[InfoTab][i*2+1]<<8;
		//hdmi_tx_regs->hdmi_universal_infoframe_hb01=hdmi_avi_buf[InfoTab][0]+(hdmi_avi_buf[InfoTab][1]<<8);
		//hdmi_tx_regs->hdmi_universal_infoframe_hb2=hdmi_avi_buf[InfoTab][2];

		}

#endif


#ifdef SUPPORT_PREFER_COLOR_SPACE
		//if (RGB_YCbCr)
		//    write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+4, 0x5c);
		//else
		//    write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+4, 0x1c);

		//for(i = 5; i < 17; i++ )    //helm for AVI info table 050721
		//    write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+i, hdmi_avi_buf[InfoTab][i]);

	if (RGB_YCbCr)
		hdmi_tx_regs->hdmi_avi_infoframe01=CRC+(hdmi_avi_buf[InfoTab][4]<<8);
	else
		hdmi_tx_regs->hdmi_avi_infoframe01=CRC+(hdmi_avi_buf[InfoTab][4]<<8);

	//for (i=0 ; i<6 ; i++)
	//  hdmi_tx_regs->hdmi_avi_infoframe67+4*i=hdmi_avi_buf[InfoTab][6+i*2]+hdmi_avi_buf[InfoTab][6+i*2+1]<<8;
	hdmi_tx_regs->hdmi_avi_infoframe23=hdmi_avi_buf[InfoTab][5]+(hdmi_avi_buf[InfoTab][6]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe45=hdmi_avi_buf[InfoTab][7]+(hdmi_avi_buf[InfoTab][8]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe67=hdmi_avi_buf[InfoTab][9]+(hdmi_avi_buf[InfoTab][10]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe89=hdmi_avi_buf[InfoTab][11]+(hdmi_avi_buf[InfoTab][12]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe1011=hdmi_avi_buf[InfoTab][13]+(hdmi_avi_buf[InfoTab][14]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe1213=hdmi_avi_buf[InfoTab][15]+(hdmi_avi_buf[InfoTab][16]<<8);

#else
	#if 0
	if ((edid_data[8]==0x42)&&(edid_data[9]==0x49))
	{ //spike 20080619 SUPPORT_PRIMA_TV_1080P
		//if (RGB_YCbCr)
		//  write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+4, 0x5c);
		//else
		//  write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+4, 0x1c);
		//for(i = 5; i < 17; i++ )    //helm for AVI info table 050721
		//  write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+i, hdmi_avi_buf[InfoTab][i]);
		if (RGB_YCbCr)
			//regs0->hdmi_avi_infoframe45=0x5c+hdmi_avi_buf[InfoTab][5]<<8;
			hdmi_tx_regs->hdmi_avi_infoframe01=CRC+(hdmi_avi_buf[InfoTab][4]<<8);
		else
			//regs0->hdmi_avi_infoframe45=0x1c+hdmi_avi_buf[InfoTab][5]<<8;
			hdmi_tx_regs->hdmi_avi_infoframe01=CRC+(hdmi_avi_buf[InfoTab][4]<<8);

		//for (i=0 ; i<6 ; i++)
		//  regs0->hdmi_avi_infoframe67+4*i=hdmi_avi_buf[InfoTab][6+i*2]+hdmi_avi_buf[InfoTab][6+i*2+1]<<8;
		hdmi_tx_regs->hdmi_avi_infoframe23=hdmi_avi_buf[InfoTab][5]+(hdmi_avi_buf[InfoTab][6]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe45=hdmi_avi_buf[InfoTab][7]+(hdmi_avi_buf[InfoTab][8]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe67=hdmi_avi_buf[InfoTab][9]+(hdmi_avi_buf[InfoTab][10]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe89=hdmi_avi_buf[InfoTab][11]+(hdmi_avi_buf[InfoTab][12]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe1011=hdmi_avi_buf[InfoTab][13]+(hdmi_avi_buf[InfoTab][14]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe1213=hdmi_avi_buf[InfoTab][15]+(hdmi_avi_buf[InfoTab][16]<<8);


	}
	else //NOT PRIMA_TV
	#endif
	{
		//for(i = 4; i < 17; i++ )    //helm for AVI info table 050721
		//  write_hdmi13_reg(SUB_9190_2, AVI_IF_ADDR+i, hdmi_avi_buf[InfoTab][i]);

		//for(i = 0; i < 7; i++ )
		//  hdmi_tx_regs->hdmi_avi_infoframe45+4*i=hdmi_avi_buf[InfoTab][4+i*2]+hdmi_avi_buf[InfoTab][4+i*2+1]<<8;

		hdmi_tx_regs->hdmi_avi_infoframe01=CRC+(hdmi_avi_buf[InfoTab][4]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe23=hdmi_avi_buf[InfoTab][5]+(hdmi_avi_buf[InfoTab][6]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe45=hdmi_avi_buf[InfoTab][7]+(hdmi_avi_buf[InfoTab][8]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe67=hdmi_avi_buf[InfoTab][9]+(hdmi_avi_buf[InfoTab][10]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe89=hdmi_avi_buf[InfoTab][11]+(hdmi_avi_buf[InfoTab][12]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe1011=hdmi_avi_buf[InfoTab][13]+(hdmi_avi_buf[InfoTab][14]<<8);
		hdmi_tx_regs->hdmi_avi_infoframe1213=hdmi_avi_buf[InfoTab][15]+(hdmi_avi_buf[InfoTab][16]<<8);

	}
#endif



}



void AVIInfoFrame(int info_idx)
{
	unsigned char CRC = 0;
	int i = 0;

	hdmi_tx_printf("<SendAVIInfoFrame>\n");
	hdmi_tx_printf("info_idx = %d \n",info_idx);

	CRC = 0x82 + 0x02 + 13;
	for(i = 4; i < 17; i++ )
		CRC += hdmi_avi_buf[info_idx][i];
	CRC = 0x100 - CRC;

	hdmi_tx_regs->hdmi_avi_infoframe01=CRC+(hdmi_avi_buf[info_idx][4]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe23=hdmi_avi_buf[info_idx][5]+(hdmi_avi_buf[info_idx][6]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe45=hdmi_avi_buf[info_idx][7]+(hdmi_avi_buf[info_idx][8]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe67=hdmi_avi_buf[info_idx][9]+(hdmi_avi_buf[info_idx][10]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe89=hdmi_avi_buf[info_idx][11]+(hdmi_avi_buf[info_idx][12]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe1011=hdmi_avi_buf[info_idx][13]+(hdmi_avi_buf[info_idx][14]<<8);
	hdmi_tx_regs->hdmi_avi_infoframe1213=hdmi_avi_buf[info_idx][15]+(hdmi_avi_buf[info_idx][16]<<8);

}

#if 0
void sendAUDInfoFrame(void)
{
	BYTE CRC,i, DataByte;
	BYTE TMP[10];
	hdmi_tx_printf("<sendAUDInfoFrame>\n");

	//hdmi_tx_regs->hdmi_infoframe_ctrl1|=0x1300; //set audio enable

	CRC = 0x84 + 0x01 + 0x0A;


	TMP[0] = 0x11;               //write 0x84 Byte 1 2 channels; IEC60958

	TMP[1] = 0x0F;    // Update DataByte 2  24bit 48KHz

	//TMP[1] = 0x07;    // Update DataByte 2  24bit 32KHz
	//TMP[1] = 0x0B;    // Update DataByte 2  24bit 44.1KHz
	//TMP[1] = 0x13;    // Update DataByte 2  24bit 88.2KHz
	//TMP[1] = 0x17;    // Update DataByte 2  24bit 96KHz
	//TMP[1] = 0x1B;    // Update DataByte 2  24bit 176.4KHz
	//TMP[1] = 0x1F;    // Update DataByte 2  24bit 192KHz

	DataByte = 0;
	TMP[2]= DataByte;   // Update DataByte 3

	TMP[3] = 0x00;

	DataByte = 0;
	TMP[4]= DataByte;   // Update DataByte 5

	for (i=0;i<5;i++)
		TMP[5+i]= 0;    // Update DataByte 6 ~ 10

	for(i=0;i<10;i++)
	{
		CRC = CRC + TMP[i];
	}


	CRC = 0x100 - CRC;
	hdmi_tx_regs->hdmi_audio_infoframe01=CRC+(TMP[0]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe23=TMP[1]+(TMP[2]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe45=TMP[3]+(TMP[4]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe67=TMP[5]+(TMP[6]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe89=TMP[7]+(TMP[8]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe1011=TMP[9];

}

#else
void sendAUDInfoFrame(void)
{
	unsigned char CRC,i, layout, DataByte;
	unsigned char TMP[10];
	hdmi_tx_printf("<sendAUDInfoFrame>\n");

	//hdmi_tx_regs->hdmi_infoframe_ctrl1|=0x1300; //set audio enable

	CRC = 0x84 + 0x01 + 0x0A;

	/* Audio Channel Count (CC2, CC1, CC0) */
	//don't modify LAYOUT bit, thus it can judge HDMI and DVI, helm 051012
	//layout = read_hdmi13_reg(0x7a, 0x2f)&0xfd;

	layout=hdmi_tx_regs->hdmi_audio_ctrl1&0xfd;

	//configure to spdif(2-channel), helm 051005
	//write_hdmi13_reg(0x7a,0x2f, layout);
	hdmi_tx_regs->hdmi_audio_ctrl1=layout;

	//layout = (read_hdmi13_reg(0x7a, 0x2f)&0x02)>>1;
	layout = (hdmi_tx_regs->hdmi_audio_ctrl1&0x02)>>1;

	if (layout)
	{
		//DataByte = (read_hdmi13_reg(0x7a, 0x20))>>4;
		DataByte = ((hdmi_tx_regs->hdmi_audio_chnl_sts2)&0x00F0)>>4;
		if (DataByte==0)
			DataByte = 0;
		else
			DataByte = DataByte - 1;
	}
	else
	{
		DataByte = 0x01;  // 2 channel
	}
	/* Audio Coding Type (CT3, CT2, CT1, CT0) */
	// DataByte &= ~0xF0;

	if (layout > 0)
		TMP[0] = 0x05;               //write 0x84 Byte 1
	else
		TMP[0] = 0x01;


	DataByte = 0;
	TMP[1]= DataByte;   // Update DataByte 2
	//TMP[1] = 0x0F;    // Update DataByte 2  24bit 48khz
	//TMP[1] = 0x07;    // Update DataByte 2  24bit 32khz
	//TMP[1] = 0x0B;    // Update DataByte 2  24bit 44.1khz
	//TMP[1] = 0x13;    // Update DataByte 2  24bit 88.2khz
	//TMP[1] = 0x17;    // Update DataByte 2  24bit 96khz
	//TMP[1] = 0x1B;    // Update DataByte 2  24bit 176.4khz
	//TMP[1] = 0x1F;    // Update DataByte 2  24bit 192khz

	DataByte = 0;
	TMP[2]= DataByte;   // Update DataByte 3

	/* Audio Channel Number (CA7 ~ CA0) */
	if (layout)
	{
		//switch((read_hdmi13_reg(0x7a, 0x20))>>4)
		switch(((hdmi_tx_regs->hdmi_audio_chnl_sts2)&0x00F0)>>4)
		{
			case 0:
			case 2: DataByte = 0x00;  break;
			case 3: DataByte = 0x01;  break;
			case 4: DataByte = 0x03;  break;
			case 5: DataByte = 0x07;  break;
			case 6: DataByte = 0x0B;  break;
			case 7: DataByte = 0x0F;  break;
			case 8: DataByte = 0x13;  break;
		}
	}
	else
	{
		DataByte = 0; // 2 channel
	}

	//printf("Channel Number [%x]\n", (read_hdmi13_reg(0x7a, 0x20))>>4);

	if (layout == 1)
		TMP[3] = 0x0B;    // Update DataByte 4
	else
		TMP[3] = 0x00;

	DataByte = 0;
	TMP[4]= DataByte;   // Update DataByte 5

	for (i=0;i<5;i++)
		TMP[5+i]= 0;    // Update DataByte 6 ~ 10

	//TXAddr = 0x80;
	//write_hdmi13_reg( 0x7a, TXAddr++ , 0x84);
	//write_hdmi13_reg( 0x7a, TXAddr++ , 0x01);
	//write_hdmi13_reg( 0x7a, TXAddr++ , 0x0A);

	//TXAddr++;

	//for(i=0;i<10;i++)
	//{
	//  write_hdmi13_reg( 0x7a, TXAddr++ , TMP[i]);
	//  CRC = CRC + TMP[i];
	//}
	for(i=0;i<10;i++)
	{
		CRC = CRC + TMP[i];
	}


	CRC = 0x100 - CRC;
	//write_hdmi13_reg( 0x7a, 0x83 , CRC);    // Send Header
	hdmi_tx_regs->hdmi_audio_infoframe01=CRC+(TMP[0]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe23=TMP[1]+(TMP[2]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe45=TMP[3]+(TMP[4]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe67=TMP[5]+(TMP[6]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe89=TMP[7]+(TMP[8]<<8);
	hdmi_tx_regs->hdmi_audio_infoframe1011=TMP[9];

}
#endif

void hdmitx_config_color_space_conversion(enum hal_hdmi_tx_color_space_e in_cs,
											enum hal_hdmi_tx_color_space_e out_cs,
											enum hal_hdmi_tx_pixel_format_e in_pixel_fmt,
											enum hal_hdmi_tx_pixel_format_e out_pixel_fmt)
{
	unsigned int value = 0, mode = 0, mask = ((0x3 << 11) | (0x3f << 4));
	unsigned char is_up = DISABLE, is_down = DISABLE;

	if (in_pixel_fmt == HDMI_TX_PIXEL_FORMAT_YUV422)
	{
		is_up = ENABLE;
	}

	if (out_pixel_fmt == HDMI_TX_PIXEL_FORMAT_YUV422)
	{
		is_down = ENABLE;
	}

	value = hdmi_tx_regs->hdmi_video_ctrl1 & (~mask);

	mode = ((g_csc_cfg[in_cs][out_cs].ycc_range & 0x1)
				| ((g_csc_cfg[in_cs][out_cs].rgb_range & 0x1) << 1)
				| ((g_csc_cfg[in_cs][out_cs].conversion_type & 0x1) << 2)
				| ((g_csc_cfg[in_cs][out_cs].colorimetry & 0x1) << 3));

	value |= (((mode & 0xf) << 4)
				| ((g_csc_cfg[in_cs][out_cs].on_off & 0x1) << 8)
				| ((g_csc_cfg[in_cs][out_cs].manual & 0x1) << 9));

	value |= ((is_up << 11) | (is_down << 12));

	hdmi_tx_regs->hdmi_video_ctrl1 = value;

	if (g_csc_cfg[in_cs][out_cs].manual)
	{
		// todo: need to config coefficient
		// limited RGB to full RGB
		// limited YUV to full YUV
		// full RGB to limited RGB
		// full YUV to limited YUV
	}
}

void HDMI_TX_480P(unsigned int color_depth,
					enum hal_hdmi_tx_color_space_e in_cs,
					enum hal_hdmi_tx_color_space_e out_cs,
					enum hal_hdmi_tx_pixel_format_e pixel_fmt)
{
#ifdef HDMI_TX_PTG
	int yc444_422 = 0, rgb_yc = 0;
#endif
	unsigned short info_idx = 0;
	enum hal_hdmi_tx_pixel_format_e out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;

	hdmi_tx_printf("HDMI_TX_480P\n");
	hdmi_tx_printf("Deep mode     = %d (4:8 bits, 5:10 bits, 6:12 bits, 7:16 bits)\n", color_depth);
	hdmi_tx_printf("in_cs         = %d (0:limit rgb, 1:limited yuv, 2:full rgb, 3:full yuv)\n", in_cs);
	hdmi_tx_printf("out_cs        = %d (0:limit rgb, 1:limited yuv, 2:full rgb, 3:full yuv)\n", out_cs);
	hdmi_tx_printf("pixel_fmt     = %d (0:rgb, 1:yuv444, 2:yuv422)\n", pixel_fmt);


	hdmitx_clk(TP_480P);
	hdmitx_sys_init();

#ifdef HDMI_TX_PTG
	switch (pixel_fmt)
	{
		case HDMI_TX_PIXEL_FORMAT_YUV444:
			yc444_422 = 1;
			rgb_yc    = 0;
			break;
		case HDMI_TX_PIXEL_FORMAT_YUV422:
			yc444_422 = 0;
			rgb_yc    = 0;
			break;
		case HDMI_TX_PIXEL_FORMAT_RGB:
		default:
			yc444_422 = 0;
			rgb_yc    = 1;
			break;
	}
	hdmitx_tp_init(0, TP_480P, 0, 0, yc444_422, rgb_yc, 3);
#endif

	switch (pixel_fmt)
	{
		case HDMI_TX_PIXEL_FORMAT_YUV444:
			if ((out_cs == HDMI_TX_COLOR_SPACE_LIMITED_RGB)
				|| (out_cs == HDMI_TX_COLOR_SPACE_FULL_RGB))
			{
				info_idx      = 0;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			else
			{
				info_idx      = 2;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV444;
			}
			break;
		case HDMI_TX_PIXEL_FORMAT_YUV422:
			if ((out_cs == HDMI_TX_COLOR_SPACE_LIMITED_RGB)
				|| (out_cs == HDMI_TX_COLOR_SPACE_FULL_RGB))
			{
				info_idx      = 0;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			else
			{
				info_idx      = 1;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV422;
			}
			break;
		case HDMI_TX_PIXEL_FORMAT_RGB:
		default:
			if (out_cs == HDMI_TX_COLOR_SPACE_LIMITED_YUV)
			{
				info_idx      = 2;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV444;
			}
			else if (out_cs == HDMI_TX_COLOR_SPACE_FULL_YUV)
			{
				info_idx      = 1;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV422;
			}
			else
			{
				info_idx      = 0;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			break;
	}

	hdmitx_config_color_space_conversion(in_cs, out_cs, pixel_fmt, out_pixel_fmt);
	sendAUDInfoFrame();
	AVIInfoFrame(info_idx);

	hdmitx_video_cfg(color_depth, 0x3, HDMI_MODE);
	hdmitx_audio_cfg(0x1, 0, 0xf, 0x0, 0x1, 0x0, 0x1800);

	hdmi_tx_regs->hdmi_infoframe_ctrl1|=0x1b1b;
	hdmi_tx_regs->hdmi_tmdstx_ctrl3=(1<<8 | 1<<4 | 1<<2 | 1) ;
}

void DVI_TX_480P(void)
{

	hdmitx_clk(TP_480P);
	hdmitx_sys_init();

	hdmitx_tp_init(0, TP_480P, 0, 0, 1, 1, 3);

	sendAUDInfoFrame();
	SendAVIInfoFrame();

	hdmitx_video_cfg(DC_8B, 0x3, DVI_MODE);
	//hdmitx_audio_cfg(1, 0, 0xf, 0x0, 0x1, 0x0); //for i2s
			hdmitx_audio_cfg(0, 0, 0xf, 0x0, 0x1, 0x0, 0x1800);//for spidf

	//regs2->G387_HDMI_TX[28] = (1<<8 | 1<<4 | 1<<2 | 1) ;
	hdmi_tx_regs->hdmi_tmdstx_ctrl3=(1<<8 | 1<<4 | 1<<2 | 1) ;

	//regs0->stamp  = 0x3333;

}




void HDMI_TX_576P(unsigned int color_depth)
{
#ifdef HDMI_TX_PTG
	unsigned int w_data = 0;
#endif
	hdmi_tx_printf("HDMI_TX_576P\n");
	hdmi_tx_printf("Deep mode = %d (4:8 bits, 5:10 bits, 6:12 bits, 7:16 bits)\n", color_depth);
	hdmitx_clk(TP_576P);
	hdmitx_sys_init();
			//w_data = (0<<11 | 3<<8 | 0<<6 | 0<<5 | 1<<4 | 1<<3 | 3<<1 | 1);
	/*hdmi_tx_regs->hdmi_video_pat_gen1=w_data;
	hdmi_tx_regs->hdmi_video_pat_gen2=0x35F;
	hdmi_tx_regs->hdmi_video_pat_gen3=0x270;
	hdmi_tx_regs->hdmi_video_pat_gen4=0x3F;
	hdmi_tx_regs->hdmi_video_pat_gen5=0x4;
	hdmi_tx_regs->hdmi_video_pat_gen6=0x83;
	hdmi_tx_regs->hdmi_video_pat_gen7=0x353;
	hdmi_tx_regs->hdmi_video_pat_gen8=0x2D;
	hdmi_tx_regs->hdmi_video_pat_gen9=0x26D;
	AVIInfoFrame(TP_576P);

	*/

#ifdef HDMI_TX_PTG
	w_data = (0<<11 | 7<<8 | 0<<6 | 0<<5 | 1<<4 | 1<<3 | 3<<1 |1);
	hdmi_tx_regs->hdmi_video_pat_gen1=w_data;
	hdmi_tx_regs->hdmi_video_pat_gen2=0x360;
	hdmi_tx_regs->hdmi_video_pat_gen3=0x271;
	hdmi_tx_regs->hdmi_video_pat_gen4=0x40;
	hdmi_tx_regs->hdmi_video_pat_gen5=0x4;
	hdmi_tx_regs->hdmi_video_pat_gen6=0x84;
	hdmi_tx_regs->hdmi_video_pat_gen7=0x353;
	hdmi_tx_regs->hdmi_video_pat_gen8=0x2c;
	hdmi_tx_regs->hdmi_video_pat_gen9=0x26c;
#endif

	AVIInfoFrame(10);

	sendAUDInfoFrame();

	hdmitx_video_cfg(color_depth, 0x3, HDMI_MODE);
	//hdmitx_audio_cfg(1, 0, 0xf, 0x0, 0x1, 0x0); //for i2s
	hdmitx_audio_cfg(0, 0, 0xf, 0x0, 0x1, 0x0, 0x1800);//for spidf
	hdmi_tx_regs->hdmi_infoframe_ctrl1|=0x1b1b;
	//regs2->G387_HDMI_TX[28] = (1<<8 | 1<<4 | 1<<2 | 1) ;
	hdmi_tx_regs->hdmi_tmdstx_ctrl3=(1<<8 | 1<<4 | 1<<2 | 1) ;

	//regs0->stamp  = 0x3333;

}


void DVI_TX_576P(void)
{
	unsigned int w_data;
	hdmi_tx_printf("DVI_TX_576P");


	hdmitx_sys_init();


	w_data = (0<<11 | 7<<8 | 0<<6 | 0<<5 | 1<<4 | 1<<3 | 3<<1 |1);
	hdmi_tx_regs->hdmi_video_pat_gen1=w_data;
	hdmi_tx_regs->hdmi_video_pat_gen2=0x360;
	hdmi_tx_regs->hdmi_video_pat_gen3=0x271;
	hdmi_tx_regs->hdmi_video_pat_gen4=0x40;
	hdmi_tx_regs->hdmi_video_pat_gen5=0x4;
	hdmi_tx_regs->hdmi_video_pat_gen6=0x84;
	hdmi_tx_regs->hdmi_video_pat_gen7=0x353;
	hdmi_tx_regs->hdmi_video_pat_gen8=0x2c;
	hdmi_tx_regs->hdmi_video_pat_gen9=0x26c;
	sendAUDInfoFrame();
	AVIInfoFrame(TP_576P);
	hdmitx_video_cfg(DC_8B, 0x3, DVI_MODE);
	//hdmitx_audio_cfg(1, 0, 0xf, 0x0, 0x1, 0x0); //for i2s
			hdmitx_audio_cfg(0, 0, 0xf, 0x0, 0x1, 0x0, 0x1800);//for spidf
	//regs2->G387_HDMI_TX[28] = (1<<8 | 1<<4 | 1<<2 | 1) ;
	hdmi_tx_regs->hdmi_tmdstx_ctrl3=(1<<8 | 1<<4 | 1<<2 | 1) ;

	//regs0->stamp  = 0x3333;

}

void HDMI_TX_720P60(unsigned int color_depth,
						enum hal_hdmi_tx_color_space_e in_cs,
						enum hal_hdmi_tx_color_space_e out_cs,
						enum hal_hdmi_tx_pixel_format_e pixel_fmt)
{
#ifdef HDMI_TX_PTG
	int yc444_422 = 0, rgb_yc = 0;
#endif
	unsigned short info_idx = 0;
	enum hal_hdmi_tx_pixel_format_e out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
	unsigned int hv_pol = 0;

	hdmi_tx_printf("HDMI_TX_720P60\n");
	hdmi_tx_printf("Deep mode     = %d (4:8 bits, 5:10 bits, 6:12 bits, 7:16 bits)\n", color_depth);
	hdmi_tx_printf("in_cs         = %d (0:limit rgb, 1:limited yuv, 2:full rgb, 3:full yuv)\n", in_cs);
	hdmi_tx_printf("out_cs        = %d (0:limit rgb, 1:limited yuv, 2:full rgb, 3:full yuv)\n", out_cs);
	hdmi_tx_printf("pixel_fmt     = %d (0:rgb, 1:yuv444, 2:yuv422)\n", pixel_fmt);


	hdmitx_clk(TP_720P);
	hdmitx_sys_init();

#ifdef HDMI_TX_PTG
	switch (pixel_fmt)
	{
		case HDMI_TX_PIXEL_FORMAT_YUV444:
			yc444_422 = 1;
			rgb_yc    = 0;
			break;
		case HDMI_TX_PIXEL_FORMAT_YUV422:
			yc444_422 = 0;
			rgb_yc    = 0;
			break;
		case HDMI_TX_PIXEL_FORMAT_RGB:
		default:
			yc444_422 = 0;
			rgb_yc    = 1;
			break;
	}
	hdmitx_tp_init(0, TP_720P, 0, 0, yc444_422, rgb_yc, hv_pol);
#endif

	switch (pixel_fmt)
	{
		case HDMI_TX_PIXEL_FORMAT_YUV444:
			if ((out_cs == HDMI_TX_COLOR_SPACE_LIMITED_RGB)
				|| (out_cs == HDMI_TX_COLOR_SPACE_FULL_RGB))
			{
				info_idx      = 6;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			else
			{
				info_idx      = 16;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV444;
			}
			break;
		case HDMI_TX_PIXEL_FORMAT_YUV422:
			if ((out_cs == HDMI_TX_COLOR_SPACE_LIMITED_RGB)
				|| (out_cs == HDMI_TX_COLOR_SPACE_FULL_RGB))
			{
				info_idx      = 6;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			else
			{
				info_idx      = 15;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV422;
			}
			break;
		case HDMI_TX_PIXEL_FORMAT_RGB:
		default:
			if (out_cs == HDMI_TX_COLOR_SPACE_LIMITED_YUV)
			{
				info_idx      = 16;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV444;
			}
			else if (out_cs == HDMI_TX_COLOR_SPACE_FULL_YUV)
			{
				info_idx      = 15;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV422;
			}
			else
			{
				info_idx      = 6;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			break;
	}

	hdmitx_config_color_space_conversion(in_cs, out_cs, pixel_fmt, out_pixel_fmt);

	AVIInfoFrame(info_idx);

	sendAUDInfoFrame();

	hdmitx_video_cfg(color_depth, hv_pol, HDMI_MODE);
	hdmitx_audio_cfg(1, 0, 0xf, 0x0, 0x1, 0x0, 0x1800); //for i2s
	// hdmitx_audio_cfg(0, 0, 0xf, 0x0, 0x1, 0x0);//for spidf
	hdmi_tx_regs->hdmi_infoframe_ctrl1|=0x1b1b;
	//regs2->G387_HDMI_TX[28] = (1<<8 | 1<<4 | 1<<2 | 1) ;
	hdmi_tx_regs->hdmi_tmdstx_ctrl3=(1<<8 | 1<<4 | 1<<2 | 1) ;

	//regs0->stamp  = 0x3333;

}

void HDMI_TX_1080P60(unsigned int color_depth,
						enum hal_hdmi_tx_color_space_e in_cs,
						enum hal_hdmi_tx_color_space_e out_cs,
						enum hal_hdmi_tx_pixel_format_e pixel_fmt)
{
#ifdef HDMI_TX_PTG
	int yc444_422 = 0, rgb_yc = 0;
#endif
	unsigned short info_idx = 0;
	enum hal_hdmi_tx_pixel_format_e out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
	unsigned int hv_pol = 0;

	hdmi_tx_printf("HDMI_TX_1080P60\n");
	hdmi_tx_printf("Deep mode     = %d (4:8 bits, 5:10 bits, 6:12 bits, 7:16 bits)\n", color_depth);
	hdmi_tx_printf("in_cs         = %d (0:limit rgb, 1:limited yuv, 2:full rgb, 3:full yuv)\n", in_cs);
	hdmi_tx_printf("out_cs        = %d (0:limit rgb, 1:limited yuv, 2:full rgb, 3:full yuv)\n", out_cs);
	hdmi_tx_printf("pixel_fmt     = %d (0:rgb, 1:yuv444, 2:yuv422)\n", pixel_fmt);

	hdmitx_clk(TP_1080P);
	hdmitx_sys_init();

#ifdef HDMI_TX_PTG
	switch (pixel_fmt)
	{
		case HDMI_TX_PIXEL_FORMAT_YUV444:
			yc444_422 = 1;
			rgb_yc    = 0;
			break;
		case HDMI_TX_PIXEL_FORMAT_YUV422:
			yc444_422 = 0;
			rgb_yc    = 0;
			break;
		case HDMI_TX_PIXEL_FORMAT_RGB:
		default:
			yc444_422 = 0;
			rgb_yc    = 1;
			break;
	}
	hdmitx_tp_init(0, TP_1080P, 0, 0, yc444_422, rgb_yc, hv_pol);
#endif

	switch (pixel_fmt)
	{
		case HDMI_TX_PIXEL_FORMAT_YUV444:
			if ((out_cs == HDMI_TX_COLOR_SPACE_LIMITED_RGB)
				|| (out_cs == HDMI_TX_COLOR_SPACE_FULL_RGB))
			{
				info_idx      = 9;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			else
			{
				info_idx      = 18;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV444;
			}
			break;
		case HDMI_TX_PIXEL_FORMAT_YUV422:
			if ((out_cs == HDMI_TX_COLOR_SPACE_LIMITED_RGB)
				|| (out_cs == HDMI_TX_COLOR_SPACE_FULL_RGB))
			{
				info_idx      = 9;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			else
			{
				info_idx      = 17;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV422;
			}
			break;
		case HDMI_TX_PIXEL_FORMAT_RGB:
		default:
			if (out_cs == HDMI_TX_COLOR_SPACE_LIMITED_YUV)
			{
				info_idx      = 18;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV444;
			}
			else if (out_cs == HDMI_TX_COLOR_SPACE_FULL_YUV)
			{
				info_idx      = 17;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_YUV422;
			}
			else
			{
				info_idx      = 9;
				out_pixel_fmt = HDMI_TX_PIXEL_FORMAT_RGB;
			}
			break;
	}

	hdmitx_config_color_space_conversion(in_cs, out_cs, pixel_fmt, out_pixel_fmt);

	AVIInfoFrame(info_idx);

	sendAUDInfoFrame();

	hdmitx_video_cfg(color_depth, hv_pol, HDMI_MODE);
	hdmitx_audio_cfg(1, 0, 0xf, 0x0, 0x1, 0x0, 0x1800); //for i2s
	// hdmitx_audio_cfg(0, 0, 0xf, 0x0, 0x1, 0x0);//for spidf
	hdmi_tx_regs->hdmi_infoframe_ctrl1|=0x1b1b;
	//regs2->G387_HDMI_TX[28] = (1<<8 | 1<<4 | 1<<2 | 1) ;
	hdmi_tx_regs->hdmi_tmdstx_ctrl3=(1<<8 | 1<<4 | 1<<2 | 1) ;

	//regs0->stamp  = 0x3333;

}

void HDMI_TX_DISABLE(void)
{
	unsigned int read_data;

	read_data=hdmi_tx_regs->hdmi_pwr_ctrl;

	hdmi_tx_printf("[hdmitx] disable....:%x\n", read_data);

	hdmi_tx_regs->hdmi_pwr_ctrl=(read_data&0xffe0);
	//hdmi_tx_regs->hdmi_pwr_ctrl=0x1400 ;

}

// return HDMI mode (DVI/HDMI)
// return -1, if error
int HDMI_TX_MODE(unsigned char* pEdidData)
{
		unsigned char hdmi_mode = eHDMI_DVI_mode; /*1 hdmi, 0 dvi*/

		if (pEdidData == NULL)
		{
				hdmi_tx_printf_E("HDMI_TX_MODE err\n");
				return -1;
		}

		if(pEdidData[EXT_FLAG_OFFSET])
		{
				int offset = EDID_CEA_EXT_OFFSET;
		int start_of_dtd = 0;//start of Detailed Timing Description
				int first_extension_offset=0;
				unsigned char i =0;
				if (pEdidData[EXT_FLAG_OFFSET] > 1)
				{
						if(pEdidData[EXT_TAG_OFFSET] == EXT_BLOCK_MAP_TAG)
								first_extension_offset = EDID_CEA_EXT_OFFSET;
						if(pEdidData[EXT_TAG_OFFSET*2] == EXT_BLOCK_MAP_TAG)
								first_extension_offset = EDID_CEA_EXT_OFFSET*2;
				}

				offset += first_extension_offset;

		start_of_dtd = pEdidData[offset + 2];//Detailed Timing Description

				offset += 4;

				for(i = 0; i < start_of_dtd - 4; i++)
				{
						//Vendor-specific Data Block (VSDB)
						if((pEdidData[offset]&0xe0) == 0x60)
						{
								/* judge HDMI mode or DVI mode*/
								if((pEdidData[offset]&0xf0) >= 5)
								{
										if((pEdidData[offset+1] == 0x03)
												&&(pEdidData[offset+2] == 0x0c)
												&&(pEdidData[offset+3] == 0x00)
												)
										{
												/* HDMI mode  */
												hdmi_mode = eHDMI_HDMI_mode;
												break;
										}
										else
										{
												/* DVI mode*/
												hdmi_mode = eHDMI_DVI_mode;
												break;
										}
								}
						}
						offset++;
				}
		}

		return hdmi_mode;
}

// return TV resolution (480p/576p/auto)
// return -1, if error
int GET_TV_RES(unsigned char* pEdidData)
{
	int tv_resolution = eHDMI_TV_RES_480p;
	int offset = EDID_CEA_EXT_OFFSET;
	int first_extension_offset = 0;
	unsigned char tv_support_480p = 0;
	unsigned char tv_support_576p = 0;
	unsigned char i = 0;
	unsigned char start_of_dtd = 0;
	unsigned char video_data_length = 0;
	unsigned char vedio_ID_code = 0;

		if (pEdidData == NULL)
		{
				hdmi_tx_printf_E("GET_TV_RES err\n");
				return -1;
		}

		if(pEdidData[EXT_FLAG_OFFSET])
		{

				if (pEdidData[EXT_FLAG_OFFSET] > 1)
				{
						if(pEdidData[EXT_TAG_OFFSET] == EXT_BLOCK_MAP_TAG)
								first_extension_offset = EDID_CEA_EXT_OFFSET;
						if(pEdidData[EXT_TAG_OFFSET*2] == EXT_BLOCK_MAP_TAG)
								first_extension_offset = EDID_CEA_EXT_OFFSET*2;
				}

				offset += first_extension_offset ;

				start_of_dtd = pEdidData[offset+2];//Detailed Timing Description

				offset += 4;

				for(i = 0; i < start_of_dtd - 4; i++)
				{
						//Video Data Block
						if((pEdidData[offset] & 0xe0) == 0x40)
						{
								video_data_length = pEdidData[offset] & 0x1f;
				for(i = 0; i < video_data_length; i++)
				{
					offset++;
					vedio_ID_code = pEdidData[offset] & 0x7f;
					if((vedio_ID_code == 2)||(vedio_ID_code == 3))
					{
						tv_support_480p = 1;
						tv_resolution = eHDMI_TV_RES_480p;
					}
					else if((vedio_ID_code == 17)||(vedio_ID_code == 18))
					{
						tv_support_576p = 1;
						tv_resolution = eHDMI_TV_RES_576p;
					}

					if((tv_support_480p == 1) && (tv_support_576p== 1))
					{
						tv_resolution = eHDMI_TV_RES_both;
						hdmi_tx_printf0("tv_resolution support 480/576p\n");
						break;
					}
				}
				break;
						}
						offset++;
				}
		}

	return tv_resolution;
}

void hal_hdmi_tx_disable(void)
{

}

void hal_hdmi_tx_config_video(struct hal_hdmi_tx_video_attr_t *video_attr)
{
	g_video_attr.timing          = video_attr->timing;
	g_video_attr.color_depth     = video_attr->color_depth;
	g_video_attr.in_color_space  = video_attr->in_color_space;
	g_video_attr.out_color_space = video_attr->out_color_space;
	g_video_attr.pixel_fmt       = video_attr->pixel_fmt;
}

void hal_hdmi_tx_enable(void)
{
	unsigned int color_depth = 0;

	switch (g_video_attr.color_depth)
	{
		case HDMI_TX_COLOR_DEPTH_30BITS:
			color_depth = DC_10B;
			break;
		case HDMI_TX_COLOR_DEPTH_36BITS:
			color_depth = DC_12B;
			break;
		case HDMI_TX_COLOR_DEPTH_48BITS:
			color_depth = DC_16B;
			break;
		case HDMI_TX_COLOR_DEPTH_24BITS:
		default:
			color_depth = DC_8B;
			break;
	}

	switch (g_video_attr.color_depth)
	{
		case HDMI_TX_COLOR_DEPTH_30BITS:
			color_depth = DC_10B;
			break;
		case HDMI_TX_COLOR_DEPTH_36BITS:
			color_depth = DC_12B;
			break;
		case HDMI_TX_COLOR_DEPTH_48BITS:
			color_depth = DC_16B;
			break;
		case HDMI_TX_COLOR_DEPTH_24BITS:
		default:
			color_depth = DC_8B;
			break;
	}

	switch (g_video_attr.timing)
	{
		case HDMI_TX_TIMING_576P:
			HDMI_TX_576P(color_depth);
			break;
		case HDMI_TX_TIMING_720P50:
			break;
		case HDMI_TX_TIMING_720P60:
			HDMI_TX_720P60(color_depth,
							g_video_attr.in_color_space,
							g_video_attr.out_color_space,
							g_video_attr.pixel_fmt);
			break;
		case HDMI_TX_TIMING_1080P50:
			break;
		case HDMI_TX_TIMING_1080P60:
			HDMI_TX_1080P60(color_depth,
							g_video_attr.in_color_space,
							g_video_attr.out_color_space,
							g_video_attr.pixel_fmt);
			break;
		case HDMI_TX_TIMING_480P:
		default:
			HDMI_TX_480P(color_depth,
							g_video_attr.in_color_space,
							g_video_attr.out_color_space,
							g_video_attr.pixel_fmt);
			break;
	}
}
