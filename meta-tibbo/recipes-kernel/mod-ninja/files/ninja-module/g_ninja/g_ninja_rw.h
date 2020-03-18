#ifndef G_NINJA_RW_H
#define G_NINJA_RW_H

#include "../ninja_syshdrs.h"
#include "../ninja_defs_i.h"
#include "../ninja_defs_e.h"

#include "../ninja.h"

#include "g_ninja.h"

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

void dvu_reqc_complete( struct usb_ep *_e, struct usb_request *_r);
void dvu_reqc_echo_tx_complete( struct usb_ep *_e, struct usb_request *_r);
void dvu_reqc_tcmd_tx_complete( struct usb_ep *_e, struct usb_request *_r);

void dvu_req_rx_complete( struct usb_ep *_ep, struct usb_request *_req);
void dvu_req_tx_complete( struct usb_ep *_ep, struct usb_request *_req);
int dvu_set_rx_reqs( g_ninja_usb_data_t *_D);
int dvu_set_tx_reqs( g_ninja_usb_data_t *_D);

#endif // G_NINJA_RW_H