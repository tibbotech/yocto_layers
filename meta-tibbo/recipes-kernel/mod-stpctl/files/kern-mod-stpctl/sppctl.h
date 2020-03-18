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

#ifndef SPPCTL_H
#define SPPCTL_H

#define MNAME "sppctl"
#define M_LIC "GPL v2"
#define M_AUT "Dvorkin Dmitry dvorkin@tibbo.com"
#define M_NAM "SunPlus PinCtl module"
#define M_ORG "Tibbo Tech."
#define M_CPR "(C) 2019-2019"

//#define FW_DEFNAME "sppctl.bin"
#define FW_DEFNAME NULL

#include "sppctl_syshdrs.h"
#include "sppctl_defs.h"

#include "sppctl_sysfs.h"

#include <mach/io_map.h>

//#define MOON_REG_BASE 0x9C000000
//#define MOON_REG_N(n) 0x80*(n)+MOON_REG_BASE
#define MOON_REG_N(n) PA_IOB_ADDR(0x80*(n))

void sppctl_pin_set( sppctl_pdata_t *_p, u8 _pin, u8 _fun);
u8 sppctl_fun_get( sppctl_pdata_t *_p, u8 _pin);
void sppctl_loadfw( struct device *_dev, const char *_fwname);

extern char *list_funcs[];
extern const size_t sizeof_listF;


#endif // SPPCTL_H
