#include "l2sw_desc.h"
#include "l2sw_define.h"

u32 fix_net_dma(void *addr)
{
	dma_addr_t dma = (dma_addr_t) addr;
	dma_addr_t tmp = (dma >> 28) & 0xf;
    
	/*If virtual address is 8xxx_xxxx or Axxx_xxxx or Cxxx_xxxx, dma address is 0xxx_xxxx*/
	if ((tmp == 8) || (tmp == 0xa) || (tmp == 0xc))
		return (dma & 0xfffffff);
	/*If virtual address is Dxxx_xxxx, dma address is 1xxx_xxxx*/
	if ( tmp == 0xd)
		return ((dma & 0xfffffff) | 0x10000000);
	/*If virtual address is 9xxx_xxxx or Bxxx_xxxx or Exxx_xxxx,dma address is 4xxx_xxxx*/
	if ((tmp == 9) || (tmp == 0xb) || (tmp == 0xe))
		return ((dma & 0xfffffff) | 0x40000000);
	/*If virtual address is Fxxx_xxxx then dma address is 5xxx_xxxx*/
	if (tmp == 0xf)
		return ((dma & 0xfffffff) | 0x50000000);

	return dma;

}

void rx_descs_flush(struct l2sw_mac *mac)
{
	u32 i, j;
	volatile struct mac_desc *rx_desc;
	struct skb_info *rx_skbinfo;

	for (i = 0; i < RX_DESC_QUEUE_NUM; i++) {
		rx_desc = mac->rx_desc[i];
		rx_skbinfo = mac->rx_skb_info[i];
		for (j = 0; j < mac->rx_desc_num[i]; j++) {
			rx_desc[j].addr1 = rx_skbinfo[j].mapping;
			rx_desc[j].cmd1 = OWN_BIT;
			rx_desc[j].cmd2 = (j == mac->rx_desc_num[i] - 1) ? EOR_BIT|mac->rx_desc_buff_size : mac->rx_desc_buff_size;
		}
	}
}

void tx_descs_clean(struct l2sw_mac *mac)
{
	u32 i;
	s32 buflen;

	if (mac->tx_desc == NULL) {
		return;
	}

	for (i = 0; i < TX_DESC_NUM; i++) {
		if (mac->tx_temp_skb_info[i].mapping) {
			buflen = (mac->tx_temp_skb_info[i].skb != NULL) ? mac->tx_temp_skb_info[i].skb->len : MAC_TX_BUFF_SIZE;
			dma_unmap_single(&mac->pdev->dev, mac->tx_temp_skb_info[i].mapping, buflen, DMA_TO_DEVICE);
			mac->tx_temp_skb_info[i].mapping = 0;
		}
		if (mac->tx_temp_skb_info[i].skb) {
			dev_kfree_skb(mac->tx_temp_skb_info[i].skb);
			mac->dev_stats.tx_dropped++;
			mac->tx_temp_skb_info[i].skb = NULL;
		}

		(mac->tx_desc + i)->cmd1 = 0;
		(mac->tx_desc + i)->cmd2 = 0;
		(mac->tx_desc + i)->addr1 = 0;
		(mac->tx_desc + i)->addr2 = 0;
	}
}

void rx_descs_clean(struct l2sw_mac *mac)
{
	u32 i, j;
	struct skb_info *skbinfo;

	for (i = 0; i < RX_DESC_QUEUE_NUM; i++) {
		if (mac->rx_skb_info[i] == NULL) {
			continue;
		}
		skbinfo = mac->rx_skb_info[i];
		for (j = 0; j < mac->rx_desc_num[i]; j++) {
			if (skbinfo[j].skb) {
				dma_unmap_single(&mac->pdev->dev, skbinfo[j].mapping, mac->rx_desc_buff_size, DMA_FROM_DEVICE);
				dev_kfree_skb(skbinfo[j].skb);
				skbinfo[j].skb = NULL;
				skbinfo[j].mapping = 0;
			}
			(mac->rx_desc[i] + j)->cmd1 = 0;
			(mac->rx_desc[i] + j)->cmd2 = 0;
			(mac->rx_desc[i] + j)->addr1 = 0;
		}
		kfree(mac->rx_skb_info[i]);
		mac->rx_skb_info[i] = NULL;
	}
}

void descs_clean(struct l2sw_mac *mac)
{
	rx_descs_clean(mac);
	tx_descs_clean(mac);
}

void descs_free(struct l2sw_mac *mac)
{
	u32 i;

	descs_clean(mac);
	mac->tx_desc = NULL;
	for (i = 0; i < RX_DESC_QUEUE_NUM; i++) {
		mac->rx_desc[i] = NULL;
	}

	/*  Free descriptor area  */
	if (mac->desc_base != NULL) {
		dma_free_coherent(&mac->pdev->dev, mac->desc_size, mac->desc_base, mac->desc_dma);
		mac->desc_base = NULL;
		mac->desc_dma = 0;
		mac->desc_size = 0;
	}
}

u32 tx_descs_init(struct l2sw_mac *mac)
{
    memset(mac->tx_desc, '\0', sizeof(struct mac_desc) * TX_DESC_NUM);
    
    return 0;
}

u32 rx_descs_init(struct l2sw_mac *mac)
{
    struct sk_buff *skb;
    u32 i, j;
    struct mac_desc *rxdesc;
    struct skb_info *rx_skbinfo;

    for (i = 0; i < RX_DESC_QUEUE_NUM; i++) {
        mac->rx_skb_info[i] = (struct skb_info*)kmalloc(sizeof(struct skb_info) * mac->rx_desc_num[i], GFP_KERNEL);

        if (mac->rx_skb_info[i] == NULL) {
            goto MEM_ALLOC_FAIL;
        }
        rx_skbinfo = mac->rx_skb_info[i];
        rxdesc = mac->rx_desc[i];
        for (j = 0; j < mac->rx_desc_num[i]; j++) {
            skb = __dev_alloc_skb(mac->rx_desc_buff_size + RX_OFFSET, GFP_ATOMIC | GFP_DMA);
            if (!skb) {
                goto MEM_ALLOC_FAIL;
            }

            skb->dev = mac->net_dev;
            skb_reserve(skb, RX_OFFSET);/* +data +tail */

            rx_skbinfo[j].skb = skb;
            rx_skbinfo[j].mapping = dma_map_single(&mac->pdev->dev, skb->data, mac->rx_desc_buff_size, DMA_FROM_DEVICE);
            rxdesc[j].addr1 = rx_skbinfo[j].mapping;
			rxdesc[j].addr2 = 0;
            rxdesc[j].cmd1 = OWN_BIT ;
			rxdesc[j].cmd2 = (j == mac->rx_desc_num[i] - 1) ? EOR_BIT|mac->rx_desc_buff_size : mac->rx_desc_buff_size;
        }
    }

    return 0;

MEM_ALLOC_FAIL:
    rx_descs_clean(mac);
    return -ENOMEM;

}

u32 descs_init(struct l2sw_mac *mac)
{
	u32 rc;

	if ((rc = tx_descs_init(mac)) != 0) {
		return rc;
	}
	return rx_descs_init(mac);
}

u32 descs_alloc(struct l2sw_mac *mac)
{
    u32 i;
    s32 desc_size;

    /*  Alloc descriptor area  */
    desc_size = (TX_DESC_NUM) * sizeof(struct mac_desc);
    for (i = 0; i < RX_DESC_QUEUE_NUM; i++) {
        desc_size += mac->rx_desc_num[i] * sizeof(struct mac_desc);
    }
    mac->desc_base = dma_alloc_coherent(&mac->pdev->dev, desc_size, &mac->desc_dma, GFP_KERNEL);
    if (mac->desc_base == NULL) {
        return -ENOMEM;
    }
    mac->desc_size = desc_size;

    /*  Setup Tx descriptor  */
    mac->tx_desc = (struct mac_desc*)mac->desc_base;

    /*  Setup Rx descriptor*/
    mac->rx_desc[0] = &mac->tx_desc[TX_DESC_NUM + MAC_GUARD_DESC_NUM];
    for (i = 1; i < RX_DESC_QUEUE_NUM; i++) {
        mac->rx_desc[i] = mac->rx_desc[0] + mac->rx_desc_num[i - 1];
    }
    return 0;

}

