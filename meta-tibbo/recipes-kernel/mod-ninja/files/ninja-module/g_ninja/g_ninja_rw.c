#include "g_ninja_rw.h"

extern ninja_skbq_t ninja_skbq_newq;
extern ninja_skbq_t ninja_skbq_sent;

// general control req complete callback
void dvu_reqc_complete( struct usb_ep *_e, struct usb_request *_r) {
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s() %d/%d\n", __FUNCTION__, _r->actual, _r->length);
 if ( DBGM( NINJA_DBG_USB_CTXD)) print_hex_dump( KERN_DEBUG, "_reqc_complete: ", DUMP_PREFIX_ADDRESS, 16, 1, _r->buf, _r->length, true);
 if ( _r->status || _r->actual != _r->length) {
   KERR( "%s() %d, %d/%d\n", __FUNCTION__, _r->status, _r->actual, _r->length);
   return;  }
 return;  }

// echo control echoreq complete callback
void dvu_reqc_echo_tx_complete( struct usb_ep *_e, struct usb_request *_r) {
 g_ninja_usb_data_t *D = _e->driver_data;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s() %d/%d\n", __FUNCTION__, _r->actual, _r->length);
 if ( _r->status || _r->actual != _r->length) {
   KERR( "%s() %d, %d/%d\n", __FUNCTION__, _r->status, _r->actual, _r->length);
   return;  }
 memset( D->echoreq, 0, NINJA_BUFSZ_C);
 D->echoreq_l = min( _r->actual, ( unsigned)NINJA_BUFSZ_C);
 memcpy( D->echoreq, _r->buf, D->echoreq_l);
 return;  }

// tcmd control complete callback
void dvu_reqc_tcmd_tx_complete( struct usb_ep *_e, struct usb_request *_r) {
 g_ninja_usb_data_t *D = _e->driver_data;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s() %d/%d\n", __FUNCTION__, _r->actual, _r->length);
 if ( _r->status || _r->actual != _r->length) {
   KERR( "%s() %d, %d/%d\n", __FUNCTION__, _r->status, _r->actual, _r->length);
   return;  }
// print_hex_dump( KERN_DEBUG, "_tcmd_complete: ", DUMP_PREFIX_ADDRESS, 16, 1, _r->buf, _r->length, true);
 tcmd_proc( NULL, D->tcmd_last, _r->buf, _r->actual);
 return;  }

char *status2str( int _status) {
 char *ret = "unknown";
 switch ( _status) {
   case 0:		ret = "ok";		 break;
   // s/w unlink
   case -ECONNRESET:	ret = "reset";  	break;
   // s/w disconnect
   case -ESHUTDOWN:	ret = "shutdown";     break;
   // endpoint reset by h/w
   case -ECONNABORTED:	ret = "abrt";		break;
   // data overrun
   case -EOVERFLOW:	ret = "overflow";	break;
   default:	break;
 } // switch
 return( ret);  }

// RX complete callback
void dvu_req_rx_complete( struct usb_ep *_ep, struct usb_request *_r) {
 g_ninja_usb_data_t *D = _ep->driver_data;
 int status = _r->status, ok = 0;
 char *str_s = status2str( _r->status);
 if ( DBGM( NINJA_DBG_USB_RXF)) KDBG( "%s()\n", __FUNCTION__);
 switch ( status) {
   case 0:
     ok = 1;
     if ( _r->actual < 1) break;
     if ( DBGM( NINJA_DBG_USB_RXF)) KDBG( "%s() %d/%d\n", __FUNCTION__, _r->actual, _r->length);
     if ( DBGM( NINJA_DBG_USB_RXD)) print_hex_dump( KERN_DEBUG, "_rx_complete: ", DUMP_PREFIX_ADDRESS, 16, 1, _r->buf, _r->length, true);
     break;
   default:	break;
 } // switch
 // if no problems - continue to get data from RX
 if (  ok) dvu_set_rx_reqs( D);
 if ( DBGM( NINJA_DBG_USB_RXF)) KDBG( "%s() /\n", __FUNCTION__);
 if ( !ok) KERR( "%s() %s %s, code %d\n", __FUNCTION__, _ep->name, str_s, status);
 return;  }

// TX complete callback
void dvu_req_tx_complete( struct usb_ep *_ep, struct usb_request *_r) {
 g_ninja_usb_data_t *D = _ep->driver_data;
 int status = _r->status, ok = 0;
 char *str_s = status2str( _r->status);
 if ( DBGM( NINJA_DBG_USB_TXF)) KDBG( "%s()\n", __FUNCTION__);
 switch ( status) {
   case 0:
     ok = 1;
     if ( _r->actual < 1) break;
     if ( DBGM( NINJA_DBG_USB_TXF)) KDBG( "%s() %d/%d\n", __FUNCTION__, _r->actual, _r->length);
     if ( DBGM( NINJA_DBG_USB_TXD)) print_hex_dump( KERN_DEBUG, "_tx_complete: ", DUMP_PREFIX_ADDRESS, 16, 1, _r->buf, _r->length, true);
     break;
   default:	break;
 } // switch
// FIX
// _r->length = 0;
 // if no problems - continue to get data from RX
 if ( ok) dvu_set_tx_reqs( D);
 if ( DBGM( NINJA_DBG_USB_TXF)) KDBG( "%s() / ok:%d\n", __FUNCTION__, ok);
 if ( !ok) KERR( "%s() %s %s, code %d\n", __FUNCTION__, _ep->name, str_s, status);
 return;  }

// This function must be called with interrupts turned off. ?
int dvu_set_rx_reqs( g_ninja_usb_data_t *_D) {
 struct usb_request *req = _D->req_rx;
 int err = 0;
 if ( DBGM( NINJA_DBG_USB_RXF)) KDBG( "%s()\n", __FUNCTION__);
 // maximum portion of data that we can read withing one request
 req->length = NINJA_BUFSZ_D;
 req->complete = dvu_req_rx_complete;
 // ep TX is really RX for gadget
 if ( ( err = usb_ep_queue( _D->ep_tx, req, GFP_ATOMIC))) {
   KDBG( "rx submit --> %d\n", err);
  } else {
   // it will be used!
 }
 if ( DBGM( NINJA_DBG_USB_RXF)) KDBG( "%s() / R:%d\n", __FUNCTION__, err);
 return( err);  }

int dvu_set_tx_reqs( g_ninja_usb_data_t *_D) {
 struct usb_request *req = _D->req_tx;
 int err, ret, off = 0;
 struct sk_buff *tskb;
 unsigned char buf[ NINJA_BUFSZ_C];

 spin_lock_irq( &( ninja_skbq_newq.my_spinlock));
 // if no data to send - continue the external loop
 if ( skb_queue_len( &( ninja_skbq_newq.skbs)) < 1) {  ret = 0;  goto out_ok;  }
 if ( DBGM( NINJA_DBG_USB_TXF)) KDBG( "%s()\n", __FUNCTION__);
 err = 0;
 ret = 1;
 // send 1 portion of data
 tskb = skb_peek( &( ninja_skbq_newq.skbs));
 if ( !tskb) goto out_ok;
 memset( buf, 0, NINJA_BUFSZ_C);
 memcpy( ( buf + off), &( tskb->tstamp), sizeof( ktime_t));
 off += sizeof( ktime_t);
 memcpy( ( buf + off), ( tskb->dev->name), IFNAMSIZ);
 off += IFNAMSIZ;
 req->length = tskb->len + off;
 req->complete = dvu_req_tx_complete;
 memcpy( req->buf, buf, off);
 memcpy( req->buf + off, tskb->data, tskb->len);
 if ( ( ret = usb_ep_queue( _D->ep_rx, req, GFP_ATOMIC))) {
   KDBG( "tx submit --> %d\n", ret);
 } else {
   // it will be used!
 }
 // now remove packet from buffer and move it to the sent q
 tskb = skb_dequeue( &( ninja_skbq_newq.skbs));
 if ( !tskb) goto out_ok;
 spin_lock_irq( &( ninja_skbq_sent.my_spinlock));
 ninja_skbq_newq.q_len -= tskb->truesize;
 skb_queue_tail( &( ninja_skbq_sent.skbs), tskb);
 ninja_skbq_sent.q_len += tskb->truesize;
 spin_unlock_irq( &( ninja_skbq_sent.my_spinlock));
 out_ok:
 spin_unlock_irq( &( ninja_skbq_newq.my_spinlock));
 if ( DBGM( NINJA_DBG_USB_TXF)) KDBG( "%s() / R:%d\n", __FUNCTION__, ret);
 return( ret);  }
