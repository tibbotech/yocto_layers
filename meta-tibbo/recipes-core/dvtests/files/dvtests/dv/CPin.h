// This I Love
// (C) dvorkin@tibbo.com, 2016

#ifndef __CPIN_H__
#define __CPIN_H__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define PIN_DIR_I                   0
#define PIN_DIR_O                   1

class CPin {

protected:

 int pin;
 int fp_rw;
 
 int x_open( int _pin, const char *_what);
 void x_close( void) {
   if ( this->fp_rw) close( this->fp_rw);
   this->fp_rw = -1;  }
 
public:

 CPin( void) {  this->pin = 0;  this->fp_rw = -1;  }
 ~CPin( void) {  this->x_close();  }
 
 // call once
 // ret: 0 - OK, -1 - err
 int init( int _pin);
 int cur( void) {  return( this->pin);  }

 // ret: pin value = 0|1
 int R( void);
 // ret: fwrite return code | < 0 on error
 int W( int _val);

 // ret: -1 - err, 0 == PIN_DIR_I, 1 == PIN_DIR_O
 int dir_get( void);
 // ret: -1 - err, 0 - success
 int dir_set( int _dir);

};

#endif
