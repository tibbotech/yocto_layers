#ifndef NINJA_HNDL_H
#define NINJA_HNDL_H

#include "ninja_syshdrs.h"
#include "ninja_defs_i.h"
#include "ninja_defs_e.h"

#include "ninja_th.h"
#include "ninja_io_net.h"
#include "ninja_cmd.h"
#include "ninja.h"

// shared
int netcmd_proc( kthread_t *_tp, unsigned char *_in, int _inl);
int dv_s_sendnjskb( kthread_t *_tp, struct sk_buff *_skb);
int dv_io_rw_dch( kthread_t *_tp);
int dv_io_rw_cch( kthread_t *_tp);

#endif // NINJA_HNDL_H
