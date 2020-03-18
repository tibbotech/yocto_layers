#ifndef G_NINJA_REQ_H
#define G_NINJA_REQ_H

#include "../ninja_syshdrs.h"
#include "../ninja_defs_i.h"
#include "../ninja_defs_e.h"

#include "../ninja.h"

#include "g_ninja.h"

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

struct usb_request *dvu_req_new( struct usb_ep *_e, unsigned _l, gfp_t _flags);
void dvu_req_del( struct usb_ep *_e, struct usb_request *_r);
struct usb_request *dvu_req2_new( struct usb_ep *_e, unsigned _l, gfp_t _flags, unsigned char *_b);
void dvu_req2_del( struct usb_ep *_e, struct usb_request *_r);

#endif // G_NINJA_REQ_H