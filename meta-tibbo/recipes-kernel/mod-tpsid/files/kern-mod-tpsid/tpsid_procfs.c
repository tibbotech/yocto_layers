#include "tpsid_procfs.h"

static int dv_pf_open( struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_release(struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_W( struct file *_f, const char __user *_buf, size_t _cnt, loff_t *_l) {  return( 0);  }

static int dv_pf_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 unsigned char ret;
 if ( *_l > 0) return( 0);
 if ( copy_to_user( _buf, tpsid, TPSID_LEN)) return( -EFAULT);
 *_l += ret = TPSID_LEN;
 return( ret);  }

static int dv_pf_Rt( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 unsigned char ret;
 unsigned char buff[ TPSID_LEN*2 + 1];
 int i;
 if ( *_l > 0) return( 0);
 memset( buff, 0, TPSID_LEN*2 + 1);
 for ( i = 0; i < TPSID_LEN; i++) {
   sprintf( buff + i*2, "%02X", tpsid[ i]);
 }
 if ( copy_to_user( _buf, buff, TPSID_LEN*2)) return( -EFAULT);
 *_l += ret = TPSID_LEN*2;
 return( ret);  }

static struct file_operations fo_I;	// on/off
static struct file_operations fo_It;	// text representation

// ---------- main (exported) functions
void tpsid_procfs_init( void) {
 struct proc_dir_entry *tfs, *tfst;
 char tmps[ 10];
 // register procfs entry
 fo_It.owner 	= fo_I.owner = THIS_MODULE;
 fo_It.open 	= fo_I.open = dv_pf_open;
 fo_It.llseek 	= fo_I.llseek =  no_llseek;
 fo_It.release 	= fo_I.release = dv_pf_release;
 fo_It.write 	= fo_I.write = dv_pf_W;
 fo_I.read 	= dv_pf_R;
 fo_It.read 	= dv_pf_Rt;
 memset( tmps, 0, 10);
 sprintf( tmps, "%s", MNAME);
 tfs  = proc_create_data( tmps, 0, NULL, &fo_I, NULL);
 memset( tmps, 0, 10);
 sprintf( tmps, "%st", MNAME);
 tfst = proc_create_data( tmps, 0, NULL, &fo_It, NULL);
 return;  }

void tpsid_procfs_clean( void) {
 char tmps[ 10];
 memset( tmps, 0, 10);
 sprintf( tmps, "%s", MNAME);
 remove_proc_entry( tmps, NULL);
 memset( tmps, 0, 10);
 sprintf( tmps, "%st", MNAME);
 remove_proc_entry( tmps, NULL);
 return;  }
