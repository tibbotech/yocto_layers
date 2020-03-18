#ifndef TPSID_PROCFS_H
#define TPSID_PROCFS_H

#include "tpsid_syshdrs.h"
#include "tpsid_defs.h"

#include "tpsid.h"
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

void tpsid_procfs_init( void);
void tpsid_procfs_clean( void);

#endif // TPSID_PROCFS_H
