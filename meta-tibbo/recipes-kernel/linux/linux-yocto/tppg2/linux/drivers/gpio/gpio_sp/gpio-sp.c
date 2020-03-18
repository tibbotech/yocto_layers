#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <mach/io_map.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/gpio/driver.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <mach/gpio_drv.h>

/* #define GPIO_FUNC_DEBUG */
#define GPIO_KDBG_INFO
#define GPIO_KDBG_ERR

#ifdef GPIO_FUNC_DEBUG
	#define FUNC_DEBUG() printk(KERN_INFO "%s: %d %s()\n", __FILE__, __LINE__, __func__)
#else
	#define FUNC_DEBUG()
#endif

#ifdef GPIO_KDBG_INFO
	#define DBG_INFO(fmt, args ...) printk(KERN_INFO "K_GPIO: " fmt, ## args)
#else
	#define DBG_INFO(fmt, args ...)
#endif

#ifdef GPIO_KDBG_ERR
	#define DBG_ERR(fmt, args ...)  printk(KERN_ERR "K_GPIO: " fmt, ## args)
#else
	#define DBG_ERR(fmt, args ...)
#endif

#define DEVICE_NAME             "sp_gpio"
#define REG_GRP_OFS(GRP, OFFSET)        VA_IOB_ADDR((GRP) * 32 * 4 + (OFFSET) * 4) 
#define GPIO_FIRST(X)   (REG_GRP_OFS(101, (25+X)))
#define GPIO_MASTER(X)  (REG_GRP_OFS(6, (0+X)))
#define GPIO_OE(X)      (REG_GRP_OFS(6, (8+X)))
#define GPIO_OUT(X)     (REG_GRP_OFS(6, (16+X)))
#define GPIO_IN(X)      (REG_GRP_OFS(6, (24+X)))
#define GPIO_I_INV(X)   (REG_GRP_OFS(7, (0+X)))
#define GPIO_O_INV(X)   (REG_GRP_OFS(7, (8+X)))
#define GPIO_OD(X)      (REG_GRP_OFS(7, (16+X)))
#define GPIO_SFT_CFG(G,X)      (REG_GRP_OFS(G, (0+X)))
#define NUM_GPIO_MAX    (256)
static DEFINE_SPINLOCK(slock_gpio);

struct sp_gpio {
	struct gpio_chip chip;
	void __iomem *base;
	int irq;
	struct device		*dev;
};

long gpio_input_invert_1(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}

	value = (1 << (bit & 0x0f) | 1 << ((bit & 0x0f) + 0x10));
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_I_INV(idx)) | value), GPIO_I_INV(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_input_invert_1);

long gpio_input_invert_0(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}
	value = ((ioread32(GPIO_I_INV(idx)) | (1 << ((bit & 0x0f) + 0x10))) & ~( 1 << (bit & 0x0f)));
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32(value, GPIO_I_INV(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_input_invert_0);

long gpio_output_invert_1(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}

	value = (1 << (bit & 0x0f) | 1 << ((bit & 0x0f) + 0x10));
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_O_INV(idx)) | value), GPIO_O_INV(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_output_invert_1);

long gpio_output_invert_0(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}
	value = ((ioread32(GPIO_O_INV(idx)) | (1 << ((bit & 0x0f) + 0x10))) & ~( 1 << (bit & 0x0f)));
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32(value, GPIO_O_INV(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_output_invert_0);

long gpio_open_drain_1(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}

	value = (1 << (bit & 0x0f) | 1 << ((bit & 0x0f) + 0x10));
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_OD(idx)) | value), GPIO_OD(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_open_drain_1);

long gpio_open_drain_0(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}
	value = ((ioread32(GPIO_OD(idx)) | (1 << ((bit & 0x0f) + 0x10))) & ~( 1 << (bit & 0x0f)));
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32(value, GPIO_OD(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_open_drain_0);

long gpio_first_1(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 5;
	if (idx > 4) {
		return -EINVAL;
	}
	
	value = 1 << (bit & 0x1f);
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_FIRST(idx)) | value), GPIO_FIRST(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_first_1);

long gpio_first_0(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 5;
	if (idx > 4) {
		return -EINVAL;
	}

	value = 1 << (bit & 0x1f);
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_FIRST(idx)) & (~value)), GPIO_FIRST(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);
	return 0;
}
EXPORT_SYMBOL(gpio_first_0);

long gpio_master_1(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}

	value = (1 << (bit & 0x0f) | 1 << ((bit & 0x0f) + 0x10));
	//if (gpio_check_first(idx, value) == -EINVAL) {
	//	return -EINVAL;
	//}
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_MASTER(idx)) | value), GPIO_MASTER(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_master_1);

long gpio_master_0(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}
	
	value = ((ioread32(GPIO_MASTER(idx)) | (1 << ((bit & 0x0f) + 0x10)) ) & ~( 1 << (bit & 0x0f)));
	//if (gpio_check_first(idx, value) == -EINVAL) {
	//	return -EINVAL;
	//}
		
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32(value, GPIO_MASTER(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_master_0);

long gpio_set_oe(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}

	value = (1 << (bit & 0x0f) |  1 << ((bit & 0x0f) + 0x10));

#if 0
	if (gpio_check_first(idx, value) == -EINVAL) {
		return -EINVAL;
	}
	if (gpio_check_master(idx, value) == -EINVAL) {
		return -EINVAL;
	}
#endif
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_OE(idx)) | value), GPIO_OE(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_set_oe);

long gpio_clr_oe(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
  if (idx > 8) {
		return -EINVAL;
	}
	
	value = ((ioread32(GPIO_OE(idx)) | (1 << ((bit & 0x0f) + 0x10)) ) & ~( 1 << (bit & 0x0f)));
#if 0
	if (gpio_check_first(idx, value) == -EINVAL) {
		return -EINVAL;
	}
	if (gpio_check_master(idx, value) == -EINVAL) {
		return -EINVAL;
	}
#endif
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32(value, GPIO_OE(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_clr_oe);

long gpio_out_1(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}

	value = (1 << (bit & 0x0f) | 1 << ((bit & 0x0f) + 0x10));
	
#if 0
	if (gpio_check_first(idx, value) == -EINVAL) {
		return -EINVAL;
	}
	if (gpio_check_master(idx, value) == -EINVAL) {
		return -EINVAL;
	}
	if (gpio_check_oe(idx, value) == -EINVAL) {
		return -EINVAL;
	}
#endif
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32((ioread32(GPIO_OUT(idx)) | value), GPIO_OUT(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_out_1);

long gpio_out_0(u32 bit)
{
	u32 idx, value;
	unsigned long flags;

	idx = bit >> 4;
	if (idx > 8) {
		return -EINVAL;
	}

	value = ((ioread32(GPIO_OUT(idx)) | (1 << ((bit & 0x0f) + 0x10)) ) & ~( 1 << (bit & 0x0f)));
#if 0
	if (gpio_check_first(idx, value) == -EINVAL) {
		return -EINVAL;
	}
	if (gpio_check_master(idx, value) == -EINVAL) {
		return -EINVAL;
	}
	if (gpio_check_oe(idx, value) == -EINVAL) {
		return -EINVAL;
	}
#endif
	spin_lock_irqsave(&slock_gpio, flags);
	iowrite32(value, GPIO_OUT(idx));
	spin_unlock_irqrestore(&slock_gpio, flags);

	return 0;
}
EXPORT_SYMBOL(gpio_out_0);

long gpio_in(u32 bit, u32 *gpio_in_value)
{

	u32 idx, value;
	unsigned long flags;
	
	idx = bit >> 5;
	if (idx > 5) {
		return -EINVAL;
	}
	
	value = 1 << (bit & 0x1f);
	
	spin_lock_irqsave(&slock_gpio, flags);
	*gpio_in_value = (ioread32(GPIO_IN(idx)) & value) ? 1 : 0;
	spin_unlock_irqrestore(&slock_gpio, flags);
		
	return 0;
}
EXPORT_SYMBOL(gpio_in);

u32 gpio_in_val(u32 bit)
{
	u32 value = 0;

	gpio_in(bit, &value);

	return value;
}
EXPORT_SYMBOL(gpio_in_val);

long gpio_pin_mux_sel(PMXSEL_ID id, u32 sel)
{
	u32 grp ,idx, max_value, reg_val, mask, bit_num;
	unsigned long flags;
	
	grp = (id >> 24) & 0xff;
	if (grp > 0x03) {
		return -EINVAL;
	}	
	
	idx = (id >> 16) & 0xff;
	if (idx > 0x1f) {
		return -EINVAL;
	}
	
	max_value = (id >> 8) & 0xff;
	if (sel > max_value) {
		return -EINVAL;
	}
	
	bit_num = id & 0xff;
	
	if (max_value == 1) {
		mask = 0x01 << bit_num;
	}
	else if ((max_value == 2) || (max_value == 3)) {
		mask = 0x03 << bit_num;
	}
	else {
		mask = 0x7f << bit_num;
	}	

	spin_lock_irqsave(&slock_gpio, flags);	
	reg_val = ioread32(GPIO_SFT_CFG(grp,idx));
	reg_val |= mask << 0x10 ;
	reg_val &= (~mask);	
	reg_val = ((sel << bit_num) | (mask << 0x10));		
	iowrite32(reg_val, GPIO_SFT_CFG(grp,idx));
	spin_unlock_irqrestore(&slock_gpio, flags);
	

	return 0;
}
EXPORT_SYMBOL(gpio_pin_mux_sel);

long gpio_pin_mux_get(PMXSEL_ID id, u32 *sel)
{
	u32 grp , idx, max_value, reg_val, mask, bit_num;
	unsigned long flags;
	
	grp = (id >> 24) & 0xff;
	
	idx = (id >> 16) & 0xff;
	if (idx > 0x11) {
		return -EINVAL;
	}

	max_value = (id >> 8) & 0xff;
	if (sel > max_value) {
		return -EINVAL;
	}
	
	bit_num = id & 0xff;

	if (max_value == 1) {
		mask = 0x01 << bit_num;
	}
	else if ((max_value == 2) || (max_value == 3)) {
		mask = 0x03 << bit_num;
	}
	else {
		mask = 0x7f << bit_num;
	}

	spin_lock_irqsave(&slock_gpio, flags);
	reg_val = ioread32(GPIO_SFT_CFG(grp,idx));
	reg_val &= mask;
	*sel = (reg_val >> bit_num);
	spin_unlock_irqrestore(&slock_gpio, flags);
	return 0;
}
EXPORT_SYMBOL(gpio_pin_mux_get);

static long gpio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long err = 0;
	u32 bit;
	u32 gpio_in_value = 0;
	PMXSEL_T pin_mux;

	FUNC_DEBUG();


	if (cmd == GPIO_IOC_PIN_MUX_SEL) {
		if (unlikely(copy_from_user(&pin_mux, (void *)arg, sizeof(pin_mux)))) {
			return -EFAULT;
		}
	} else {
		if (unlikely(copy_from_user(&bit, (void *)arg, sizeof(bit)))) {
			return -EFAULT;
		}
	}

	switch (cmd) {
	case GPIO_IOC_INPUT_INVERT_1:
		err = gpio_input_invert_1(bit);
		break;
	case GPIO_IOC_INPUT_INVERT_0:
		err = gpio_input_invert_0(bit);
		break;
	case GPIO_IOC_OUTPUT_INVERT_1:
		err = gpio_output_invert_1(bit);
		break;
	case GPIO_IOC_OUTPUT_INVERT_0:
		err = gpio_output_invert_0(bit);
		break;
	case GPIO_IOC_OPEN_DRAIN_1:
		err = gpio_open_drain_1(bit);
		break;
	case GPIO_IOC_OPEN_DRAIN_0:
		err = gpio_open_drain_0(bit);
		break;
	case GPIO_IOC_FIRST_1:
		err = gpio_first_1(bit);
		break;
	case GPIO_IOC_FIRST_0:
		err = gpio_first_0(bit);
		break;
	case GPIO_IOC_MASTER_1:
		err = gpio_master_1(bit);
		break;
	case GPIO_IOC_MASTER_0:
		err = gpio_master_0(bit);
		break;
	case GPIO_IOC_SET_OE:
		err = gpio_set_oe(bit);
		break;
	case GPIO_IOC_CLR_OE:
		err = gpio_clr_oe(bit);
		break;
	case GPIO_IOC_OUT_1:
		err = gpio_out_1(bit);
		break;
	case GPIO_IOC_OUT_0:
		err = gpio_out_0(bit);
		break;
	case GPIO_IOC_IN:
		err = gpio_in(bit, &gpio_in_value);
		err |= copy_to_user((void *)arg, &gpio_in_value, sizeof(gpio_in_value));
		break;
	case GPIO_IOC_PIN_MUX_GET:
		err = gpio_pin_mux_get((PMXSEL_ID)pin_mux.id, &pin_mux.val);
		err |= copy_to_user((void *)arg, &pin_mux.val, sizeof(pin_mux.val));
		break;
	case GPIO_IOC_PIN_MUX_SEL:
		err = gpio_pin_mux_sel((PMXSEL_ID)pin_mux.id, pin_mux.val);
		break;
	default:
		err = -EINVAL;
		break;
	}

	return err;
}

static irqreturn_t sunplus_gpio_irq(int irq, void *args)
{
	printk("sunplus_gpio_irq\n");
	return IRQ_HANDLED;
}

static int gpio_open(struct inode *inode, struct file *filp)
{
	FUNC_DEBUG();
	return 0;
}

static int gpio_release(struct inode *inode, struct file *filp)
{
	FUNC_DEBUG();
	return 0;
}

static int sp_gpio_dir_in(struct gpio_chip *chip, unsigned int gpio)
{
	//struct sp_gpio *gpio = gpiochip_get_data(chip);
	GPIO_F_SET(gpio, GPIO_FUNC_GPIO);
	GPIO_M_SET(gpio, 1);
	GPIO_E_SET(gpio, GPIO_DIR_INPUT);
	return 0;
}

static int sp_gpio_dir_out(struct gpio_chip *gc, unsigned int gpio,
				int val)
{

	GPIO_F_SET(gpio, GPIO_FUNC_GPIO);
	GPIO_M_SET(gpio, 1);
	GPIO_E_SET(gpio, GPIO_DIR_OUTPUT);
	return 0;
}

static int sp_gpio_get(struct gpio_chip *gc, unsigned int gpio)
{
	GPIO_E_SET(gpio, GPIO_DIR_INPUT);
	return GPIO_I_GET(gpio);
}

static void sp_gpio_set(struct gpio_chip *gc, unsigned int gpio, int val)
{
	GPIO_E_SET(gpio, GPIO_DIR_OUTPUT);
	GPIO_O_SET(gpio, val);
}

static int sunplus_gpio_platform_driver_probe(struct platform_device *pdev)
{
	struct sp_gpio *pdata;
	struct gpio_chip *chip;
	struct resource *res;
	struct device_node *np = pdev->dev.of_node;
	int err = 0;
	
	int ret,npins,i;
	struct resource *ires;
	if (!np) {
		dev_err(&pdev->dev, "invalid devicetree node\n");
		return -EINVAL;
	}

	if (!of_device_is_available(np)) {
		dev_err(&pdev->dev, "devicetree status is not available\n");
		return -ENODEV;
	}
	
	pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (pdata == NULL)
		return -ENOMEM;
	chip = &pdata->chip;
	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (IS_ERR(res)) {
		dev_err(&pdev->dev, "get resource memory from devicetree node\n");
		return PTR_ERR(res);
	}

	pdata->base = devm_ioremap_resource(&pdev->dev, res);
	pdev->dev.platform_data = chip;
	chip->label = DEVICE_NAME;
	chip->parent = &pdev->dev;
	chip->owner = THIS_MODULE;
	chip->direction_input = sp_gpio_dir_in;
	chip->get = sp_gpio_get;
	chip->direction_output = sp_gpio_dir_out;
	chip->set = sp_gpio_set;
	chip->base = -1;
	chip->ngpio = NUM_GPIO_MAX;
	if (IS_ERR(pdata->base)) {
		dev_err(&pdev->dev, "mapping resource memory\n");
		return PTR_ERR(pdata->base);
	}	

	err = devm_gpiochip_add_data(&pdev->dev, chip, pdata);
	if (err < 0) {
		dev_err(&pdev->dev, "failed to add gpio chip\n");
		return err;
	}

	dev_info(&pdev->dev, "SP GPIO driver probed.\n");
	
	npins = platform_irq_count(pdev);

	for (i = 0; i < npins; i++) {
		ires = platform_get_resource(pdev, IORESOURCE_IRQ, i);
		if (!res) {
			return -ENODEV;
		}		
		ret = request_irq(ires->start, sunplus_gpio_irq, 0, DEVICE_NAME, ires);
		if (ret) { 
			printk("request_irq() failed (%d)\n", ret); 
		}
	}
	return 0;
}

	
#if 0
static int sp_gpio_remove(struct platform_device *pdev)
{
	struct sp_gpio *pdata;
	pdata = platform_get_drvdata(pdev);
	if (pdata == NULL)
		return -ENODEV;

	return gpiochip_remove(&pdata->chip);
}
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static const struct of_device_id sp_gpio_of_match[] = {
	{ .compatible = "sunplus,sp_gpio", },
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, sp_gpio_of_match);

static struct platform_driver sp_gpio_driver = {
	.probe		= sunplus_gpio_platform_driver_probe,
	.driver = {
		.name	= DEVICE_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = sp_gpio_of_match,
	}
};
module_platform_driver(sp_gpio_driver);

static struct file_operations fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = gpio_ioctl,
	.open           = gpio_open,
	.release        = gpio_release
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunplus Technology");
MODULE_DESCRIPTION("Sunplus GPIO driver");
 


