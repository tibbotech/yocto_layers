#ifndef NINJA_PROCFS_H
#define NINJA_PROCFS_H

#include "ninja_syshdrs.h"
#include "ninja_defs_i.h"
#include "ninja_defs_e.h"

#include "ninja_th.h"
#include "ninja_io.h"
#include "ninja.h"
#include <linux/proc_fs.h>

void ninja_procfs_init( void);
void ninja_procfs_clean( void);

#endif // NINJA_PROCFS_H
