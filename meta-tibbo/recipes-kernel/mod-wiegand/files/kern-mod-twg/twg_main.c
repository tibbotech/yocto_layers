#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>

#include "twg.h"
#include "twg_procfs.h"
#include "twg_sysfs.h"

int change_mode( twg_pdata_t *_p, uint8_t _m);

void clean_buff( twg_pdata_t *_p) {
 memset( _p->d.buff, 0, TWG_MAX_BUF/8);
 _p->d_done = _p->d.blen = 0;
 return;  }

// IRQ top part
static irqreturn_t twg_wX_ih( int _irq, void *_pdata) {
 twg_pdata_t *_p = ( twg_pdata_t *)_pdata;
 unsigned long flags;
 uint8_t w1_val = 0;
 if ( _p->d_done) clean_buff( _p);
 if ( _p->d.blen >= TWG_MAX_BUF) return( IRQ_HANDLED);
 local_irq_save( flags);
 if ( _p->mode == 0) {	// clock/data, read w1
   w1_val = gpio_get_value( _p->pin_w1) ? 0 : 0x80;
   _p->d.buff[ _p->d.blen / 8] |= ( w1_val >> ( _p->d.blen % 8));
   _p->d.blen++;
 } else {		// wiegand mode: w1:1, w0:0
   w1_val = gpio_get_value( _p->pin_w1) ? 0 : 0x80;
//   w1_val = ( _irq == _p->irq_w1 ? 1 : 0);
   _p->d.buff[ _p->d.blen / 8] |= ( w1_val >> ( _p->d.blen % 8));
   _p->d.blen++;
 }
 local_irq_restore( flags);
 return( IRQ_WAKE_THREAD);  }

// IRQ bottom part
static irqreturn_t twg_wX_iht( int _irq, void *_pdata) {
 twg_pdata_t *_p = ( twg_pdata_t *)_pdata;
 del_timer_sync( &( _p->timer));
 // we deleted the timer. if buffer overrun - let's stop IRQ
 if ( _p->d.blen >= TWG_MAX_BUF) change_mode( _p, TWG_MODE_OFF);
 // checks...
 _p->timer.expires = jiffies + msecs_to_jiffies( _p->det);
 add_timer( &( _p->timer));
 return( IRQ_HANDLED);  }

void twg_timer_fn( unsigned long _pdata) {
 twg_pdata_t *_p = ( twg_pdata_t *)_pdata;
 int tmp_mode = _p->mode;
 if ( _p->d.blen >= TWG_MAX_BUF) {
   _p->stat_over++;
   printk( KERN_ERR "%s resume from buffer overrun\n", _p->name);
   _p->mode = TWG_MODE_OFF;
   change_mode( _p, tmp_mode);
   return;  }
 _p->d_done = 1;
 _p->stat_done++;
 // end of data
 if ( _p->debug > 0) printk( KERN_DEBUG "%s end of data (%d bits)\n", _p->name, _p->d.blen);
 if ( _p->debug > 1) print_hex_dump_bytes( "", DUMP_PREFIX_NONE, _p->d.buff, _p->d.blen/8 + 1);
 return;  }

// 0 = Clock/Data
// 1 = WG reader
int change_mode( twg_pdata_t *_p, uint8_t _m) {
 if ( _p->mode == _m) return( 1);
 if ( _p->pin_sw && _m != TWG_MODE_OFF) gpio_set_value( _p->pin_sw, _m);
 // == 99 - switch off
 if ( _m != TWG_MODE_OFF) clean_buff( _p);
 if ( _p->irq_w0) free_irq( _p->irq_w0, ( void *)_p);
 if ( _p->irq_w1) free_irq( _p->irq_w1, ( void *)_p);
 _p->irq_w0 = _p->irq_w1 = 0;
 // w0, w1
 if ( _m == 0) {	// going to Clock/Data mode
   if ( _p->pin_w0) _p->irq_w0 = gpio_to_irq( _p->pin_w0);
   if ( _p->irq_w0 < 0) {
     _p->irq_w0 = 0;
     printk( KERN_ERR "%s: W0 get irq failed\n", _p->name);
     return( -EIO);  }
   if ( request_threaded_irq( _p->irq_w0, twg_wX_ih, twg_wX_iht,
          IRQF_TRIGGER_FALLING/* | IRQF_TRIGGER_RISING*/ | IRQF_ONESHOT, 
          _p->name, _p)) {
     printk( KERN_ERR "%s: W0 irq request failed\n", _p->name);
     return( -EIO);  }
 }
 if ( _m == 1) {	// going to WG reader mode
   if ( _p->pin_w0) _p->irq_w0 = gpio_to_irq( _p->pin_w0);
//   if ( _p->pin_w1) _p->irq_w1 = gpio_to_irq( _p->pin_w1);
   if ( _p->irq_w0 < 0) {
     _p->irq_w0 = 0;
     printk( KERN_ERR "%s: W0 get irq failed\n", _p->name);
     return( -EIO);  }
   if ( request_threaded_irq( _p->irq_w0, twg_wX_ih, twg_wX_iht,
          IRQF_TRIGGER_FALLING/* | IRQF_TRIGGER_RISING*/ | IRQF_ONESHOT, 
          _p->name, _p)) {
     printk( KERN_ERR "%s: W0 irq request failed\n", _p->name);
     return( -EIO);  }
//   if ( _p->irq_w1 < 0) {
//     _p->irq_w1 = 0;
//     printk( KERN_ERR "%s: W1 get irq failed\n", _p->name);
//     return( -EIO);  }
//   if ( request_threaded_irq( _p->irq_w1, twg_wX_ih, twg_wX_iht,
//          IRQF_TRIGGER_FALLING/* | IRQF_TRIGGER_RISING*/ | IRQF_ONESHOT, 
//          _p->name, _p)) {
//     printk( KERN_ERR "%s: W1 irq request failed\n", _p->name);
//     return( -EIO);  }
 }
 if ( _m != TWG_MODE_OFF) _p->mode = _m;
 return( 1);  }

static int twg_probe( struct platform_device *_pdev) {
 twg_pdata_t *p = NULL;
 int ret = -ENODEV;
 uint32_t *init_v32p;
 uint8_t mode_init = 1;
 int lenp;
 struct device_node *np = _pdev->dev.of_node;
 p = ( twg_pdata_t *)kzalloc( sizeof( twg_pdata_t), GFP_KERNEL);
 if ( p == NULL) {  ret = -ENOMEM;  goto err;  }
 p->mode = TWG_MODE_OFF;
 p->det = 50;	// 50 ms data end timeout
 p->clear_on_read = 1;
 memset( p->name, 0, TWG_MAX_NAM);
 strcpy( p->name, np->name);
 dev_set_name( &( _pdev->dev), "%s", p->name);
 p->pin_w0 = of_get_named_gpio( np, "wg-w0", 0);
 p->pin_w1 = of_get_named_gpio( np, "wg-w1", 0);
 p->pin_sw = of_get_named_gpio( np, "wg-sw", 0);
 p->pin_oc = of_get_named_gpio( np, "wg-oc", 0);
// FIXME: add choose mode in OF
 if ( ( init_v32p = of_get_property( _pdev->dev.of_node, "mode", &lenp))) {
   *init_v32p = be32_to_cpup( init_v32p);
   mode_init = ( uint8_t)( *init_v32p);
 }
 if ( !gpio_is_valid( p->pin_w0)) p->pin_w0 = 0;
 if ( !gpio_is_valid( p->pin_w1)) p->pin_w1 = 0;
 if ( !gpio_is_valid( p->pin_sw)) p->pin_sw = 0;
 if ( !gpio_is_valid( p->pin_oc)) p->pin_oc = 0;
 if ( p->pin_w0 == 0) {
   printk( KERN_ERR "%s: W0 pin not defined\n", p->name);
   goto err;  }
 if ( p->pin_w1 == 0) {
   printk( KERN_ERR "%s: W1 pin not defined\n", p->name);
   goto err;  }
 if ( p->pin_sw == 0) printk( KERN_INFO "%s: no optional SW pin\n", p->name);
 if ( p->pin_oc == 0) printk( KERN_INFO "%s: no optional OC pin\n", p->name);
 if ( devm_gpio_request( &( _pdev->dev), p->pin_w0, "w0") < 0) {
   printk( KERN_ERR "%s: W0 pin request failed\n", p->name);
   goto err;  }
 if ( devm_gpio_request( &( _pdev->dev), p->pin_w1, "w1") < 0) {
   printk( KERN_ERR "%s: W1 pin request failed\n", p->name);
   goto err;  }
 if ( p->pin_sw && devm_gpio_request( &( _pdev->dev), p->pin_sw, "sw") < 0) {
   printk( KERN_ERR "%s: SW pin request failed\n", p->name);
   goto err;  }
 if ( p->pin_oc && devm_gpio_request( &( _pdev->dev), p->pin_oc, "oc") < 0) {
   printk( KERN_ERR "%s: OC pin request failed\n", p->name);
   goto err;  }
 gpio_direction_input( p->pin_w0);
// gpio_set_debounce( p->pin_w0, 20);
 gpio_direction_input( p->pin_w1);
 // FIXME: double check this
 if ( p->pin_sw) gpio_direction_output( p->pin_sw, mode_init);
 if ( p->pin_oc) gpio_direction_output( p->pin_oc, 1);
 init_timer( &( p->timer));
 p->timer.function = twg_timer_fn;
 p->timer.data = ( unsigned long)p;
 _pdev->dev.platform_data = p;
 printk( KERN_INFO "%s configured (%s)\n", MNAME, p->name);
 twg_procfs_init( _pdev);
 twg_sysfs_init( _pdev);
 change_mode( p, mode_init);
 return( 0);
 err:
 if ( p->pin_w0 > 0) devm_gpio_free( &( _pdev->dev), p->pin_w0);
 if ( p->pin_w1 > 0) devm_gpio_free( &( _pdev->dev), p->pin_w1);
 if ( p->pin_sw > 0) devm_gpio_free( &( _pdev->dev), p->pin_sw);
 if ( p->pin_oc > 0) devm_gpio_free( &( _pdev->dev), p->pin_oc);
 if ( p) kfree( p);
 return( ret);  }

static int twg_remove( struct platform_device *_pdev) {
 twg_pdata_t *p = ( twg_pdata_t *)_pdev->dev.platform_data;
 if ( p->timer.expires > jiffies) del_timer_sync( &( p->timer));
 if ( p->irq_w0) free_irq( p->irq_w0, ( void *)p);
 if ( p->irq_w1) free_irq( p->irq_w1, ( void *)p);
 twg_sysfs_clean( _pdev);
 twg_procfs_clean( _pdev);
 if ( p->pin_w0 > 0) devm_gpio_free( &( _pdev->dev), p->pin_w0);
 if ( p->pin_w1 > 0) devm_gpio_free( &( _pdev->dev), p->pin_w1);
 if ( p->pin_sw > 0) devm_gpio_free( &( _pdev->dev), p->pin_sw);
 if ( p->pin_oc > 0) devm_gpio_free( &( _pdev->dev), p->pin_oc);
 printk( KERN_INFO "%s removed\n", p->name);
 kfree( p);
 return( 0);  }

#if !defined(CONFIG_OF)
#error "CONFIG_OF is not set"
#endif
static const struct of_device_id twg_dt_ids[] = {
 { .compatible = "tps,wg-gpio", },
 { .compatible = "tps,twg", },
 { /* zero */ }
};
MODULE_DEVICE_TABLE(of, twg_dt_ids);

MODULE_ALIAS("platform:" MNAME);
static struct platform_driver twg_driver = {
 .driver	= {
    .name		= MNAME,
    .of_match_table	= of_match_ptr( twg_dt_ids),
 },
 .probe		= twg_probe,
 .remove	= twg_remove,
};

module_platform_driver(twg_driver);

//static int __init my_init( void) {
// printk( KERN_INFO "%s module removed\n", MNAME);
// return( 0);  }
//static void __exit my_exit( void) {
// printk( KERN_INFO "%s module loaded\n", MNAME);
// return;  }
//
//module_init(my_init);
//module_exit(my_exit);

MODULE_AUTHOR(M_AUT);
MODULE_DESCRIPTION(M_NAM);
MODULE_LICENSE(M_LIC);
