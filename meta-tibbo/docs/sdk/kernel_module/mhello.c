#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */

int init_module( void) {
  printk( KERN_INFO "Repent for Coming!\n");
  return( 0);  }

void cleanup_module( void) {
  printk( KERN_INFO "Goodbye cruel world\n");
  return;  }
