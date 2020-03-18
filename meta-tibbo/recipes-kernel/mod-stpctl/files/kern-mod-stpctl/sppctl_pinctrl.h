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

#ifndef SPPCTL_PINCTRL_H
#define SPPCTL_PINCTRL_H

#include "sppctl_syshdrs.h"

#include "sppctl.h"
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/device.h>

void sppctl_pinctrl_init( struct platform_device *_pdev);
void sppctl_pinctrl_clea( struct platform_device *_pdev);

#endif // SPPCTL_PINCTRL_H
