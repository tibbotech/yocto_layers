#ifndef TWG_DEFS_I_H
#define TWG_DEFS_I_H

#define TWG_MAX_NAM 64

#ifdef TWG_DEBUG
#define DBGM(x) ( debug_mask & x)
//#define DBGM(x) true
#define KDBG(fmt,args...) printk( KERN_DEBUG "twg: "fmt,##args)
#define KINF(fmt,args...) printk( KERN_INFO "twg: "fmt,##args)
#define KERR(fmt,args...) printk( KERN_ERR "twg: "fmt,##args)
#else
#define DBGM(x) false
#define KDBG(fmt,args...) 
#define KINF(fmt,args...) 
#define KERR(fmt,args...) 
#endif

#define TWG_MODE_OFF 99

typedef struct twg_pdata_T {
 uint8_t mode;		// 0=clock/data, 1=Wg reader
 int pin_w0;
 int pin_w1;
 int pin_sw;
 int pin_oc;
 int irq_w0;
 int irq_w1;
 char name[ TWG_MAX_NAM];
 twg_data_t d;
 uint8_t  d_done;	// is buffer collecting finished or not?
 uint16_t stat_over;	// number or buffer overruns
 uint16_t stat_done;	// number or buffer overruns
 struct timer_list timer;
 uint16_t det;		// data end timeout (ms)
// struct kobject wKo;
// struct attribute_group sysfs_ag;
 uint8_t debug;
 uint8_t clear_on_read;
} twg_pdata_t;

extern int change_mode( twg_pdata_t *_p, uint8_t _m);
extern void clean_buff( twg_pdata_t *_p);

#endif // TPSID_DEFS_I_H
