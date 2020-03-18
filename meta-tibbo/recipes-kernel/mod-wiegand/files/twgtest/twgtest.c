#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include <unistd.h>

#define PFX "/sys/devices/platform/twg"
#define BLEN 512

struct wg_data {
 uint16_t len;
 char buff[ BLEN];
};

int main( int argc, char *argv[]) {
 int fd, version, ret, i;
 char fname[ 256], buf[ 256];
 struct wg_data d;
 if ( argc < 2) {
   printf( "%s sXX\n", argv[ 0]);
   printf( "\t Wiegand buffer reader\n");
   printf( "\t sXX: is socket name, reads "PFX"sXX/data\n");
   return( 1);  }
 memset( fname, 0, 256);
 sprintf( fname, "%s-%s/data", PFX, argv[ 1]);
 if ( ( fd = open( fname, O_RDONLY)) < 0) return( 0);

 memset( &d, 0, sizeof( d));
 ret = read( fd, &d, BLEN+2);
 if ( ret <= 0) {
   printf( "Error: %s(%d)\n", strerror( errno), errno);
   exit( 1);  }
 printf( "bits: %03d, data:\n", d.len);
 for ( i = 0; i < BLEN; i++) printf( "%02X", d.buff[ i]);
 printf( "\n");
 close( fd);
 return( 0);  }
