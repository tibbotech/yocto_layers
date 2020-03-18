#include <stdio.h>
#include <stdlib.h>

#include "dv/Ci2c_smbus.h"

int main( int argc, char *argv[]) {
 if ( argc < 6) {
   printf( "%s busnum 0xaddr 0xreg action [opts]\n", argv[ 0]);
   printf( " 0xaddr is hex address of device on the bus\n");
   printf( " 0xreg  is hex address of device register\n");
   printf( " action is R|W\n");
   printf( "R [opts]: len - number of bytes to read (default 1)\n");
   printf( "\tR ex.: '%s 0 2d 00 R 2' - read 2 bytes from bus#0 dev 2d reg 00\n", argv[ 0]);
   printf( "W [opts]: buf - bytes to write (in hex)\n");
   printf( "\tW ex.: '%s 0 2d 00 W \"FF\"' - write 1 byte(FF) to bus#0 dev 2d reg 00\n", argv[ 0]);
   return( 1);  }
 const char *i2c_busn_s = argv[ 1];	// busnum
 const char *i2c_addr_s = argv[ 2];	// addr in hex
 const char *i2c_dreg_s = argv[ 3];	// regn in hex
 const char *i2c_actn_s = argv[ 4];	// R or W
 const char *i2c_opt0_s = ( argc > 5 ? argv[ 5] : "");
 int i = 0;
 unsigned int i2c_busn_i = atoi( i2c_busn_s);
 unsigned char i2c_addr_i = ( unsigned char)strtol( i2c_addr_s, NULL, 16);
 unsigned char i2c_dreg_i = ( unsigned char)strtol( i2c_dreg_s, NULL, 16);
 int r = 0, opt0_i;
 unsigned char buf[ 1024];
 uint8_t val1b;
 uint16_t val2b;

 Ci2c_smbus D;
 D.set_bus( "x");	// for test
 D.set_bus( "-s01");	// for test
 D.set_bus( "-tps");	// for test
 printf( "bus#%d\n", i2c_busn_i);
 if ( ( r = D.set_bus( i2c_busn_i)) < 0) {
   printf( "ERR opening bus:%s\n", strerror( -r));
   exit( 1);  }
 printf( "addr#%0X\n", i2c_addr_i);
 if ( ( r = D.set_slave( i2c_addr_i)) < 0) {
   printf( "ERR set bus addr:%s\n", strerror( -r));
   exit( 1);  }
 memset( buf, 0, 1024);
 switch ( i2c_actn_s[ 0]) {
   case 'W':
     opt0_i = strlen( i2c_opt0_s);
     if ( opt0_i < 2 || opt0_i % 2 != 0) {
       printf( "Wrong hex string length:%d\n", opt0_i);
       exit( 1);  }
     opt0_i /= 2;
     for ( i = 0; i < opt0_i; i++) {
       sscanf( ( const char *)&( i2c_opt0_s[ i*2]), "%2hhx", &( buf[ i]));
     }
     printf( "W%db:0x", opt0_i);
     for ( i = 0; i < opt0_i; i++) printf( "%02X", buf[ i]);
     printf( "\n");
     if ( opt0_i < 2) {
       val1b = buf[ 0];
       if ( ( r = D.W1b( i2c_addr_i, i2c_dreg_i, val1b)) < opt0_i) {
         printf( "ERR writ%db:%s (%d)\n", opt0_i, strerror( -r), -r);
         exit( 1);  }
       printf( "W%db: OK\n", r);
     }
     if ( opt0_i == 2) {
       val2b = buf[ 0] << 8 | buf[ 1];
       printf( "val2b:%04X\n", val2b);
       if ( ( r = D.W2b( i2c_addr_i, i2c_dreg_i, val2b)) < opt0_i) {
         printf( "ERR writ%db:%s (%d)\n", opt0_i, strerror( -r), -r);
         exit( 1);  }
       printf( "W%db: OK\n", r);
     }
     if ( opt0_i > 2) {
       if ( ( r = D.Wbb( i2c_addr_i, i2c_dreg_i, buf, opt0_i)) < 0) {
         printf( "ERR writ%db:%s (%d)\n", opt0_i, strerror( -r), -r);
         exit( 1);  }
       printf( "W%db: OK\n", r);
     }
     break;
   default:
     opt0_i = atoi( i2c_opt0_s);
     if ( opt0_i == 0) opt0_i = 1;
     if ( opt0_i < 2) {
       if ( ( r = D.R1b( i2c_addr_i, i2c_dreg_i, val1b)) < opt0_i) {
         printf( "ERR read%db:%s (%d)\n", opt0_i, strerror( -r), -r);
         exit( 1);  }
       printf( "R%db:0x%02X\n", r, val1b);
     }
     if ( opt0_i == 2) {
       if ( ( r = D.R2b( i2c_addr_i, i2c_dreg_i, val2b)) < opt0_i) {
         printf( "ERR read%db:%s (%d)\n", opt0_i, strerror( -r), -r);
         exit( 1);  }
       printf( "R%db:0x%04X\n", r, val2b);
     }
     if ( opt0_i > 2) {
       if ( ( r = D.Rbb( i2c_addr_i, i2c_dreg_i, buf, opt0_i)) < 0) {
         printf( "ERR read%db:%s (%d)\n", opt0_i, strerror( -r), -r);
         exit( 1);  }
       printf( "R%db:0x", r);
       for ( i = 0; i < r; i++) printf( "%02X", buf[ i]);
       printf( "\n");
     }
     break;
 }
 return( 0);  }
