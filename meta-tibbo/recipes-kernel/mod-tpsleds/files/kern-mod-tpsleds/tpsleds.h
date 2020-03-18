#ifndef TPSLEDS_H
#define TPSLEDS_H

#define MNAME "tpsleds"
#define M_LIC "GPL v2"
#define M_AUT "Dvorkin Dmitry dvorkin@tibbo.com"
#define M_NAM "Tibbo TPSLeds module"
#define M_ORG "Tibbo Tech."
#define M_CPR "(C) 2015-2016"

#define TPS_MAX_LEDS 5

#include "tpsleds_syshdrs.h"
#include "tpsleds_defs.h"

#include "tpsleds_procfs.h"

// gpio1_27 - data
#define DV_BLED_DT 59
// gpio1_25 - front
#define DV_BLED_FT 57

void tps_leds_set( unsigned char _mask);

//struct ninja_skbq_T {
// spinlock_t my_spinlock;
// // current queue len
// ulong q_len;
// // number of dropped packets because of the size
// ulong q_dropped;
// struct sk_buff_head skbs;
//};
//typedef struct ninja_skbq_T ninja_skbq_t;

#endif // TPSLEDS_H
