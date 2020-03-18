
#include <stdio.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "dvc_gpio.h"

#define DVC_GPIO_DEV_PFX "/dev"

void __dvc_gpio_hdl_free( GPIO_LINE_t *_l) {
 if ( !( _l->hreq)) return;
 close( _l->hreq->fd); free( _l->hreq);  _l->hreq = NULL;  }

void dvc_gpio_chip_del( GPIO_CHIP_t *_c) {
 if ( _c->fd < 0) return;
 close( _c->fd);
 // delete lines list
 GPIO_LINE_t *clp = &( _c->lines), *cp;
 list2_t *lp, *sp;
 LIST2_EACH_SAFE( lp, sp, &( clp->list)) {
   cp = LIST2_E( lp, GPIO_LINE_t, list);
   list2_d( lp);
   dvc_gpio_evt_free( cp);
   __dvc_gpio_hdl_free( cp);
   free( cp);  }
 free( _c);
 return;  }

void dvc_gpio_free( GPIO_CHIP_t *_head) {
 GPIO_CHIP_t *cp;
 list2_t *lp, *sp;
 LIST2_EACH_SAFE( lp, sp, &( _head->list)) {
   cp = LIST2_E( lp, GPIO_CHIP_t, list);
   list2_d( lp);
   dvc_gpio_chip_del( cp);  }
 return;  }

GPIO_CHIP_t * __dvc_gpio_chip_new( const char *_name) {
 if ( !_name) return( NULL);
 GPIO_CHIP_t *ctp = NULL;
 ctp = ( GPIO_CHIP_t *)calloc( 1, sizeof( GPIO_CHIP_t));
 strcpy( ctp->name, _name);
 ctp->fd = -1;
 list2_i( &( ctp->list));
 list2_i( &( ctp->lines.list));
 return( ctp);  }

GPIO_LINE_t * __dvc_gpio_line_new( GPIO_CHIP_t *_c, const char *_name, int _off) {
 if ( !_name) return( NULL);
 GPIO_LINE_t *ctp = NULL;
 ctp = ( GPIO_LINE_t *)calloc( 1, sizeof( GPIO_LINE_t));
 strcpy( ctp->name, _name);
 ctp->off = _off;
 ctp->chip = _c;
 return( ctp);  }

GPIO_LINE_t *__gpio_line_find_off( GPIO_CHIP_t *_c, int _off) {
 list2_t *lp;
 GPIO_LINE_t *clp = &( _c->lines);
 GPIO_LINE_t *ctp = NULL;
 LIST2_EACH( lp, &( clp->list)) {
   ctp = LIST2_E( lp, GPIO_LINE_t, list);
   if ( ctp->off == _off) return( ctp);
 }
 return( NULL);  }

GPIO_CHIP_t *__dvc_gpio_find_chip( GPIO_CHIP_t *_c, __u16 _off) {
 list2_t *llp;
 GPIO_CHIP_t *cp;
 LIST2_EACH( llp, &( _c->list)) {
   cp = LIST2_E( llp, GPIO_CHIP_t, list);
   if ( _off < cp->off) continue;
   if ( _off > ( cp->off + cp->sz)) continue;
   return( cp);  }
 return( NULL);  }

GPIO_LINE_t *dvc_gpio_find_line( GPIO_CHIP_t *_c, __u16 _off) {
 GPIO_CHIP_t *cp = __dvc_gpio_find_chip( _c, _off);
 if ( !cp) return( NULL);
 return( __gpio_line_find_off( cp, _off - cp->off));  }


void dvc_gpio_chip_print( GPIO_CHIP_t *_c) {
 list2_t *lp;
 GPIO_LINE_t *clp = &( _c->lines);
 GPIO_LINE_t *ctp = NULL;
 printf( "chip:%s off:%03d sz:%02d fd:%d\n", _c->name, _c->off, _c->sz, _c->fd);
 LIST2_EACH( lp, &( clp->list)) {
   ctp = LIST2_E( lp, GPIO_LINE_t, list);
   dvc_gpio_line_print( ctp);
 }
 return;  }

void dvc_gpio_line_print( GPIO_LINE_t *_l) {
 printf( "\toff:%02d ", _l->off);
 if ( _l->flags & GPIOLINE_FLAG_KERNEL) printf( "K");
 else printf( " ");
 if ( _l->flags & GPIOLINE_FLAG_IS_OUT) printf( "O");
 else printf( " ");
 printf( " %p [%s]\n", _l, _l->cons);
 return;  }

void dvc_gpio_print( GPIO_CHIP_t *_head) {
 GPIO_CHIP_t *cp;
 list2_t *lp;
 LIST2_EACH( lp, &( _head->list)) {
   cp = LIST2_E( lp, GPIO_CHIP_t, list);
   dvc_gpio_chip_print( cp);
 }
 return;  }

//////

// wait for line event
int dvc_gpio_evt_prep( GPIO_CHIP_t *_c, __u16 _off, GPIO_LINE_t **_lp, DVC_GPIO_ERQ_t _evt, const char *_whoami) {
 *_lp = dvc_gpio_find_line( _c, _off);
 if ( !( *_lp)) return( -ENOENT);
// if ( dvc_gpio_line_is_busy( lp)) return( 0);
 if ( !( *_lp)->ereq) ( *_lp)->ereq = ( struct gpioevent_request *)calloc( 1, sizeof( struct gpioevent_request));
 if ( !( *_lp)->ereq) return( -ENOMEM);
 ( *_lp)->ereq->lineoffset = ( *_lp)->off;
 ( *_lp)->ereq->handleflags = GPIOHANDLE_REQUEST_INPUT;
 // fix: this is the parameter
 ( *_lp)->ereq->eventflags = _evt;
 // fix: this is the parameter
 if ( _whoami) strcpy( ( *_lp)->ereq->consumer_label, _whoami);
 int ret = ioctl( ( *_lp)->chip->fd, GPIO_GET_LINEEVENT_IOCTL, ( *_lp)->ereq);
 if ( ret < 0) return( -errno);
 int flags = fcntl( ( *_lp)->ereq->fd, F_GETFL, 0);
 ret = fcntl( ( *_lp)->ereq->fd, F_SETFL, flags | O_NONBLOCK);
 return( 0);  }

int dvc_gpio_evt_read( GPIO_LINE_t *_lp) {
 if ( !_lp->ereq) return( 0);
 int ret = read( _lp->ereq->fd, &( _lp->evt), sizeof( _lp->evt));
 if ( ret < 0 && errno == EAGAIN) return( 0);
 if ( ret < 0) return( -errno);
 return( 1);  }

// get/set

int dvc_gpio_Lreq( GPIO_LINE_t *_l, const char *_cons, __u8 _in, __u8 _defv) {
 if ( _l->hreq && _in == ( _l->hreq->flags & GPIOHANDLE_REQUEST_INPUT)) return( 0);
 if ( !_l->hreq) _l->hreq = ( struct gpiohandle_request *)calloc( 1, sizeof( struct gpiohandle_request));
 if ( !( _l->hreq)) return( -ENOMEM);
 __u32 flags0 = _l->hreq->flags;
 __u32 flags1 = flags0 & ~( GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_OUTPUT);
 flags1 |= ( _in ? GPIOHANDLE_REQUEST_INPUT : GPIOHANDLE_REQUEST_OUTPUT);
//printf( "flags0:%d flags1:%d fd:%d\n", flags0, flags1, _l->hreq->fd);
 if ( flags0 != flags1 && _l->hreq->fd > 0) close( _l->hreq->fd);
 _l->hreq->lineoffsets[ 0] = _l->off;
 _l->hreq->flags = flags1;
 _l->hreq->lines = 1;
 if ( !_in) _l->hreq->default_values[ 0] = _defv;
 if ( _cons) strcpy( _l->hreq->consumer_label, _cons);
 int ret = ioctl( _l->chip->fd, GPIO_GET_LINEHANDLE_IOCTL, _l->hreq);
 if ( ret < 0) return( -errno);
 return( 0);  }

int __dvc_gpio_lh( GPIO_LINE_t *_l, const char *_cons, __u8 _in, __u8 _defv) {
 int fd = -1;
 // it may use event interface handler
 if ( _in && _l->ereq) fd = _l->ereq->fd;
 // or line handle
 if ( fd <= 0 && _l->hreq) fd = _l->hreq->fd;
 // nothing. add line handle
 if ( fd <= 0) {
   if ( dvc_gpio_Lreq( _l, _cons, _in, _defv) == 0) fd = _l->hreq->fd;
 }
 if ( fd < 0) return( -errno);
 return( fd);  }

// _cons is the consumer
int dvc_gpio_Vget( GPIO_LINE_t *_l, const char *_cons) {
 int fd = __dvc_gpio_lh( _l, _cons, 1, 0);
 if ( fd < 0) return( fd);
 static struct gpiohandle_data data;
 int ret = ioctl( fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
 if ( ret < 0) return( -errno);
 return( data.values[ 0]);  }

int dvc_gpio_Vset( GPIO_LINE_t *_l, __u8 _val, const char *_cons) {
 int fd = __dvc_gpio_lh( _l, _cons, 0, _val);
 if ( fd < 0) return( fd);
 static struct gpiohandle_data data;
 data.values[ 0] = _val;
 int ret = ioctl( fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
 if ( ret < 0) return( -errno);
 return( 0);  }

// scan functions

int __gpio_scan_p_l( int _i, int _chfd, struct gpioline_info *_l) {
 int ret = 0;
 memset( _l, 0, sizeof( struct gpioline_info));
 _l->line_offset = _i;
 if ( ( ret = ioctl( _chfd, GPIO_GET_LINEINFO_IOCTL, _l)) < 0) return( -errno);
 return( 0);  }

int dvc_gpio_scan_chip( GPIO_CHIP_t *_c, const char *_p) {
 char devn[ PATH_MAX];
 int ret;
 struct gpiochip_info cinfo;
 struct gpioline_info linfo;
 GPIO_LINE_t *clp;
// printf( "%s( %s):%d\n", __FUNCTION__, _p, __LINE__);
 memset( devn, 0, PATH_MAX);
 sprintf( devn, "%s/%s", DVC_GPIO_DEV_PFX, _p);
// printf( "-> %s\n", devn);
 if ( ( _c->fd < 0) && ( _c->fd = open( devn, 0)) < 0) return( -errno);
 if ( ( ret = ioctl( _c->fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo)) < 0) {
   close( _c->fd);  _c->fd = -1;   return( -errno);  };
 _c->sz = cinfo.lines;
//printf( "GPIO chip: %s, \"%s\", %u GPIO lines\n", cinfo.name, cinfo.label, cinfo.lines);
 // ???
 // strcpy( _c->name, cinfo.name);
 for ( int i = 0; i < _c->sz; i++) {
   if ( ( ret = __gpio_scan_p_l( i, _c->fd, &linfo)) < 0) continue;
   if ( !( clp = __gpio_line_find_off( _c, linfo.line_offset))) {
     clp = __dvc_gpio_line_new( _c, linfo.name, linfo.line_offset);
     list2_a_tail( &( clp->list), &( _c->lines.list));
   }
   if ( !clp) return( -ENOMEM);
   clp->flags = linfo.flags;
   if ( linfo.consumer) strcpy( clp->cons, linfo.consumer);
   // printf( " %d+%d=%d\n", _c->off, linfo.line_offset, _c->off + linfo.line_offset);
 }
 return( 0);  }

int dvc_gpio_scan( GPIO_CHIP_t *_head) {
 DIR *dp = NULL;
 // printf( "%s():%d\n", __FUNCTION__, __LINE__);
 if ( !( dp = opendir( DVC_GPIO_DEV_PFX))) return( -errno);
 GPIO_CHIP_t *ctp;
 const struct dirent *dep;
 while ( ( dep = readdir( dp))) {
   // each /dev/gpiochip*
   if ( memcmp( dep->d_name, DVC_GPIO_CHIP_PFX, sizeof( DVC_GPIO_CHIP_PFX) - 1) != 0) continue;
   if ( !( ctp = dvc_gpio_chip_find_name( _head, dep->d_name))) {
     ctp = __dvc_gpio_chip_new( dep->d_name);
     list2_a_tail( &( ctp->list), &( _head->list));
   }
   if ( !ctp) return( -ENOMEM);
   if ( dvc_gpio_scan_chip( ctp, dep->d_name) >= 0) continue;
   // unsuccessful scan
   list2_d( &( ctp->list));
   dvc_gpio_chip_del( ctp);
 }
 closedir( dp);
 return( 0);  }
