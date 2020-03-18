#ifndef NINJA_G_NINJA_H
#define NINJA_G_NINJA_H

#include "../ninja_syshdrs.h"
#include "../ninja_defs_i.h"
#include "../ninja_defs_e.h"

#include "../ninja_cmd.h"

#include "../ninja.h"

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/composite.h>

//// shared

#define INTERFACE_DEVNULL -1
#define INTERFACE_DEFAULT 0

static const char dv_du_gname[] = "g_ninja";
static const char dv_du_longname[] = M_G_NAM;
static const char dv_du_manufacturer[] = M_ORG;
static const char dv_du_serial[] = M_G_SER;

extern struct usb_gadget_driver dv_du_driver;

struct g_ninja_usb_data_T {
	struct usb_gadget	*gadget;
	u8			config;
	s8			interface;
	struct usb_ep		*ep_rx;
	struct usb_ep		*ep_tx;
	// messages buffers
	struct usb_request	*req_c;		/* for control responses */
	struct usb_request	*req_rx;	/* read */
	struct usb_request	*req_tx;	/* write */
	
	// testing with static buffers
//	unsigned char 		buf_rx[ NINJA_BUFSZ_D];
//	unsigned char 		buf_tx[ NINJA_BUFSZ_D];
	// buffer for last echo request!
	unsigned char 		echoreq[ NINJA_BUFSZ_C + 1];
	unsigned		echoreq_l;
	
	NINJA_CMD_T tcmd_last;
//	// buffer for tcmd request!
//	unsigned char 		tcmd_req[ NINJA_BUFSZ_C + 1];
//	int			tcmd_req_l;
//	// buffer for tcmd reply!
//	unsigned char 		tcmd_rep[ NINJA_BUFSZ_C + 1];
//	int			tcmd_rep_l;
//
	int 			connected;
	
};

typedef struct g_ninja_usb_data_T g_ninja_usb_data_t;

extern g_ninja_usb_data_t g_ninja_usb_data;

#endif // NINJA_G_NINJA_H