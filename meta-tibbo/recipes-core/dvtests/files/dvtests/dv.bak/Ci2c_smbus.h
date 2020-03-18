//! \copyright Tibbo Technology Inc.
//! \author Dvorkin Dmitry <dvorkin@tibbo.com>
//! \date 06.07.2016

#ifndef __CI2C_SMBUS_H__
#define __CI2C_SMBUS_H__

#include <linux/i2c.h>

#include "Ci2c.h"

//! \brief I2C SMBUS I/O class
//! \details I2C SMBUS-specific I/O functions
//! \details Allow to interact with almost all I2C devices

class Ci2c_smbus : public Ci2c {

private:

 //! \return 1 - OK, 0 - not opened, < 0 - errno
 int do_smbus( uint16_t _addr, uint8_t _rw, uint32_t _dlen, union i2c_smbus_data &_data);

protected:

public:

 /// NOTE: it may take too long time to read/write i2c device
 /// especially if there are alot of legs/registers
 /// so make this functions non-blockable

 //! \name I/O methods
 //! \retval >0 number of bytes R/W
 //! 0 Not opened (?)
 //! <0 Errno
 //! \{
 int R1b( uint16_t _addr, uint8_t _reg, uint8_t  &_b);
 int W1b( uint16_t _addr, uint8_t _reg, uint8_t   _b);
 int R2b( uint16_t _addr, uint8_t _reg, uint16_t &_b);
 int W2b( uint16_t _addr, uint8_t _reg, uint16_t  _b);
 // max block size == 32bytes
 int Rbb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen);
 int Wbb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen);
 // quick type R/W (command-reg is data also)
 int Rqb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen);
 int Wqb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen);
 //! \}

}; // class /

#endif
