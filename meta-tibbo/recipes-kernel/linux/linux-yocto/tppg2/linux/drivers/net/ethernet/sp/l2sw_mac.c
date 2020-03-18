#include "l2sw_mac.h"

bool mac_init(struct l2sw_mac *mac)
{
    u32 index;
	
	mac->cpu_port=0x0;
	mac->vlan_id=0x0;
	mac->vlan_memset=0x9;

    for (index = 0; index < RX_DESC_QUEUE_NUM; index++) {
        mac->rx_pos[index] = 0;
    }
	DEBUG1();

    //mac_hw_reset(mac);
    mac_hw_init(mac);
	DEBUG1();

    return 1;
}

void mac_start(struct l2sw_mac *mac, u8 *enaddr, u32 flags)
{

	mac_hw_start(mac);
}

void mac_soft_reset(struct l2sw_mac *mac)
{
	u32 i;

    if (netif_carrier_ok(mac->net_dev)){
        netif_carrier_off(mac->net_dev);
        netif_stop_queue(mac->net_dev);
    }

	mac_hw_reset(mac);
	mac_hw_stop(mac);
	//descs_clean(mac);
    rx_descs_flush(mac);
	mac->tx_pos = 0;
	mac->tx_done_pos = 0;
	mac->tx_desc_full = 0;

	for (i = 0; i < RX_DESC_QUEUE_NUM; i++) {
		mac->rx_pos[i] = 0;
	}

	mac_hw_init(mac);
	mac_hw_start(mac);

    if (!netif_carrier_ok(mac->net_dev)){
    	netif_carrier_on(mac->net_dev);
		netif_start_queue(mac->net_dev);
    }
}


