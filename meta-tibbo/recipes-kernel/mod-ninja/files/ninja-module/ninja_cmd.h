#ifndef NINJA_CMD_H
#define NINJA_CMD_H

#include "ninja_syshdrs.h"
#include "ninja_defs_i.h"
#include "ninja_defs_e.h"
#include "ninja_th.h"
#include "ninja.h"

// shared
void tcmd_stat2buf( unsigned char *_b, ninja_skbq_t *_s);

// text cmd proc
int tcmd_proc( kthread_t *_tp, NINJA_CMD_T _cmd, unsigned char *_in, int _inl);

struct ninja_cmddesc_T {
 NINJA_CMD_T cmd;
 u8 need_buf_rx;
 u8 is_fast;
};

typedef struct ninja_cmddesc_T ninja_cmddesc_t;

ninja_cmddesc_t *tcmd_find_desc( NINJA_CMD_T _cmd);

#endif // NINJA_CMD_H
