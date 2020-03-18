#ifndef TPSID_DEFS_H
#define TPSID_DEFS_H

#define TPSID_LEN 6

#ifdef TPSID_DEBUG
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


extern unsigned char tpsid[ TPSID_LEN + 1];

#endif // TPSID_DEFS_H
