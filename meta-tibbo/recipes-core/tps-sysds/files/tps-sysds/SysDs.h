//
// This I Love
// (C) dvorkin@tibbo.com, 2015

#include <systemd/sd-bus.h>

#ifndef __SYSDS_H__
#define __SYSDS_H__

class SysDs {

private:

 sd_bus *bus;
 sd_bus_error error;
 sd_bus_message *m;

// long addr;

 void bus_close( void) {
   if ( this->bus) sd_bus_unref( bus);
   this->bus = NULL;
   this->error = SD_BUS_ERROR_NULL;
   this->m = NULL;
 }

public:

 SysDs( void) {  this->bus = NULL;  this->m = NULL;  this->error = SD_BUS_ERROR_NULL;  }
 ~SysDs( void) {
   sd_bus_error_free( &( this->error));
   if ( this->m) sd_bus_message_unref( this->m);
   this->bus_close();
 }

 const char *get_errM( void) {  return( this->error.message);  }


 // ret : errno
 int open_S( void);
 int open_H( const char *_host);
 int open_M( const char *_mach);

 /// _m = StartUnit, StopUnit, ReloadUnit, RestartUnit
 /// _u = unit name, ex: cups.service
 int call_StartStop( const char *_m, const char *_u);

 // read string reply
 int Rs( const char **_repl);

}; // class /

#endif // SYSDS_H
