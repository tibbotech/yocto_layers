
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
 CuartMode232,
 CuartMode422,
 CuartMode485,
};

//! \brief UART base setup functions
class Cuart {

private:

protected:

public:

 //! \name Termios helper functions

 //! Helps to define struct termios fields
 //! default speed is 115200
 static void termios_def( struct termios &_termios);

 //! ioctls get/set for port mode
 //! \retval 0 Success
 //! \retval <0 Not associated
 //! \{
 inline static int termios_get( int _fd, struct termios &_termios) {
  return( tcgetattr( _fd, &_termios));  }
 inline static int termios_set( int _fd, struct termios &_termios) {
  return( tcsetattr( _fd, TCSANOW, &_termios));  }
 //! \}


 //! \name Port mode functions
 // http://retis.sssup.it/~scordino/code/serial-rs485.txt

 //! Helps to define struct serial_rs485 fields for mode ...
 static void mode_def( struct serial_rs485 &_c, CuartMode_t _m);

 //! ioctls get/set for port mode
 //! \retval 0 Success
 //! \retval <0 Not associated
 //! \{
 inline static int mode_set( int _fd, struct serial_rs485 &_c) {
   return( ioctl( _fd, TIOCSRS485, &_c));  }
 inline static int mode_get( int _fd, struct serial_rs485 &_c) {
   return( ioctl( _fd, TIOCGRS485, &_c));  }
 //! \}

}; // class /

#endif
