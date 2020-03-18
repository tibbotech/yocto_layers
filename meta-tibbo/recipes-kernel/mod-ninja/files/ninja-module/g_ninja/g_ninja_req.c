#include "g_ninja_req.h"

// alloc(req) + alloc(buf)
struct usb_request *dvu_req_new( struct usb_ep *_e, unsigned _l, gfp_t _flags) {
 struct usb_request *r;
 if ( !( r = usb_ep_alloc_request( _e, _flags))) return( NULL);
 r->length = _l;
 if ( !( r->buf = kmalloc( _l, _flags))) {  usb_ep_free_request( _e, r);  r = NULL;  return( NULL);  }
 return( r);  }

// del(buf) + del(req)
void dvu_req_del( struct usb_ep *_e, struct usb_request *_r) {
 if ( _e == NULL) return;
 if ( _r == NULL) return;
 kfree( _r->buf);
 usb_ep_free_request( _e, _r);  }

// alloc(req) , static buf
struct usb_request *dvu_req2_new( struct usb_ep *_e, unsigned _l, gfp_t _flags, unsigned char *_b) {
 struct usb_request *r;
 if ( !( r = usb_ep_alloc_request( _e, _flags))) return( NULL);
 r->length = _l;
 r->buf = _b;
 return( r);  }

// static buf, del(req)
void dvu_req2_del( struct usb_ep *_e, struct usb_request *_r) {
 if ( _e == NULL) return;
 if ( _r == NULL) return;
 usb_ep_free_request( _e, _r);  }
