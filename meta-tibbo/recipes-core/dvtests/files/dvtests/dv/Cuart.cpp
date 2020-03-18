#include "Cuart.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define DEV_SPI_PFX "/dev/spidev"
//#define SYS_SPI_PFX "/sys/devices/platform/spi"

void Cuart::termios_def( struct termios &_t) {
 _t.c_iflag &= ~( IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
 _t.c_oflag &= ~( OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);
 _t.c_lflag &= ~( ECHO | ECHOE | ECHOKE | ECHONL | ICANON | IEXTEN | ISIG | ECHOK | ECHOCTL | ECHOPRT);
 _t.c_cflag &= ~( CSIZE | PARENB);
 _t.c_cflag &= ~CRTSCTS;
 _t.c_cflag |= CS8 | CLOCAL | CREAD;
 _t.c_cflag &= ~CBAUD;
 _t.c_cflag |= B115200; // B19200; // 
 // see http://www.unixwiz.net/techtips/termios-vmin-vtime.html
// _termios.c_cc[VMIN] = 1;
// _termios.c_cc[VTIME] = 5;
}

// see 485-related information:
// http://retis.sssup.it/~scordino/code/serial-rs485.txt

void Cuart::mode_def( struct serial_rs485 &_c, CuartMode_t _m) {
 switch ( _m) {
   case CuartMode232:
     _c.flags &= ~( SER_RS485_ENABLED);
     _c.flags &= ~( SER_RS422_ENABLED);
     break;
   case CuartMode422: // 422
     _c.flags &= ~( SER_RS485_ENABLED);
     _c.flags |= SER_RS422_ENABLED;
     break;
   case CuartMode485: // 485
     _c.flags |= SER_RS485_ENABLED;
     _c.flags &= ~( SER_RS422_ENABLED);
     _c.flags |= SER_RS485_RTS_ON_SEND;
     // _c.flags &= ~( SER_RS485_RTS_ON_SEND);
     // _c.flags |= SER_RS485_RTS_AFTER_SEND;
     _c.flags &= ~( SER_RS485_RTS_AFTER_SEND);
     break;
   default:
     break;
 }
 return;  }
