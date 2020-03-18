#include "CPin.h"

#include <dirent.h>

#define SYS_GPIO_PFX                "/sys/class/gpio/"

FILE *CPin::x_open( int _pin, const char *_what) {
 FILE *fp;
 char fpath[ PATH_MAX];
 sprintf( fpath, SYS_GPIO_PFX"gpio%d/%s", _pin, _what);
 if ( ( fp = fopen( fpath, "r+")) == NULL) return( NULL);
 return( fp);  }

int CPin::init( int _pin) {
 DIR *dp;
 char fpath[ PATH_MAX];
 if ( this->pin != _pin) {
   this->x_close();  this->pin = 0;
 }
 if ( this->fp_rw) return( 0);
 sprintf( fpath, SYS_GPIO_PFX"gpio%d", _pin);
 if ( !( dp = opendir( fpath))) {
   FILE *fp;
   if ( ( fp = fopen( SYS_GPIO_PFX"export", "w")) == NULL) return( -1);
   fprintf( fp, "%d", _pin);
   fclose( fp);
 } else {  closedir( dp);  }
 this->pin = _pin;
 return( 0);  }

int CPin::R( void) {
 char ret;
 if ( !this->fp_rw) this->fp_rw = this->x_open( this->pin, "value");
 if ( !this->fp_rw) return( -1);
 fflush( this->fp_rw);
 fseek( this->fp_rw, 0L, SEEK_SET);
 ret = fgetc( this->fp_rw);
 return( ( ret == '0' ? 0 : 1));  }

int CPin::W( int _val) {
 int ret;
 char x[ 2];
 if ( !this->fp_rw) this->fp_rw = this->x_open( this->pin, "value");
 if ( !this->fp_rw) return( -1);
 x[ 1] = '\0';
 x[ 0] = ( _val > 0 ? '1' : '0');
 fseek( this->fp_rw, 0L, SEEK_SET);
 ret = fprintf( this->fp_rw, "%s", x);
 fflush( this->fp_rw);
 return( ret);  }

int CPin::dir_get( void) {
 static char ss[ PATH_MAX];
 FILE *fp = this->x_open( this->pin, "direction");
 if ( !fp) return( -1);
 int ret = fscanf( fp, "%1s", ss);
 fclose( fp);
 if ( ret < 0) return PIN_DIR_I;
 return( ( ss[ 0] == 'i' ? PIN_DIR_I : PIN_DIR_O));  }

int CPin::dir_set( int _dir) {
 const char *ss = "in";
 int r = 0;
 FILE *fp = this->x_open( this->pin, "direction");
 if ( !fp) return( -1);
 if ( _dir == PIN_DIR_O) ss = "out";
 r = fwrite( ss, sizeof( char), strlen( ss), fp);
 fclose( fp);
 return( ( r < 2 ? -1 : 0));  }
