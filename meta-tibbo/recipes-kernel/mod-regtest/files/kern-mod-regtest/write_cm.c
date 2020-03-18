#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>

#define CM_ADDRESS_BASE 0x44E10000
//#define CM_ADDRESS_BASE 0x48044000
//#define CM_ADDRESS_BASE 0x44E00000

static void __iomem *ctrl_base;

static unsigned int base=CM_ADDRESS_BASE;
static unsigned int offset=0x40;
static unsigned int value=0xffffffff;

module_param(base, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(base, "control module register base");
module_param(offset, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(offset, "control module register offset");
module_param(value, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(value, "control module register value");

static int __init my_init(void)
{
        volatile u32 j;

        ctrl_base = ioremap( base, SZ_8K);
        if (!ctrl_base) {
                pr_err("control module ioremap failed\n");
                return -ENOMEM;
        }
        j = __raw_readl(ctrl_base + offset);
        printk(KERN_INFO "read 0x%X[0x%x] = 0x%x\n", base, offset, j);
       
        if(value != 0xffffffff)
        {
        	__raw_writel(value, ctrl_base + 0x40/*offset*/);
        	j = __raw_readl(ctrl_base + 0x40/*offset*/);
        	printk(KERN_INFO "read back contral_status reg = %x\n", j);
        }
	return 0;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "Module Unloading\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jeff Lance");
MODULE_DESCRIPTION("CM register read/write");
MODULE_LICENSE("GPL v2");
