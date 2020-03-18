#ifndef INTLTEST_PROCFS_H
#define INTLTEST_PROCFS_H

#include "intltest_syshdrs.h"
#include "intltest_defs.h"

#include "intltest.h"
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

void intltest_procfs_init( void);
void intltest_procfs_clean( void);

#endif // INTLTEST_PROCFS_H
