#include "g_ninja.h"
#include "g_ninja_req.h"
#include "g_ninja_rw.h"
#include "g_ninja_conf.h"

#define G_NINJA_DESC_STRING_MANUFACTURER_IDX 1
#define G_NINJA_DESC_STRING_PRODUCT_IDX 2
#define G_NINJA_DESC_STRING_SERIAL_IDX 3

struct usb_string dv_du_strings_dev[] = {
 { .id = G_NINJA_DESC_STRING_MANUFACTURER_IDX,	.s = dv_du_manufacturer },
 { .id = G_NINJA_DESC_STRING_PRODUCT_IDX,	.s = dv_du_longname },
 { .id = G_NINJA_DESC_STRING_SERIAL_IDX,	.s = dv_du_serial },
 {}	// end-of-list
};

struct usb_gadget_strings dv_du_stringstab_dev = {
 .language = 0x0409, .strings = dv_du_strings_dev,	// en-us
};

// 1 interface for both speeds (FS/HS)
struct usb_interface_descriptor dv_du_I = {
 .bLength		= USB_DT_INTERFACE_SIZE,
 .bDescriptorType	= USB_DT_INTERFACE,
 .bInterfaceNumber	= 0,			// first
 .bAlternateSetting	= 0,			// no alternatives
 .bNumEndpoints		= 2,
 .bInterfaceClass	= USB_CLASS_VENDOR_SPEC,
 .bInterfaceSubClass	= 0,			// because V-Spec
 .bInterfaceProtocol	= 0,			// =2 ? (bi-directional)
 .iInterface		= 0,
};

// FS enpoints
struct usb_endpoint_descriptor dv_du_E_f_rx = {
 .bLength		= USB_DT_ENDPOINT_SIZE,
 .bDescriptorType	= USB_DT_ENDPOINT,
 .bEndpointAddress	= USB_DIR_IN,
 .bmAttributes		= USB_ENDPOINT_XFER_BULK,
 .wMaxPacketSize	= cpu_to_le16(64),
};
struct usb_endpoint_descriptor dv_du_E_f_tx = {
 .bLength		= USB_DT_ENDPOINT_SIZE,
 .bDescriptorType	= USB_DT_ENDPOINT,
 .bEndpointAddress	= USB_DIR_OUT,
 .bmAttributes		= USB_ENDPOINT_XFER_BULK,
 .wMaxPacketSize	= cpu_to_le16(64),
};
// HS enpoints
struct usb_endpoint_descriptor dv_du_E_h_rx = {
 .bLength		= USB_DT_ENDPOINT_SIZE,
 .bDescriptorType	= USB_DT_ENDPOINT,
 .bEndpointAddress	= USB_DIR_IN,
 .bmAttributes		= USB_ENDPOINT_XFER_BULK,
 .wMaxPacketSize	= cpu_to_le16(512),
};
struct usb_endpoint_descriptor dv_du_E_h_tx = {
 .bLength		= USB_DT_ENDPOINT_SIZE,
 .bDescriptorType	= USB_DT_ENDPOINT,
 .bEndpointAddress	= USB_DIR_OUT,
 .bmAttributes		= USB_ENDPOINT_XFER_BULK,
 .wMaxPacketSize	= cpu_to_le16(512),
};

// FS and HS descriptors
struct usb_descriptor_header *hdr_fs[] = {
 (struct usb_descriptor_header *)&dv_du_I,
 (struct usb_descriptor_header *)&dv_du_E_f_rx,
 (struct usb_descriptor_header *)&dv_du_E_f_tx,
 NULL
};
struct usb_descriptor_header *hdr_hs[] = {
 (struct usb_descriptor_header *)&dv_du_I,
 (struct usb_descriptor_header *)&dv_du_E_h_rx,
 (struct usb_descriptor_header *)&dv_du_E_h_tx,
 NULL
};

// describes other (then current) speed sonfiguration
// needed for full-speed client
struct usb_qualifier_descriptor dv_du_Q = {
 .bLength		= sizeof( dv_du_Q),
 .bDescriptorType	= USB_DT_DEVICE_QUALIFIER,
 .bcdUSB		= cpu_to_le16( 0x0200),
 .bDeviceClass		= USB_CLASS_VENDOR_SPEC,
 .bMaxPacketSize0	= cpu_to_le16(64),		// set dynamically
 .bNumConfigurations	= 1,
};

// configuration (one, self-powered)
struct usb_config_descriptor dv_du_C = {
 .bLength		= USB_DT_CONFIG_SIZE,
 .bDescriptorType	= USB_DT_CONFIG,
// .wTotalLength	= ...		// assign later
 .bNumInterfaces	= 1,
 .bConfigurationValue	= 1,
 .iConfiguration	= 0,
 .bmAttributes		= USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
// .bMaxPower		= CONFIG_USB_GADGET_VBUS_DRAW / 2,
};

// device
struct usb_device_descriptor dv_du_D = {
 .bLength		= USB_DT_DEVICE_SIZE,
 .bDescriptorType	= USB_DT_DEVICE,
 .bcdUSB		= cpu_to_le16( 0x0200),		// usb 2.0
 .bDeviceClass		= USB_CLASS_VENDOR_SPEC,
 .bDeviceSubClass	= 0,				// because Vendor-spec
 .bDeviceProtocol	= 0,				// because V-Sp
// .bMaxPacketSize0	= cpu_to_le16( 64),		// set dynamically
 .idVendor		= cpu_to_le16( NINJA_USB_VID),
 .idProduct		= cpu_to_le16( NINJA_USB_DID),
 .bcdDevice		= cpu_to_le16( 0xFFFF),	// strictly define later
 .iManufacturer		= G_NINJA_DESC_STRING_MANUFACTURER_IDX,
 .iProduct		= G_NINJA_DESC_STRING_PRODUCT_IDX,
 .iSerialNumber		= G_NINJA_DESC_STRING_SERIAL_IDX,
 .bNumConfigurations	= 1,
};
