#include "twg_procfs.h"

static int dv_pf_open( struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_release(struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_W( struct file *_f, const char __user *_buf, size_t _cnt, loff_t *_l) {  return( 0);  }

static int dv_pf_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 int ret;
 if ( *_l > 0) return( 0);
// if ( copy_to_user( _buf, twgid, TWG_MAX_BUF)) return( -EFAULT);
 *_l += ret = TWG_MAX_BUF;
 return( ret);  }

static struct file_operations fo_I;	// on/off

// ---------- main (exported) functions
void twg_procfs_init( struct platform_device *_pdev) {
 struct proc_dir_entry *tfs;
 // register procfs entry
 fo_I.owner = THIS_MODULE;
 fo_I.open = dv_pf_open;
 fo_I.llseek =  no_llseek;
 fo_I.release = dv_pf_release;
 fo_I.read = dv_pf_R;
 fo_I.write = dv_pf_W;
 tfs = proc_create_data( dev_name( &( _pdev->dev)), 0, NULL, &fo_I, NULL);
 return;  }

void twg_procfs_clean( struct platform_device *_pdev) {
 remove_proc_entry( dev_name( &( _pdev->dev)), NULL);
 return;  }
