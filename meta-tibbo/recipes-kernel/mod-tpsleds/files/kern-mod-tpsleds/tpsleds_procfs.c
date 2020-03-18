#include "tpsleds_procfs.h"
//#include "tpsleds_cmd.h"

#define TPSLEDS_PROCFS_DIR MNAME

static int dv_pf_open( struct inode *_i, struct file *_f) {  return( 0);  }
static int dv_pf_release(struct inode *_i, struct file *_f) {  return( 0);  }

static int dv_pf_N_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 unsigned char n, ret;
 unsigned char lbuf[ 10];
 if ( *_l > 0) return( 0);
 n = simple_strtol( _f->f_path.dentry->d_iname, NULL, 10);
 *_l += sizeof( ret);
 memset( lbuf, 0, 10);
 ret = ( 1 && ( leds_current & 0x01 << n));
 sprintf( lbuf, "%d", ret);
 if ( copy_to_user( _buf, lbuf, 1)) return( -EFAULT);
 *_l += ret = strlen( lbuf);
 return( ret);  }
static int dv_pf_N_W( struct file *_f, const char __user *_buf, size_t _cnt, loff_t *_l) {
 unsigned char n, v = 0x01;
 unsigned char lbuf[ 10];
 if ( copy_from_user( lbuf, _buf, 1)) return( -EFAULT);
 n = simple_strtol( _f->f_path.dentry->d_iname, NULL, 10);
 if ( lbuf[ 0] == '0') v = 0;
 // special case: 2 = blink
 if ( lbuf[ 0] == '2') {
   // if not running - run it
   if ( ( leds_timers & ( 0x01 << n)) == 0) {
      if ( ltimers[ n].timeout == 0) ltimers[ n].timeout = msecs_to_jiffies( 400);
      ltimers[ n].timer.expires = ltimers[ n].timeout + jiffies;
      leds_timers |= ( 0x01 << n);
      add_timer( &( ltimers[ n].timer));
   }
   *_l += _cnt;
   return( _cnt);  }
// if ( *_l >= sizeof( leds_current)) return( 0);
 *_l += _cnt;
 leds_timers &= ~( 1 << n);
 if ( v == 0) leds_current &= ~( 1 << n);
 else leds_current |= ( 1 << n);
//printk( KERN_INFO "x:%d o:%c l:%d\n", v, lbuf[ 0], leds_current);
 tps_leds_set( leds_current);
 return( _cnt);  }

static int dv_pf_B_R( struct file *_f, char __user *_buf, size_t _s, loff_t *_l) {
 unsigned char n;
 unsigned int ret;
 if ( *_l > 0) return( 0);
 n = simple_strtol( _f->f_path.dentry->d_iname, NULL, 10);
 ret = ltimers[ n].timeout;
 memset( _buf, 0, _s);
 sprintf( _buf, "%d", ret);
 *_l += ret = strlen( _buf);
//printk( KERN_INFO "[%d]:%d l:%d\n", n, ret, *_l);
 return( ret);  }
static int dv_pf_B_W( struct file *_f, const char __user *_buf, size_t _cnt, loff_t *_l) {
 unsigned char lbuf[ 10], n;
 unsigned int v;
 if ( copy_from_user( lbuf, _buf,  ( _cnt >= 9 ? 9 : _cnt))) return( -EFAULT);
 n = simple_strtol( _f->f_path.dentry->d_iname, NULL, 10);
 v = ( unsigned int)simple_strtol( lbuf, NULL, 10);
 ltimers[ n].timeout = v;
 *_l += _cnt;
//printk( KERN_INFO "[%d]:%dms\n", n, v);
 return( _cnt);  }

struct proc_dir_entry *tpsleds_procfs_root;

static struct file_operations fo_N;	// on/off
static struct file_operations fo_B;	// blink freq

// ---------- main (exported) functions
void tpsleds_procfs_init( void) {
 struct proc_dir_entry *tfs;
 int i = 0;
 char tmps[ 10];
 // register procfs entry
 fo_N.owner = fo_B.owner = THIS_MODULE;
 fo_N.open = fo_B.open = dv_pf_open;
 fo_N.llseek =  fo_B.llseek = no_llseek;
 fo_N.release =  fo_B.release = dv_pf_release;
 tpsleds_procfs_root = proc_mkdir( TPSLEDS_PROCFS_DIR, NULL);
 fo_N.read = dv_pf_N_R;  fo_N.write = dv_pf_N_W;
 fo_B.read = dv_pf_B_R;  fo_B.write = dv_pf_B_W;
 for ( i = 0; i < TPS_MAX_LEDS; i++) {
   memset( tmps, 0, 10);
   sprintf( tmps, "%d", i);
   tfs = proc_create_data( tmps, 0, tpsleds_procfs_root, &fo_N, NULL);
   memset( tmps, 0, 10);
   sprintf( tmps, "%db", i);
   tfs = proc_create_data( tmps, 0, tpsleds_procfs_root, &fo_B, NULL);
 }
 return;  }

void tpsleds_procfs_clean( void) {
 int i = 0;
 char tmps[ 10];
// KDBG( "%s()\n", __FUNCTION__);
 for ( i = 0; i < TPS_MAX_LEDS; i++) {
   memset( tmps, 0, 10);
   sprintf( tmps, "%d", i);
   remove_proc_entry( tmps, tpsleds_procfs_root);
   memset( tmps, 0, 10);
   sprintf( tmps, "%db", i);
   remove_proc_entry( tmps, tpsleds_procfs_root);
 }
 if ( tpsleds_procfs_root) remove_proc_entry( TPSLEDS_PROCFS_DIR, NULL);
// KDBG( "%s() /\n", __FUNCTION__);
 return;  }
