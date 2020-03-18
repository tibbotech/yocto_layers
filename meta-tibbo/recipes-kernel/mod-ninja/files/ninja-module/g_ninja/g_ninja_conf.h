#ifndef G_NINJA_CONF_H
#define G_NINJA_CONF_H

#include "../ninja_syshdrs.h"
#include "../ninja_defs_i.h"
#include "../ninja_defs_e.h"

#include "../ninja.h"

#include "g_ninja.h"

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

int dvu_set_config( g_ninja_usb_data_t *_D, unsigned _cidx, struct usb_config_descriptor *_c);

void dvu_reset_interface( g_ninja_usb_data_t *_D);
int dvu_set_interface_i( g_ninja_usb_data_t *_D, struct usb_descriptor_header *_desc[]);
int dvu_set_interface( g_ninja_usb_data_t *_D, unsigned _i, struct usb_descriptor_header *_desc[]);


#endif // G_NINJA_CONF_H