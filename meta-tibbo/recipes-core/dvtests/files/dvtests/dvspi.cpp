#include <stdio.h>
#include <stdlib.h>

#include "dv/Cspi.h"

#define MAX_BUF 256

void dump_hex( char const *_pfx, uint8_t const *_b, size_t _l) {
 if ( _pfx) printf( "%s:", _pfx);
 for ( int i = 0; i < _l; i++) printf( "%02X", _b[ i]);
 printf( "\n");
 return;  }

int main( int argc, char *argv[]) {
 Cspi x;
// x.set_dev( "-s05", 0);
 if ( argc < 2) {
   printf( "Usage: %s <socket>\n", argv[ 0]);
   printf( "\twhere <socket> is -s05 for ex.\n");
   exit( 1);  }
 x.set_dev( argv[ 1], 0);
 x.settings_get();
 printf( "Bits\t:%d\n", x.cur_bits());
 printf( "Speed\t:%d\n", x.cur_speed());
 printf( "Delay\t:%d\n", x.cur_delay());
 printf( "Mode\t:%02x\n", x.cur_mode());
// x.set_speed( 500000);
// x.set_mode( x.cur_mode() & ~SPI_CS_HIGH);
// x.set_mode( SPI_MODE_3);
 x.set_mode( SPI_MODE_0);
 x.set_bits( 8);
 x.settings_set();
 x.settings_get();
 printf( "Bits\t:%d\n", x.cur_bits());
 printf( "Speed\t:%d\n", x.cur_speed());
 printf( "Delay\t:%d\n", x.cur_delay());
 printf( "Mode\t:%02x\n", x.cur_mode());
 int ret, len;
 uint8_t w[ MAX_BUF], r[ MAX_BUF];

 memset( w, 0, MAX_BUF);
 memset( r, 0, MAX_BUF);
 w[ 0] = 0x11;
 w[ 1] = 0x00;
 w[ 2] = 0x00;
 w[ 3] = 0x00;
// w[ 4] = 0x00;
// w[ 5] = 0x00;
// w[ 6] = 0x00;
// w[ 7] = 0x00;
 len = 8;
 if ( argc == 3) len = atoi( argv[ 2]);
 if ( argc == 4) { 
   w[ 0] = atoi( argv[ 2]);
   len = atoi( argv[ 3]);
 }
 dump_hex( "W", w, len);
 ret = x.WR( w, r, len);

 printf( "ret:%d\n", ret);
 if ( ret < 0) printf( "(%s)\n", strerror( -ret));
 if ( ret >= 0) dump_hex( "R", r, ret);
 uint8_t temp_msb = r[ 1];
 uint8_t temp_lsb = r[ 2] >> 6;
 int8_t nint;
 if ( ( temp_msb & 0x80) != 0) nint = temp_msb | ~( ( 1 << 8) - 1);
 else nint = temp_msb;
 float rv = 0.25 * temp_lsb + nint;
 printf( "temp:%f\n", rv);

 memset( w, 0, MAX_BUF);
 memset( r, 0, MAX_BUF);
 w[ 0] = 0x00;
 w[ 1] = 0x00;
 w[ 2] = 0x00;
 w[ 3] = 0x00;
 w[ 4] = 0x00;
 w[ 5] = 0x00;
 w[ 6] = 0x00;
 w[ 7] = 0x00;
 w[ 8] = 0x00;
 len = 9;
 dump_hex( "W", w, len);
 ret = x.WR( w, r, len);
 printf( "ret:%d\n", ret);
 if ( ret < 0) printf( "(%s)\n", strerror( -ret));
 if ( ret >= 0) dump_hex( "R", r, ret);

 memset( w, 0, MAX_BUF);
 memset( r, 0, MAX_BUF);
 for ( int i = 0; i < 10000; i++) {
   ret = x.WR( w, r, 256);
 }

 return( 0);  }
