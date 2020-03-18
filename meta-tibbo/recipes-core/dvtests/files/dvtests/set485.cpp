/*!
    \example tty485.cpp
*/

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
#include <signal.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include "Cuart.h"

#define BUFSZ 10*1024

int fd = -1;

// http://retis.sssup.it/~scordino/code/serial-rs485.txt

void print_conf( struct serial_rs485 *_c) {
 fprintf( stdout, "RS485 is %s\n", ( ( _c->flags & SER_RS485_ENABLED) ? "enabled" : "disabled"));
 fprintf( stdout, "RS422 is %s\n", ( ( _c->flags & SER_RS422_ENABLED) ? "enabled" : "disabled"));
 fprintf( stdout, "RS485 flags 0x%X\n", _c->flags);
 fprintf( stdout, "RS485 onsend: %s\n", ( _c->flags & SER_RS485_RTS_ON_SEND) ? "enabled" : "disabled");
 fprintf( stdout, "RS485 after send: %s\n", ( _c->flags & SER_RS485_RTS_AFTER_SEND) ? "enabled" : "disabled");
 fprintf( stdout, "RS485 delays: %d %d\n", _c->delay_rts_before_send, _c->delay_rts_after_send);
 return;  }

int main( int argc, char *argv[]) {
 int err, i = 0, Ir = 0, Sr = 0, Gret = 1;
 unsigned char Ib[ BUFSZ + 1], Sb[ BUFSZ + 1];

 for ( int x = 0; x < 23; x++) printf( "%d", x/8);
 printf( "\n");

 if ( argc < 2) {
   fprintf( stderr, "Usage: %s <serialport> [0,1,2]\n", argv[ 0]);
   fprintf( stderr, "\t0=232, 1=422, 2=485\n");
   return( 1);  }

 // stdin/stdout non-block
 if ( fcntl( STDIN_FILENO,  F_SETFL, O_NONBLOCK | fcntl( STDIN_FILENO,  F_GETFL, 0)) == -1) {
   fprintf( stderr, "Err: Can't make STDin non-block\n");
   return( 1);  }
 if ( fcntl( STDOUT_FILENO, F_SETFL, O_NONBLOCK | fcntl( STDOUT_FILENO, F_GETFL, 0)) == -1) {
   fprintf( stderr, "Err: Can't make STDout non-block\n");
   return( 1);  }

 if ( fd < 0 && ( fd = open( argv[ 1], O_NONBLOCK | O_RDWR)) == -1) {
   fprintf( stderr, "Err: Can't open '%s': %d(%s)\n", argv[ 1], errno, strerror( errno));
   return( 1);  }

 struct serial_rs485 rs485conf;
 if ( Cuart::mode_get( fd, rs485conf) < 0) {
   fprintf( stderr, "Err: Can't get rs485 conf for '%s': %d(%s)\n", argv[ 1], errno, strerror( errno));
   return( 1);  }
 print_conf( &rs485conf);
 int mod = 0;
 if ( argc > 2) mod = atoi( argv[ 2]);
 CuartMode_t umode = CuartMode232;
 switch ( mod) {
   case 0:  break;
   case 1:  umode =  CuartMode422;  break;
   case 2:  umode =  CuartMode485;  break;
   default:  break;
 }
 Cuart::mode_def( rs485conf, umode);
 if ( argc > 2 && Cuart::mode_set( fd, rs485conf) < 0) {
   fprintf( stderr, "Err: Can't set rs485 conf for '%s': %d(%s)\n", argv[ 1], errno, strerror( errno));
   return( 1);  }
 if ( argc > 2 && Cuart::mode_get( fd, rs485conf) < 0) {
   fprintf( stderr, "Err: Can't get rs485 conf for '%s': %d(%s)\n", argv[ 1], errno, strerror( errno));
   return( 1);  }
 print_conf( &rs485conf);

 struct termios my_termios;
 Cuart::termios_def( my_termios);
 Cuart::termios_set( fd, my_termios);
 my_termios.c_cc[ VMIN] = 1;
 my_termios.c_cc[ VTIME] = 5;

 return( 0);  }
