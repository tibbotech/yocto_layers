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

#ifndef SPPCTL_SYSHDRS_H
#define SPPCTL_SYSHDRS_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/slab.h>

#include <linux/platform_device.h>
#include <linux/firmware.h>
#include <linux/of_address.h>

// for traditional pinctrl interface
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinmux.h>

#endif // SPPCTL_SYSHDRS_H
