#ifndef TWG_PROCFS_H
#define TWG_PROCFS_H

#include "twg_syshdrs.h"

#include "twg.h"
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

void twg_procfs_init( struct platform_device *_pdev);
void twg_procfs_clean( struct platform_device *_pdev);

#endif // TWG_PROCFS_H
