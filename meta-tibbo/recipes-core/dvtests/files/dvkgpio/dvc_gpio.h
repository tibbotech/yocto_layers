#ifndef DVC_GPIO_H
#define DVC_GPIO_H

#include <sys/ioctl.h>
#include <linux/gpio.h>

#include <sys/types.h>
#include <dirent.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "dvc_list2.h"

#define DVC_GPIO_CHIP_PFX "gpiochip"

typedef struct GPIO_CHIP GPIO_CHIP_t;

typedef struct {
 char name[ 32];
 __u16 off;
 __u32 flags;
 char cons[ 32];
 struct gpioevent_request  *ereq;
 struct gpiohandle_request *hreq;
 struct gpioevent_data evt;
 GPIO_CHIP_t *chip;
 list2_t list;
} GPIO_LINE_t;

typedef struct GPIO_CHIP {
 char name[ 30];
 int fd;		// file handle
 __u16 off;		// offset for pins handled, i.e. 0, 1, 2... 500
 __u16 sz;		// number of pins
 GPIO_LINE_t lines;	// lines list
 list2_t list;
} GPIO_CHIP_t;

void dvc_gpio_chip_del( GPIO_CHIP_t *_c);

// ret: 0 is OK
int dvc_gpio_scan( GPIO_CHIP_t *_head);
int dvc_gpio_scan_chip( GPIO_CHIP_t *_c, const char *_p);

void dvc_gpio_free( GPIO_CHIP_t *_head);

static GPIO_CHIP_t *dvc_gpio_chip_find_name( GPIO_CHIP_t *_head, const char *_name) {
 list2_t *lp;
 GPIO_CHIP_t *ctp;
 LIST2_EACH( lp, &( _head->list)) {
   ctp = LIST2_E( lp, GPIO_CHIP_t, list);
   if ( strcasecmp( ctp->name, _name) == 0) return( ctp);
 }
 return( NULL);  }

GPIO_LINE_t *dvc_gpio_find_line( GPIO_CHIP_t *_head, __u16 _off);

static inline int dvc_gpio_is_busy( GPIO_LINE_t *_l) {  return( ( _l->flags & GPIOLINE_FLAG_KERNEL));  }

// debug functions

void dvc_gpio_print( GPIO_CHIP_t *_head);
void dvc_gpio_line_print( GPIO_LINE_t *_l);
void dvc_gpio_chip_print( GPIO_CHIP_t *_c);

// event-related calls

typedef enum {
 DVC_GPIO_EVT_RISE = 0x01,
 DVC_GPIO_EVT_FALL = 0x02,
} DVC_GPIO_EVT_t;

typedef enum {
 DVC_GPIO_ERQ_RISE = GPIOEVENT_REQUEST_RISING_EDGE,
 DVC_GPIO_ERQ_FALL = GPIOEVENT_REQUEST_FALLING_EDGE,
 DVC_GPIO_ERQ_BOTH = GPIOEVENT_REQUEST_BOTH_EDGES,
} DVC_GPIO_ERQ_t;

// ret: 0 is OK, < 0 - errno
int dvc_gpio_evt_prep( GPIO_CHIP_t *_c, __u16 _off, GPIO_LINE_t **_lp, DVC_GPIO_ERQ_t _evt, const char *_whoami);
// ret: <0 - errno, 0 - nothing, 1 - has event
int dvc_gpio_evt_read( GPIO_LINE_t *_l);
static inline void dvc_gpio_evt_free( GPIO_LINE_t *_l) {
 if ( !( _l->ereq)) return;
 close( _l->ereq->fd);  free( _l->ereq);  _l->ereq = NULL;  }

// line set/get calls

// _wai : who am I? consumer name. Meanful only for the first call

int dvc_gpio_Vget( GPIO_LINE_t *_l, const char *_wai);
// ret: 0 is OK
int dvc_gpio_Vset( GPIO_LINE_t *_l, __u8 _val, const char *_wai);
// set in/out. ret: 0 is OK,
// _defv is the default value
int dvc_gpio_Lreq( GPIO_LINE_t *_l, const char *_wai, __u8 _in, __u8 _defv);
static inline int dv_gpio_Lisout( GPIO_LINE_t *_l) {
 if ( _l->hreq) return( ( _l->hreq->flags & GPIOHANDLE_REQUEST_OUTPUT));
 return( ( _l->flags & GPIOLINE_FLAG_IS_OUT));  }

#endif // DVC_GPIO_H
