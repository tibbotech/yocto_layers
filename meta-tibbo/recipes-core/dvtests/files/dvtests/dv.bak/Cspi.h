
//! \copyright Tibbo Technology Inc.
//! \author Dvorkin Dmitry <dvorkin@tibbo.com>
//! \date 06.07.2016

#ifndef __CSPI_H__
#define __CSPI_H__

#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <linux/spi/spidev.h>

//! \brief General perpose SPI I/O class
class Cspi {

private:

 long addr;
 uint32_t mode;
 uint8_t bits;
 uint32_t speed;
 uint16_t delay;
 struct spi_ioc_transfer tr;

protected:

 int f_rw;
 int busn;
 int x_open( uint16_t _busn, uint8_t _dev);
 void x_close( void) {
   if ( this->f_rw >= 0) close( this->f_rw);
   this->f_rw = -1;
 }

public:

 Cspi( void) {
   this->busn = this->addr = this->f_rw = -1;
   this->bits = 8;  this->speed = 500000;  this->delay = 0;
 }
 Cspi( Cspi &_o) {  this->busn = _o.busn;  this->addr = this->f_rw = -1;  }
 ~Cspi( void) {  this->x_close();  }
 
 //! \retval <0 Errno
 int set_dev( uint16_t _busn, uint8_t _dev);
 //! Access by TPS socket name, ex: "-sXX" + device number (usually 0)
 //! \retval <0 Errno
 int set_dev( const char *_sock, uint8_t _dev = 0);

 //! \name Bulk settings apply/load methods
 //! \retval <0 Errno
 //! \{
 int settings_get( void);
 int settings_set( void);
 //! \}

 //! \name Set/Get properties methods
 //! Methods below are for class properties access only
 //! \warning Call settings_set()/settings_get() for sync with real device
 //! \{
 inline void set_bits( uint8_t _bits) {  this->bits = _bits;  }
 inline uint8_t cur_bits( void) {  return( this->bits);  }
 inline void set_speed( uint32_t _speed) {  this->speed = _speed;  }
 inline uint32_t cur_speed( void) {  return( this->speed);  }
 inline void set_delay( uint16_t _delay) {  this->delay = _delay;  }
 inline uint16_t cur_delay( void) {  return( this->delay);  }
 //! \note
 //! See
 //! https://www.kernel.org/doc/htmldocs/device-drivers/API-struct-spi-device.html
 //! for possible mode flags
 inline void set_mode( uint32_t _mode) {  this->mode = _mode;  }
 inline uint32_t cur_mode( void) {  return( this->mode);  }
 //! \}

 //! \return Current Cspi bus number
 //! \retval <0 Not associated
 int cur_bus( void) {  return( this->busn);  }
 //! \return Current device number on Cspi bus
 //! \retval <0 Not associated
 int cur_dev( void) {  return( this->addr);  }

 //! \name I/O methods
 //! \retval >0 number of bytes R/W
 //! 0 Not opened (?)
 //! <0 Errno
 //! \{
 inline int R( uint8_t const *_b, size_t _l) {  return( this->WR( _b, NULL, _l));  }
 inline int W( uint8_t const *_b, size_t _l) {  return( this->WR( NULL, _b, _l));  }
 int WR( uint8_t const *_w, uint8_t const *_r, uint32_t _l);
 //! \}

 //! \retval -1 Not found
 static int find_bus( const char *_sock);

}; // class /

#endif
