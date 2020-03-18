
#include "l2sw_hal.h"

//static struct gmac_reg* gmac_reg_base = (struct gmac_reg*)GMAC_REG_BASE;
static struct l2sw_reg* ls2w_reg_base = NULL;


int l2sw_reg_base_set(void __iomem *baseaddr)
{
	printk("@@@@@@@@@l2sw l2sw_reg_base_set =[%x]\n", (int)baseaddr);
    ls2w_reg_base = (struct l2sw_reg*)baseaddr;
	if (ls2w_reg_base == NULL){
		return -1;
	}
	else{
		return 0;
	}
}

int l2sw_pinmux_set()
{
	int reg;

	GPIO_PIN_MUX_SEL(PMX_L2SW_CLK_OUT,40);
	GPIO_PIN_MUX_SEL(PMX_L2SW_MAC_SMI_MDC,34);
	GPIO_PIN_MUX_SEL(PMX_L2SW_LED_FLASH0,35);
	GPIO_PIN_MUX_SEL(PMX_L2SW_LED_FLASH1,23);
	GPIO_PIN_MUX_SEL(PMX_L2SW_LED_ON0,44);
	GPIO_PIN_MUX_SEL(PMX_L2SW_LED_ON1,32);
	GPIO_PIN_MUX_SEL(PMX_L2SW_MAC_SMI_MDIO,33);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P0_MAC_RMII_TXEN,43);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P0_MAC_RMII_TXD0,41);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P0_MAC_RMII_TXD1,42);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P0_MAC_RMII_CRSDV,37);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P0_MAC_RMII_RXD0,38);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P0_MAC_RMII_RXD1,39);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P0_MAC_RMII_RXER,36);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P1_MAC_RMII_TXEN,31);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P1_MAC_RMII_TXD0,29);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P1_MAC_RMII_TXD1,30);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P1_MAC_RMII_CRSDV,25);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P1_MAC_RMII_RXD0,26);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P1_MAC_RMII_RXD1,27);
	GPIO_PIN_MUX_SEL(PMX_L2SW_P1_MAC_RMII_RXER,24);
	GPIO_PIN_MUX_SEL(PMX_DAISY_MODE,1);


}


#if 0
int mac_g1_base_set(void __iomem *baseaddr)
{
	static struct G1* g1_base = NULL;
    g1_base = (struct G1*)baseaddr;
	if (g1_base == NULL){
		return -1;
	}
	else{
		return 0;
	}
}
#endif

void mac_hw_stop(struct l2sw_mac *mac)
{
    u32 reg;
    
    ls2w_reg_base->sw_int_mask_0 = 0xffffffff;
    ls2w_reg_base->sw_int_status_0 = 0xffffffff & (~MAC_INT_PSC);

	reg = ls2w_reg_base->cpu_cntl;
	ls2w_reg_base->cpu_cntl = DIS_PORT_TX | reg;

	reg = ls2w_reg_base->port_cntl0;
	ls2w_reg_base->port_cntl0 = DIS_PORT_RX | reg;	

}

void mac_hw_reset(struct l2sw_mac *mac)
{
	
	#if 0
	u32 reg;
    wmb();
    
	reg = ls2w_reg_base->mac_glb_sys_cfgcmd;
	ls2w_reg_base->mac_glb_sys_cfgcmd = reg | SOFT_RST_N;
    
    while((reg & SOFT_RST_N) == 0x1){
		reg = ls2w_reg_base->mac_glb_sys_cfgcmd;
	}
	#endif
}

void mac_hw_start(struct l2sw_mac *mac)
{
	u32 reg;
	
#if 0 
    /* tell gmac how many desc can use*/
	reg = ls2w_reg_base->mac_rx_desc0_addr;
	reg &= ~(0x1ff << 16);
	reg |= RX_QUEUE0_DESC_NUM << 16;
	ls2w_reg_base->mac_rx_desc0_addr = reg;
    
	reg = ls2w_reg_base->mac_rx_desc1_addr;
	reg &= ~ (0x1ff << 16);
	reg |= RX_QUEUE1_DESC_NUM << 16;
	ls2w_reg_base->mac_rx_desc1_addr = reg;

    reg = ls2w_reg_base->mac_glb_port_cfg;
	ls2w_reg_base->mac_glb_port_cfg = (~DIS_PORT_TX) & (~DIS_PORT_RX) & reg;
#endif
	//HWREG_W(tx_hbase_addr_0,(UINT32)(mac->tx_desc[1]));
	//HWREG_W(tx_lbase_addr_0,(UINT32)(mac->tx_desc[0]));
	//HWREG_W(rx_hbase_addr_0,(UINT32)(mac->rx_desc[1]));
	//HWREG_W(rx_lbase_addr_0,(UINT32)(mac->rx_desc[0]));
	//phy address
	//reg=HWREG_R(mac_force_mode);
	//HWREG_W(mac_force_mode,(reg&(~(0x1f<<16)))|(PHY0_ADDR<<16));

	HWREG_W(PVID_config0,(1<<4)+0);
	HWREG_W(VLAN_memset_config0,(6<<8)+9);


	//enable soc port0 crc padding
	reg=HWREG_R(cpu_cntl);
	HWREG_W(cpu_cntl,(reg&(~(0x1<<6)))|(0x1<<8));
	//enable port0
	reg=HWREG_R(port_cntl0);
	HWREG_W(port_cntl0,reg&(~(0x1<<24)));
	

	//reg=HWREG_R(cpu_cntl);
	//HWREG_W(cpu_cntl,reg|0x2a);

	reg=HWREG_R(cpu_cntl);
	HWREG_W(cpu_cntl,reg&(~(0x3F<<0)));

	//tx_mib_counter_print();


	ls2w_reg_base->sw_int_mask_0 = 0x00000000;
}

void mac_hw_addr_set(struct l2sw_mac *mac)
{

#if 0

	u32 reg;

	reg = ((u32)(mac->mac_addr[3]) << 24) | ((u32)(mac->mac_addr[2]) << 16)
	      | ((u32)(mac->mac_addr[1]) << 8) | ((u32)(mac->mac_addr[0]));
	ls2w_reg_base->mac_glb_macaddr0 = reg;

	reg = ((u32)(mac->mac_addr[5]) << 8) | ((u32)(mac->mac_addr[4]));
	ls2w_reg_base->mac_glb_macaddr1 = reg;
#endif

	HWREG_W(w_mac_15_0_bus,mac->mac_addr[0]+(mac->mac_addr[1]<<8));
	HWREG_W(w_mac_47_16,mac->mac_addr[2]+(mac->mac_addr[3]<<8)+(mac->mac_addr[4]<<16)+(mac->mac_addr[5]<<24));
}

void mac_hw_init(struct l2sw_mac *mac)
{
	u32 reg;
	u8 port_map=0x0;
	u8 cpu_port=mac->cpu_port;
	u8 age=0x0;
	u8 proxy=0x0;
	u8 mc_ingress=0x0;




    /* descriptor base address */

    ls2w_reg_base->tx_hbase_addr_0 = mac->desc_dma;
    ls2w_reg_base->rx_lbase_addr_0 = mac->desc_dma + sizeof(struct mac_desc) * (TX_DESC_NUM + MAC_GUARD_DESC_NUM);

    ls2w_reg_base->rx_hbase_addr_0 = mac->desc_dma + sizeof(struct mac_desc) * (TX_DESC_NUM + MAC_GUARD_DESC_NUM + RX_QUEUE0_DESC_NUM);
    
    /* TX Configuration Register */

	reg=HWREG_R(mac_force_mode);
	HWREG_W(mac_force_mode,(reg&(~(0x1f<<16)))|(PHY0_ADDR<<16));
	reg=HWREG_R(mac_force_mode);
	HWREG_W(mac_force_mode,(reg&(~(0x1f<<24)))|(PHY1_ADDR<<24));
	//enable soc port0 crc padding
	reg=HWREG_R(cpu_cntl);
	HWREG_W(cpu_cntl,(reg&(~(0x1<<6)))|(0x1<<8));
	//enable port0
	reg=HWREG_R(port_cntl0);
	HWREG_W(port_cntl0,reg&(~(0x3<<24)));

	//MAC address initial
	//soc port MAC address config

	mac_hw_addr_set(mac);
	HWREG_W(wt_mac_ad0,(port_map<<12)+(cpu_port<<10)+(mac->vlan_id<<7)+(age<<4)+(proxy<<3)+(mc_ingress<<2)+0x1);
	reg=HWREG_R(wt_mac_ad0);
	while((reg&(0x1<<1))==0x0) {
		printk("wt_mac_ad0 = [%x]\n", reg);
		reg=HWREG_R(wt_mac_ad0);
	}



}

u32 mac_hw_addr_update(struct l2sw_mac *mac)
{
	u32 regl, regh;
	u8  addr[6];

	regl = ls2w_reg_base->MAC_ad_ser0;
	regh = ls2w_reg_base->MAC_ad_ser1;
	addr[0] = regl & 0xFF;
	addr[1] = (regl >> 8) & 0xFF;
	addr[2] = regh & 0xFF;
	addr[3] = (regh >> 8) & 0xFF;
	addr[4] = (regh >> 16) & 0xFF;
	addr[5] = (regh >> 24) & 0xFF;

	if (is_valid_ether_addr(addr)) {
		memcpy(mac->mac_addr, addr, sizeof(addr));
        return 0;
	} 
    
    return -1;
}

void rx_mode_set(struct l2sw_mac *mac)
{
	struct net_device *netdev;
    
	netdev = mac->net_dev;
	if (netdev == NULL) {
		return;
	}
}

u32 mdio_read(u32 phy_id, u16 regnum)
{
    u32 val, time;
	DEBUG1();

    ls2w_reg_base->phy_cntl_reg0 = ((regnum << 8) & PHY_REG_MASK) | PHY_RD_CMD | PHY_CMD_TRI | (phy_id & PHY_ADR_MASK) | (0 << 15);
    //gmac_reg_base->mac_glb_phy_cmd = ((regnum << 8) & PHY_REG_MASK) | PHY_RD_CMD | PHY_CMD_TRI | MAC_PHY_ADDR | (0 << 15);
    DEBUG1();
    time = MDIO_RW_TIMEOUT_RETRY_NUMBERS;
    do {
        val = ls2w_reg_base->phy_cntl_reg1;
        if (val & PHY_RD_RDY) {
            return val >> 16;
        }
    } while (--time);
	DEBUG1();

    return (val >> 16);
}

u32 mdio_write(u32 phy_id, u32 regnum, u16 val)
{
        u32 time, regval;

        ls2w_reg_base->phy_cntl_reg0 = ((val << 16) & PHY_WT_DATA_MASK) | PHY_WT_CMD | ((regnum << 8) & PHY_REG_MASK) | PHY_CMD_TRI | (phy_id & PHY_ADR_MASK) | (0 << 15);
        //gmac_reg_base->mac_glb_phy_cmd = ((val << 16) & PHY_WT_DATA_MASK) | PHY_WT_CMD | ((regnum << 8) & PHY_REG_MASK) | PHY_CMD_TRI | MAC_PHY_ADDR | (0 << 15);
    
        time = MDIO_RW_TIMEOUT_RETRY_NUMBERS;
        do {
            regval = ls2w_reg_base->phy_cntl_reg1;
            if (regval & PHY_WT_DONE) {
                return 0;
            }
        } while (--time);

        return -EIO;
}

inline void rx_finished(u32 queue, u32 rx_count)
{

#if 0
    u32 reg, rx_desc_sw_addr[RX_DESC_QUEUE_NUM], rx_desc_sw_addr_next;
    
	if(queue) {
		reg = ls2w_reg_base->rx_hbase_addr_0;
		rx_desc_sw_addr[1] = (reg >> 16) & 0x1ff;
		reg &= ~(0x1ff<<16);
		rx_desc_sw_addr_next = ((rx_desc_sw_addr[1] + rx_count));
		reg |=  rx_desc_sw_addr_next << 16;
		ls2w_reg_base->rx_hbase_addr_0 = reg;
	}
	else {
		reg = ls2w_reg_base->rx_lbase_addr_0;
		rx_desc_sw_addr[0] = (reg >> 16) & 0x1ff;
		reg &= ~(0x1ff<<16);
		rx_desc_sw_addr_next = ((rx_desc_sw_addr[0] + rx_count));
		reg |= rx_desc_sw_addr_next << 16;
		ls2w_reg_base->rx_lbase_addr_0 = reg;
	}
#endif
}

inline void tx_trigger(u32 tx_pos)
{
    u32 reg;
    
    reg = ls2w_reg_base->tx_lbase_addr_0;
    reg &= ~(0xff << 16);
    reg |= tx_pos << 16;
    ls2w_reg_base->tx_lbase_addr_0 = reg;

	HWREG_W(cpu_tx_trig,(0x1<<0));
}

/*read or write a single register */
inline u32 reg_control(u8 mode, u32 reg, u32 value)
{
  
    if (mode == REG_WRITE) {

        switch (reg) {
        case MAC_GLB_INT_MASK:
            ls2w_reg_base->sw_int_mask_0 = value;
            break;
            
        case MAC_GLB_INT_STATUS:
            ls2w_reg_base->sw_int_status_0 = value;
            break;

        case MAC_GLB_PHY_ADDR:
    		ls2w_reg_base->mac_force_mode = value;
            break;
                        
        default:
            break;
        }

        return 0;
    }
    else if (mode == REG_READ) {
        
        switch (reg) {
        case MAC_GLB_PORT_STA:
            reg = ls2w_reg_base->port_ability;
            break;

        case MAC_RX_DESC0_ADDR:
            reg = (ls2w_reg_base->rx_lbase_addr_0) & 0x1ff;
            break;

        case MAC_RX_DESC1_ADDR:
            reg = (ls2w_reg_base->rx_hbase_addr_0) & 0x1ff;
            break;

        case MAC_TX_DESC_ADDR:
            reg = ls2w_reg_base->tx_hbase_addr_0;
            break;

        case MAC_GLB_INT_STATUS:
            reg = ls2w_reg_base->sw_int_status_0;
            break;

        case MAC_GLB_PHY_ADDR:
            reg = ls2w_reg_base->mac_force_mode;
            break;
            
        default:
            break;
        }
        
        return reg;
    }
}

void rx_mib_counter_print()
{
	DEBUG0("rx_mib_counter_dump\n");
#if 0
    DEBUG0("mib rx desc got = %d\r\ndesc put = %d\r\n",((ls2w_reg_base->mac_mib_rx_totaldes_cnt)&0xffff), ((ls2w_reg_base->mac_mib_rx_totaldes_cnt)>>16)&0xffff);
    DEBUG0("mac_mib_rx_pause       = %d \r\n",ls2w_reg_base->mac_mib_rx_pause);       
    DEBUG0("mac_mib_rx_fcserr      = %d \r\n",ls2w_reg_base->mac_mib_rx_fcserr);      
    DEBUG0("mac_mib_rx_alignerr    = %d \r\n",ls2w_reg_base->mac_mib_rx_alignerr);       // 18
    DEBUG0("mac_mib_rx_frag        = %d \r\n",ls2w_reg_base->mac_mib_rx_frag);       // 19
    DEBUG0("mac_mib_rx_runt        = %d \r\n",ls2w_reg_base->mac_mib_rx_runt);       // 20
    DEBUG0("mac_mib_rx_long        = %d \r\n",ls2w_reg_base->mac_mib_rx_long);       // 21
    DEBUG0("mib rx drop = %d\r\nmib rx desc under run = %d\r\n",(ls2w_reg_base->mac_mib_rx_desc_udr>>16)&0xffff,(ls2w_reg_base->mac_mib_rx_desc_udr)&0xffff);
    DEBUG0("mac_mib_rx_abort    = %d \r\n",ls2w_reg_base->mac_mib_rx_fifo_ovf);       // 23
    DEBUG0("mac_mib_rx_totalpkt    = %d \r\n",ls2w_reg_base->mac_mib_rx_totalpkt);       // 24
    DEBUG0("mac_mib_rx_totalpkt\%ffff    = %d \r\n",ls2w_reg_base->mac_mib_rx_totalpkt&0xffff);       // 24
    DEBUG0("mac_mib_rx_jambo       = %d \r\n",ls2w_reg_base->mac_mib_rx_jambo);       // 25
    DEBUG0("mac_mib_rx_burst_too   = %d \r\n",ls2w_reg_base->mac_mib_rx_burst_too_long);      // 26
    DEBUG0("Receive packet number after pause %d\r\n",ls2w_reg_base->mac_rxnum_after_pause); 
#endif
}

void tx_mib_counter_print()
{
	DEBUG0("@@@@@@ls2w_reg_base_dump\n");
#if 1
    DEBUG0("sw_int_status_0     = %x\r\n",ls2w_reg_base->sw_int_status_0);
	DEBUG0("sw_int_mask_0       = %x\r\n",ls2w_reg_base->sw_int_mask_0);
	DEBUG0("fl_cntl_th          = %x\r\n",ls2w_reg_base->fl_cntl_th);
	DEBUG0("cpu_fl_cntl_th      = %x\r\n",ls2w_reg_base->cpu_fl_cntl_th);
	DEBUG0("pri_fl_cntl         = %x\r\n",ls2w_reg_base->pri_fl_cntl);
    DEBUG0("vlan_pri_th         = %x\r\n",ls2w_reg_base->vlan_pri_th);
	DEBUG0("En_tos_bus          = %x\r\n",ls2w_reg_base->En_tos_bus);
	DEBUG0("Tos_map0            = %x\r\n",ls2w_reg_base->Tos_map0);
	DEBUG0("Tos_map1            = %x\r\n",ls2w_reg_base->Tos_map1);
	DEBUG0("Tos_map2            = %x\r\n",ls2w_reg_base->Tos_map2);
	DEBUG0("Tos_map3            = %x\r\n",ls2w_reg_base->Tos_map3);
	DEBUG0("Tos_map4            = %x\r\n",ls2w_reg_base->Tos_map4);
	DEBUG0("Tos_map5            = %x\r\n",ls2w_reg_base->Tos_map5);
	DEBUG0("Tos_map6            = %x\r\n",ls2w_reg_base->Tos_map6);
	DEBUG0("Tos_map7            = %x\r\n",ls2w_reg_base->Tos_map7);
	DEBUG0("global_que_status   = %x\r\n",ls2w_reg_base->global_que_status);
	DEBUG0("addr_tbl_srch       = %x\r\n",ls2w_reg_base->addr_tbl_srch);
	DEBUG0("addr_tbl_st         = %x\r\n",ls2w_reg_base->addr_tbl_st);
	DEBUG0("MAC_ad_ser0         = %x\r\n",ls2w_reg_base->MAC_ad_ser0);
	DEBUG0("MAC_ad_ser1         = %x\r\n",ls2w_reg_base->MAC_ad_ser1);
    DEBUG0("wt_mac_ad0          = %x\r\n",ls2w_reg_base->wt_mac_ad0);
	DEBUG0("w_mac_15_0_bus      = %x\r\n",ls2w_reg_base->w_mac_15_0_bus);
	DEBUG0("w_mac_47_16         = %x\r\n",ls2w_reg_base->w_mac_47_16);
	DEBUG0("PVID_config0        = %x\r\n",ls2w_reg_base->PVID_config0);
	DEBUG0("PVID_config1        = %x\r\n",ls2w_reg_base->PVID_config1);
	DEBUG0("VLAN_memset_config0 = %x\r\n",ls2w_reg_base->VLAN_memset_config0);
	DEBUG0("VLAN_memset_config1 = %x\r\n",ls2w_reg_base->VLAN_memset_config1);
	DEBUG0("port_ability        = %x\r\n",ls2w_reg_base->port_ability);
	DEBUG0("port_st             = %x\r\n",ls2w_reg_base->port_st);
	DEBUG0("cpu_cntl            = %x\r\n",ls2w_reg_base->cpu_cntl);
	DEBUG0("port_cntl0          = %x\r\n",ls2w_reg_base->port_cntl0);
	DEBUG0("port_cntl1          = %x\r\n",ls2w_reg_base->port_cntl1);
	DEBUG0("port_cntl2          = %x\r\n",ls2w_reg_base->port_cntl2);
	DEBUG0("sw_glb_cntl         = %x\r\n",ls2w_reg_base->sw_glb_cntl);
	DEBUG0("l2sw_rsv1           = %x\r\n",ls2w_reg_base->l2sw_rsv1);
	DEBUG0("led_port0           = %x\r\n",ls2w_reg_base->led_port0);
	DEBUG0("led_port1           = %x\r\n",ls2w_reg_base->led_port1);
	DEBUG0("led_port2           = %x\r\n",ls2w_reg_base->led_port2);
	DEBUG0("led_port3           = %x\r\n",ls2w_reg_base->led_port3);
	DEBUG0("led_port4           = %x\r\n",ls2w_reg_base->led_port4);
	DEBUG0("watch_dog_trig_rst  = %x\r\n",ls2w_reg_base->watch_dog_trig_rst);
	DEBUG0("watch_dog_stop_cpu  = %x\r\n",ls2w_reg_base->watch_dog_stop_cpu);
	DEBUG0("phy_cntl_reg0       = %x\r\n",ls2w_reg_base->phy_cntl_reg0);
	DEBUG0("phy_cntl_reg1       = %x\r\n",ls2w_reg_base->phy_cntl_reg1);
	DEBUG0("mac_force_mode      = %x\r\n",ls2w_reg_base->mac_force_mode);
	DEBUG0("VLAN_group_config0  = %x\r\n",ls2w_reg_base->VLAN_group_config0);
	DEBUG0("VLAN_group_config1  = %x\r\n",ls2w_reg_base->VLAN_group_config1);
	DEBUG0("flow_ctrl_th3       = %x\r\n",ls2w_reg_base->flow_ctrl_th3);
	DEBUG0("queue_status_0      = %x\r\n",ls2w_reg_base->queue_status_0);
	DEBUG0("debug_cntl          = %x\r\n",ls2w_reg_base->debug_cntl);
	DEBUG0("queue_status_0      = %x\r\n",ls2w_reg_base->queue_status_0);
	DEBUG0("debug_cntl          = %x\r\n",ls2w_reg_base->debug_cntl);
	DEBUG0("l2sw_rsv2           = %x\r\n",ls2w_reg_base->l2sw_rsv2);
	DEBUG0("mem_test_info       = %x\r\n",ls2w_reg_base->mem_test_info);
	DEBUG0("sw_int_status_1     = %x\r\n",ls2w_reg_base->sw_int_status_1);
	DEBUG0("sw_int_mask_1       = %x\r\n",ls2w_reg_base->sw_int_mask_1);
	DEBUG0("cpu_tx_trig         = %x\r\n",ls2w_reg_base->cpu_tx_trig);
	DEBUG0("tx_hbase_addr_0     = %x\r\n",ls2w_reg_base->tx_hbase_addr_0);
	DEBUG0("tx_lbase_addr_0     = %x\r\n",ls2w_reg_base->tx_lbase_addr_0);
	DEBUG0("rx_hbase_addr_0     = %x\r\n",ls2w_reg_base->rx_hbase_addr_0);
	DEBUG0("rx_lbase_addr_0     = %x\r\n",ls2w_reg_base->rx_lbase_addr_0);
	DEBUG0("tx_hw_addr_0        = %x\r\n",ls2w_reg_base->tx_hw_addr_0);
	DEBUG0("tx_lw_addr_0        = %x\r\n",ls2w_reg_base->tx_lw_addr_0);
	DEBUG0("rx_hw_addr_0        = %x\r\n",ls2w_reg_base->rx_hw_addr_0);
	DEBUG0("rx_lw_addr_0        = %x\r\n",ls2w_reg_base->rx_lw_addr_0);
	DEBUG0("cpu_port_cntl_reg_0 = %x\r\n",ls2w_reg_base->cpu_port_cntl_reg_0);





#endif
}



void l2sw_enable_port()
{
    u32 reg;

		
	struct moon_regs * MOON5_REG = (volatile struct moon_regs *)ioremap(RF_GRP(5, 0), 32);
	//set clock
	reg = MOON5_REG->sft_cfg[5];
	MOON5_REG->sft_cfg[5] = (reg|0xF<<16|0xF);
	//enable port
    reg=HWREG_R(port_cntl0);
    HWREG_W(port_cntl0,reg&(~(0x3<<24)));


	//phy address
	reg=HWREG_R(mac_force_mode);
	HWREG_W(mac_force_mode,(reg&(~(0x1f<<16)))|(PHY0_ADDR<<16));
	reg=HWREG_R(mac_force_mode);
	HWREG_W(mac_force_mode,(reg&(~(0x1f<<24)))|(PHY1_ADDR<<24));

}




int phy_cfg()
{
    unsigned int reg;
	
	#if 0 // for zy702 fpga ephy init
	mdio_write(0x0,0x1f,0x013d);
    mdio_write(0x0,0x10,0x3ffe);
    mdio_write(0x0,0x1f,0x063d);
    mdio_write(0x0,0x13,0x8000);

    mdio_write(0x0,0x1f,0x023d);
    mdio_write(0x0,0x18,0x1000);
    mdio_write(0x0,0x1f,0x063d);
    mdio_write(0x0,0x15,0x132c);

    mdio_write(0x0,0x1f,0x013d);
    mdio_write(0x0,0x17,0x003b);
    mdio_write(0x0,0x1f,0x063d);
    mdio_write(0x0,0x14,0x7088);

    mdio_write(0x0,0x1f,0x033d);
    mdio_write(0x0,0x11,0x8530);
    mdio_write(0x0,0x1f,0x003d);
	
    mdio_write(0x0,0x1f,0x003d);
    mdio_write(0x0,0x13,0x3102);
	#endif
	


	return 0;
}
