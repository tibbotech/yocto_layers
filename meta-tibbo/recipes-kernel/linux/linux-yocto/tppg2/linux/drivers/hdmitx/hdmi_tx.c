/*----------------------------------------------------------------------------*
 *					INCLUDE DECLARATIONS
 *---------------------------------------------------------------------------*/
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include "include/hal_hdmi_tx.h"
#include <mach/hdmi_tx.h>

/*----------------------------------------------------------------------------*
 *					MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 *					DATA TYPES
 *---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 *					GLOBAL VARIABLES
 *---------------------------------------------------------------------------*/
static int hdmitx_probe(struct platform_device *dev);

// device driver operation functions
static const struct file_operations hdmitx_fops =
{
	.owner          = THIS_MODULE,
	// .open           = hdmitx_fops_open,
	// .release        = hdmitx_fops_release,
	// .unlocked_ioctl = hdmitx_fops_ioctl,
	// .mmap           = disp_mmap,
};

// platform device
static struct platform_device hdmitx_device = {
	.name = "hdmitx",
	.id   = -1,
};

// platform driver
static struct platform_driver hdmitx_driver = {
	.probe    = hdmitx_probe,
	// .remove   = hdmitx_remove,
	// .suspend  = hdmitx_suspend,
	// .resume   = hdmitx_resume,
	// .shutdown = hdmitx_shotdown,
	.driver   =
	{
		.name  = "hdmitx",
		.owner = THIS_MODULE,
	},
};

/*----------------------------------------------------------------------------*
 *					EXTERNAL DECLARATIONS
 *---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 *					FUNCTION DECLARATIONS
 *---------------------------------------------------------------------------*/
void hdmitx_enable_video(enum hdmitx_video_timing_e timing)
{
	struct hal_hdmi_tx_video_attr_t video_attr = {0};

	video_attr.in_color_space  = HDMI_TX_COLOR_SPACE_LIMITED_RGB;
	video_attr.out_color_space = HDMI_TX_COLOR_SPACE_LIMITED_RGB;
	video_attr.pixel_fmt       = HDMI_TX_PIXEL_FORMAT_RGB;
	video_attr.color_depth     = HDMI_TX_COLOR_DEPTH_24BITS;

	switch (timing)
	{
		case HDMITX_VIDEO_TIMING_576P:
			video_attr.timing = HDMI_TX_TIMING_576P;
			break;
		case HDMITX_VIDEO_TIMING_720P60:
			video_attr.timing = HDMI_TX_TIMING_720P60;
			break;
		case HDMITX_VIDEO_TIMING_1080P60:
			video_attr.timing = HDMI_TX_TIMING_1080P60;
			break;
		case HDMITX_VIDEO_TIMING_480P:
		default:
			video_attr.timing = HDMI_TX_TIMING_480P;
			break;
	}

	hal_hdmi_tx_disable();
	hal_hdmi_tx_config_video(&video_attr);
	hal_hdmi_tx_enable();
}

static int hdmitx_probe(struct platform_device *dev)
{
	return 0;
}

static int __init hdmitx_init(void)
{
	int ret = 0;

	ret = platform_device_register(&hdmitx_device);
	if (ret)
	{
		printk("hdmitx device register failed, ret = %d\n", ret);
		platform_device_unregister(&hdmitx_device);
	}
	else
	{
		ret = platform_driver_register(&hdmitx_driver);
		if (ret)
		{
			printk("hdmitx driver register failed, ret = %d\n", ret);
			platform_driver_unregister(&hdmitx_driver);
		}
	}

    return ret;
}

static void __exit hdmitx_exit(void)
{
	platform_driver_unregister(&hdmitx_driver);
	platform_device_unregister(&hdmitx_device);
}

module_init(hdmitx_init);
module_exit(hdmitx_exit);
MODULE_DESCRIPTION("HDMI TX driver");
MODULE_LICENSE("GPL");