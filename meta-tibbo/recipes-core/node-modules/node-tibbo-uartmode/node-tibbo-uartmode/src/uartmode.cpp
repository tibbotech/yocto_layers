#include "uartmode.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// see 485-related information:
// http://retis.sssup.it/~scordino/code/serial-rs485.txt

void tibbo_uartmode::mode_def( struct serial_rs485 *_c, CuartMode_t _m) {
 switch ( _m) {
   case CuartMode232:
     _c->flags &= ~( SER_RS485_ENABLED);
     _c->flags &= ~( SER_RS422_ENABLED);
     break;
   case CuartMode422: // 422
     _c->flags &= ~( SER_RS485_ENABLED);
     _c->flags |= SER_RS422_ENABLED;
     break;
   case CuartMode485: // 485
     _c->flags |= SER_RS485_ENABLED;
     _c->flags &= ~( SER_RS422_ENABLED);
     _c->flags |= SER_RS485_RTS_ON_SEND;
     // _c.flags &= ~( SER_RS485_RTS_ON_SEND);
     // _c.flags |= SER_RS485_RTS_AFTER_SEND;
     _c->flags &= ~( SER_RS485_RTS_AFTER_SEND);
     break;
   default:
     break;
 }
 return;  }

CuartMode_t tibbo_uartmode::G( const char *_port) {
 struct serial_rs485 c;
 int fd = open( _port, O_RDWR), ret = 0;
 if ( fd < 0) return( CuartModeNaN);
 ret = ioctl( fd, TIOCGRS485, &c);
 close( fd);
 if ( ret < 0) return( CuartModeNaN);
 if ( c.flags & SER_RS485_ENABLED) return( CuartMode485);
 if ( c.flags & SER_RS422_ENABLED) return( CuartMode422);
 return( CuartMode232);  }

int tibbo_uartmode::S( const char *_port, CuartMode_t _m) {
 int fd = open( _port, O_RDWR), ret = 0;
 struct serial_rs485 c;
 if ( fd < 0) return( fd);
 ret = ioctl( fd, TIOCGRS485, &c);
 tibbo_uartmode::mode_def( &c, _m);
 ret = ioctl( fd, TIOCSRS485, &c);
 close( fd);
 return( ret);  }
