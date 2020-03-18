#include "ninja_th.h"

kthread_t *dv_th_init( unsigned char *_thn, int (*_thfnp)( void *_data), int (*_thdfnp)( kthread_t *)) {
 th_tasks_t task = { .O = _thfnp, .W = NULL, .C = _thdfnp };
 return( dv_th_init_task( _thn, task));  }

kthread_t *dv_th_init_task( unsigned char *_thn, th_tasks_t _task) {
 kthread_t *tp = kmalloc( sizeof( kthread_t), GFP_KERNEL);
 thread_counter++;
 if ( tp == NULL) {
   KERR( "alloc thread '%s' error\n", _thn);
   return( NULL);  }
 memset( tp, 0, sizeof( kthread_t));
 strncpy( tp->thname, _thn, NINJA_THNAME_LEN);
 tp->thread = kthread_create( _task.O, tp, _thn);
 if ( IS_ERR( tp->thread)) {
   KERR( "start '%s' thread error\n", tp->thname);
   kfree( tp);
   return( NULL);  }
 tp->W = _task.W;
 tp->C = _task.C;
 tp->state = DV_THS_RESTARTING;
 return( tp);  }

kthread_t *dv_th_run( kthread_t *_tp) {
 // spin for *tp ?
 if ( !_tp) return( _tp);
 _tp->state = DV_THS_RUNNING;
 list_add( &( _tp->list), &( ninja_threads.list));
 wake_up_process( _tp->thread);
 return( _tp);  }

// this is just for signalling thread to stop in while cicles!
kthread_t *dv_th_stop( kthread_t *_tp) {
 if ( !_tp) {
   KERR( "'%s' thread is null0???\n", _tp->thname);
   return( NULL);  }
 if ( !_tp->thread) {
   KERR( "'%s' thread is null1???\n", _tp->thname);
   return( _tp);  }
 KDBG( "signalling '%s' thread to stop\n", _tp->thname);
 kthread_stop( _tp->thread);
 KDBG( "'%s' thread stop signalled\n", _tp->thname);
 return( NULL); }

// 0 == success, -1 == delete the resources
int th_state_handle( kthread_t *_tp) {
 switch ( _tp->state) {
   case DV_THS_RUNNING: // thread is running
     break;
   case DV_THS_RESTARTING: // thread is in restarting state
     KINF( "%s restarting thread '%s' (FIX)\n", __FUNCTION__, _tp->thname);
     break;
   case DV_THS_STOPPING: // thread have to be stopped
     dv_th_stop( _tp);
     break;
   case DV_THS_DESTROY: // thread needs to destroy it's resources
     if ( _tp->C) _tp->C( _tp);
     break;
   case DV_THS_DELIST: // thread needs to be delisted
     return( -1);
     break;
   default:  // unknown thread state
     KERR( "%s unknown thread '%s' state %d\n", __FUNCTION__, _tp->thname, _tp->state);
     break;
 }
 return( 0);  }
