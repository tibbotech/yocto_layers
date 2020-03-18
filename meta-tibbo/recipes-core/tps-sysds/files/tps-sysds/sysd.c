#include <stdio.h>
#include <stdlib.h>
#include "SysDs.h"

int main( int argc, char *argv[]) {
 const char *path;
 int r;
 
 if ( argc < 3) {
   printf( "Usage: %s <action> <unit>\n", argv[ 0]);
   printf( "where\n");
   printf( "\t<action>:\tStartUnit|StopUnit\n");
   printf( "\t<unit>:\tunit name, ex.: cups.service\n");
   exit( 1);  }

 SysDs X;
 r = X.open_S();
 if ( r < 0) {
   printf( "xx: %s\n", strerror( r));
   return( 1);  }
 
 r = X.call_StartStop( argv[ 1], argv[ 2]);
 if ( r < 0) {
   printf( "y0: %s\n", strerror( r));
   printf( "y1: %s\n", X.get_errM());
   return( 1);  }

 // read string reply
 r = X.Rs( &path);
 printf( "Queued service job as %s.\n", path);

 return( 0);  }
