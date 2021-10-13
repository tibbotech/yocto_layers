#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/machine.h>

#include <linux/gpio/consumer.h>

#define CM_ADDRESS_BASE 0x44E10000

//module_param(base, int, S_IRUSR | S_IWUSR);
//MODULE_PARM_DESC(base, "control module register base");

static int __init my_init(void) {
 struct device *dev = NULL;
 struct pinctrl *lst = NULL;
 struct pinctrl_state *pst = NULL;
 char *drv_name = /*"i2c-tps"*/"i2c-s01";
 printk( KERN_INFO "Module loading...\n");
 dev = bus_find_device_by_name( &platform_bus_type, NULL, drv_name);
 printk( KERN_INFO "Got dev ref ...%s\n", ( dev ? "non-null" : "null"));
 lst = devm_pinctrl_get( dev);
 if ( IS_ERR( lst)) {
   printk( KERN_INFO "can't get pinctrl\n");
   return( 0);  }
////
// struct gpio_pin_range *pin_range;
// pin_range = kzalloc( sizeof(*pin_range), GFP_KERNEL);
// struct pinctrl_dev *pcd = of_pinctrl_get( /*struct device_node *np*/);
// int r = pinctrl_get_group_pins( pcd, "i2c_s01_pins",
//           &pin_range->range.pins, &pin_range->range.npins);
// if ( r < 0) printk( KERN_INFO "can't get pin groups\n");
////
 struct gpio_descs *gs = gpiod_get_array( dev, NULL, 0);
 if ( IS_ERR( gs)) {
   printk( KERN_INFO "can't get gpios for %s (%lx)\n", drv_name, gs);
int ccc = gpiod_count( dev, NULL);
   printk( KERN_INFO "XX:%d\n", ccc);
struct gpio_desc *xxx = gpiod_get_index( dev, NULL, 0, 0);
   printk( KERN_INFO "XX:%lx\n", xxx);
//enum gpio_lookup_flags lookupflags = 0;
//xxx = gpiod_find( dev, NULL, 0, &lookupflags);
//   printk( KERN_INFO "XX:%lx\n", xxx);
   return( 0);  }
 printk( KERN_INFO "got gpiosN:%d\n", gs->ndescs);
// pst = lst->state;
// if ( pst && pst->name) printk( KERN_INFO "cur state:%s\n", pst->name);
// pst = pinctrl_lookup_state( lst, "norts");
// if ( IS_ERR( pst)) {
//   printk( KERN_INFO "can't get norts\n");
//   return( 0);  }
// if ( pinctrl_select_state( lst, pst) != 0) {
//   printk( KERN_INFO "can't get norts\n");
//   return( 0);  }
 return( 0);  }

static void __exit my_exit( void) {
 printk( KERN_INFO "Module Unloading\n");
 return;  }

module_init( my_init);
module_exit( my_exit);

MODULE_AUTHOR("Dvorkin Dmitry");
MODULE_DESCRIPTION("Change pins mode");
MODULE_LICENSE("GPL v2");
