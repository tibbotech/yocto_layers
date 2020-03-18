#ifndef NINJA_H
#define NINJA_H

#define MNAME "ninja"
#define M_LIC "GPL"
#define M_AUT "Dvorkin Dmitry dvorkin@tibbo.com"
#define M_NAM "Tibbo Ninja module"
#define M_ORG "Tibbo Tech."
#define M_CPR "(C) 2011-2014"

#define M_G_NAM "Ninja Gadget"
#define M_G_SER "0123456789.0123456789.0123456789"

#include "ninja_syshdrs.h"
#include "ninja_defs_i.h"
#include "ninja_defs_e.h"

#include "ninja_th.h"
#include "ninja_io_net.h"
#include "ninja_io_usb.h"
#include "ninja_procfs.h"

struct ninja_skbq_T {
 spinlock_t my_spinlock;
 // current queue len
 ulong q_len;
 // number of dropped packets because of the size
 ulong q_dropped;
 struct sk_buff_head skbs;
};
typedef struct ninja_skbq_T ninja_skbq_t;

#endif // NINJA_H
