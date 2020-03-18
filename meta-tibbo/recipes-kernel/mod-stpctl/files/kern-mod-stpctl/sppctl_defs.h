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

#ifndef SPPCTL_DEFS_H
#define SPPCTL_DEFS_H

#define SPPCTL_MAX_NAM 64
#define SPPCTL_MAX_BUF PAGE_SIZE

#define KINF(fmt,args...) printk( KERN_INFO MNAME": "fmt,##args)
#define KERR(fmt,args...) printk( KERN_ERR MNAME": "fmt,##args)

#ifdef SPPCTL_DEBUG
#define DBGM(x) ( debug_mask & x)
#define KDBG(fmt,args...) printk( KERN_DEBUG MNAME": "fmt,##args)
//#define DBGM(x) true
#else
#define DBGM(x) false
#define KDBG(fmt,args...) 
#endif

typedef struct sppctl_pdata_T {
 char name[ SPPCTL_MAX_NAM];
 void __iomem *moon_base;
 u8 debug;
 char fwname[ SPPCTL_MAX_NAM];
 void *sysfs_sdp;
 // pinctrl-related
 struct pinctrl_desc pdesc;
 struct pinctrl_dev *pcdp;
} sppctl_pdata_t;

typedef struct sppctl_reg_T {
 u16 v;		// value part
 u16 m;		// mask part
} sppctl_reg_t;

#endif // SPPCTL_DEFS_H
