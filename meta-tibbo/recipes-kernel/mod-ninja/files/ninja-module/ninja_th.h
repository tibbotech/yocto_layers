#ifndef NINJA_TH_H
#define NINJA_TH_H

#include "ninja_syshdrs.h"
#include "ninja_defs_i.h"
#include "ninja_defs_e.h"

#include <linux/kthread.h>
#include <linux/delay.h>

enum DV_TH_STATES {
 DV_THS_DELIST	 	= -3,
 DV_THS_DESTROY 	= -2,
 DV_THS_STOPPING 	= -1,
 DV_THS_RESTARTING 	=  0,
 DV_THS_RUNNING 	=  1,
};

struct kthread_T {
 unsigned char thname[ NINJA_THNAME_LEN];
 struct task_struct *thread;
 struct socket *sock;
 void *peer_id;
 int is_worker;
 // per-thread data (usually IN buffer for net)
 void *th_data;
 // thread state
 enum DV_TH_STATES state;
 // callback to function inside the thread cicle
 int 	(*W)( struct kthread_T *_tp);
 // call this from _worker_ thread (destroy/close callback)
 int 	(*C)( struct kthread_T *_tp);
 // linkage between each other
 struct list_head list;
};
typedef struct kthread_T kthread_t;

extern int thread_counter;

extern kthread_t ninja_threads;

struct th_tasks_T {
 int	(*O)( void *_tp);
 int	(*W)( struct kthread_T *_tp);
 int	(*C)( struct kthread_T *_tp);
};
typedef struct th_tasks_T th_tasks_t;

// shared
// _thfnp - start, _thdfnp - destruction handler
kthread_t *dv_th_init( unsigned char *_thn, int (*_thfnp)( void *_data), int (*_thdfnp)( kthread_t *_tp));
kthread_t *dv_th_init_task( unsigned char *_thn, th_tasks_t _task);
kthread_t *dv_th_run( kthread_t *_thp);
kthread_t *dv_th_stop( kthread_t *_thp);

int th_state_handle( kthread_t *_tp);

#endif // NINJA_TH_H
