#ifndef NINJA_IO_NET_H
#define NINJA_IO_NET_H

#include "ninja_syshdrs.h"
#include "ninja_defs_i.h"
#include "ninja_defs_e.h"

#include "ninja_th.h"
#include "ninja_io.h"
#include "ninja.h"

// shared
int dv_s_O_tcps( void *_tp);
int dv_s_O_udps( void *_tp);
int dv_s_R( kthread_t *_tp, unsigned char *_b, int _l);
int dv_s_W( kthread_t *_tp, unsigned char *_b, int _l);
int dv_s_C( kthread_t *_tp);

int isP2P( kthread_t *_tp);

void ninja_io_net_init( void);
void ninja_io_net_clean( void);

// per-thread buffers
struct th_buffs_T {
 unsigned char *b_d_in;
 unsigned char *b_c_in;
};
typedef struct th_buffs_T th_buffs_t;

// per-thread buffers allocation function
th_buffs_t *dv_s_new_thdata( kthread_t *_tp);

#endif // NINJA_IO_NET_H
