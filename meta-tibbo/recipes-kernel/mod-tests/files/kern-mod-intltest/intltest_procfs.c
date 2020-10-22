#include "intltest_procfs.h"

#define BLEN 128

static int dv_pf_open( struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_release(struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_W( struct file *_f, const char __user *_buf, size_t _cnt, loff_t *_l) {  return( 0);  }

static int dv_pf_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 unsigned char n, ret;
 unsigned char lbuf[ BLEN + 1];
 if ( *_l > 0) return( 0);
 memset( lbuf, 0,  BLEN + 1);
 sprintf( lbuf, "min:%lld max:%lld\n", intl_min, intl_max);
 n = strlen( lbuf);
 if ( copy_to_user( _buf, lbuf, n)) return( -EFAULT);
 *_l += ret = n;
 return( ret);  }

static struct file_operations fo_I;	// on/off

// ---------- main (exported) functions
void intltest_procfs_init( void) {
 struct proc_dir_entry *tfs;
 char tmps[ 10];
 // register procfs entry
 fo_I.owner = THIS_MODULE;
 fo_I.open = dv_pf_open;
 fo_I.llseek =  no_llseek;
 fo_I.release = dv_pf_release;
 fo_I.read = dv_pf_R;
 fo_I.write = dv_pf_W;
 memset( tmps, 0, 10);
 sprintf( tmps, "%s", MNAME);
 tfs = proc_create_data( tmps, 0, NULL, &fo_I, NULL);
 return;  }

void intltest_procfs_clean( void) {
 char tmps[ 10];
 memset( tmps, 0, 10);
 sprintf( tmps, "%s", MNAME);
 remove_proc_entry( tmps, NULL);
 return;  }
