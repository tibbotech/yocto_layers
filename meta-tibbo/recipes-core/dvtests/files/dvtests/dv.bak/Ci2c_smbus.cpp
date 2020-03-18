//
// This I Love
// (C) Dvorkin Dmitry <dvorkin@tibbo.com>
//
#include "Ci2c_smbus.h"

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int Ci2c_smbus::R1b( uint16_t _addr, uint8_t _reg, uint8_t  &_b) {
 int ret = 0;
 i2c_smbus_data d;
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_READ,  I2C_SMBUS_BYTE_DATA, d)) <= 0) return( -errno);
 _b = d.byte;
 return( 1);  }

int Ci2c_smbus::W1b( uint16_t _addr, uint8_t _reg, uint8_t   _b) {
 int ret = 0;
 i2c_smbus_data d;
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 d.byte = _b;
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_WRITE, I2C_SMBUS_BYTE_DATA, d)) <= 0) return( -errno);
 return( 1);  }

int Ci2c_smbus::R2b( uint16_t _addr, uint8_t _reg, uint16_t &_b) {
 int ret = 0;
 i2c_smbus_data d;
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_READ,  I2C_SMBUS_WORD_DATA, d)) <= 0) return( -errno);
 _b = d.word;
 return( 2);  }

int Ci2c_smbus::W2b( uint16_t _addr, uint8_t _reg, uint16_t  _b) {
 int ret = 0;
 i2c_smbus_data d;
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 d.word = _b;
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_WRITE, I2C_SMBUS_WORD_DATA, d)) <= 0) return( -errno);
 return( 2);  }

int Ci2c_smbus::Rbb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) {
 int ret = 0;
 i2c_smbus_data d;
 if ( !_b) return( 0);
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 if ( _blen > I2C_SMBUS_BLOCK_MAX) _blen = I2C_SMBUS_BLOCK_MAX;
 d.block[ 0] = _blen;
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_READ,  I2C_SMBUS_I2C_BLOCK_DATA, d)) <= 0) return( -errno);
 ret = d.block[ 0];
 memcpy( _b, &( d.block[ 1]), ret);
 return( ret);  }

int Ci2c_smbus::Wbb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) {
 int ret = 0;
 i2c_smbus_data d;
 if ( !_b) return( 0);
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 if ( _blen > I2C_SMBUS_BLOCK_MAX) _blen = I2C_SMBUS_BLOCK_MAX;
 d.block[ 0] = _blen;
 memcpy( &( d.block[ 1]), _b, _blen);
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_WRITE, I2C_SMBUS_I2C_BLOCK_DATA, d)) <= 0) return( -errno);
 ret = _blen;
 return( ret);  }

int Ci2c_smbus::Rqb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) {
 int ret = 0;
 i2c_smbus_data d;
 if ( !_b) return( 0);
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 if ( _blen > I2C_SMBUS_BLOCK_MAX) _blen = I2C_SMBUS_BLOCK_MAX;
 d.block[ 0] = _blen;
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_READ,  I2C_SMBUS_QUICK, d)) <= 0) return( -errno);
 ret = d.block[ 0];
 memcpy( _b, &( d.block[ 1]), ret);
 return( ret);  }

int Ci2c_smbus::Wqb( uint16_t _addr, uint8_t _reg, uint8_t *_b, uint8_t _blen) {
 int ret = 0;
 i2c_smbus_data d;
 if ( !_b) return( 0);
 if ( this->f_rw < 0) return( 0);
 if ( this->set_slave( _addr) != 1) return( -errno);
 if ( _blen > I2C_SMBUS_BLOCK_MAX) _blen = I2C_SMBUS_BLOCK_MAX;
 d.block[ 0] = _blen;
 memcpy( &( d.block[ 1]), _b, _blen);
 if ( ( ret = this->do_smbus( _reg, I2C_SMBUS_WRITE, I2C_SMBUS_QUICK, d)) <= 0) return( -errno);
 ret = _blen;
 return( ret);  }

int Ci2c_smbus::do_smbus( uint16_t _addr, uint8_t _rw, uint32_t _dlen, union i2c_smbus_data &_data) {
 int r;
 i2c_smbus_ioctl_data d;
 d.read_write = _rw;
 // FIX: potential problem. _addr may be 10bit.
 d.command = _addr;
 d.size = _dlen;
 d.data = &_data;
 if ( this->f_rw < 0) return( 0);
 if ( ( r = ioctl( this->f_rw, I2C_SMBUS, &d)) < 0) return( -errno);
 return( 1);  }

