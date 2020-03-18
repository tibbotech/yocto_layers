#ifndef TWG_SYSFS_H
#define TWG_SYSFS_H

#include "twg_syshdrs.h"

#include "twg.h"
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/device.h>

void twg_sysfs_init( struct platform_device *_pdev);
void twg_sysfs_clean( struct platform_device *_pdev);

#endif // TWG_SYSFS_H
