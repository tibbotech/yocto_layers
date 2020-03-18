#include "ninja_io_usb.h"
#include "ninja_cmd.h"
#include "ninja_hndl.h"

#include "g_ninja/g_ninja.h"
#include "g_ninja/g_ninja_req.h"
#include "g_ninja/g_ninja_rw.h"

int dv_u_C( kthread_t *_tp) {
 if ( !_tp) return( 0);
 KDBG( "'%s' close\n", _tp->thname);
 _tp->state = DV_THS_DELIST;		// ready to put thread resorces
 return( 0);  }

// _tp - current thread pointer
int dv_u_task( kthread_t *_tp) {
 g_ninja_usb_data_t *D = &g_ninja_usb_data;
 struct usb_request *r;

 if ( !D->connected) return( 0);
 // re-assign TX/RX
 r = D->req_tx;
 if ( r && r->length < 1) dvu_set_tx_reqs( D);
 r = D->req_rx;
 if ( r && r->length < 1) dvu_set_rx_reqs( D);

 // do slow requests there
// if ( dev->tcmd_req_l > 0) {
//   tcmd_proc( NULL, dev->tcmd_last, dev->tcmd_req, dev->tcmd_req_l);
//   dev->tcmd_req_l = 0;
// }
 return( 0);  }

// _tp - current thread pointer
int dv_u_O( void *_tp) {
 kthread_t *tp = ( kthread_t *)_tp;
 while ( dv_io( tp) == 0);
 return( 0);  }

// ---------- main (exported) functions
void ninja_io_usb_init( void) {
 kthread_t *usbth;
 th_tasks_t tasks[] = {
  { .O = &dv_u_O, .W = &dv_u_task, .C = &dv_u_C },
 };
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
 usb_gadget_probe_driver( &dv_du_driver);
#else
 usb_gadget_probe_driver( &dv_du_driver, &dv_gf_bind);
#endif
 usbth = dv_th_init_task( MNAME"/usbs", tasks[ 0]);
 dv_th_run( usbth);
 return;  }

void ninja_io_usb_clean( void) {
 KDBG( "%s()\n", __FUNCTION__);
 usb_gadget_unregister_driver( &dv_du_driver);
 KDBG( "%s() /\n", __FUNCTION__);
 return;  }
