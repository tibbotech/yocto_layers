#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#define DV_DRV_NAME "dv-irq-tst"

struct dv_irq_T {
 s16 irq_pin;
 u16 irq_fired;
 int irq;
};

static struct dv_irq_T dv_irq_d;

// top part of IRQ
static irqreturn_t test_irq_ih( int _irq, void *_data) {
 struct dv_irq_T* data = ( struct dv_irq_T*)_data;
 unsigned long flags;
 local_irq_save(flags);
 data->irq_fired++;
 local_irq_restore(flags);
 return( IRQ_WAKE_THREAD);  }

static irqreturn_t test_irq_iht( int _irq, void *_data) {
 struct dv_irq_T* data = ( struct dv_irq_T*)_data;
 printk( KERN_INFO "%s: irq %d fired %d times\n", DV_DRV_NAME, _irq, data->irq_fired);
 return( IRQ_HANDLED);  }

static int dv_git_probe( struct platform_device *_pdev) {
 int ret = 0;
 if ( !_pdev->dev.of_node) {
   printk( KERN_INFO "%s: %s OF not found\n", DV_DRV_NAME, __FUNCTION__);
   return( -ENXIO);  }
 dv_irq_d.irq_pin = of_get_named_gpio( _pdev->dev.of_node, "gpios", 0);
 printk( KERN_INFO "%s: %s is gpio pin: %d?\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
 if ( gpio_is_valid( dv_irq_d.irq_pin)) {
   // gpio_to_irq
   printk( KERN_INFO "%s: %s requesting gpio pin %d...\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   ret = gpio_request( dv_irq_d.irq_pin, DV_DRV_NAME "-irqpin");
   if ( ret < 0) {
     printk( KERN_INFO "%s: %s gpio_request(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
     return( ret);   }
   printk( KERN_INFO "%s: %s setting pin %d to IN...\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   ret = gpio_direction_input( dv_irq_d.irq_pin);
   if ( ret < 0) {
     printk( KERN_INFO "%s: %s gpio_direction_input(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
     return( ret);   }
   printk( KERN_INFO "%s: %s mapping pin %d to interrupt...\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   ret = gpio_to_irq( dv_irq_d.irq_pin);
   if ( ret < 0) {
     printk( KERN_INFO "%s: %s gpio_to_irq(%d) failed(%d)\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin, ret);
     return( ret);   }
   dv_irq_d.irq = ret;
 } else {
   // direct irq
   printk( KERN_INFO "%s: %s trying premuxed irq...\n", DV_DRV_NAME, __FUNCTION__);
   ret = platform_get_irq( _pdev, 0);
   if ( ret < 0) {
     printk( KERN_INFO "%s: %s platform_get_irq failed(%d)\n", DV_DRV_NAME, __FUNCTION__, ret);
     return( ret);   }
   dv_irq_d.irq = ret;
 }
 printk( KERN_INFO "%s: %s pin %d mapped to irq %d\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin, ret);
 ret = request_threaded_irq( dv_irq_d.irq, test_irq_ih, test_irq_iht,
   /*IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,*/
   /*IRQF_TRIGGER_RISING | IRQF_ONESHOT,*/
   IRQF_TRIGGER_RISING | IRQF_ONESHOT,
   DV_DRV_NAME, ( void *)&dv_irq_d);
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s request_irq(%d) failed (%d)\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq, ret);
   goto err_free_gpio;   }
 printk( KERN_INFO "%s: pin:%d irq:%d\n", DV_DRV_NAME, dv_irq_d.irq_pin, dv_irq_d.irq);
 return( 0);
 err_free_gpio:
 gpio_free( dv_irq_d.irq_pin);
 return( ret);  }

static int dv_git_remove( struct platform_device *_pdev) {
 if ( dv_irq_d.irq > 0) {
   printk( KERN_INFO "%s: free irq:%d\n", DV_DRV_NAME, dv_irq_d.irq);
   free_irq( dv_irq_d.irq, ( void *)&dv_irq_d);
 }
 if ( dv_irq_d.irq_pin) gpio_free( dv_irq_d.irq_pin);
 return( 0);  }

#if !defined(CONFIG_OF)
#error "CONFIG_OF is not set"
#endif
static const struct of_device_id dv_git_dt_ids[] = {
 { .compatible = DV_DRV_NAME, },
 { /* zero */ }
};
MODULE_DEVICE_TABLE(of, dv_git_dt_ids);

static struct platform_driver dv_git_driver = {
 .driver	= {
    .name		= DV_DRV_NAME,
    .of_match_table	= of_match_ptr( dv_git_dt_ids),
 },
 .probe		= dv_git_probe,
 .remove	= dv_git_remove,
};

static int __init dv_init( void) {
 int ret;
 dv_irq_d.irq = 0;
 ret = platform_driver_register( &dv_git_driver);
 if ( ret) printk( KERN_ERR "%s: probe failed: %d\n", DV_DRV_NAME, ret);
 return( ret);  }

static void __exit dv_exit( void) {
 platform_driver_unregister( &dv_git_driver);
 return;  }

module_init(dv_init);
module_exit(dv_exit);

MODULE_AUTHOR("Dvorkin Dmitry");
MODULE_DESCRIPTION("GPIO IRQ test");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DV_DRV_NAME);
