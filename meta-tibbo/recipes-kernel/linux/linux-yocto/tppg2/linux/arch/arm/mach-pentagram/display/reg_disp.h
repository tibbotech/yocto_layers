#ifndef __REG_DISP_H__
#define __REG_DISP_H__
/**
 * @file reg_disp.h
 * @brief 
 * @author PoChou Chen
 */
#define STATIC_ASSERT(b) extern int _static_assert[ b ? 1 : -1 ]

typedef unsigned char	UINT8;
typedef char			INT8;
typedef unsigned short	UINT16;
typedef short			INT16;
typedef int				INT32;
typedef unsigned int	UINT32;
typedef unsigned char	BOOL;

typedef struct _DISP_DDFCH_REG_ {
	UINT32 ddfch_latch_en					; // 00
	UINT32 ddfch_mode_option				; // 01
	UINT32 ddfch_enable						; // 02
	UINT32 ddfch_urgent_thd					; // 03
	UINT32 ddfch_cmdq_thd					; // 04
	UINT32 g185_reserved0					; // 05
	UINT32 ddfch_luma_base_addr_0			; // 06
	UINT32 ddfch_luma_base_addr_1			; // 07
	UINT32 ddfch_luma_base_addr_2			; // 08
	UINT32 ddfch_crma_base_addr_0			; // 09
	UINT32 ddfch_crma_base_addr_1			; // 10
	UINT32 ddfch_crma_base_addr_2			; // 11
	UINT32 g185_reserved1[3]				; // 12-14
	UINT32 ddfch_frame_id					; // 15
	UINT32 ddfch_free_run_control			; // 16
	UINT32 g185_reserved2[3]				; // 17-19
	UINT32 ddfch_vdo_frame_size				; // 20
	UINT32 ddfch_vdo_crop_size				; // 21
	UINT32 ddfch_vdo_crop_offset			; // 22
	UINT32 ddfch_config_0					; // 23
	UINT32 ddfch_config_1					; // 24
	UINT32 g185_reserved3					; // 25
	UINT32 ddfch_checksum_info				; // 26
	UINT32 ddfch_error_flag_info			; // 27
	UINT32 ddfch_bist						; // 28
	UINT32 ddfch_axi_ipbj_info				; // 29
	UINT32 g185_reserved4					; // 30
	UINT32 ddfch_others_info				; // 31
} DISP_DDFCH_REG_t;
STATIC_ASSERT(sizeof(DISP_DDFCH_REG_t) == (32 * 4));

typedef struct _DISP_VPOST_REG_ {
	UINT32 vpost_config1					; // 00
	UINT32 vpost_i_xlen						; // 01
	UINT32 vpost_i_ylen						; // 02
	UINT32 vpost_i_xstart					; // 03
	UINT32 vpost_i_ystart					; // 04
	UINT32 vpost_o_xlen						; // 05
	UINT32 vpost_o_ylen						; // 06
	UINT32 vpost_bgc_1						; // 07
	UINT32 vpost_bgc_2						; // 08
	UINT32 vpost_config2					; // 09
	UINT32 vpost_mas_sla					; // 10
	UINT32 vpost_o_act_xstart				; // 11
	UINT32 vpost_o_act_ystart				; // 12
	UINT32 vpost_chksum_out					; // 13
	UINT32 vpost_error_message				; // 14
	UINT32 vpost_reserved0[17]				; // 15~31
} DISP_VPOST_REG_t;
STATIC_ASSERT(sizeof(DISP_VPOST_REG_t) == (32 * 4));

typedef struct _DISP_OSD_REG_ {
	UINT32 osd_ctrl							; // 00
	UINT32 osd_en							; // 01
	UINT32 osd_base_addr					; // 02
	UINT32 osd_reserved0[3]					; // 03-05
	UINT32 osd_bus_monitor_l				; // 06
	UINT32 osd_bus_monitor_h				; // 07
	UINT32 osd_req_ctrl						; // 08
	UINT32 osd_debug_cmd_lock				; // 09
	UINT32 osd_debug_burst_lock				; // 10
	UINT32 osd_debug_xlen_lock				; // 11
	UINT32 osd_debug_ylen_lock				; // 12
	UINT32 osd_debug_queue_lock				; // 13
	UINT32 osd_crc_chksum					; // 14
	UINT32 osd_reserved1					; // 15
	UINT32 osd_hvld_offset					; // 16
	UINT32 osd_hvld_width					; // 17
	UINT32 osd_vvld_offset					; // 18
	UINT32 osd_vvld_height					; // 19
	UINT32 osd_data_fetch_ctrl				; // 20
	UINT32 osd_bist_ctrl					; // 21
	UINT32 osd_non_fetch_0					; // 22
	UINT32 osd_non_fetch_1					; // 23
	UINT32 osd_non_fetch_2					; // 24
	UINT32 osd_non_fetch_3					; // 25
	UINT32 osd_bus_status					; // 26
	UINT32 osd_3d_h_offset					; // 27
	UINT32 osd_reserved3					; // 28
	UINT32 osd_src_decimation_sel			; // 29
	UINT32 osd_bus_time_0					; // 30
	UINT32 osd_bus_time_1					; // 31
} DISP_OSD_REG_t;
STATIC_ASSERT(sizeof(DISP_OSD_REG_t) == (32 * 4));

typedef struct _DISP_GPOST_REG_ {
	UINT32 gpost0_config					; // 00
	UINT32 gpost0_mskl						; // 01
	UINT32 gpost0_mskr						; // 02
	UINT32 gpost0_mskt						; // 03
	UINT32 gpost0_mskb						; // 04
	UINT32 gpost0_bg1						; // 05
	UINT32 gpost0_bg2						; // 06
	UINT32 gpost0_contrast_config			; // 07
	UINT32 gpost0_adj_src					; // 08
	UINT32 gpost0_adj_des					; // 09
	UINT32 gpost0_adj_slope0				; // 10
	UINT32 gpost0_adj_slope1				; // 11
	UINT32 gpost0_adj_slope2				; // 12
	UINT32 gpost0_adj_bound					; // 13
	UINT32 gpost0_bri_value					; // 14
	UINT32 gpost0_hue_sat_en				; // 15
	UINT32 gpost0_chroma_satsin				; // 16
	UINT32 gpost0_chroma_satcos				; // 17
	UINT32 gpost0_master_en					; // 18
	UINT32 gpost0_master_horizontal			; // 19
	UINT32 gpost0_master_vertical			; // 20
	UINT32 gpost0_reserved0[11]				; // 21-31
} DISP_GPOST_REG_t;
STATIC_ASSERT(sizeof(DISP_GPOST_REG_t) == (32 * 4));

typedef struct _DISP_TGEN_REG_ {
	UINT32 tgen_config						; // 00
	UINT32 tgen_reset						; // 01
	UINT32 tgen_user_int1_config			; // 02
	UINT32 tgen_user_int2_config			; // 03
	UINT32 tgen_dtg_config					; // 04
	UINT32 g213_reserved_1[2]				; // 05-06
	UINT32 tgen_dtg_venc_line_rst_cnt		; // 07
	UINT32 tgen_dtg_total_pixel				; // 08
	UINT32 tgen_dtg_ds_line_start_cd_point	; // 09
	UINT32 tgen_dtg_total_line				; // 10
	UINT32 tgen_dtg_field_end_line			; // 11
	UINT32 tgen_dtg_start_line				; // 12
	UINT32 tgen_dtg_status1					; // 13
	UINT32 tgen_dtg_status2					; // 14
	UINT32 tgen_dtg_status3					; // 15
	UINT32 tgen_dtg_status4					; // 16
	UINT32 tgen_dtg_clk_ratio_low			; // 17
	UINT32 tgen_dtg_clk_ratio_high			; // 18
	UINT32 g213_reserved_2[4]				; // 19-22
	UINT32 tgen_dtg_adjust1					; // 23
	UINT32 tgen_dtg_adjust2					; // 24
	UINT32 tgen_dtg_adjust3					; // 25
	UINT32 tgen_dtg_adjust4					; // 26
	UINT32 tgen_dtg_adjust5					; // 27
	UINT32 g213_reserved_3					; // 28
	UINT32 tgen_source_sel					; // 29
	UINT32 tgen_dtg_field_start_adj_lcnt	; // 30
	UINT32 g213_reserved_4					; // 31
} DISP_TGEN_REG_t;
STATIC_ASSERT(sizeof(DISP_TGEN_REG_t) == (32 * 4));

typedef struct _DISP_DMIX_REG_ {
	UINT32 dmix_config0						; // 00
	UINT32 dmix_config1						; // 01
	UINT32 dmix_config2						; // 02
	UINT32 dmix_plane_alpha					; // 03
	UINT32 dmix_plane_alpha2				; // 04
	UINT32 dmix_plane_alpha3				; // 05
	UINT32 g217_reserved0					; // 06
	UINT32 dmix_chksum						; // 07
	UINT32 dmix_ptg_config					; // 08
	UINT32 dmix_ptg_config_2				; // 09
	UINT32 dmix_ptg_config_3				; // 10
	UINT32 dmix_ptg_config_4				; // 11
	UINT32 dmix_ptg_config_5				; // 12
	UINT32 dmix_ptg_config_6				; // 13
	UINT32 g217_reserved1[2]				; // 14-15
	UINT32 dmix_yc_adjust					; // 16
	UINT32 dmix_luma_cp1					; // 17
	UINT32 dmix_luma_cp2					; // 18
	UINT32 dmix_luma_cp3					; // 19
	UINT32 dmix_luma_slope0					; // 20
	UINT32 dmix_luma_slope1					; // 21
	UINT32 dmix_luma_slope2					; // 22
	UINT32 dmix_luma_slope3					; // 23
	UINT32 dmix_chroma_satcos				; // 24
	UINT32 dmix_chroma_satsin				; // 25
	UINT32 dmix_pix_en_sel					; // 26
	UINT32 g217_reserved2[5]				; // 27~31
} DISP_DMIX_REG_t;
STATIC_ASSERT(sizeof(DISP_DMIX_REG_t) == (32 * 4));

typedef struct _DISP_DVE_REG_ {
	UINT32 dve_vsync_start_top				; // 00
	UINT32 dve_vsync_start_bot				; // 01
	UINT32 dve_vsync_h_point				; // 02
	UINT32 dve_vsync_pd_cnt					; // 03
	UINT32 dve_hsync_start					; // 04
	UINT32 dve_hsync_pd_cnt					; // 05
	UINT32 dve_hdmi_mode_1					; // 06
	UINT32 dve_v_vld_top_start				; // 07
	UINT32 dve_v_vld_top_end				; // 08
	UINT32 dve_v_vld_bot_start				; // 09
	UINT32 dve_v_vld_bot_end				; // 10
	UINT32 dve_de_h_start					; // 11
	UINT32 dve_de_h_end						; // 12
	UINT32 dve_mp_tg_line_0_length			; // 13
	UINT32 dve_mp_tg_frame_0_line			; // 14
	UINT32 dve_mp_tg_act_0_pix				; // 15
	UINT32 dve_hdmi_mode_0					; // 16
	UINT32 g234_reserved[15]				; // 17-31

	UINT32 color_bar_mode					; // 00
	UINT32 color_bar_v_total				; // 01
	UINT32 color_bar_v_active				; // 02
	UINT32 color_bar_v_active_start			; // 03
	UINT32 color_bar_h_total				; // 04
	UINT32 color_bar_h_active				; // 05
	UINT32 g235_reserved[26]				; // 06-31
} DISP_DVE_REG_t;
STATIC_ASSERT(sizeof(DISP_DVE_REG_t) == (2* 32 * 4));

typedef struct _DISP_REG_ {
	//DDFCH (G185)
	DISP_DDFCH_REG_t	ddfch;
	//skip
	int reserved0[10 * 32];
	//OSD (G196)
	DISP_OSD_REG_t		osd;
	//skip
	int reserved1[2 * 32];
	//VPOST (G199)
	DISP_VPOST_REG_t	vpost;
	//skip
	int reserved2[6 * 32];
	//GPOST (G206)
	DISP_GPOST_REG_t	gpost;
	//skip
	int reserved3[6 * 32];
	//TGEN (213)
	DISP_TGEN_REG_t		tgen;
	//skip
	int reserved4[3 * 32];
	//DMIX (217)
	DISP_DMIX_REG_t		dmix;
	//skip
	int reserved5[16 * 32];
	//DVE (234, 235)
	DISP_DVE_REG_t		dve;
} DISP_REG_t;
STATIC_ASSERT(sizeof(DISP_REG_t) == (51 * 32 * 4));

typedef enum DRV_Status_t {
	/* common status values */
	DRV_SUCCESS,				/*!< successful outcome					*/
	DRV_ERR_FAILURE,			/*!< operation failed					*/
	DRV_ERR_INVALID_HANDLE,		/*!< invalid handle						*/
	DRV_ERR_INVALID_ID,			/*!< invalid identifier					*/
	DRV_ERR_INVALID_PARAM,		/*!< invalid parameter					*/
	DRV_ERR_INVALID_OP,			/*!< requested operation is invalid		*/
	DRV_ERR_MEMORY_ALLOC,		/*!< problem allocating memory			*/
	DRV_ERR_MEMORY_SIZE,		/*!< problem with the size of memory	*/
	/* < supplied							*/
	DRV_ERR_RESOURCE_UNAVAILABLE,
	DRV_ERR_TIMEOUT,			/*!< timeout							*/
	DRV_WARN_NO_ACTION,			/* < the function completed successfully,*/
	/* < but no action was taken            */
	DRV_WARN_PARAM_CLIPPED,		/*!< the function has completed			*/
	/*!< successfully, though a parameter was	*/
	/*!< clipped to within a valid range.		*/
	DRV_WARN_BUFFER_EMPTY,
	DRV_WARN_BUFFER_FULL,
	DRV_WARN_UNINITED,			/*!< driver has not been initialized yet */
	DRV_WARN_INITED,			/*!< driver has been initialized already */
	DRV_ERR_MODE_MISMATCH,		/*!< deinterlacer off*/
	DRV_ERR_MAX					/*!< Max error number*/
} DRV_Status_e;

#endif	//__REG_DISP_H__

