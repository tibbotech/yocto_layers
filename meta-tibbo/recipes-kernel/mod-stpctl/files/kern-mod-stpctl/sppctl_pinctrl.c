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

#include "sppctl_pinctrl.h"

static int sppctl_pctrl_get_groups_count( struct pinctrl_dev *_pctldev) {
 KINF( "%s()\n", __FUNCTION__);
// struct pxa_pinctrl *pctl = pinctrl_dev_get_drvdata( _pctldev);
// return( pctl->ngroups);  }
 return( 0);  }

static const char *sppctl_pctrl_get_group_name( struct pinctrl_dev *_pctldev,
 unsigned _tgroup) {
 KINF( "%s()\n", __FUNCTION__);
 return( "nogrp");  }
// struct pxa_pinctrl *pctl = pinctrl_dev_get_drvdata( _pctldev);
// struct pxa_pinctrl_group *group = pctl->groups + tgroup;
// return( group->name);  }

//static int sppctl_pctrl_get_group_pins(struct pinctrl_dev *pctldev,
// unsigned tgroup, const unsigned **pins, unsigned *num_pins) {
// struct pxa_pinctrl *pctl = pinctrl_dev_get_drvdata(pctldev);
// struct pxa_pinctrl_group *group = pctl->groups + tgroup;
// *pins = ( unsigned *)&group->pin;
// *num_pins = 1;
// return( 0);  }

static const struct pinctrl_pin_desc stpctl_pins[] = {
 PINCTRL_PIN( 0, "pin0"),
 PINCTRL_PIN( 1, "pin1"),
};

//static const struct pinconf_ops stpctl_pconf_ops = {
// .pin_config_group_get	= xxx_pconf_group_get,
// .pin_config_group_set	= xxx_pconf_group_set,
//#ifdef CONFIG_GENERIC_PINCONF
// .is_generic		= true,
//#endif
//};

static const struct pinctrl_ops sppctl_pctl_ops = {
#ifdef CONFIG_OF
 .dt_node_to_map	= pinconf_generic_dt_node_to_map_all,
 .dt_free_map		= pinconf_generic_dt_free_map,
#endif
 .get_groups_count	= sppctl_pctrl_get_groups_count,
 .get_group_name	= sppctl_pctrl_get_group_name,
// .get_group_pins	= sppctl_pctrl_get_group_pins,
};

static struct pinctrl_desc stpctl_pinctrl_desc = {
// .confops	= &stpctl_pconf_ops,
 .pctlops	= &sppctl_pctl_ops,
// .pmxops	= &xxx_pinmux_ops,
// .npins		= 64;
 .owner		= THIS_MODULE,
};

// ---------- main (exported) functions
void sppctl_pinctrl_init( struct platform_device *_pdev) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_pdev->dev.platform_data;
 // init pdesc
 _p->pdesc = stpctl_pinctrl_desc;
 _p->pdesc.name = dev_name( &( _pdev->dev));
 _p->pdesc.pins = &( stpctl_pins[ 0]);
 _p->pdesc.npins = ARRAY_SIZE( stpctl_pins);
 _p->pcdp = devm_pinctrl_register( &( _pdev->dev), &( _p->pdesc), _p);
 
 return;  }

void sppctl_pinctrl_clea( struct platform_device *_pdev) {
 sppctl_pdata_t *_p = ( sppctl_pdata_t *)_pdev->dev.platform_data;
 pinctrl_unregister( _p->pcdp);
 return;  }
