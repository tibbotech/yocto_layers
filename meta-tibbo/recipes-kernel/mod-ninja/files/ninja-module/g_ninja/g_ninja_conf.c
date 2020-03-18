#include "g_ninja_conf.h"

// change our operational config
int dvu_set_config( g_ninja_usb_data_t *_D, unsigned _cidx, struct usb_config_descriptor *_c) {
 int ret = 0;
 unsigned power;
 struct usb_gadget *gadget = _D->gadget;
 switch ( _cidx) {
   case 1:  ret = 0;  break;
   default:  ret = -EINVAL;
     /* FALL THROUGH */
   case 0:  break;
 }
 if ( ret) {
   if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s OTG:%d\n", __FUNCTION__, _D->gadget->is_otg);
   usb_gadget_vbus_draw( _D->gadget, _D->gadget->is_otg ? 8 : 100);
   return( ret);  }
 power = 2 * _c->bMaxPower;
 usb_gadget_vbus_draw( _D->gadget, power);
 _D->config = _cidx;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG("%s speed:%s config #%d: %d mA\n", __FUNCTION__, usb_speed_string( gadget->speed), _cidx, power);
 return( ret); }

void dvu_reset_interface( g_ninja_usb_data_t *_D) {
 if ( _D->interface < 0) return;
 if ( _D->ep_rx && _D->ep_rx->desc) usb_ep_disable( _D->ep_rx);
 if ( _D->ep_tx && _D->ep_tx->desc) usb_ep_disable( _D->ep_tx);
 _D->ep_rx->desc = _D->ep_tx->desc = NULL;
 _D->interface = INTERFACE_DEVNULL;
 return;  }

int dvu_set_interface_i( g_ninja_usb_data_t *_D, struct usb_descriptor_header *_desc[]) {
 int ret = 0;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s()\n", __FUNCTION__);
 _D->ep_rx->desc = ( struct usb_endpoint_descriptor *)_desc[ 1];
 _D->ep_rx->driver_data = _D;
 _D->ep_tx->desc = ( struct usb_endpoint_descriptor *)_desc[ 2];
 _D->ep_tx->driver_data = _D;
 if ( ( ret = usb_ep_enable( _D->ep_rx)) != 0) {
   KERR( "%s rx %s E:%d\n", __FUNCTION__, _D->ep_rx->name , ret);
   goto done;  }
 if ( ( ret = usb_ep_enable( _D->ep_tx)) != 0) {
   KERR( "%s tx %s E:%d\n", __FUNCTION__, _D->ep_tx->name , ret);
   goto done;  }
done:
 // on error, disable endpoints
 if ( ret != 0) {
   usb_ep_disable( _D->ep_rx);  usb_ep_disable( _D->ep_tx);
   _D->ep_rx->desc = _D->ep_tx->desc = NULL;  }
 if ( DBGM( NINJA_DBG_USB_CTF)) KINF( "%s() / R:%d\n", __FUNCTION__, ret);
 return( ret);  }

int dvu_set_interface( g_ninja_usb_data_t *_D, unsigned _i, struct usb_descriptor_header *_desc[]) {
 int ret = 0;
 if ( DBGM( NINJA_DBG_USB_CTF)) KDBG( "%s(%d)\n", __FUNCTION__, _i);
 // Free the current interface
 dvu_reset_interface( _D);
 ret = dvu_set_interface_i( _D, _desc);
 if ( ret) dvu_reset_interface( _D);
 else _D->interface = _i;
 if ( DBGM( NINJA_DBG_USB_CTF)) KINF( "%s() / R:%d\n", __FUNCTION__, ret);
 return( ret);  }
