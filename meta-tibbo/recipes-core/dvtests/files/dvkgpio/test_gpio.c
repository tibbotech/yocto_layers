
#define DVC_GPIO_DEBUG 1

#include "dvc_gpio.h"

#include <stdio.h>

static GPIO_CHIP_t gpio_chips;

int main( int argc, char *argv[]) {
 int stat;
 // mandatory: lists initialization
 list2_i( &( gpio_chips.list));
 list2_i( &( gpio_chips.lines.list));
 // get information about chips and lines (may be done periodically)
 dvc_gpio_scan( &gpio_chips);

 GPIO_LINE_t *linep;
 
//return( 0);
 GPIO_CHIP_t *cp;
 // print GPIO information
#ifdef DVC_GPIO_DEBUG
 dvc_gpio_print( &( gpio_chips));
#endif
 // mandatory: set gpio groups offsets (for correct line search)
 cp = dvc_gpio_chip_find_name( &gpio_chips, "gpiochip0");
 if ( cp) cp->off = 32*0;
 cp = dvc_gpio_chip_find_name( &gpio_chips, "gpiochip1");
 if ( cp) cp->off = 32*1;
 cp = dvc_gpio_chip_find_name( &gpio_chips, "gpiochip2");
 if ( cp) cp->off = 32*2;
 cp = dvc_gpio_chip_find_name( &gpio_chips, "gpiochip3");
 if ( cp) cp->off = 32*3;
 cp = dvc_gpio_chip_find_name( &gpio_chips, "gpiochip4");
 if ( cp) cp->off = 510;
 cp = dvc_gpio_chip_find_name( &gpio_chips, "gpiochip5");
 if ( cp) cp->off = 494;
// test
 printf( "before...\n");
 // on/off line S08A 1 million times
 linep = dvc_gpio_find_line( &( gpio_chips), 78);
 int i = 0, err = 0;
 printf( "stat: %d %s\n", stat, strerror( -stat));
 // line has to be in OUT state
 printf( "line 78 is %s : %s(%d)\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"), strerror( err), err);
sleep( 3);
 // OUT
 err = dvc_gpio_Lreq( linep, "dv-O-78", 0, 0);
 printf( "line 78 is %s : %s(%d)\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"), strerror( err), err);
sleep( 3);
printf( "set 1\n");
 err = dvc_gpio_Vset( linep, 1, "dv-1-78");
sleep( 3);
printf( "set 0\n");
 err = dvc_gpio_Vset( linep, 0, "dvt-2-78");
sleep( 3);
 // IN
 err = dvc_gpio_Lreq( linep, "dv-I-78", 1, 0);
 printf( "line 78 is %s : %s(%d)\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"), strerror( err), err);
sleep( 3);
 err = dvc_gpio_Lreq( linep, "dv-o-78", 0, 0);
 printf( "line 78 is %s : %s(%d)\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"), strerror( err), err);
sleep( 3);
printf( "set 1\n");
 err = dvc_gpio_Vset( linep, 1, "dv-1-78");
sleep( 3);
printf( "set 0\n");
 err = dvc_gpio_Vset( linep, 0, "dv-2-78");
sleep( 3);
// test /
return( 0);


 // test 99 out
// stat = dvc_gpio_Vget( linep, "dvt-x-99-2");
// printf( "stat: %d %s\n", stat, strerror( -stat));
 GPIO_LINE_t *l99;
 l99 = dvc_gpio_find_line( &( gpio_chips), 502);
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( l99) ? "OUT" : "IN"));
 stat = dvc_gpio_Vset( l99, 0, "dvt-O-99");
 printf( "stat: %d %s\n", stat, strerror( -stat));
 sleep( 2);
 stat = dvc_gpio_Vset( l99, 1, "dvt-O-99");
 printf( "stat: %d %s\n", stat, strerror( -stat));
 sleep( 2);
 stat = dvc_gpio_Vset( l99, 0, "dvt-O-99");
 printf( "stat: %d %s\n", stat, strerror( -stat));


return( 0);
 // print GPIO information
#ifdef DVC_GPIO_DEBUG
 dvc_gpio_print( &( gpio_chips));
#endif

 // basic operation: get the line #99 (MD)
 linep = dvc_gpio_find_line( &( gpio_chips), 99);
#ifdef DVC_GPIO_DEBUG
 printf( "LINE 99 found:\n");
 if ( linep) dvc_gpio_line_print( linep);
#endif

 // get another line #12 (usually bisy with some driver)
 linep = dvc_gpio_find_line( &( gpio_chips), 13);
 stat = dvc_gpio_Vget( linep, "dvt-13");
 printf( "stat: %d %s\n", stat, strerror( -stat));
 if ( stat == -EBUSY) printf( "Taken by %s\n", linep->cons);
#ifdef DVC_GPIO_DEBUG
 printf( "LINE 12 found:\n");
 if ( linep) dvc_gpio_line_print( linep);
#endif

 // event test : prepare MD for event reading
 int x0 = dvc_gpio_evt_prep( &( gpio_chips), 99, &linep, DVC_GPIO_ERQ_FALL, "DvMD");
 if ( x0 <= 0) printf( "x0:%d %s\n", x0, strerror( -x0));
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"));


 // let user press the button several times, while you're busy with other task
 sleep( 2);
 // read collected events up to 1000 cycles
 int y0;
// int i = 0;
// while ( i++ < 1000) {
//   y0 = dvc_gpio_evt_read( linep);
//   if ( y0 == 0) continue;	// or break. it means you read all events
//   if ( y0  < 0) {  printf( "error:%s\n", strerror( -y0));  break;  }
//   printf( "evt:%s at%lld\n", ( linep->evt.id & DVC_GPIO_EVT_RISE) ? "rise" : "fall", linep->evt.timestamp);
// }
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"));
 // read line value using EVT fd (after event loop)
 stat = dvc_gpio_Vget( linep, "dvt-99-evt");
 printf( "stat: %d %s\n", stat, strerror( -stat));
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"));
 // free EVT resources and read with line handler (several times, for test)
 dvc_gpio_evt_free( linep);
 stat = dvc_gpio_Vget( linep, "dvt-x-99");
 printf( "stat: %d %s\n", stat, strerror( -stat));
 stat = dvc_gpio_Vget( linep, "dvt-x-99-2");
 printf( "stat: %d %s\n", stat, strerror( -stat));
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"));

// // test find line
// i = 0;
// while ( i++ < 100000) dvc_gpio_find_line( &( gpio_chips), 99);

 printf( "before...\n");
 // on/off line S08A 1 million times
 linep = dvc_gpio_find_line( &( gpio_chips), 78);
 i = 0;
// while ( i++ < 1000000) stat = dvc_gpio_Vset( linep, i % 2, "dvt-O-99");
 printf( "stat: %d %s\n", stat, strerror( -stat));
 // line has to be in OUT state
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"));
 // back to IN
 dvc_gpio_Lreq( linep, "dv-I-99", 1, 0);
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"));
 // back to OUT
 dvc_gpio_Lreq( linep, "dv-I-99", 0, 0);
 printf( "line 99 is %s\n", ( dv_gpio_Lisout( linep) ? "OUT" : "IN"));

 // just to add complexity: rescan GPIOs
 dvc_gpio_scan( &gpio_chips);
#ifdef DVC_GPIO_DEBUG
 // print new info
// dvc_gpio_print( &( gpio_chips));
#endif

 // mandatory: cleanup (destructor)
 dvc_gpio_free( &( gpio_chips));
 return( 0);  }
