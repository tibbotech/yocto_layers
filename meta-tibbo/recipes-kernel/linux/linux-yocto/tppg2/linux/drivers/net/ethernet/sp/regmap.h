#define REGISTER_BASE   0x43C00000

typedef unsigned char       BYTE    ;
typedef unsigned short      WORD    ;
typedef unsigned long       DWORD   ;

typedef unsigned long long  UINT64  ;
typedef unsigned int        UINT32  ;
typedef unsigned short      UINT16  ;
typedef unsigned char       UINT8   ;

typedef long long           INT64   ;
typedef int                 INT32   ;
typedef short               INT16   ;
typedef signed char         INT8    ;

//=================================================================================================
/*
 * TYPE: RegisterFile_L2SW
 */
typedef struct
{
UINT32 sw_int_status_0;
UINT32 sw_int_mask_0;
UINT32 fl_cntl_th;
UINT32 cpu_fl_cntl_th;
UINT32 pri_fl_cntl;
UINT32 vlan_pri_th;
UINT32 En_tos_bus;
UINT32 Tos_map0;
UINT32 Tos_map1;
UINT32 Tos_map2;
UINT32 Tos_map3;
UINT32 Tos_map4;
UINT32 Tos_map5;
UINT32 Tos_map6;
UINT32 Tos_map7;
UINT32 global_que_status;
UINT32 addr_tbl_srch;
UINT32 addr_tbl_st;
UINT32 MAC_ad_ser0;
UINT32 MAC_ad_ser1;
UINT32 wt_mac_ad0;
UINT32 w_mac_15_0_bus;
UINT32 w_mac_47_16;
UINT32 PVID_config0;
UINT32 PVID_config1;
UINT32 VLAN_memset_config0;
UINT32 VLAN_memset_config1;
UINT32 port_ability;
UINT32 port_st;
UINT32 cpu_cntl;
UINT32 port_cntl0;
UINT32 port_cntl1;
UINT32 port_cntl2;
UINT32 sw_glb_cntl;
UINT32 l2sw_rsv1;
UINT32 led_port0;
UINT32 led_port1;
UINT32 led_port2;
UINT32 led_port3;
UINT32 led_port4;
UINT32 watch_dog_trig_rst;
UINT32 watch_dog_stop_cpu;
UINT32 phy_cntl_reg0;
UINT32 phy_cntl_reg1;
UINT32 mac_force_mode;
UINT32 VLAN_group_config0;
UINT32 VLAN_group_config1;
UINT32 flow_ctrl_th3;
UINT32 queue_status_0;
UINT32 debug_cntl;
UINT32 l2sw_rsv2;
UINT32 mem_test_info;
UINT32 sw_int_status_1;
UINT32 sw_int_mask_1;
UINT32 l2sw_rsv3[76];
UINT32 cpu_tx_trig;
UINT32 tx_hbase_addr_0;
UINT32 tx_lbase_addr_0;
UINT32 rx_hbase_addr_0;
UINT32 rx_lbase_addr_0;
UINT32 tx_hw_addr_0;
UINT32 tx_lw_addr_0;
UINT32 rx_hw_addr_0;
UINT32 rx_lw_addr_0;
UINT32 cpu_port_cntl_reg_0;
UINT32 tx_hbase_addr_1;
UINT32 tx_lbase_addr_1;
UINT32 rx_hbase_addr_1;
UINT32 rx_lbase_addr_1;
UINT32 tx_hw_addr_1;
UINT32 tx_lw_addr_1;
UINT32 rx_hw_addr_1;
UINT32 rx_lw_addr_1;
UINT32 cpu_port_cntl_reg_1;
} RegisterFile_L2SW;

#define regs_l2sw   ((volatile RegisterFile_L2SW *)(REGISTER_BASE))

