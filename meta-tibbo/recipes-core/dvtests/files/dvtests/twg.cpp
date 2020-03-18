/*!
    \example twg.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <arpa/inet.h>

#include "CWg.h"

int main( int argc, char *argv[]) {
 int ret, i = 0;
 wg_data_t data;

 CWg twg;
 if ( argc < 2) {
   printf( "Usage: %s <sock>\n", argv[ 0]);
   exit( 1);  }
 if ( ( ret = twg.init( argv[ 1])) < 0) {
   printf( "Error opening Wiegand socket %s: %s(%d)\n", argv[ 1], strerror( errno), errno);
   exit( 1);  }
 // if Wiegand is opened successfully - it works then!
 i = twg.mode_get();
 printf( "Mode: %d\n", i);
 memset( &data, 0, sizeof( wg_data_t));
 if ( ( ret = twg.R( data)) < 0) {
   printf( "Error reading WG data: %s(%d)\n", strerror( errno), errno);
 }
 // return code <=0 - Wiegand have no data. (non-blocking call here!)
 if ( ret > 0) {
   printf( "Bits:%d\n", data.blen);
   printf( "Buff:");
   for ( i = 0; i < data.blen/8; i++) printf( "%02X", data.buff[ i]);
   if ( data.blen % 8 != 0) printf( "%02X", data.buff[ data.blen/8]);
   printf( "\n");
 }
 printf( "set Out0=0 for 1 second...\n");
 twg.out0_set( 0);
 sleep( 1);
 printf( "set Out0=1 for 1 second...\n");
 twg.out0_set( 1);
 sleep( 1);
 return( 0);  }
