#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/libertas_spi.h>

//#include <linux/platform_data/spi-omap2-mcspi.h>

#include "ga1000v0.h"

#define DV_DRV_NAME "tpsga1000v0"

struct spi_device *spi_libertasp;

// WLAN pins
struct tps_ga1000v0_libertas_pins_T {
 u16 pin_reset;
 u16 pin_strap;
};
typedef struct tps_ga1000v0_libertas_pins_T tps_ga1000v0_libertas_pins_t;

tps_ga1000v0_libertas_pins_t tps_ga1000v0_libertas_pins_d = {
 .pin_reset 	= TPSGA1000V0_GPIO_RESET,
 .pin_strap 	= TPSGA1000V0_GPIO_STRAP,
};

static int tps_ga1000v0_libertas_setup( struct spi_device *_spi) {
 int err = 0, status;
 tps_ga1000v0_libertas_pins_t *tp = &tps_ga1000v0_libertas_pins_d;
 gpio_direction_output( tp->pin_reset, 0);
 msleep( 100);
 gpio_direction_output( tp->pin_strap, 1);
 msleep( 100);
 gpio_direction_output( tp->pin_reset, 1);
 msleep( 100);
 printk( KERN_INFO "%s: setup on master bus#%d maxHz:%d devHz:%d\n", MNAME, _spi->master->bus_num, _spi->master->max_speed_hz, _spi->max_speed_hz);
 _spi->bits_per_word = 16;
 spi_setup( _spi);
 return( err);  }

static int tps_ga1000v0_libertas_teardown( struct spi_device *spi) {
 tps_ga1000v0_libertas_pins_t *tp = &tps_ga1000v0_libertas_pins_d;
 gpio_set_value( tp->pin_reset, 0);
 printk( KERN_ERR "%s: down\n", MNAME);
 return( 0);  }

//struct omap2_mcspi_device_config tps_ga1000v0_libertas_chip = {
// .turbo_mode = 1,
// .cs_per_word = 1,
//};

struct libertas_spi_platform_data tps_ga1000v0_libertas_pdata = {
 .use_dummy_writes	= 0,	// 0 for slow SPI, 1 = if ok
 .setup			= tps_ga1000v0_libertas_setup,
 .teardown		= tps_ga1000v0_libertas_teardown,
};

static struct spi_board_info tps_ga1000v0_spi_device = {
   .modalias		= "libertas_spi",
//   .max_speed_hz	= 13000000,
   .max_speed_hz	= -1,
   .irq			= -1,
   .chip_select		= 1,
//   .controller_data	= &tps_ga1000v0_libertas_chip,
   .platform_data	= &tps_ga1000v0_libertas_pdata,
};

static int tpsga1000v0_probe( struct spi_device *_spi) {
 struct device_node *np = _spi->dev.of_node;
 tps_ga1000v0_libertas_pins_t *tp = &tps_ga1000v0_libertas_pins_d;
 u16 tpin;
 u32 *v32p;
 int ret, lenp;
 if ( !np) {
   printk( KERN_INFO "%s: %s OF not found\n", MNAME, __FUNCTION__);
   return( -ENXIO);  }
 if ( ( tpin = of_get_named_gpio( np, "gpio-reset", 0))) tp->pin_reset = tpin;
 if ( ( tpin = of_get_named_gpio( np, "gpio-strap", 0))) tp->pin_strap = tpin;
 printk( KERN_INFO "%s: gpios reset:%d strap:%d\n", MNAME, tp->pin_reset, tp->pin_strap);
 if ( !gpio_is_valid( tp->pin_reset)) {
   printk( KERN_INFO "%s: %s gpio reset pin (%d) is invalid\n", MNAME, __FUNCTION__, tp->pin_reset);
   tp->pin_reset = 0;
   return( -ENXIO);  }
 if ( !gpio_is_valid( tp->pin_strap)) {
   printk( KERN_INFO "%s: %s gpio strap pin (%d) is invalid\n", MNAME, __FUNCTION__, tp->pin_strap);
   tp->pin_strap = 0;
   return( -ENXIO);  }
 ret = gpio_request( tp->pin_reset, MNAME "-pin-reset");
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_request(%d,reset) failed\n", MNAME, __FUNCTION__, tp->pin_reset);
   return( ret);   }
 ret = gpio_request( tp->pin_strap, MNAME "-pin-strap");
 if ( ret < 0) {
   printk( KERN_INFO "%s: %s gpio_request(%d,strap) failed\n", MNAME, __FUNCTION__, tp->pin_strap);
   gpio_free( tp->pin_strap);
   return( ret);   }
 tps_ga1000v0_spi_device.max_speed_hz = _spi->master->max_speed_hz - 1;
 if ( ( v32p = of_get_property( np, "max-hz", &lenp))) {
   *v32p = be32_to_cpup( v32p);
   tps_ga1000v0_spi_device.max_speed_hz = *v32p;
 }
 printk( KERN_INFO "%s: max_hz=%d\n", MNAME, tps_ga1000v0_spi_device.max_speed_hz);
 if ( ( v32p = of_get_property( np, "irq", &lenp))) {
   *v32p = be32_to_cpup( v32p);
   tps_ga1000v0_spi_device.irq = *v32p;
   printk( KERN_INFO "%s: irq=%d\n", MNAME, *v32p);
 }
 if ( ( v32p = of_get_property( np, "gpio-irq", &lenp))) {
   *v32p = be32_to_cpup( v32p);
   tps_ga1000v0_spi_device.irq = gpio_to_irq( *v32p);
   printk( KERN_INFO "%s: gpio-irq=%d irq:%d\n", MNAME, *v32p, tps_ga1000v0_spi_device.irq);
 }
 tps_ga1000v0_spi_device.bus_num = _spi->master->bus_num;
 spi_libertasp = spi_new_device( _spi->master, &tps_ga1000v0_spi_device);
 return( 0);  }

static int tpsga1000v0_remove( struct spi_device *_spi) {
 tps_ga1000v0_libertas_pins_t *tp = &tps_ga1000v0_libertas_pins_d;
 if ( tp->pin_reset > 0) gpio_free( tp->pin_reset);
 if ( tp->pin_strap > 0) gpio_free( tp->pin_strap);
 tp->pin_reset = tp->pin_strap = 0;
 dev_set_drvdata( &_spi->dev, NULL);
 if ( spi_libertasp) spi_unregister_device( spi_libertasp);
 spi_libertasp = NULL;
 printk( KERN_INFO "%s: unloaded\n", MNAME);
 return( 0);  }

#if !defined(CONFIG_OF)
#error "CONFIG_OF is not set"
#endif
static const struct of_device_id tpsga1000v0_dt_ids[] = {
 { .compatible = MNAME, },
 { /* zero */ }
};
MODULE_DEVICE_TABLE(of, tpsga1000v0_dt_ids);

static struct spi_driver tpsga1000v0_driver = {
 .driver	= {
    .name		= MNAME,
    .of_match_table	= of_match_ptr( tpsga1000v0_dt_ids),
 },
 .probe		= tpsga1000v0_probe,
 .remove	= tpsga1000v0_remove,
};

module_spi_driver(tpsga1000v0_driver);

MODULE_AUTHOR(M_AUT);
MODULE_DESCRIPTION(M_NAM);
MODULE_LICENSE(M_LIC);
MODULE_ALIAS("spi:tpsga1000v0");
