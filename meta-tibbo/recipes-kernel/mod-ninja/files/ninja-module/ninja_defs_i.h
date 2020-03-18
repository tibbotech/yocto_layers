#ifndef NINJA_DEFS_I_H
#define NINJA_DEFS_I_H

// ///////////////
// this one for gadget internals
// ///////////////

// Netlink high-speed interface proto num for in-target interaction
#define NINJA_NETLINK_ID 31

// default max memory for skb queue
#define NINJA_MAXQ_MEM 120000		// in Kb

// if not enogth room in Q - policy for droping packets:
// -1 - yes (from head)
// 0 - no (do not add)
// +1 - yes (from tail)
#define NINJA_MAXQ_FREE 0

// thread name max length
#define NINJA_THNAME_LEN 64

// debugging
extern ulong debug_mask;

#ifdef NINJA_DEBUG
#define DBGM(x) ( debug_mask & x)
//#define DBGM(x) true
#define KDBG(fmt,args...) printk( KERN_DEBUG "ninja: "fmt,##args)
#define KINF(fmt,args...) printk( KERN_INFO "ninja: "fmt,##args)
#define KERR(fmt,args...) printk( KERN_ERR "ninja: "fmt,##args)
#else
#define DBGM(x) false
#define KDBG(fmt,args...) 
#define KINF(fmt,args...) 
#define KERR(fmt,args...) 
#endif

//#define NINJA_DBG_NET_PROC 0x00000001
//#define NINJA_DBG_NET_DUMP 0x00000002
#define NINJA_DBG_USB_CTF	0x00000100
#define NINJA_DBG_USB_CRXD	0x00000200
#define NINJA_DBG_USB_CTXD	0x00000400
#define NINJA_DBG_USB_RXF	0x00000800
#define NINJA_DBG_USB_TXF	0x00001000
#define NINJA_DBG_USB_RXD	0x00002000
#define NINJA_DBG_USB_TXD	0x00004000
#define NINJA_DBG_CMD		0x00010000
//#define NINJA_DBG_ 0x00000101

#endif // NINJA_DEFS_I_H
