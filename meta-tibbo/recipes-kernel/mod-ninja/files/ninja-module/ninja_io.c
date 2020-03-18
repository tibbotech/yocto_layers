#include "ninja_io.h"

// _tp - current thread pointer
int dv_io( void *_tp) {
 int err, nosleep;
 kthread_t *tp = ( kthread_t *)_tp;

 nosleep = 0;
 while ( 1) {
   if ( kthread_should_stop()) break;
   schedule();
   if ( nosleep == 0) msleep( 10);
//   else msleep( 1);
   nosleep = 0;
   if ( !tp->W) break;
   err = tp->W( tp);
   if ( err > 0) nosleep = 1;
   if ( err < 0) break;
 }
 KDBG( "'%s' finish %s\n", tp->thname, __FUNCTION__);
 // ask to call ->destroy for me
 tp->state = DV_THS_DESTROY;
 // always close connection
 return( 1);  }
