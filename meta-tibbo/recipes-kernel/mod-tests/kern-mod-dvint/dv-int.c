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

#define DV_DRV_NAME "dv-int"

struct dv_irq_T {
 u16 irq_pin;
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

static int dv_int_probe( struct platform_device *_pdev) {
 int ret = 0;
 char *msg;
 if ( !_pdev->dev.of_node) {
   printk( KERN_INFO "%s: %s OF not found\n", DV_DRV_NAME, __FUNCTION__);
   return( -ENXIO);  }
 ret = of_property_read_string( _pdev->dev.of_node, "message", &msg);
 if ( ret == 0) printk( KERN_INFO "%s: message from DTB:%s\n", DV_DRV_NAME, msg);
 dv_irq_d.irq_pin = of_get_named_gpio( _pdev->dev.of_node, "gpios", 0);
 if ( !gpio_is_valid( dv_irq_d.irq_pin)) {
   printk( KERN_INFO "%s: %s gpio pin (%d) is invalid\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   dv_irq_d.irq_pin = 0;
   return( -ENXIO);  }
 ret = gpio_request( dv_irq_d.irq_pin, DV_DRV_NAME "-irqpin");
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_request(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   return( ret);   }
 ret = gpio_to_irq( dv_irq_d.irq_pin);
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_to_irq(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   goto err_free_gpio;   }
 dv_irq_d.irq = ret;
 ret = request_threaded_irq( dv_irq_d.irq, test_irq_ih, test_irq_iht,
   /*IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,*/
   /*IRQF_TRIGGER_RISING | IRQF_ONESHOT,*/
   IRQF_TRIGGER_RISING | IRQF_ONESHOT,
   DV_DRV_NAME, ( void *)&dv_irq_d);
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s request_irq(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq);
   goto err_free_gpio;   }
 printk( KERN_INFO "%s: pin:%d irq:%d\n", DV_DRV_NAME, dv_irq_d.irq_pin, dv_irq_d.irq);
 return( 0);
 err_free_gpio:
 gpio_free( dv_irq_d.irq_pin);
 return( ret);  }

static int dv_int_remove( struct platform_device *_pdev) {
 if ( dv_irq_d.irq > 0) free_irq( dv_irq_d.irq, ( void *)&dv_irq_d);
 gpio_free( dv_irq_d.irq_pin);
 return( 0);  }

#if !defined(CONFIG_OF)
#error "CONFIG_OF is not set"
#endif
static const struct of_device_id dv_int_dt_ids[] = {
 { .compatible = DV_DRV_NAME, },
 { /* zero */ }
};
MODULE_DEVICE_TABLE(of, dv_int_dt_ids);

static struct platform_driver dv_int_driver = {
 .driver	= {
    .name		= DV_DRV_NAME,
    .of_match_table	= of_match_ptr( dv_int_dt_ids),
 },
 .probe		= dv_int_probe,
 .remove	= dv_int_remove,
};

static int __init dv_init( void) {
 int ret;
 dv_irq_d.irq = 0;
 ret = platform_driver_register( &dv_int_driver);
 if ( ret) printk( KERN_ERR "%s: probe failed: %d\n", DV_DRV_NAME, ret);
 return( ret);  }

static void __exit dv_exit( void) {
 platform_driver_unregister( &dv_int_driver);
 return;  }

module_init(dv_init);
module_exit(dv_exit);

MODULE_AUTHOR("Dvorkin Dmitry");
MODULE_DESCRIPTION("Simplest IRQ test");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DV_DRV_NAME);
