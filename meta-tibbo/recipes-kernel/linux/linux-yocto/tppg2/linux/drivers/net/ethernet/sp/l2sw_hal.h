#ifndef __L2SW_HAL_H__
#define __L2SW_HAL_H__

#include "l2sw_register.h"
#include "l2sw_define.h"
#include "l2sw_desc.h"
#include <mach/gpio_drv.h>

#define GMAC_REG_BASE 0x40004b80


#define HWREG_W(M, N) (ls2w_reg_base->M = N)
#define HWREG_R(M) (ls2w_reg_base->M)


/* for reg_control() */
#define REG_WRITE 0
#define REG_READ  1

#define PHY0_ADDR		0x0
#define PHY1_ADDR		0x1


#define MAC_GLB_INT_STATUS 15100
#define MAC_GLB_INT_MASK  15101
#define MAC_GLB_PORT_STA 15106

#define MAC_TX_DESC_ADDR 15207

#define MAC_RX_DESC0_ADDR 15315
#define MAC_RX_DESC1_ADDR 15316

#define MAC_GLB_PHY_ADDR 15402


#define MDIO_RW_TIMEOUT_RETRY_NUMBERS 500

int l2sw_reg_base_set( void __iomem *baseaddr);

int l2sw_pinmux_set(void);

int mac_g1_base_set(void __iomem *baseaddr);

void mac_hw_stop(struct l2sw_mac *mac);

void mac_hw_reset(struct l2sw_mac *mac);

void mac_hw_start(struct l2sw_mac *mac);

void mac_hw_addr_set(struct l2sw_mac *mac);

void mac_hw_init(struct l2sw_mac *mac);

u32 mac_hw_addr_update(struct l2sw_mac *mac);

void rx_mode_set(struct l2sw_mac *mac);

u32 mdio_read(u32 phy_id, u16 regnum);

u32 mdio_write(u32 phy_id, u32 regnum, u16 val);

void rx_finished(u32 queue, u32 rx_count);

void tx_trigger(u32 tx_pos);

u32 reg_control(u8 mode, u32 reg, u32 value);

void rx_mib_counter_print(void);

void tx_mib_counter_print(void);

int phy_cfg(void);

void l2sw_enable_port(void);

void l2sw_mac_set(void);


#endif

