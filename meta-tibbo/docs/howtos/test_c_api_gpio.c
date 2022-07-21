#include <linux/gpio.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DEV_NAME "/dev/gpiochip0"
#define RATE 10000

int fd, ret, pinN;
struct gpiochip_info info;
struct gpioline_info line_info;

struct timespec ts0, ts1;
struct timeval tv0;

void dv_sleep( unsigned int _ssec, unsigned long _nsec) {
 ts0.tv_sec = _ssec;
 ts0.tv_nsec = _nsec;
 tv0.tv_sec = _ssec;
 tv0.tv_usec = _nsec/1000;
// if ( _ssec > 0 || _nsec > 100000) nanosleep( &ts0, NULL);
// else select( 0, NULL, NULL, NULL, &tv0);
 select( 0, NULL, NULL, NULL, &tv0);
 return;  }

int main( int argc, char *argv[]) {
 if ( argc < 2) {
   printf( "Usage: %s <pin>\n", argv[ 0]);
   return( 0);  }
 if ( ( fd = open(DEV_NAME, O_RDONLY)) < 0) {
    printf("Unabled to open %s: %s", DEV_NAME, strerror(errno));
    return( -1);  }
 pinN = atoi( argv[ 1]);
 printf( "Trying pin#%d\n", pinN);
 // get line info
 line_info.line_offset = pinN;
 if ( ( ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &line_info)) < 0) {
   printf( "Unable to get line info: %s", strerror(errno));
   return( -1);  }
 printf("name: %s, consumer: %s. Flags:\t[%s]\t[%s]\t[%s]\t[%s]\t[%s]\n",
               line_info.name,
               line_info.consumer,
               (line_info.flags & GPIOLINE_FLAG_IS_OUT) ? "OUTPUT" : "INPUT",
               (line_info.flags & GPIOLINE_FLAG_ACTIVE_LOW) ? "ACTIVE_LOW" : "ACTIVE_HIGHT",
               (line_info.flags & GPIOLINE_FLAG_OPEN_DRAIN) ? "OPEN_DRAIN" : "...",
               (line_info.flags & GPIOLINE_FLAG_OPEN_SOURCE) ? "OPENSOURCE" : "...",
               (line_info.flags & GPIOLINE_FLAG_KERNEL) ? "KERNEL" : "");
 // now set OUTPUT
 struct gpiohandle_request rq;
 rq.lineoffsets[0] = pinN;
 rq.lines = 1;
 rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
 if ( ( ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq)) < 0) {
   printf("Unable to line handle from ioctl : %s", strerror(errno));
   return( -1);  }
 // switch 0/1 1000 times
 tv0.tv_sec = 0;
 struct gpiohandle_data data;
 memset( &ts0, 0, sizeof( ts0));
 clock_gettime( CLOCK_REALTIME, &ts1);
 uint64_t k = 0;
 for ( int i = 0; i < RATE; i++) {
    while ( abs( ( ts1.tv_nsec - ts0.tv_nsec)) < 1000000000/RATE) {
      clock_gettime( CLOCK_REALTIME, &ts1);
      k++;   }
    ts0 = ts1;
    data.values[ 0] = i % 2;
    ret = ioctl( rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    if ( ret < 0) {
      printf("Unable to set line value using ioctl : %s", strerror(errno));
      return( -1);  }
    if ( ( i % ( RATE/10)) == 0) printf( "%d skipped %lld cycles\n", i, k);
 }
 close(fd);
 return( 0);  }
