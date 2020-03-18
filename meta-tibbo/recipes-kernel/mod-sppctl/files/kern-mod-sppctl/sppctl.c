/*
 * Driver for SunPlus/Tibbo SC7021 additional pin multiplexing controller
 *
 * Copyright (C) 2019 Dvorkin Dmitry <dvorkin@tibbo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include "sppctl.h"
#include "sppctl_sysfs.h"

char *list_funcs[] = {
 "L2SW_CLK_OUT",
 "L2SW_MAC_SMI_MDC",
 "L2SW_LED_FLASH0",
 "L2SW_LED_FLASH1",
 "L2SW_LED_ON0",
 "L2SW_LED_ON1",
 "L2SW_MAC_SMI_MDIO",
 "L2SW_P0_MAC_RMII_TXEN",
 "L2SW_P0_MAC_RMII_TXD0",
 "L2SW_P0_MAC_RMII_TXD1",
 "L2SW_P0_MAC_RMII_CRSDV",
 "L2SW_P0_MAC_RMII_RXD0",
 "L2SW_P0_MAC_RMII_RXD1",
 "L2SW_P0_MAC_RMII_RXER",
 "L2SW_P1_MAC_RMII_TXEN",
 "L2SW_P1_MAC_RMII_TXD0",
 "L2SW_P1_MAC_RMII_TXD1",
 "L2SW_P1_MAC_RMII_CRSDV",
 "L2SW_P1_MAC_RMII_RXD0",
 "L2SW_P1_MAC_RMII_RXD1",
 "L2SW_P1_MAC_RMII_RXER",
 "DAISY_MODE",
 "SDIO_CLK",
 "SDIO_CMD",
 "SDIO_D0",
 "SDIO_D1",
 "SDIO_D2",
 "SDIO_D3",
 "PWM0",
 "PWM1",
 "PWM2",
 "PWM3",
 "PWM4",
 "PWM5",
 "PWM6",
 "PWM7",
 "ICM0_D",
 "ICM1_D",
 "ICM2_D",
 "ICM3_D",
 "ICM0_CLK",
 "ICM1_CLK",
 "ICM2_CLK",
 "ICM3_CLK",
 "SPIM0_INT",
 "SPIM0_CLK",
 "SPIM0_EN",
 "SPIM0_DO",
 "SPIM0_DI",
 "SPIM1_INT",
 "SPIM1_CLK",
 "SPIM1_CEN",
 "SPIM1_DO",
 "SPIM1_DI",
 "SPIM2_INT",
 "SPIM2_CLK",
 "SPIM2_CEN",
 "SPIM2_DO",
 "SPIM2_DI",
 "SPIM3_INT",
 "SPIM3_CLK",
 "SPIM3_CEN",
 "SPIM3_DO",
 "SPIM3_DI",
 "SPI0S_INT",
 "SPI0S_CLK",
 "SPI0S_EN",
 "SPI0S_DO",
 "SPI0S_DI",
 "SPI1S_INT",
 "SPI1S_CLK",
 "SPI1S_EN",
 "SPI1S_DO",
 "SPI1S_DI",
 "SPI2S_INT",
 "SPI2S_CLK",
 "SPI2S_EN",
 "SPI2S_DO",
 "SPI2S_DI",
 "SPI3S_INT",
 "SPI3S_CLK",
 "SPI3S_EN",
 "SPI3S_DO",
 "SPI3S_DI",
 "I2CM0_CK",
 "I2CM0_DAT",
 "I2CM1_CK",
 "I2CM1_DAT",
 "I2CM2_CK",
 "I2CM2_D",
 "I2CM3_CK",
 "I2CM3_D",
 "UA1_TX",
 "UA1_RX",
 "UA1_CTS",
 "UA1_RTS",
 "UA2_TX",
 "UA2_RX",
 "UA2_CTS",
 "UA2_RTS",
 "UA3_TX",
 "UA3_RX",
 "UA3_CTS",
 "UA3_RTS",
 "UA4_TX",
 "UA4_RX",
 "UA4_CTS",
 "UA4_RTS",
 "TIMER0_INT",
 "TIMER1_INT",
 "TIMER2_INT",
 "TIMER3_INT",
 "GPIO_INT0",
 "GPIO_INT1",
 "GPIO_INT2",
 "GPIO_INT3",
 "GPIO_INT4",
 "GPIO_INT5",
 "GPIO_INT6",
 "GPIO_INT7",
 NULL
};

const size_t sizeof_listF = sizeof( list_funcs)/sizeof( *( list_funcs));

void sppctl_pin_set( sppctl_pdata_t *_p, uint8_t _pin, uint8_t _fun) {
 uint32_t *r;
 sppctl_reg_t x = {  .m = 0x007F, .v = ( uint16_t)_pin  };
 uint8_t func = ( _fun >> 1) << 2;
 if ( _fun % 2 == 0) ;
 else {  x.v <<= 8;  x.m <<= 8;  }
 if ( _p->debug > 1) KINF( "%s(%X,%X) off:%X m:%X v:%X\n", __FUNCTION__, func, _pin, _fun, x.m, x.v);
 r = ( uint32_t *)&x;
 writel( *r, _p->moon_base + func);
 return;  }

uint8_t sppctl_fun_get( sppctl_pdata_t *_p,  uint8_t _fun) {
 uint8_t pin = 0x00;
 uint8_t func = ( _fun >> 1) << 2;
 sppctl_reg_t *x;
 uint32_t r = readl( _p->moon_base + func);
 x = ( sppctl_reg_t *)&r;
 if ( _fun % 2 == 0) pin = x->v & 0x00FF;
 else pin = x->v >> 8;
 if ( _p->debug > 1) KINF( "%s(%X) off:%X m:%X v:%X pin:%X\n", __FUNCTION__, _fun, func, x->m, x->v, pin);
 return( pin);  }

static void sppctl_fwload_cb( const struct firmware *_fw, void *_ctx) {
 int i = -1;
 sppctl_pdata_t *p = ( sppctl_pdata_t *)_ctx;
 if ( !_fw) {  KERR( "Firmare not found\n");  return;  }
 if ( _fw->size < sizeof_listF - 1) {
   KERR( " fw size %d < %d\n", _fw->size, sizeof_listF - 1);  return;
   goto out;  }
 while ( list_funcs[ ++i] && i < _fw->size) sppctl_pin_set( p, _fw->data[ i], i);
 out:
 release_firmware( _fw);
 return;  }

void sppctl_loadfw( struct device *_dev, const char *_fwname) {
 int ret;
 sppctl_pdata_t *p = ( sppctl_pdata_t *)_dev->platform_data;
 if ( !_fwname) return;
 if ( strlen( _fwname) < 1) return;
 KINF( "fw:%s", _fwname);
 ret = request_firmware_nowait( THIS_MODULE, true, _fwname, _dev, GFP_KERNEL, p, sppctl_fwload_cb);
 if ( ret) KERR( "Can't load '%s'\n", _fwname);
 return;  }

static int sppctl_probe( struct platform_device *_pdev) {
 int ret = -ENODEV;
 struct device_node *np = NULL;
 sppctl_pdata_t *p = NULL;
 struct resource res;
 const char *fwfname = FW_DEFNAME;
 p = ( sppctl_pdata_t *)kzalloc( sizeof( sppctl_pdata_t), GFP_KERNEL);
 if ( p == NULL) {  ret = -ENOMEM;  goto err;  }
#if defined(CONFIG_OF)
 np = _pdev->dev.of_node;
#endif
 memset( p->name, 0, SPPCTL_MAX_NAM);
 if ( np) strcpy( p->name, np->name);
 else strcpy( p->name, MNAME);
 dev_set_name( &( _pdev->dev), "%s", p->name);
 if ( !np || of_address_to_resource( np, 0, &res)) {
   p->moon_base = devm_ioremap( &( _pdev->dev), MOON_REG_N(2), SZ_256);
   KINF( "st mapped:%X, size:%d\n", MOON_REG_N(2), SZ_256);
 } else {
   p->moon_base = devm_ioremap( &( _pdev->dev), res.start, resource_size( &res));
   KINF( "of mapped:%X, size:%d\n", res.start, resource_size( &res));
 }
 _pdev->dev.platform_data = p;
 sppctl_sysfs_init( _pdev);
#if defined(CONFIG_OF)
 of_property_read_string( np, "fwname", &fwfname);
#endif
 if ( fwfname) strcpy( p->fwname, fwfname);
 sppctl_loadfw( &( _pdev->dev), p->fwname);
 return( 0);
 err:
 return( ret);  }

static int sppctl_remove( struct platform_device *_pdev) {
 sppctl_pdata_t *p = ( sppctl_pdata_t *)_pdev->dev.platform_data;
 sppctl_sysfs_clean( _pdev);
 kfree( p);
 return( 0);  }

#if defined(CONFIG_OF)
static const struct of_device_id sppctl_dt_ids[] = {
 { .compatible = "pentagram,sppctl", },
 { .compatible = "sunplus,sppctl", },
 { .compatible = "tps,sppctl", },
 { .compatible = "tibbo,sppctl", },
 { /* zero */ }
};
MODULE_DEVICE_TABLE(of, sppctl_dt_ids);
#endif
MODULE_ALIAS("platform:" MNAME);
static struct platform_driver sppctl_driver = {
 .driver		= {
    .name		= MNAME,
#if defined(CONFIG_OF)
    .of_match_table	= of_match_ptr( sppctl_dt_ids),
#endif
 },
 .probe		= sppctl_probe,
 .remove	= sppctl_remove,
};

module_platform_driver(sppctl_driver);

MODULE_AUTHOR(M_AUT);
MODULE_DESCRIPTION(M_NAM);
MODULE_LICENSE("GPL v2");
