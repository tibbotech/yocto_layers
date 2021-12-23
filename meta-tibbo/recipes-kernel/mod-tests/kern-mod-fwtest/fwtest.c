#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/firmware.h>

#include <linux/slab.h>

static void sample_firmware_load(char *firmware, int size) {
       u8 *buf = kmalloc(size + 1, GFP_KERNEL);
       memcpy(buf, firmware, size);
       buf[size] = '\0';
       printk(KERN_INFO "firmware_example: Firmware: %s\n", buf);
       kfree(buf);
}

static void sample_probe(struct device *dev) {
       /* uses the default method to get the firmware */
       const struct firmware *fw_entry;
       printk(KERN_INFO "firmware_example: ghost device inserted\n");

       if (request_firmware(&fw_entry, "sample_firware.bin", dev) != 0) {
               printk(KERN_ERR "firmware_example: Firmware not available\n");
               return;
       }
       sample_firmware_load(fw_entry->data, fw_entry->size);
       release_firmware(fw_entry);
       /* finish setting up the device */
}

static void ghost_release(struct device *dev) {
       printk(KERN_DEBUG "firmware_example : ghost device released\n");
}

static struct device ghost_device = {
//       .bus_id = "ghost0",
       .release = ghost_release
};

static int __init sample_init(void) {
       device_register(&ghost_device);
       sample_probe(&ghost_device);
       return 0;
}

static void __exit sample_exit(void) {
       device_unregister(&ghost_device);
}

module_init(sample_init);
module_exit(sample_exit);

MODULE_LICENSE("GPL");
