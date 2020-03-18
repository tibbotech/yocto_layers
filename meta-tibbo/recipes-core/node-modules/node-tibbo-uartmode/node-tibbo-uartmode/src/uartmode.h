
//! \copyright Tibbo Technology Inc.
//! \author Dvorkin Dmitry <dvorkin@tibbo.com>
//! \date 17.07.2016

#ifndef __CUART_H__
#define __CUART_H__

#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>

#define SER_RS422_ENABLED (SER_RS485_ENABLED << 5)

enum CuartMode_t {
 CuartModeNaN = -1,
 CuartMode232 = 0,
 CuartMode422 = 1,
 CuartMode485 = 2,
};

//! \brief UART base setup functions
class tibbo_uartmode {

private:

protected:

public:

 static void mode_def( struct serial_rs485 *_c, CuartMode_t _m);

 static CuartMode_t G( const char *_port);
 static int S( const char *_port, CuartMode_t _m);

}; // class /

#endif
