#ifndef NINJA_NETLINK_H
#define NINJA_NETLINK_H

#include "ninja_syshdrs.h"
#include "ninja_defs_i.h"
#include "ninja_defs_e.h"

#include "ninja_th.h"
#include "ninja_io.h"
#include "ninja.h"
#include <linux/netlink.h>

void ninja_netlink_init( void);
void ninja_netlink_clean( void);

#endif // NINJA_NETLINK_H
