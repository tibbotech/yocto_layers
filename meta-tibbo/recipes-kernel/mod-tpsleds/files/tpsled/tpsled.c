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

#include <unistd.h>

#define PFX "/proc/tpsleds"

int main( int argc, char *argv[]) {
 int fd, version, ret;
 char fname[ 256], buf[ 256];
 if ( argc < 2) {
   printf( "%s <lednum> [[<val>] <period>]\n", argv[ 0]);
   printf( "\t <val>: 0-off/1-on/2-blink\n");
   printf( "\t <period>: in ms\n");
   return( 1);  }
 memset( fname, 0, 256);
 sprintf( fname, "%s/%s", PFX, argv[ 1]);
 if ( ( fd = open( fname, O_RDWR)) < 0) return( 0);
 // read led state
 if ( argc == 2) {
   memset( buf, 0, 256);
   ret = read( fd, buf, 80);
   printf( "%s\n", buf);
 } else {
   ret = write( fd, argv[ 2], 1);
 }
 close( fd);
 if ( argc == 4) {
   memset( fname, 0, 256);
   sprintf( fname, "%s/%sb", PFX, argv[ 1]);
   if ( ( fd = open( fname, O_RDWR)) < 0) return( 0);
   ret = write( fd, argv[ 3], strlen( argv[ 3]));
   close( fd);
 }
 return( 0);  }
