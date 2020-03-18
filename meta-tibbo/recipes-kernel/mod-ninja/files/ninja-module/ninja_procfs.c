#include "ninja_procfs.h"
#include "ninja_cmd.h"

#define NINJA_PROCFS_DIR MNAME

extern ninja_skbq_t ninja_skbq_newq;
extern ninja_skbq_t ninja_skbq_sent;

extern ulong maxq_mem;
extern int netlink_id;

static int dv_pf_open( struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_release(struct inode *_i, struct file *_f) {  return( 0);  }

static int dv_pf_stat_newq_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 if ( *_l >= sizeof( ninja_stats_t)) return( 0);
 *_l += sizeof( ninja_stats_t);
 tcmd_stat2buf( _buf, &ninja_skbq_newq);
 return( sizeof( ninja_stats_t));  }
static int dv_pf_stat_newq_W( struct file *_f, const char __user *_buf, size_t _cnt, loff_t *_l) {
 if ( *_l >= sizeof( ninja_stats_t)) return( 0);
 *_l += sizeof( ninja_stats_t);
 tcmd_proc( NULL, NINJA_CMD_CLEARP , "N", 1);
 return( _cnt);  }
static int dv_pf_stat_sent_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 if ( *_l >= sizeof( ninja_stats_t)) return( 0);
 *_l += sizeof( ninja_stats_t);
 tcmd_stat2buf( _buf, &ninja_skbq_sent);
 return( sizeof( ninja_stats_t));  }
static int dv_pf_lim_mem_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 if ( *_l >= sizeof( maxq_mem)) return( 0);
 *_l += sizeof( maxq_mem);
 memcpy( _buf, &maxq_mem, sizeof( maxq_mem));
 return( sizeof( maxq_mem));  }
static int dv_pf_netlink_id_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 if ( *_l >= sizeof( netlink_id)) return( 0);
 *_l += sizeof( netlink_id);
 memcpy( _buf, &netlink_id, sizeof( netlink_id));
 return( sizeof( netlink_id));  }

int dv_pf_C( kthread_t *_tp) {
 if ( !_tp) return( 0);
 KDBG( "'%s' close\n", _tp->thname);
 // mark me as ready to put thread resources
 _tp->state = DV_THS_DELIST;
 return( 0);  }

// _tp - current thread pointer
int dv_pf_task( kthread_t *_tp) {
 return( 0);  }

// _tp - current thread pointer
int dv_pf_O( void *_tp) {
 kthread_t *tp = ( kthread_t *)_tp;
 while ( dv_io( tp) == 0);
 return( 0);  }

struct proc_dir_entry *ninja_procfs_root;

static struct file_operations fo_0;
static struct file_operations fo_1;
static struct file_operations fo_2;
static struct file_operations fo_3;

// ---------- main (exported) functions
void ninja_procfs_init( void) {
 struct proc_dir_entry *tfs;
 kthread_t *procfsth;
 th_tasks_t tasks[] = {
  { .O = &dv_pf_O, .W = &dv_pf_task, .C = &dv_pf_C },
 };
 // register procfs entry
 fo_0.owner = fo_1.owner = fo_2.owner = fo_3.owner = THIS_MODULE;
 fo_0.open = fo_1.open = fo_2.open = fo_3.open = dv_pf_open;
 fo_0.llseek = fo_1.llseek = fo_2.llseek = fo_3.llseek = no_llseek;
 fo_0.release = fo_1.release = fo_2.release = fo_3.release = dv_pf_release;
 ninja_procfs_root = proc_mkdir( NINJA_PROCFS_DIR, NULL);
 fo_0.read = dv_pf_stat_newq_R;  fo_0.write = dv_pf_stat_newq_W;
 tfs = proc_create_data( "stat_newq", 0, ninja_procfs_root, &fo_0, NULL);
 fo_1.read = dv_pf_stat_sent_R;
 tfs = proc_create_data( "stat_sent", 0, ninja_procfs_root, &fo_1, NULL);
 fo_2.read = dv_pf_lim_mem_R;
 tfs = proc_create_data( "lim_mem", 0, ninja_procfs_root, &fo_2, NULL);
 fo_3.read = dv_pf_netlink_id_R;
 tfs = proc_create_data( "netlink_id", 0, ninja_procfs_root, &fo_3, NULL);
 procfsth = dv_th_init_task( MNAME"/procfs", tasks[ 0]);
 dv_th_run( procfsth);
 return;  }

void ninja_procfs_clean( void) {
 KDBG( "%s()\n", __FUNCTION__);
 remove_proc_entry( "stat_newq", ninja_procfs_root);
 remove_proc_entry( "stat_sent", ninja_procfs_root);
 remove_proc_entry( "lim_mem", ninja_procfs_root);
 remove_proc_entry( "netlink_id", ninja_procfs_root);
 if ( ninja_procfs_root) remove_proc_entry( NINJA_PROCFS_DIR, NULL);
 KDBG( "%s() /\n", __FUNCTION__);
 return;  }
