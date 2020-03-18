#include "l2sw_mdio.h"

static int mii_read(struct mii_bus *bus, int phy_id, int regnum)
{
	u32 ret;

	ret = mdio_read(phy_id, regnum);
	return ret;
}

static int mii_write(struct mii_bus *bus, int phy_id, int regnum, u16 val)
{
	u32 ret;

	ret = mdio_write(phy_id, regnum, val);
	return ret;
}

u32 mdio_init(struct platform_device *pdev, struct net_device *netdev)
{
	struct mii_bus *mii_bus;
	struct l2sw_mac *mac = netdev_priv(netdev);
	u32 ret, i,reg;

#ifdef PHY_CONFIG
    struct resource res;
	struct device_node *np = of_get_parent(mac->phy_node);

#endif

	if ((mii_bus = mdiobus_alloc()) == NULL) {
		ERROR0("[%s][%d] Allocate mdio bus failed!\n", __FUNCTION__, __LINE__);
		return -ENOMEM;
	}

	mii_bus->name = "7020 MII bus";
	mii_bus->parent = &pdev->dev;
	mii_bus->priv = mac;
	mii_bus->read = mii_read;
	mii_bus->write = mii_write;
	//mii_bus->irq = mac->phy_irq;
	for (i = 0; i < PHY_MAX_ADDR; ++i) {
		mii_bus->irq[i] = PHY_POLL;
	}

#if 0
    /* need do auto-negotiation here? */
    reg = mdio_read(MAC_PHY_ADDR, 1);
    DEBUG0("[%s][%d] mdio_read reg=0x%x\n", __FUNCTION__, __LINE__, reg);
    while((reg & (1 << 5)) == 0x0){
        DEBUG0("[%s][%d] Waiting for auto-negotiation complete\n", __FUNCTION__, __LINE__);
        reg = mdio_read(MAC_PHY_ADDR, 1);
    }
#endif

#ifdef PHY_CONFIG
    of_address_to_resource(np, 0, &res);
	snprintf(mii_bus->id, MII_BUS_ID_SIZE, "%.8llx", (unsigned long long)res.start);
	if (mac->phy_node) {
		if (of_mdiobus_register(mii_bus, np)) {
			ERROR0("[%s][%d] Register mii bus failed. err = %d\n", __FUNCTION__, __LINE__, ret);
        	mdiobus_free(mii_bus);        
		}
	}else{
		mac->mii_bus = mii_bus;
		return 0;
	}
	//if (ret = of_mdiobus_register(mii_bus, np)){
    //    ERROR0("[%s][%d] Register mii bus failed. err = %d\n", __FUNCTION__, __LINE__, ret);
    //    mdiobus_free(mii_bus);        
    //}
#else	
    mii_bus->phy_mask = ~0x1f; /*  default phy address mask, need to change ?  */
	snprintf(mii_bus->id, MII_BUS_ID_SIZE, "%x", pdev->id);
	if ((ret = mdiobus_register(mii_bus)) < 0) { /* this will fill phy_map ... */
		ERROR0("[%s][%d] Register mii bus failed. err = %d\n", __FUNCTION__, __LINE__, ret);
		mdiobus_free(mii_bus);
	}	
#endif

    mac->mii_bus = mii_bus;
	return ret;

}

void mdio_remove(struct l2sw_mac *mac)
{
	if (mac->mii_bus != NULL) {
		mdiobus_unregister(mac->mii_bus);
		mdiobus_free(mac->mii_bus);
		mac->mii_bus = NULL;
	}
}

#ifdef PHY_RUN_STATEMACHINE
static void mii_linkchange(struct net_device *netdev)
{

}
#endif

int mac_phy_probe(struct net_device *netdev)
{

    struct l2sw_mac *mac = netdev_priv(netdev);
    struct phy_device *phydev = NULL;

	if (!mac->phy_node) {
		DEBUG0("mac_phy_probe: no PHY setup\n");
		return 0;
	}
#ifdef PHY_CONFIG
    if (mac->phy_node) {
		phydev = of_phy_connect(mac->net_dev,
					mac->phy_node,
					mii_linkchange,
					0,
					PHY_INTERFACE_MODE_RGMII_ID);
	}
	if (!phydev) {
		ERROR0("%s: no PHY found\n", netdev->name);
		return -1;
	}
#else
    /* find the first (lowest address) PHY on the current MAC's MII bus */
    for (phyaddr = 0; phyaddr < PHY_MAX_ADDR; phyaddr++) {
        if (mac->mii_bus->phy_map[phyaddr]) {
            phydev = mac->mii_bus->phy_map[phyaddr];
            break; /* break out with first one found */
        }
    }
    if (phydev == NULL) {
        ERROR0("[%s][%d] No ephy!\n", __FUNCTION__, __LINE__);
        return -ENODEV;
    }
#endif
    
#ifdef PHY_RUN_STATEMACHINE

#ifndef PHY_CONFIG
    phydev = phy_connect(netdev, dev_name(&phydev->dev), &mii_linkchange, 0, PHY_INTERFACE_MODE_GMII);
    if (IS_ERR(phydev)) {
        ERROR0("[%s][%d] phy_connect failed!\n", __FUNCTION__, __LINE__);
        return -ENODEV;
    }
#endif /* PHY_CONFIG */   

    phydev->supported &= (PHY_GBIT_FEATURES | SUPPORTED_Pause |
							SUPPORTED_Asym_Pause);
    
    phydev->advertising = phydev->supported;
    phydev->irq = PHY_IGNORE_INTERRUPT;
    mac->phy_dev = phydev;    
#endif /* PHY_RUN_STATEMACHINE */

    return 0;
}

void mac_phy_start(struct net_device *netdev)
{
    struct l2sw_mac *mac = netdev_priv(netdev);

#ifdef PHY_RUN_STATEMACHINE
    phy_start(mac->phy_dev);
#else
    u32 phyaddr;
    struct phy_device *phydev = NULL;

    /* find the first (lowest address) PHY on the current MAC's MII bus */
    for (phyaddr = 0; phyaddr < PHY_MAX_ADDR; phyaddr++) {
        if (mac->mii_bus->phy_map[phyaddr]) {
            phydev = mac->mii_bus->phy_map[phyaddr];
            break; /* break out with first one found */
        }
    }

    phydev = phy_attach(netdev, dev_name(&phydev->dev), 0, PHY_INTERFACE_MODE_GMII);
    if (IS_ERR(phydev)) {
        ERROR0("[%s][%d] phy_connect failed!\n", __FUNCTION__, __LINE__);
        return;
    }

    phydev->supported &= (PHY_GBIT_FEATURES | SUPPORTED_Pause |
							SUPPORTED_Asym_Pause);
    phydev->advertising = phydev->supported;
    phydev->irq = PHY_IGNORE_INTERRUPT;
    mac->phy_dev = phydev;

    phy_start_aneg(phydev);
#endif
}

void mac_phy_stop(struct net_device *netdev)
{
    struct l2sw_mac *mac = netdev_priv(netdev);

    if (mac->phy_dev != NULL) {
#ifdef PHY_RUN_STATEMACHINE
        phy_stop(mac->phy_dev);
#else
        phy_detach(mac->phy_dev);
        mac->phy_dev = NULL;
#endif
    }
}

void mac_phy_remove(struct net_device *netdev)
{
    struct l2sw_mac *mac = netdev_priv(netdev);

    if (mac->phy_dev != NULL) {
#ifdef PHY_RUN_STATEMACHINE
        phy_disconnect(mac->phy_dev);
#endif
        mac->phy_dev = NULL;
    }
}


