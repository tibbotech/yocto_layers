#include "g_ninja.h"
#include "g_ninja_req.h"
#include "g_ninja_rw.h"
#include "g_ninja_conf.h"

//extern struct usb_string dv_du_strings_dev[];
extern struct usb_gadget_strings dv_du_stringstab_dev;
//// 1 interface for both speeds (FS/HS)
//extern struct usb_interface_descriptor dv_du_I;
//// FS enpoints
//static struct usb_endpoint_descriptor dv_du_E_f_rx;
//static struct usb_endpoint_descriptor dv_du_E_f_tx;
//// HS enpoints
//static struct usb_endpoint_descriptor dv_du_E_h_rx;
//static struct usb_endpoint_descriptor dv_du_E_h_tx;
// FS and HS descriptors
extern struct usb_descriptor_header *hdr_fs[];
extern struct usb_descriptor_header *hdr_hs[];
// describes other (then current) speed sonfiguration
// needed for full-speed client
extern struct usb_qualifier_descriptor dv_du_Q;
// configuration (one, self-powered)
extern struct usb_config_descriptor dv_du_C;
// device
extern struct usb_device_descriptor dv_du_D;

g_ninja_usb_data_t g_ninja_usb_data;

static struct usb_descriptor_header **g_speed_hdr( enum usb_device_speed _s, int _other) {
 struct usb_descriptor_header **ret = NULL;
 //KDBG( "%s(%s)\n", __FUNCTION__, usb_speed_string( _s));
 if ( _s <= USB_SPEED_FULL) ret = hdr_fs;
 if ( _s >= USB_SPEED_HIGH) ret = hdr_hs;
 if ( _other > -1) ret = ( ret == hdr_fs ? hdr_hs : hdr_fs);
 return( ret);  }

// _b - buf
static int config_buf( enum usb_device_speed _s, u8 *_b, u8 _t, unsigned _index, int _is_otg) {
 int len, idx = -1;
 struct usb_descriptor_header **function = NULL;
 if ( _t == USB_DT_OTHER_SPEED_CONFIG) idx = _index;
 function = g_speed_hdr( _s, idx);
 KDBG( "%s speed: %s (%d)\n", __FUNCTION__, usb_speed_string( _s), idx);
 if ( _index >= dv_du_D.bNumConfigurations) return( -EINVAL);
 len = usb_gadget_config_buf( &dv_du_C, _b, NINJA_BUFSZ_C, function);
 if ( len < 0) return( len);
 ( ( struct usb_config_descriptor *)_b)->bDescriptorType = _t;
 return( len); }

static void dv_gf_unbind( struct usb_gadget *_g) {
 g_ninja_usb_data_t *D = &g_ninja_usb_data;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s()\n", __FUNCTION__);
 D->connected = 0;
 usb_gadget_disconnect( _g);
 usb_ep_autoconfig_reset( _g);
 // free REQs there
KERR( "%s R0:%d\n", __FUNCTION__, D->req_rx->status);
 usb_ep_disable( D->ep_rx);
KERR( "%s T0:%d\n", __FUNCTION__, D->req_tx->status);
 usb_ep_disable( D->ep_tx);
 if ( D->req_rx) {
KERR( "%s R1\n", __FUNCTION__);
   usb_ep_dequeue( D->ep_tx, D->req_rx);
KERR( "%s R2\n", __FUNCTION__);
   dvu_req_del( D->ep_tx, D->req_rx);
KERR( "%s R3\n", __FUNCTION__);
   D->req_rx = NULL;  }
 if ( D->req_tx) {
KERR( "%s T1\n", __FUNCTION__);
   usb_ep_dequeue( D->ep_rx, D->req_tx);
KERR( "%s T2\n", __FUNCTION__);
   dvu_req_del( D->ep_rx, D->req_tx);
KERR( "%s T3\n", __FUNCTION__);
   D->req_tx = NULL;  }
 if ( D->req_c ) {  dvu_req_del( _g->ep0 , D->req_c );  D->req_c = NULL;  }
// ? usb_gadget_disconnect
// usb_free_descriptors( );
// usb_put_function ..
 set_gadget_data( _g, NULL);
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s() /\n", __FUNCTION__);
 return;  }

static int dv_gf_bind( struct usb_gadget *_g
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
 , struct usb_gadget_driver *_driver
#endif
) {
 g_ninja_usb_data_t *D = &g_ninja_usb_data;
 struct usb_descriptor_header **ep = NULL;
 int gcnum, status = -ENOMEM;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s()\n", __FUNCTION__);
 D->config = 0;
 D->interface = INTERFACE_DEVNULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)
 gcnum = usb_gadget_controller_number( _g);
 if ( gcnum >= 0) dv_du_D.bcdDevice = cpu_to_le16( 0x0200 + gcnum);
#else
 gcnum = get_default_bcdDevice();
 if ( gcnum >= 0) dv_du_D.bcdDevice = cpu_to_le16( gcnum);
#endif
 if ( gcnum < 0) KERR( "controller %s not recognized\n", _g->name);
 usb_ep_autoconfig_reset( _g);
 KINF( "speed cur:%s max:%s\n", usb_speed_string( _g->speed), usb_speed_string( _g->max_speed));
 ep = g_speed_hdr( _g->max_speed, -1);
 D->ep_rx = usb_ep_autoconfig( _g, ( struct usb_endpoint_descriptor *)ep[ 1]);
 if ( !D->ep_rx) {  KERR( "EP RX conf error for %s\n", _g->name);  return( -ENODEV);  }
 D->ep_rx->driver_data = D->ep_rx;
 D->ep_tx = usb_ep_autoconfig( _g, ( struct usb_endpoint_descriptor *)ep[ 2]);
 if ( !D->ep_tx) {  KERR( "EP TX conf error for %s\n", _g->name);  return( -ENODEV);  }
 D->ep_tx->driver_data = D->ep_tx;
 dv_du_D.bMaxPacketSize0 = _g->ep0->maxpacket;
 // usb_ep_autoconfig wrongly changes highspeed bulk ep to 64 max packet size
 ( ( struct usb_endpoint_descriptor *)( hdr_hs[ 1]))->wMaxPacketSize = cpu_to_le16( 512);
 ( ( struct usb_endpoint_descriptor *)( hdr_hs[ 2]))->wMaxPacketSize = cpu_to_le16( 512);
 usb_gadget_set_selfpowered( _g);
 // add 1 REQ for control msg (ep0), and N to in/out
 D->req_c = dvu_req_new( _g->ep0, NINJA_BUFSZ_C, GFP_KERNEL);
 if ( !D->req_c) {  status = -ENOMEM;  goto fail;  }
 // connect ep1 with RX buffer
 D->req_rx = dvu_req_new( D->ep_tx, NINJA_BUFSZ_D, GFP_KERNEL);
 if ( !D->req_rx) {  status = -ENOMEM;  goto fail;  }
 // connect ep1 with TX buffer
 D->req_tx = dvu_req_new( D->ep_rx, NINJA_BUFSZ_D, GFP_KERNEL);
 if ( !D->req_tx) {  status = -ENOMEM;  goto fail;  }
 D->gadget = _g;
 set_gadget_data( _g, D);
 _g->ep0->driver_data = D;
 D->connected = 0;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s using %s tx, %s rx\n", _g->name, D->ep_rx->name, D->ep_tx->name);
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s() /\n", __FUNCTION__);
 return( 0);
 fail:
 KERR( "%s() failed /\n", __FUNCTION__);
 dv_gf_unbind( _g);
 return( status);  }

int dv_gf_req_std( struct usb_gadget *_g, const struct usb_ctrlrequest *_ctrl, u16 _v, u16 _i, u16 _l, struct usb_request *_req) {
 g_ninja_usb_data_t *D = get_gadget_data( _g);
 int ret = -EOPNOTSUPP;
 struct usb_descriptor_header **t_hdr = NULL;
 struct usb_endpoint_descriptor *t_ep = NULL;
 char *str_r, *str_v;
 str_r = str_v = "unknown";
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s()\n", __FUNCTION__);
 switch ( _ctrl->bRequest) {
   case USB_REQ_GET_DESCRIPTOR:  str_r = "get_descriptor";
     if ( _ctrl->bRequestType != USB_DIR_IN) break;
     switch ( _v >> 8) {
       case USB_DT_DEVICE:  str_v = "device";
         ret = min( _l, ( u16)sizeof( dv_du_D));
         memcpy( _req->buf, &dv_du_D, ret);
         break;
       case USB_DT_DEVICE_QUALIFIER:  str_v = "device_qualifier";
         // return updated bMaxPacketSize0 each time ?
         t_hdr = g_speed_hdr( _g->speed, 0);
         t_ep = ( struct usb_endpoint_descriptor *)( t_hdr[ 1]);
         dv_du_Q.bMaxPacketSize0 = t_ep->wMaxPacketSize;
         ret = min( _l, ( u16)sizeof( dv_du_Q));
         memcpy( _req->buf, &dv_du_Q, ret);
         break;
       case USB_DT_OTHER_SPEED_CONFIG:  str_v = "other_speed_config";
       case USB_DT_CONFIG:
         if ( _v >> 8 == USB_DT_CONFIG) str_v = "config";
         ret = config_buf( _g->speed, _req->buf, _v >> 8, _v & 0xff, _g->is_otg);
         if ( ret >= 0) ret = min( _l, ( u16)ret);
         break;
       case USB_DT_STRING:  str_v = "string";
         ret = usb_gadget_get_string( &dv_du_stringstab_dev, _v & 0xff, _req->buf);
         break;
       case USB_DT_INTERFACE:  str_v = "interface";	break;
       case USB_DT_ENDPOINT:  str_v = "endpoint";	break;
       default:  KINF( "Unimplemented descriptor:0x%x\n", _v >> 8);  break;
     } // switch _v
     break;
   case USB_REQ_SET_CONFIGURATION:  str_r = "set_configuration";
     if ( _ctrl->bRequestType != 0) break;
     ret = dvu_set_config( D, _v, &dv_du_C);
     break;
   case USB_REQ_GET_CONFIGURATION:  str_r = "get_configuration";
     if ( _ctrl->bRequestType != USB_DIR_IN) break;
     *( u8 *)_req->buf = D->config;
     ret = min( _l, ( u16)1);
     break;
   case USB_REQ_SET_INTERFACE:  str_r = "set_interface";
     if ( _ctrl->bRequestType != USB_RECIP_INTERFACE || !D->config) break;
     ret = dvu_set_interface( D, INTERFACE_DEFAULT, g_speed_hdr( _g->speed, -1));
     break;
   case USB_REQ_GET_INTERFACE:  str_r = "get_interface";
     if ( _ctrl->bRequestType != ( USB_DIR_IN | USB_RECIP_INTERFACE) || !D->config) break;
     *( u8 *)_req->buf = D->interface;
     ret = min( _l, ( u16)1);
     break;
   default:  KINF( "Unimplemented request:0x%x", _ctrl->bRequest);  break;
 } // switch bRequest
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s() / %s->%s R:%d\n", __FUNCTION__, str_r, str_v, ret);
 return( ret);  }

// vendor-specific request
int dv_gf_req_vs( struct usb_gadget *_g, const struct usb_ctrlrequest *_ctrl, u16 _v, u16 _i, u16 _l, struct usb_request *_req) {
 g_ninja_usb_data_t *D = get_gadget_data( _g);
 int ret = -EOPNOTSUPP;
 ninja_cmddesc_t *tcmd_desc = NULL;
 char *str_r, *str_v, *testmessage = "testmessage";
 str_r = str_v = "unknown";
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s(%d)\n", __FUNCTION__, _ctrl->bRequest);
 tcmd_desc = tcmd_find_desc( _ctrl->bRequest);
 if ( !tcmd_desc) KINF( "%s() no tcmd_desc found for %d req\n", __FUNCTION__, _ctrl->bRequest);
 switch ( _ctrl->bRequest) {
   case NINJA_CMD_INIT:		str_r = "init";
     if ( D->connected) {  ret = 0;  break;  }
     ret = dvu_set_interface( D, INTERFACE_DEFAULT, g_speed_hdr( _g->speed, -1));
     if ( ret != 0) break;
     D->connected = 1;
     if ( ( ret = dvu_set_rx_reqs( D))) break;
     if ( ( ret = dvu_set_tx_reqs( D))) break;
     break;
   case NINJA_CMD_CLEARP:	str_r = "clearp";
     ret = tcmd_proc( NULL, _ctrl->bRequest, _req->buf, _l);
     break;
   case NINJA_CMD_STATS:	str_r = "stats";
     ret = tcmd_proc( NULL, _ctrl->bRequest, _req->buf, _l);
     break;
   case NINJA_CMD_TESTMSG:	str_r = "testmsg";
     // out(at our side IN flag) only
     ret = min( _l, ( u16)strlen( testmessage));
     memcpy( _req->buf, testmessage, ret);
     break;
   case NINJA_CMD_ECHO:		str_r = "echo";
     if ( !( _ctrl->bRequestType & USB_DIR_IN)) {
       _req->complete = dvu_reqc_echo_tx_complete;
       ret = _l;  break;  }
     ret = 0;
     if ( _l < 6) break;
     ret = min( D->echoreq_l, ( unsigned)_l);
     memcpy( _req->buf, D->echoreq, ret);
     memcpy( _req->buf, "reply:", 6);
     break;
   default:			str_r = "tcmd";
     // ret > 0
     // if have input: place callback _complete_ and make ret = _l
     // if have output: fill buffer _req->buf, ret == sizeof reply
     // ret = 0 if success result or no arguments needed
     //
     // if IN (write to us) request...
     if ( !( _ctrl->bRequestType & USB_DIR_IN)) {
       // if no IN buffer needed and quick - do there
       if ( tcmd_desc && !tcmd_desc->need_buf_rx) {
         ret = tcmd_proc( NULL, _ctrl->bRequest, _req->buf, _l);
         break;  }
       // if IN buffer needed - do later (in complete)
       D->tcmd_last = _ctrl->bRequest;
       _req->complete = dvu_reqc_tcmd_tx_complete;
       ret = _l;  break;  }
     // IF OUT (read from us) request...
     ret = tcmd_proc( NULL, _ctrl->bRequest, _req->buf, _l);
     break;
 } // switch bRequest
 if ( DBGM( NINJA_DBG_USB_CTF)) KINF( "%s() / %s->%s R:%d\n", __FUNCTION__, str_r, str_v, ret);
 return( ret);  }

static int dv_gf_setup( struct usb_gadget *_g, const struct usb_ctrlrequest *_ctrl) {
 g_ninja_usb_data_t *D = get_gadget_data( _g);
 struct usb_request *req = D->req_c;
 int value = -EOPNOTSUPP;
 u16 wIndex = le16_to_cpu( _ctrl->wIndex);
 u16 wValue = le16_to_cpu( _ctrl->wValue);
 u16 wLength = le16_to_cpu( _ctrl->wLength);
 char *str_d_rt_d = "unknown";		// dbg req direction
 char *str_d_rt_t = "unknown";		// dbg req type
 char *str_d_rt_r = "unknown";		// dbg req recipient
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s()\n", __FUNCTION__);
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "ctrl req%02x.%02x v%04x i%04x l%d\n",
         _ctrl->bRequestType, _ctrl->bRequest, wValue, wIndex, wLength); 
 req->complete = dvu_reqc_complete;
 if ( _ctrl->bRequestType & USB_DIR_IN ) str_d_rt_d = "IN (Dev->Host)";
 else str_d_rt_d = "OUT(Host->Dev)";
 switch ( _ctrl->bRequestType & USB_RECIP_MASK) {
   case USB_RECIP_DEVICE:  	str_d_rt_r = "device"; 	 	break;
   case USB_RECIP_INTERFACE:  	str_d_rt_r = "interface";	break;
   case USB_RECIP_ENDPOINT: 	str_d_rt_r = "endpoint";	break;
   case USB_RECIP_OTHER:	str_d_rt_r = "other";		break;
   case USB_RECIP_PORT:		str_d_rt_r = "port";		break;
   case USB_RECIP_RPIPE:	str_d_rt_r = "rpipe";		break;
 }
 if ( DBGM( NINJA_DBG_USB_CRXD)) print_hex_dump( KERN_DEBUG, "gf_setup rx: ", DUMP_PREFIX_ADDRESS, 16, 1, req->buf, wLength, true);
 // process request
 switch ( _ctrl->bRequestType & USB_TYPE_MASK) {
   case USB_TYPE_STANDARD:	str_d_rt_t = "standart";
     value = dv_gf_req_std( _g, _ctrl, wValue, wIndex, wLength, req);
     break;
   case USB_TYPE_CLASS:		str_d_rt_t = "class";
     break;
   case USB_TYPE_VENDOR:	str_d_rt_t = "vendor";
     value = dv_gf_req_vs( _g, _ctrl, wValue, wIndex, wLength, req);
     break;
   case USB_TYPE_RESERVED:	str_d_rt_t = "reserved";
     break;
   default:  break;
 } // switch bRequestType
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "req decoded D:%s T:%s R:%s\n", str_d_rt_d, str_d_rt_t, str_d_rt_r);
 // respond with data transfer before status phase?
 if ( value >= 0) {
   req->length = value;
   req->zero = value < wLength;
   if ( DBGM( NINJA_DBG_USB_CTXD)) print_hex_dump( KERN_DEBUG, "gf_setup tx:", DUMP_PREFIX_ADDRESS, 16, 1, req->buf, req->length, true);
   if ( ( value = usb_ep_queue( _g->ep0, req, GFP_ATOMIC)) < 0) {
     if ( DBGM( NINJA_DBG_USB_CTF)) KERR( "ep0_queue() R:%d\n", value);
     req->status = 0;
     req->complete( _g->ep0, req);
   }
 }
 if ( DBGM( NINJA_DBG_USB_CTF)) KINF( "%s() / R:%d\n", __FUNCTION__, value);
 // if value < 0 host stalls. 0 - success
 return( value);  }

static void dv_gf_disconnect( struct usb_gadget *_g) {
 g_ninja_usb_data_t *D = get_gadget_data( _g);
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s()\n", __FUNCTION__);
 D->connected = 0;
 return;  }

struct usb_gadget_driver dv_du_driver = {
 .function	= ( char *)dv_du_longname,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
 .max_speed	= USB_SPEED_SUPER,
#else
 .speed		= USB_SPEED_HIGH,
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
 .bind		= dv_gf_bind,
#endif
 .unbind	= dv_gf_unbind,
 .setup		= dv_gf_setup,
 .disconnect	= dv_gf_disconnect,
 .driver	= {
    .name	= ( char *)dv_du_gname,
    .owner	= THIS_MODULE,
 },
};
