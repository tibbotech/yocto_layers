#ifndef TPSLEDS_DEFS_H
#define TPSLEDS_DEFS_H

// debugging
extern ulong debug_mask;

#ifdef TPSLEDS_DEBUG
#define DBGM(x) ( debug_mask & x)
//#define DBGM(x) true
#define KDBG(fmt,args...) printk( KERN_DEBUG "tpsleds: "fmt,##args)
#define KINF(fmt,args...) printk( KERN_INFO "tpsleds: "fmt,##args)
#define KERR(fmt,args...) printk( KERN_ERR "tpsleds: "fmt,##args)
#else
#define DBGM(x) false
#define KDBG(fmt,args...) 
#define KINF(fmt,args...) 
#define KERR(fmt,args...) 
#endif

extern unsigned char leds_current;
extern unsigned char leds_num;

typedef struct {
 unsigned short number;
 unsigned int timeout;
 struct timer_list timer;
} led_timer_t;

// bits for each: 0 - disabled, 1- enabled
extern unsigned char leds_timers;

extern led_timer_t ltimers[];

#endif // TPSLEDS_DEFS_H
