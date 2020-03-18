#include "SysDs.h"

int SysDs::open_S( void) {
 this->bus_close();
 // uses env $DBUS_SESSION_BUS_ADDRESS
 int r = sd_bus_open_system( &( this->bus));
 return( r);  }

int SysDs::open_H( const char *_host) {
 this->bus_close();
 int r = sd_bus_open_system_remote( &( this->bus), _host);
 return( r);  }

int SysDs::open_M( const char *_mach) {
 this->bus_close();
 int r = sd_bus_open_system_machine( &( this->bus), _mach);
 return( r);  }

// _m = method name (ex.: StartUnit)
// _s = service (ex.: cups.service)
int SysDs::call_StartStop( const char *_m, const char *_s) {
 sd_bus_error_free( &( this->error));
 this->error = SD_BUS_ERROR_NULL;
 if ( this->m) sd_bus_message_unref( this->m);
 this->m = NULL;
 /* Issue the method call and store the respons message in m */
 int r = sd_bus_call_method( this->bus,
       "org.freedesktop.systemd1",           /* service to contact */
       "/org/freedesktop/systemd1",          /* object path */
       "org.freedesktop.systemd1.Manager",   /* interface name */
       _m,                                   /* method name */
       &( this->error),                      /* object to return error in */
       &( this->m),                          /* return message on success */
       "ss",                                 /* input signature */
       _s,                                   /* first argument */
       "replace");                           /* second argument */
 return( r);  }

int SysDs::Rs( const char **_repl) {
 if ( this->m == NULL) return( 0);
 if ( _repl == NULL) return( 0);
 int r = sd_bus_message_read( this->m, "o", _repl);
 return( r);  }
