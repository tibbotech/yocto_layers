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

#include "intltest.h"
#include "intltest_procfs.h"

#define DV_DRV_NAME "intltest"

struct dv_irq_T {
 u16 irq_pin;
 u32 irq_fired;
 int irq;
};

static struct dv_irq_T dv_irq_d;

ktime_t int_last;
s64 intl_min, intl_max;

// top part of IRQ
static irqreturn_t test_irq_ih( int _irq, void *_data) {
 struct dv_irq_T* data = ( struct dv_irq_T*)_data;
 unsigned long flags;
 ktime_t int_curr;
// FIX
 s64 d;
 local_irq_save(flags);
 data->irq_fired++;
// int_curr = ktime_get();
 int_curr.tv64 = jiffies_64;
 if ( int_last.tv64 != 0) {
//   d = ktime_us_delta( int_curr, int_last);
   d = int_curr.tv64 - int_last.tv64;
   if ( intl_min == 0) intl_min = d;
   if ( d < intl_min) intl_min = d;
   if ( d > intl_max) intl_max = d;
 }
 int_last = int_curr;
 local_irq_restore(flags);
 return( IRQ_HANDLED);  }
// return( IRQ_WAKE_THREAD);  }

static irqreturn_t test_irq_iht( int _irq, void *_data) {
 struct dv_irq_T* data = ( struct dv_irq_T*)_data;
 if ( data->irq_fired % 1000000 == 0) {
   printk( KERN_INFO "%s: irq %d fired %ld times\n", DV_DRV_NAME, _irq, data->irq_fired);
 }
 return( IRQ_HANDLED);  }


static int __init intltest_init( void) {
 int ret = 0;
 int_last.tv64 = 0;
 dv_irq_d.irq_pin = 15;		// S1A

 ret = gpio_request( dv_irq_d.irq_pin, DV_DRV_NAME "-0pin");
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_request(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   return( ret);   }
 gpio_direction_input( dv_irq_d.irq_pin);
 ret = gpio_to_irq( dv_irq_d.irq_pin);
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_to_irq(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq_pin);
   goto err_free_gpio;   }
 dv_irq_d.irq = ret;
// ret = request_threaded_irq( dv_irq_d.irq, test_irq_ih, test_irq_iht,
//   /*IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,*/
//   /*IRQF_TRIGGER_RISING | IRQF_ONESHOT,*/
//   IRQF_TRIGGER_RISING | IRQF_ONESHOT,
//   DV_DRV_NAME, ( void *)&dv_irq_d);
 ret = request_irq( dv_irq_d.irq, test_irq_ih,
   /*IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,*/
   /*IRQF_TRIGGER_RISING | IRQF_ONESHOT,*/
   IRQF_TRIGGER_RISING | IRQF_ONESHOT | IRQF_NO_THREAD,
   DV_DRV_NAME, ( void *)&dv_irq_d);
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s request_irq(%d) failed\n", DV_DRV_NAME, __FUNCTION__, dv_irq_d.irq);
   goto err_free_gpio;   }
 printk( KERN_INFO "%s: pin:%d irq:%d\n", DV_DRV_NAME, dv_irq_d.irq_pin, dv_irq_d.irq);
 intltest_procfs_init();
 return( 0);
 err_free_gpio:
 gpio_free( dv_irq_d.irq_pin);
 return( ret);  }

static void __exit intltest_exit( void) {
 if ( dv_irq_d.irq > 0) free_irq( dv_irq_d.irq, ( void *)&dv_irq_d);
 gpio_free( dv_irq_d.irq_pin);
 intltest_procfs_clean();
 return;  }

module_init(intltest_init);
module_exit(intltest_exit);

MODULE_AUTHOR(M_AUT);
MODULE_DESCRIPTION(M_NAM);
MODULE_LICENSE("GPL v2");
