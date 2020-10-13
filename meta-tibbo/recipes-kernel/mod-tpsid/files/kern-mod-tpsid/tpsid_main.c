#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/nvmem-consumer.h>
#include <linux/platform_device.h>

#include "tpsid.h"
#include "tpsid_procfs.h"

#define DV_DRV_NAME "tpsid"
#define OTP_NAME "mac_addr0"

unsigned char tpsid[ TPSID_LEN + 1];

char *mac_read_sp7021_otp( struct device *_d) {
 char *ret = NULL;
 ssize_t otp_l = 0;
 u8 i = 0, reverse = 0;
 struct nvmem_cell *c = nvmem_cell_get( _d, OTP_NAME);
 if ( IS_ERR_OR_NULL( c)) {
   dev_err( _d, "OTP read failure:%ld", PTR_ERR( c));
   return( NULL);  }
 ret = nvmem_cell_read( c, &otp_l);
 nvmem_cell_put( c);
 dev_dbg( _d, "%d bytes read from OTP", otp_l);
 if ( otp_l < TPSID_LEN) {
   dev_err( _d, "OTP %d len, < %d", otp_l, TPSID_LEN);
   return( NULL);  }
 if ( ret[ 5] == 0xFC && ret[ 4] == 0x4B && ret[ 3] == 0xBC &&
    ( ret[ 0] != 0xFC || ret[ 1] != 0x4B || ret[ 2] != 0xBC)) reverse = 1;
 for ( i = 0; i < otp_l && i < TPSID_LEN; i++) {
   tpsid[ i] = ret[ ( reverse ? otp_l - i - 1 : i)];  }
 return( ret);  }

void mac_read_am335x_efuse( void) {
 void __iomem *regs;
 unsigned int addrL, addrH;
 regs = ioremap( DV_AM335X_EFUSE, SZ_8K);
 // read EMAC0 mac from TI efuse
 addrH = readl( regs + 0x30 + 0x4);
 addrL = readl( regs + 0x30 + 0x0);
 tpsid[ 0] =   addrH & 0x000000ff;
 tpsid[ 1] = ( addrH & 0x0000ff00) >> 8;
 tpsid[ 2] = ( addrH & 0x00ff0000) >> 16;
 tpsid[ 3] = ( addrH & 0xff000000) >> 24;
 tpsid[ 4] =   addrL & 0x000000ff;
 tpsid[ 5] = ( addrL & 0x0000ff00) >> 8;
 return;  }

static int tpsid_probe( struct platform_device *_pd) {
 int ret = 0, i = 0;
 unsigned char buff[ TPSID_LEN*2 + 1];
 memset( tpsid, 0, TPSID_LEN + 1);
#ifdef CONFIG_ARCH_PENTAGRAM
 mac_read_sp7021_otp( &( _pd->dev));
#else
 mac_read_am335x_efuse();
#endif

 memset( buff, 0, TPSID_LEN*2 + 1);
 for ( i = 0; i < TPSID_LEN; i++) {
   sprintf( buff + i*2, "%02X", tpsid[ i]);
 }
 printk( KERN_INFO "%s: ID:%s\n", DV_DRV_NAME, buff);
 tpsid_procfs_init();
 return( ret);  }

static int tpsid_remove( struct platform_device *_pd) {
 tpsid_procfs_clean();
 return( 0);  }

static const struct of_device_id of_tpsid_ids[] = {
 { .compatible = DV_DRV_NAME },
 { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_tpsid_ids);
MODULE_ALIAS("platform:" DV_DRV_NAME);
static struct platform_driver tpsid_driver = {
 .driver 	= {
    .name 	= DV_DRV_NAME,
    .owner	= THIS_MODULE,
    .of_match_table = of_match_ptr( of_tpsid_ids),
 },
 .probe		= tpsid_probe,
 .remove 	= tpsid_remove,
};
module_platform_driver(tpsid_driver);

MODULE_AUTHOR(M_AUT);
MODULE_DESCRIPTION(M_NAM);
MODULE_LICENSE("GPL v2");
