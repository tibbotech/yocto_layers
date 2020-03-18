#ifndef __GMAC_DESC_H__
#define __GMAC_DESC_H__

#include "l2sw_define.h"

u32 fix_net_dma(void *addr);

void rx_descs_flush(struct l2sw_mac *mac);

void tx_descs_clean(struct l2sw_mac *mac);

void rx_descs_clean(struct l2sw_mac *mac);

void descs_clean(struct l2sw_mac *mac);

void descs_free(struct l2sw_mac *mac);

u32 tx_descs_init(struct l2sw_mac *mac);

u32 rx_descs_init(struct l2sw_mac *mac);

u32 descs_init(struct l2sw_mac *mac);

u32 descs_alloc(struct l2sw_mac *mac);

#endif
