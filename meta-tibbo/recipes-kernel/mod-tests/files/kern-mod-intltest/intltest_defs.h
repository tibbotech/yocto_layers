#ifndef INTLTEST_DEFS_H
#define INTLTEST_DEFS_H

#define INTLTEST_LEN 6

#ifdef INTLTEST_DEBUG
#define DBGM(x) ( debug_mask & x)
//#define DBGM(x) true
#define KDBG(fmt,args...) printk( KERN_DEBUG "intltest: "fmt,##args)
#define KINF(fmt,args...) printk( KERN_INFO "intltest: "fmt,##args)
#define KERR(fmt,args...) printk( KERN_ERR "intltest: "fmt,##args)
#else
#define DBGM(x) false
#define KDBG(fmt,args...) 
#define KINF(fmt,args...) 
#define KERR(fmt,args...) 
#endif

extern ktime_t int_last;
extern s64 intl_min, intl_max;

#endif // INTLTEST_DEFS_H
