#include "l2sw_driver.h"

#if 1

static char def_mac_addr[ETHERNET_MAC_ADDR_LEN] = {0x88,0x88,0x88,0x88,0x88,0x88};


/*********************************************************************
*
* net_device_ops
*
**********************************************************************/

static inline void rx_skb(struct l2sw_mac *mac,
                              struct sk_buff *skb, struct mac_desc *desc)
{
	mac->dev_stats.rx_packets++;
	mac->dev_stats.rx_bytes += skb->len;
	//mac->net_dev->last_rx = jiffies;

	netif_rx(skb);
}

static inline void interrupt_finish(struct l2sw_mac *mac) 
{
    u32 reg;
    
    struct net_device *net_dev = (struct net_device *)mac->net_dev;
    u32 status = (u32)mac->int_status;
        
	if (status & MAC_INT_PSC) { /* link status changed*/
        reg = reg_control(REG_READ, MAC_GLB_PORT_STA, 0);
		if (!netif_carrier_ok(net_dev) && (reg & (1<<24))) {
			netif_carrier_on(net_dev);
			netif_start_queue(net_dev);
		} 
        else if (netif_carrier_ok(net_dev) && !(reg & PHY_LINK)) {
			netif_carrier_off(net_dev);
			netif_stop_queue(net_dev);
		}
	}

	if (status & (MAC_INT_RX_DES_ER | MAC_INT_TX_DES_ER)) {
		if (status & MAC_INT_RX_DES_ER) {
			mac->dev_stats.rx_fifo_errors++;
		}
		if (status & MAC_INT_TX_DES_ER) {
			mac->dev_stats.tx_fifo_errors++;
		}
	}

    //reg_control(REG_WRITE, MAC_GLB_INT_MASK, 0x00000000);
}


static inline void  rx_interrupt(struct l2sw_mac *mac, u32 irq_status)
{
	struct sk_buff    *skb, *new_skb;
	struct skb_info   *sinfo;
	volatile struct mac_desc   *desc;
	u32 rx_pos, pkg_len;
	u32 cmd, protocol;
	u32 num, rx_count;
    s32 queue;

    /* read the addr we can recive */

	
	for (queue = RX_DESC_QUEUE_NUM - 1; queue >= 0; queue--) {
			
		if ((MAC_INT_RX_DONE_L |irq_status) == 0) {
			continue;
		}
	
		rx_pos = mac->rx_pos[queue];
        rx_count =  mac->rx_desc_num[queue];
		//DEBUG0("@@@@@@@@@ethernet_interrupt rx_pos 	 = [%d]\n",rx_pos);
		//DEBUG0("@@@@@@@@@ethernet_interrupt rx_count = [%d]\n",rx_count);
		
		for (num = 0; num < rx_count; num++) {
			sinfo = mac->rx_skb_info[queue] + rx_pos;
			desc = mac->rx_desc[queue] + rx_pos;
			cmd = desc->cmd1;

			if (cmd & OWN_BIT) {
//				printk(KERN_EMERG "rx: own by nic, pos=%d, desc=%p", rx_pos, desc);
				break;
			}
			
			if (cmd & ERR_CODE) {                
				if (cmd & RX_FAE_BIT) {
                    ERROR0("[%s][%d] rx frame error\n", __FUNCTION__, __LINE__);
					mac->dev_stats.rx_frame_errors++;
				}
				
				if (cmd & RX_CRC_BIT) {
                    ERROR0("[%s][%d] rx crc error\n", __FUNCTION__, __LINE__);
					mac->dev_stats.rx_crc_errors++;
				}
				#if 0
                if (cmd & RX_PLE_BIT) {
                    ERROR0("[%s][%d] rx packet length error\n", __FUNCTION__, __LINE__);
                }
                if (cmd & IPCE) {
                    ERROR0("[%s][%d] rx ipv4 header checksum error\n", __FUNCTION__, __LINE__);
                }
                if (cmd & L4CE) {
                    ERROR0("[%s][%d] rx layer 4 header checksum error\n", __FUNCTION__, __LINE__);
                }
				if (cmd & (RX_RWT_BIT )) {
					ERROR0("[%s][%d] rx length error\n", __FUNCTION__, __LINE__);
					mac->dev_stats.rx_length_errors++;
				}
				#endif
				mac->dev_stats.rx_dropped++;

				goto NEXT;
			}

			if (cmd & RX_TU_BIT) {
				goto NEXT;
			}
			
			#if 0 
			if (cmd & PPPA) {
				mac->dev_stats.collisions++;
			}
			#endif
			
			pkg_len = cmd & LEN_MASK;

			if (pkg_len < 64) {
				goto NEXT;
			}
			
			new_skb = __dev_alloc_skb(mac->rx_desc_buff_size + RX_OFFSET, GFP_ATOMIC | GFP_DMA); /* allocate an skbuff for receiving, and it's an inline func*/
			if (new_skb == NULL) {
				mac->dev_stats.rx_dropped++;
				goto NEXT;
			}

			mac->dev_stats.rx_packets++;
			mac->dev_stats.rx_bytes += pkg_len;

			dma_unmap_single(&mac->pdev->dev, sinfo->mapping, mac->rx_desc_buff_size, DMA_FROM_DEVICE);

			skb = sinfo->skb;
			protocol = cmd & ((PDI_MASK) >> 16);
			skb->ip_summed = CHECKSUM_NONE;

			__skb_put(skb, (pkg_len - 4 > mac->rx_desc_buff_size) ? mac->rx_desc_buff_size : pkg_len - 4); /*skb_put will judge id tail exceed end, but __skb_put won't*/
			skb->protocol = eth_type_trans(skb, mac->net_dev);
			new_skb->dev = mac->net_dev;

			sinfo->mapping = dma_map_single(&mac->pdev->dev, new_skb->data, mac->rx_desc_buff_size, DMA_FROM_DEVICE);
			sinfo->skb = new_skb;
			#if 0// dump rx data
			DEBUG0("@@@@@@@@@ethernet_interrupt RX DUMP pkg_len = [%d]\n",pkg_len);
			u8 * pdata = skb->data;
			int i; 
			for(i = 0; i < pkg_len; i++)
			{
				DEBUG0("i = %d data[%d]\n", i,*(pdata+i) );
			}
			
			#endif 
			rx_skb(mac, skb, desc);
			desc->addr1 = sinfo->mapping;
NEXT:
			desc->cmd2 = (rx_pos==mac->rx_desc_num[queue]-1)?EOR_BIT|MAC_RX_LEN_MAX:MAC_RX_LEN_MAX;
			desc->cmd1 = (OWN_BIT | ( mac->rx_desc_buff_size & LEN_MASK));

			NEXT_RX(queue, rx_pos);
		}

        /* notify gmac how many desc(rx_count) we can use again */
        //  rx_finished(queue, rx_count);

		mac->rx_pos[queue] = rx_pos;
	}
}




static void rx_do_tasklet(unsigned long data) 
{
	struct l2sw_mac *mac = (struct l2sw_mac *) data;

	rx_interrupt(mac, mac->int_status);
    //reg_control(REG_WRITE, MAC_GLB_INT_STATUS, (MAC_INT_RX) & mac->int_status);
    interrupt_finish(mac);
}

#ifdef RX_POLLING
static int rx_poll(struct napi_struct *napi, int budget)
{
    int ret = 0;
    struct l2sw_mac *mac = container_of(napi, struct l2sw_mac, napi);

    rx_interrupt(mac, mac->int_status);
    napi_complete(napi);
    interrupt_finish(mac);

    return 0;
}
#endif

static inline void tx_interrupt(struct l2sw_mac *mac)
{
	u32 tx_done_pos;
	u32 cmd, reg, tx_desc_hw_addr;
	struct skb_info *skbinfo;
	
    
	tx_done_pos = mac->tx_done_pos;
	//DEBUG0("@@@@@@@@@tx_interrupt tx_done_pos = [%d]\n",tx_done_pos);
    while (tx_done_pos != mac->tx_pos || (mac->tx_desc_full == 1)) {
		cmd = mac->tx_desc[tx_done_pos ].cmd1;
		//DEBUG0("@@@@@@@@@tx_interrupt1 tx_done_pos = [%d]\n",tx_done_pos);
		skbinfo = &mac->tx_temp_skb_info[tx_done_pos];
    	if (skbinfo->skb == NULL) {
			ERROR0("[%s][%d] skb is null\n", __FUNCTION__, __LINE__);
		}

		if (cmd & (ERR_CODE)) {
			mac->dev_stats.tx_errors++;
            /*
			if (status & OWC_BIT) {
				mac->dev_stats.tx_window_errors++;
			}*/
			ERROR0("[%s][%d] tx  error [%x]\n", __FUNCTION__, __LINE__,cmd);
			#if 0
			if (cmd & BUR_BIT) {
                ERROR0("[%s][%d] tx aborted error\n", __FUNCTION__, __LINE__);
				mac->dev_stats.tx_aborted_errors++;
			}
			if (cmd & LNKF_BIT) {
                ERROR0("[%s][%d] tx link failure\n", __FUNCTION__, __LINE__);
				mac->dev_stats.tx_carrier_errors++;
			}
    		if (cmd & TWDE_BIT){
                ERROR0("[%s][%d] tx watch dog timer expired\n", __FUNCTION__, __LINE__);
    		}
    		if (cmd & TBE_MASK){
                ERROR0("[%s][%d] tx descriptor bit error\n", __FUNCTION__, __LINE__);
    		}
			#endif
		} 
		else {
		
			mac->dev_stats.collisions
			        += (cmd & CC_MASK) >> 16;
			mac->dev_stats.tx_packets++;
			mac->dev_stats.tx_bytes += skbinfo->len;
		}
        
        dma_unmap_single(&mac->pdev->dev, skbinfo->mapping, skbinfo->len, DMA_TO_DEVICE);
        skbinfo->mapping = 0;
        dev_kfree_skb_irq(skbinfo->skb); //dev_kfree_skb
		skbinfo->skb = NULL;

		NEXT_TX(tx_done_pos);
		if (mac->tx_desc_full == 1) {
			mac->tx_desc_full = 0;
		}
	}

	mac->tx_done_pos = tx_done_pos;
	if ((TX_DESC_AVAIL(mac) > 0) && netif_queue_stopped(mac->net_dev)) {
		netif_wake_queue(mac->net_dev);
	}

}

static void tx_do_tasklet(unsigned long data) 
{
	struct l2sw_mac *mac = (struct l2sw_mac *) data;

	tx_interrupt(mac);
    reg_control(REG_WRITE, MAC_GLB_INT_STATUS, (MAC_INT_TX) & mac->int_status);
    wmb();
    interrupt_finish(mac);
}

void rx_mib_counter(void)
{
    rx_mib_counter_print();
}

void tx_mib_counter(void)
{
    tx_mib_counter_print();
}

static irqreturn_t ethernet_interrupt(int irq, void *dev_id)
{
	struct net_device *net_dev;
	struct l2sw_mac *mac;
	u32 status;
	//DEBUG0("@@@@@@@@@ethernet_interrupt \n");
	net_dev = (struct net_device*)dev_id;
	if (unlikely(net_dev == NULL)) {
        ERROR0("[%s][%d] sphe: isr net_dev is NULL!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	mac = (struct l2sw_mac*)netdev_priv(net_dev);
	spin_lock(&(mac->lock));
    /*
	if (unlikely(!netif_running(net_dev))) {
		spin_unlock(&mac->lock);
        ERROR0("[%s][%d] %s interrupt when network device unruning\n", __FUNCTION__, __LINE__, net_dev->name);
		return -1;
	}
        */
    reg_control(REG_WRITE, MAC_GLB_INT_MASK, 0xffffffff); /* mask interrupt */
    status =  reg_control(REG_READ, MAC_GLB_INT_STATUS, 0);
	//DEBUG0("@@@@@@@@@ethernet_interrupt status = [%x]\n",status);
    rmb();
    if (status == 0){
        reg_control(REG_WRITE, MAC_GLB_INT_MASK, 0x00000000);
        ERROR0("[%s][%d] interrput status is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
    reg_control(REG_WRITE, MAC_GLB_INT_STATUS, status);
    wmb();
    mac->int_status = status;

#ifndef RX_POLLING
	if (status & MAC_INT_RX) {
       // DEBUG0("@@@@@@@@@ethernet_interrupt RX!!!!!!!!!!!!!!!!!!!!! = [%x]\n",status);
		if ( MAC_INT_RX0_LAN_FULL & status ) {
            ERROR0("[%s][%d] MAC_INT_RX0_LAN_FULL status = [%x]\n", __FUNCTION__, __LINE__,status);
		}

		if ( MAC_INT_RX_DES_ER & status ) {
            ERROR0("[%s][%d] MAC_INT_RX_DES_ER status = [%x]\n", __FUNCTION__, __LINE__, status);
		}
	        
#ifndef INTERRUPT_IMMEDIATELY	   
	    tasklet_schedule(&mac->rx_tasklet);
#else /* INTERRUPT_IMMEDIATELY */     

		rx_interrupt(mac, status);
        //reg_control(REG_WRITE, MAC_GLB_INT_STATUS, (MAC_INT_RX) & mac->int_status);
        interrupt_finish(mac);
#endif /* INTERRUPT_IMMEDIATELY */
        
	}
#else /* RX_POLLING */
    if (napi_schedule_prep(&mac->napi)) {
        __napi_schedule(&mac->napi);
    }
#endif /* RX_POLLING */

	if (status & MAC_INT_TX) {
		//DEBUG0("[%s][%d] MAC_INT_TX status = [%x]\n", __FUNCTION__, __LINE__, status);
        if (MAC_INT_TX_DES_ER & status) {
            ERROR0("[%s][%d] Send Descriptor Error\n", __FUNCTION__, __LINE__);
            mac_soft_reset(mac);
			goto OUT;
		}
		if ( MAC_INT_TX_H_DESCF & status) {
			ERROR0("[%s][%d] Transmit FIFO Underrun\n", __FUNCTION__, __LINE__);
		}
		if (MAC_INT_TX_L_DESCF & status) {
			ERROR0("[%s][%d] Warning: Transmit Abort\n", __FUNCTION__, __LINE__);
		}
        
#ifndef INTERRUPT_IMMEDIATELY
        tasklet_schedule(&mac->tx_tasklet);
#else
		tx_interrupt(mac);
        reg_control(REG_WRITE, MAC_GLB_INT_STATUS, (MAC_INT_TX) & mac->int_status);
        wmb();
        interrupt_finish(mac);
#endif
	}

OUT:
    reg_control(REG_WRITE, MAC_GLB_INT_MASK, 0x00000000);
	spin_unlock(&(mac->lock));
	return IRQ_HANDLED;
}

static int ethernet_open(struct net_device *net_dev)
{
    struct l2sw_mac *mac;
	u32 rc, i, reg;
	
	DEBUG0("@@@@@@@@@ethernet_open \n");
#ifndef INTERRUPT_IMMEDIATELY
    //tasklet_enable(&mac->rx_tasklet);
    //tasklet_enable(&mac->tx_tasklet);
#endif

	mac = (struct l2sw_mac*)netdev_priv(net_dev);

#ifdef RX_POLLING
    napi_enable(&mac->napi);
#endif

	mac->rx_desc_num[0] = RX_QUEUE0_DESC_NUM;
#if RX_DESC_QUEUE_NUM > 1
	mac->rx_desc_num[1] = RX_QUEUE1_DESC_NUM;
#endif

	for (i = 0; i < RX_DESC_QUEUE_NUM; i++) {
		mac->rx_desc[i] = NULL;
		mac->rx_skb_info[i] = NULL;
		mac->rx_pos[i] = 0;
	}

	mac->rx_desc_buff_size = MAC_RX_LEN_MAX;
	mac->tx_done_pos = 0;
	mac->tx_desc = NULL;
	mac->tx_pos = 0;
	mac->tx_desc_full = 0;
	for (i = 0; i < TX_DESC_NUM; i++) {
		mac->tx_temp_skb_info[i].skb = NULL;
	}

	rc = descs_alloc(mac);
	if (rc) {
        ERROR0("[%s][%d] ethernet driver alloc mac descriptors fialed...\n", __FUNCTION__, __LINE__);
		return rc;
	}

	rc = descs_init(mac);
	if (rc) {
        ERROR0("[%s][%d] ethernet driver init mac descriptors fialed...\n", __FUNCTION__, __LINE__);
		goto INIT_DESC_FAIL;
	}

	mac_hw_stop(mac);

	/*register interrupt function to system*/
	//DEBUG0("@@@@@@@@@request_irq net_dev->irq= [%d] \n", net_dev->irq);
	//DEBUG0("@@@@@@@@@request_irq net_dev->name= [%s] \n", net_dev->name);
	rc = request_irq(net_dev->irq, ethernet_interrupt, IRQF_TRIGGER_HIGH, net_dev->name, net_dev);
	if (rc != 0) {
        ERROR0("[%s][%d] ethernet driver request irq %d fialed...rc [%d]\n", __FUNCTION__, __LINE__, net_dev->irq, rc);
		goto REQUEST_IRQ_FAIL;
	}
	DEBUG1();
	mac->tx_desc_full = 0;

	/*  Start phy  */
	//netif_carrier_off(net_dev);
	//mac_phy_start(net_dev);

	/*start hardware port,open interrupt, start system tx queue*/
	mac_init(mac);
	DEBUG1();
	//phy_cfg();
	mac_hw_start(mac);
	DEBUG1();

#if 0
    /* wait phy link*/
	while(1){
        reg = reg_control(REG_READ, MAC_GLB_PORT_STA, 0);
		if (reg & PHY_LINK){
            DEBUG0("[%s][%d] phy link ok, reg = 0x%x\n", __FUNCTION__, __LINE__, reg);	
			break;
		}
		else{
			udelay(1000);
            DEBUG0("[%s][%d] phy link failed, reg = 0x%x\n", __FUNCTION__, __LINE__, reg);	
		}
	}
#else
   // reg = mdio_read(MAC_PHY_ADDR, 1);
   // while((reg & (1 << 5)) == 0x0){
   //     reg = mdio_read(MAC_PHY_ADDR, 1);
   // }
#endif

	DEBUG1();


	if (netif_carrier_ok(net_dev)) {
		netif_start_queue(net_dev);
	}

	return 0;

REQUEST_IRQ_FAIL:
INIT_DESC_FAIL:
	descs_free(mac);
	return rc;
}

static int ethernet_stop(struct net_device *net_dev)
{
	struct l2sw_mac *mac;
	unsigned long flags;

	mac = (struct l2sw_mac*)netdev_priv(net_dev);

	spin_lock_irqsave(&mac->lock, flags);
	netif_stop_queue(net_dev);
	netif_carrier_off(net_dev);
    
#ifdef RX_POLLING
        napi_disable(&mac->napi);
#endif

	spin_unlock_irqrestore(&mac->lock, flags);

	mac_phy_stop(net_dev);
	mac_hw_stop(mac);

	synchronize_irq(net_dev->irq);
	free_irq(net_dev->irq, net_dev);
	descs_free(mac);
	return 0;

}

/* Transmit a packet (called by the kernel) */
static int ethernet_start_xmit(struct sk_buff *skb, struct net_device *net_dev)
{
    struct l2sw_mac *mac;
    u32 tx_pos;
    u32 cmd1;
	u32 cmd2;
	u32 force_dp=0x1;
	u32	to_vlan=0x1;
    volatile struct mac_desc *txdesc;

    cmd1 = 0;
	cmd2 = 0;
    mac = (struct l2sw_mac*)netdev_priv(net_dev);

    spin_lock_irq(&mac->lock); //or use spin_lock_irqsave ?
    if (mac->tx_desc_full == 1) { /* no desc left, wait for tx interrupt*/
        spin_unlock_irq(&mac->lock);
		DEBUG0("@@@@@@@@@l2sw ethernet_start_xmit fail \n");
        return -1;
    }

    tx_pos = mac->tx_pos;
	//DEBUG0("@@@@@@@@@l2sw ethernet_start_xmit skb->len = [%d] \n",skb->len);
    /* if skb size shorter than 60, fill it with '\0' */
    if (skb->len < ETH_ZLEN) {
        if (skb_tailroom(skb) >= (ETH_ZLEN - skb->len)) {
            memset(__skb_put(skb, ETH_ZLEN - skb->len), '\0', ETH_ZLEN - skb->len);
        } else {
            struct sk_buff *old_skb = skb;
            skb = dev_alloc_skb(ETH_ZLEN + TX_OFFSET);
            if (skb) {
                memset(skb->data + old_skb->len, '\0', ETH_ZLEN - old_skb->len);
                memcpy(skb->data, old_skb->data, old_skb->len);
                skb_put(skb, ETH_ZLEN); /* add data to an sk_buff */
                dev_kfree_skb_irq(old_skb);
            } else {
                skb = old_skb;
            }
        }
    }

    txdesc = &mac->tx_desc[tx_pos];
    mac->tx_temp_skb_info[tx_pos].len = skb->len;
    mac->tx_temp_skb_info[tx_pos].skb = skb;
	mac->tx_temp_skb_info[tx_pos].mapping = dma_map_single(&mac->pdev->dev, skb->data, skb->len, DMA_TO_DEVICE);	
    //cmd1 = FS_BIT | LS_BIT | (skb->len & LEN_MASK);
	cmd1 = (OWN_BIT | FS_BIT | LS_BIT | (force_dp<<18) | (to_vlan<<12)| (skb->len& LEN_MASK));
	cmd2 = (mac->tx_pos+1==TX_DESC_NUM)?EOR_BIT|(skb->len&LEN_MASK):(skb->len&LEN_MASK);

    txdesc->addr1 = mac->tx_temp_skb_info[tx_pos].mapping;
    txdesc->cmd1 = cmd1;
	txdesc->cmd2 = cmd2;

    wmb();
    NEXT_TX(tx_pos);
    mac->tx_pos = tx_pos;
    
    if (mac->tx_pos == mac->tx_done_pos) {
		netif_stop_queue(net_dev);
		mac->tx_desc_full = 1;
	}

    /* trigger gmac to transmit*/
    tx_trigger(tx_pos);
    wmb();
    
    spin_unlock_irq(&mac->lock);

    return 0;
}

static void ethernet_set_rx_mode(struct net_device *dev)
{
	struct l2sw_mac *mac;
	DEBUG0("@@@@@@@@@l2sw ethernet_set_rx_mode \n");
	mac = (struct l2sw_mac*)netdev_priv(dev);
	rx_mode_set(mac);
}

static int ethernet_set_mac_address(struct net_device *ndev, void *addr)
{
	struct sockaddr *hwaddr = (struct sockaddr *)addr;
	struct l2sw_mac *mac = netdev_priv(ndev);

	if (netif_running(ndev)) {
		return -EBUSY;
	}

	memcpy(ndev->dev_addr, hwaddr->sa_data, ndev->addr_len);

	/* set the Ethernet address */
	memcpy(mac->mac_addr, hwaddr->sa_data, ndev->addr_len);
	mac_hw_addr_set(mac);

	return 0;
}

static int ethernet_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
    struct l2sw_mac *mac = netdev_priv(dev);
    struct phy_device *phydev = mac->phy_dev;

	DEBUG0("@@@@@@@@@l2sw ethernet_do_ioctl \n");
    if (!netif_running(dev))
        return -EINVAL;

    if (!phydev)
        return -ENODEV;

    switch (cmd) {
    case SIOCGMIIPHY:
    case SIOCGMIIREG:
    case SIOCSMIIREG:
        return phy_mii_ioctl(phydev, ifr, cmd);

    default:
        printk(KERN_INFO "GEM: ioctl %d not implemented.\n", cmd);
        return -EOPNOTSUPP;
    }

}

static int ethernet_change_mtu(struct net_device *dev, int new_mtu)
{
    return eth_change_mtu(dev, new_mtu);
}

static void ethernet_tx_timeout(struct net_device *net_dev)
{

}

static struct net_device_stats *ethernet_get_stats(struct net_device *net_dev)
{
	struct l2sw_mac *mac;
	DEBUG0("@@@@@@@@@l2sw ethernet_get_stats \n");
	mac = (struct l2sw_mac*)netdev_priv(net_dev);
	return &mac->dev_stats;
}


static struct net_device_ops netdev_ops = {
	.ndo_open		= ethernet_open,
	.ndo_stop		= ethernet_stop,
	.ndo_start_xmit		= ethernet_start_xmit,
	.ndo_set_rx_mode = ethernet_set_rx_mode,
	.ndo_set_mac_address    = ethernet_set_mac_address,
	.ndo_do_ioctl		= ethernet_do_ioctl,
	.ndo_change_mtu		= ethernet_change_mtu,
	.ndo_tx_timeout		= ethernet_tx_timeout,
	.ndo_get_stats		= ethernet_get_stats,
};

/*********************************************************************
*
* platform_driver
*
**********************************************************************/

static u32 netdev_init(struct platform_device *pdev)
{
	u32 ret = -ENODEV;
	struct l2sw_mac *mac;
	struct net_device *net_dev;
	struct resource *res;
	struct resource *r_mem = NULL;

	DEBUG0("l2sw netdev_init \n");
	/* allocate the devices, and also allocate l2sw_mac, we can get it by  netdev_priv() */
	if ((net_dev = alloc_etherdev(sizeof(struct l2sw_mac))) == NULL) {
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, net_dev);
	printk("l2sw l2sw ndev->name =[%s]\n", net_dev->name);
	mac = netdev_priv(net_dev);
	mac->net_dev = net_dev;
    mac->pdev = pdev;
    spin_lock_init(&mac->lock);
    /*
        * spin_lock: return if it obtain spin lock, or it will wait (not sleep)
        * spin_lock_irqsave: save flags, disable interrupt, obtain spin lock           
        * spin_lock_irq:                       disable interrupt, obtain spin lock, if in a interrupt, don't need to use spin_lock_irqsave
        */

	if ((r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0)) != NULL) {
		printk("l2sw l2sw r_mem->start =[%x]\n", r_mem->start);
		if (l2sw_reg_base_set(ioremap(r_mem->start, (r_mem->end - r_mem->start + 1))) != 0){
			ERROR0("[%s][%d] remap fail!\n", __FUNCTION__, __LINE__);
			goto out_freedev;
		}
		
	} else {
		ERROR0("[%s][%d] no MEM resource!\n", __FUNCTION__, __LINE__);
		goto out_freedev;
	}
	

	if ((res = platform_get_resource(pdev, IORESOURCE_IRQ, 0)) != NULL) {
		//printk("l2sw res->start =[%x]\n", res->start);
		//printk("l2sw res->name =[%s]\n", res->name);
		net_dev->irq = res->start;

	} else {
		ERROR0("[%s][%d] no IRQ resource!\n", __FUNCTION__, __LINE__);
		goto out_freedev;
	}

	l2sw_pinmux_set();
	l2sw_enable_port();
	

	phy_cfg();


#if 1 // phy_probe??
#ifdef PHY_CONFIG
    mac->phy_node = of_parse_phandle(pdev->dev.of_node, "phy-handle", 0);
#endif

	if ((ret = mdio_init(pdev, net_dev))) {
		ERROR0("[%s][%d] mdio init failed!\n", __FUNCTION__, __LINE__);
		goto out_freedev;
	}

	
	if ((ret = mac_phy_probe(net_dev))) {
		ERROR0("[%s][%d] phy probe failed!\n", __FUNCTION__, __LINE__);
		goto out_freemdio;
	}
#endif
	net_dev->netdev_ops = &netdev_ops;
    
#ifdef RX_POLLING
    netif_napi_add(net_dev, &mac->napi, rx_poll, RX_NAPI_WEIGHT);
#endif

    if (mac_hw_addr_update(mac) != 0) {
        memcpy(mac->mac_addr, def_mac_addr, ETHERNET_MAC_ADDR_LEN);
    }
	memcpy(net_dev->dev_addr, mac->mac_addr, ETHERNET_MAC_ADDR_LEN);
	if ((ret = register_netdev(net_dev))) {
		ERROR0("[%s][%d] error %i registering device \"%s\"\n", __FUNCTION__, __LINE__, ret, net_dev->name);
		goto out_freemdio;
	}

#ifndef INTERRUPT_IMMEDIATELY
    mac->int_status = 0;
	tasklet_init(&mac->rx_tasklet, rx_do_tasklet, (unsigned long)mac);
	//tasklet_disable(&mac->rx_tasklet);
	tasklet_init(&mac->tx_tasklet, tx_do_tasklet, (unsigned long)mac);
	//tasklet_disable(&mac->tx_tasklet);
#endif
	

	DEBUG0("l2sw netdev_init su\n");
	return 0;

out_freemdio:
	if (mac->mii_bus != NULL) {
		mdio_remove(mac);
	}
out_freedev:
	if (net_dev != NULL) {
		free_netdev(net_dev);
	}
	platform_set_drvdata(pdev, NULL);
	return ret;
}

static int l2sw_probe(struct platform_device *pdev)
{	

	int reg;
	
	DEBUG0("l2sw_probe \n");
	if (platform_get_drvdata(pdev) != NULL) {
		return -ENODEV;
	}
	
	return netdev_init(pdev);
}

static int l2sw_remove(struct platform_device *pdev)
{
	struct net_device *net_dev;
	struct l2sw_mac *mac;

	if ((net_dev = platform_get_drvdata(pdev)) == NULL) {
		return 0;
	}

	mac = (struct l2sw_mac*)netdev_priv(net_dev);

#ifndef INTERRUPT_IMMEDIATELY
    tasklet_kill(&mac->rx_tasklet);
	tasklet_kill(&mac->tx_tasklet);
#endif

	mac_phy_remove(net_dev);
	mdio_remove(mac);
	unregister_netdev(net_dev);
	free_netdev(net_dev);
	platform_set_drvdata(pdev, NULL);
	return 0;

}

#ifdef CONFIG_PM
static int l2sw_suspend(struct device *dev)
{
    struct platform_device *pdev = to_platform_device(dev);
	struct net_device *ndev = platform_get_drvdata(pdev);
    
	netif_device_detach(ndev);
    //shut down 
    
	return 0;
}

static int l2sw_resume(struct device *dev)
{
    struct platform_device *pdev = to_platform_device(dev);
	struct net_device *ndev = platform_get_drvdata(pdev);

    //reset & init
	netif_device_attach(ndev);
	return 0;
}

static const struct dev_pm_ops l2sw_pm_ops = {
	.suspend	= l2sw_suspend,
	.resume		= l2sw_resume,
};
#endif

#ifdef PHY_CONFIG
static const struct of_device_id sp_l2sw_of_match[] = {
	{ .compatible = "sunplus,sp-l2sw" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, sp_l2sw_of_match);
#endif

static struct platform_driver l2sw_driver = {
	.probe   = l2sw_probe,
	.remove  = l2sw_remove,
	.driver  = {
		.name  = "sp_l2sw",
		.owner = THIS_MODULE,
		.of_match_table = sp_l2sw_of_match,
#ifdef CONFIG_PM
		.pm = &l2sw_pm_ops, // not sure
#endif
	},
};



static int __init l2sw_init(void)
{
    u32 status;
	DEBUG0("l2sw init \n");
    status = platform_driver_register(&l2sw_driver);

    return status;
}

static void __exit l2sw_exit(void)
{
	platform_driver_unregister(&l2sw_driver);
}
#endif

module_init(l2sw_init);
module_exit(l2sw_exit);
