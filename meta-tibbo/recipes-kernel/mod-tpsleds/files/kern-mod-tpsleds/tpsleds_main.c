#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
//#include <linux/irq.h>
#include <linux/interrupt.h>

#include "tpsleds.h"
#include "tpsleds_procfs.h"

#define DV_DRV_NAME "tpsleds"

unsigned char leds_current;
unsigned char leds_num;

struct tpsleds_T {
 u16 pin_d;		// data pin
 u16 pin_f;		// front pin
};

static struct tpsleds_T tpsleds_d;

unsigned char leds_timers;	// bits field

led_timer_t ltimers[ TPS_MAX_LEDS];

static void tps_led( unsigned char _val) {
 // zero - enable led at current position, 1 - disable
 gpio_set_value( tpsleds_d.pin_d, ( _val ? 0 : 1));
 // notify
 gpio_set_value( tpsleds_d.pin_f, 1);
 gpio_set_value( tpsleds_d.pin_f, 0);
 return;  }

void tps_leds_set( unsigned char _mask) {
 int i = 0;
 leds_current = _mask;
 // 0x10 == 10000
 for ( i = 0; i < TPS_MAX_LEDS; i++) {
   tps_led( ( _mask & 0x01 << i));
 }
 return;  }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 4, 15, 0))
void tps_led_timer( struct timer_list *_t) {
 led_timer_t *lt = from_timer( lt, _t, timer);
 unsigned long _d = lt->number;
#else
void tps_led_timer( unsigned long _d) {
#endif
 if ( _d > leds_num) return;
 // if switched off - exit;
 if ( ( leds_timers & 0x01 << _d) == 0) return;
 if ( ltimers[ _d].timeout == 0) return;
 // switch state : if 1 -> 0 and wice versa
 if ( 1 && ( leds_current & 0x01 << _d)) leds_current &= ~( 1 << _d);
 else leds_current |= ( 1 << _d);
 tps_leds_set( leds_current);
 mod_timer( &( ltimers[ _d].timer), ltimers[ _d].timeout + jiffies);
 return;  }

static int tpsleds_probe( struct platform_device *_pdev) {
 int ret = 0;
 unsigned char init_val = 0x7;
 u32 *init_v32p;
 int lenp;
 leds_num = TPS_MAX_LEDS;
 if ( !_pdev->dev.of_node) {
   printk( KERN_INFO "%s: %s OF not found\n", DV_DRV_NAME, __FUNCTION__);
   return( -ENXIO);  }
 tpsleds_d.pin_d = of_get_gpio( _pdev->dev.of_node, 0);
//printk( KERN_INFO "%s: 00 :%d\n", DV_DRV_NAME, tpsleds_d.pin_d);
 tpsleds_d.pin_f = of_get_gpio( _pdev->dev.of_node, 1);
 if ( !gpio_is_valid( tpsleds_d.pin_d)) tpsleds_d.pin_d = DV_BLED_DT;
 if ( !gpio_is_valid( tpsleds_d.pin_d)) {
   printk( KERN_INFO "%s: %s gpio pin (%d) is invalid\n", DV_DRV_NAME, __FUNCTION__, tpsleds_d.pin_d);
   tpsleds_d.pin_d = 0;
   return( -ENXIO);  }
 if ( !gpio_is_valid( tpsleds_d.pin_f)) tpsleds_d.pin_f = DV_BLED_FT;
 if ( !gpio_is_valid( tpsleds_d.pin_f)) {
   printk( KERN_INFO "%s: %s gpio pin (%d) is invalid\n", DV_DRV_NAME, __FUNCTION__, tpsleds_d.pin_f);
   tpsleds_d.pin_f = 0;
   return( -ENXIO);  }
 ret = gpio_request( tpsleds_d.pin_d, DV_DRV_NAME "-pin-data");
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_request(%d) failed\n", DV_DRV_NAME, __FUNCTION__, tpsleds_d.pin_d);
   return( ret);   }
 ret = gpio_request( tpsleds_d.pin_f, DV_DRV_NAME "-pin-front");
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_request(%d) failed\n", DV_DRV_NAME, __FUNCTION__, tpsleds_d.pin_f);
   gpio_free( tpsleds_d.pin_d);
   return( ret);   }
 gpio_direction_output( tpsleds_d.pin_d, 0);		// for data
 gpio_direction_output( tpsleds_d.pin_f, 0);		// front
 init_v32p = of_get_property( _pdev->dev.of_node, "init-val", &lenp);
 if ( init_v32p) {
   *init_v32p = be32_to_cpup( init_v32p);
   init_val = ( unsigned char)( *init_v32p);
 }
 init_v32p = of_get_property( _pdev->dev.of_node, "max-leds", &lenp);
 if ( init_v32p) {
   *init_v32p = be32_to_cpup( init_v32p);
   if ( ( *init_v32p) < 32) leds_num = ( unsigned char)( *init_v32p);
 }
 tps_leds_set( init_val);
 for ( lenp = 0; lenp < leds_num; lenp++) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 4, 15, 0))
   timer_setup( &( ltimers[ lenp].timer), tps_led_timer, 0);
#else
   init_timer( &( ltimers[ lenp].timer));
   ltimers[ lenp].timer.function = tps_led_timer;
   ltimers[ lenp].timer.data = lenp;
#endif
   ltimers[ lenp].timeout = 0;
 }
 printk( KERN_INFO "%s: pins data:%d front:%d init:%d leds num:%d\n", DV_DRV_NAME, tpsleds_d.pin_d, tpsleds_d.pin_f, init_val, leds_num);
 return( 0);  }

static int tpsleds_remove( struct platform_device *_pdev) {
 int i = 0;
 for ( i = 0; i < leds_num; i++) {
   del_timer_sync( &( ltimers[ i].timer));
 }
 if ( tpsleds_d.pin_d > 0) gpio_free( tpsleds_d.pin_d);
 if ( tpsleds_d.pin_f > 0) gpio_free( tpsleds_d.pin_f);
 return( 0);  }

#if !defined(CONFIG_OF)
#error "CONFIG_OF is not set"
#endif
static const struct of_device_id tpsleds_dt_ids[] = {
 { .compatible = DV_DRV_NAME, },
 { /* zero */ }
};
MODULE_DEVICE_TABLE(of, tpsleds_dt_ids);

static struct platform_driver tpsleds_driver = {
 .driver	= {
    .name		= DV_DRV_NAME,
    .of_match_table	= of_match_ptr( tpsleds_dt_ids),
 },
 .probe		= tpsleds_probe,
 .remove	= tpsleds_remove,
};

static int __init tpsleds_init( void) {
 int ret;
 tpsleds_procfs_init();
 ret = platform_driver_register( &tpsleds_driver);
 if ( ret) printk( KERN_ERR "%s: probe failed: %d\n", DV_DRV_NAME, ret);
 return( ret);  }

static void __exit tpsleds_exit( void) {
 tpsleds_procfs_clean();
 platform_driver_unregister( &tpsleds_driver);
 return;  }

module_init(tpsleds_init);
module_exit(tpsleds_exit);

MODULE_AUTHOR(M_AUT);
MODULE_DESCRIPTION(M_NAM);
MODULE_LICENSE("GPL v2");
