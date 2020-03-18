
//! \copyright Tibbo Technology Inc.
//! \author Dvorkin Dmitry <dvorkin@tibbo.com>
//! \date 06.07.2016

#ifndef __CI2C_H__
#define __CI2C_H__

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

//! \struct i2cmap_t
//! \brief I2C bus name to # mapping structure
//! \details simple structure for custom I2C bus name to bus # custom mapping
struct i2cmap_t {
 //! Bus name, "-tps" or "-s05" for example
 const char *name;
 //! Real bus ID, 1 for example
 uint8_t id;
};

//! \brief Base Virtual I2C I/O class
//! \details Virtual I2C I/O functions have to be declared in child class
class Ci2c {

private:

 long addr;

protected:

 int f_rw;
 int busn;
 int open_x( uint16_t _busn);

public:

 Ci2c( void) {  this->busn = this->addr = this->f_rw = -1;  }
 Ci2c( Ci2c &_o) {  this->busn = _o.busn;  this->addr = this->f_rw = -1;  }
 ~Ci2c( void) {
   if ( this->f_rw >= 0) close( this->f_rw);
 }
 
 //! \name Bus methods
 //! \retval 1 OK
 //! \retval <0 Errno
 //! \{
 int set_bus( uint16_t _busn);
 int set_bus( const char *_buss, i2cmap_t *_map = NULL);
 int cur_bus( void) {  return( this->busn);  }
 //! \}

 //! \name Virtual I/O methods
 /// It may take too long time to read/write i2c device
 /// so make this functions non-blockable
 //! \retval >0 Number of bytes R/W
 //! \retval 0 Not opened (?)
 //! \retval <0 Errno
 //! \{
 virtual int R1b( uint16_t _addr, uint8_t _reg, uint8_t  &_b) = 0;
 virtual int W1b( uint16_t _addr, uint8_t _reg, uint8_t   _b) = 0;
 virtual int R2b( uint16_t _addr, uint8_t _reg, uint16_t &_b) = 0;
 virtual int W2b( uint16_t _addr, uint8_t _reg, uint16_t  _b) = 0;
 // max block size == 32bytes
 virtual int Rbb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) = 0;
 virtual int Wbb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) = 0;
 // quick type R/W (command-reg is data also)
 virtual int Rqb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) = 0;
 virtual int Wqb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) = 0;
 //! \}

 //! \brief Set slave device to work with.
 /// No need to call it obviously. Call it only for test if device exists.
 /// BUT nowtime result is not guaranteed if it can return an error "no such device"
 /// due to SMBUS i2c driver realization.
 ///
 /// Better to use R* or W* functions to probe the devices on a bus,
 /// for example R1b( _addr, 0x00, b);
 // _addr may be 7 or 10 bits
 //! \retval 1 OK
 //! \retval 0 Not opened
 //! \retval <0 Errno
 int set_slave( uint16_t _addr);

 //! \retval -1 Not found
 static int find_bus( const char *_sock);

}; // class /

#endif
