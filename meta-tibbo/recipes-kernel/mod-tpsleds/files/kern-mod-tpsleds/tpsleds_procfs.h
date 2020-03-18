#ifndef TPSLEDS_PROCFS_H
#define TPSLEDS_PROCFS_H

#include "tpsleds_syshdrs.h"
#include "tpsleds_defs.h"

#include "tpsleds.h"
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

void tpsleds_procfs_init( void);
void tpsleds_procfs_clean( void);

#endif // TPSLEDS_PROCFS_H
