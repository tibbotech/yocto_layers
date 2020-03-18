
//! \copyright Tibbo Technology Inc.
//! \author Dvorkin Dmitry <dvorkin@tibbo.com>
//! \date 27.07.2016

#ifndef __CWG_H__
#define __CWG_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>

#define TWG_MAX_BUF 512

//! \struct wg_data_t
//! \brief I2C bus name to # mapping structure
//! \details simple structure for custom I2C bus name to bus # custom mapping
typedef struct wg_data_T {
 //! number of bits read from Wiegand
 uint16_t blen;
 //! raw buffer, no odd/even checks
 uint8_t  buff[ TWG_MAX_BUF/8];
} wg_data_t;


//! \brief Wiegand Tibbit#08 I/O class
class CWg {

private:

 void null_sock( void) {  memset( this->sock, 0, 64);  }

protected:

 int f_rw;
 char sock[ 64];
 
 int x_open( const char *_sock, const char *_what, int _flags);
 void x_close( void) {
   this->null_sock();
   if ( this->f_rw) close( this->f_rw);
   this->f_rw = -1;  }

// int find_sock( char *_sock);
 
public:

 CWg( void) {
   this->null_sock();
   this->f_rw = 0;  }
 ~CWg( void) {  this->x_close();  }
 
 //! \brief Open Wiegand device on socket _sock
 //! \retval 1 OK
 //! \retval 0 Not opened
 //! \retval <0 Errno
 int init( const char *_sock);

 //! \brief Get current socket name
 inline char *cur_sock( void) {  return( this->sock);  }

 //! \brief Get/Set current Tibbit#08 mode
 //! \retval == 1 mode is Wiegand
 //! \retval == 0 mode is Clock/Data
 //! \retval <0 failed (check errno value)
 int mode_get( void);
 //! _mode == 1 mode is Wiegand
 //! _mode == 0 mode is Clock/Data
 //! \retval <0 failed (check errno value)
 int mode_set( uint8_t _mode);

 //! \brief Get/Set value of open collector pin
 //! \retval 0 OK
 //! \retval <0 failed (check errno value)
 int out0_get( void);
 //! \retval 0 OK
 //! \retval <0 failed (check errno value)
 int out0_set( uint8_t _val);

 //! \brief Get/Set value of 'clear on read' flag
 //! \retval 0 OK
 //! \retval <0 failed (check errno value)
 int conr_get( void);
 //! \retval 0 OK
 //! \retval <0 failed (check errno value)
 int conr_set( uint8_t _val);

 //! \brief Read Wiegand buffer
 /// After this function call Wiegand device buffer gets empty
 /// This call is non-blocking, check return code to see if data exist
 //! \retval >0 OK
 //! \retval <=0 Data is not ready
 int R( wg_data_t &_buf);

};

#endif
