#include "CPin.h"

#include <dirent.h>

#define SYS_GPIO_PFX                "/sys/class/gpio/"

int CPin::x_open( int _pin, const char *_what) {
 int fp;
 char fpath[ PATH_MAX];
 sprintf( fpath, SYS_GPIO_PFX"gpio%d/%s", _pin, _what);
 if ( ( fp = open( fpath, O_RDWR)) < 0) return( -1);
 return( fp);  }

int CPin::init( int _pin) {
 DIR *dp;
 char fpath[ PATH_MAX];
 if ( this->pin != _pin) {
   this->x_close();  this->pin = 0;
 }
 if ( this->fp_rw >= 0) return( 0);
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
 char ret, ss[ 2];
 if ( this->fp_rw < 0) this->fp_rw = this->x_open( this->pin, "value");
 if ( this->fp_rw < 0) return( -1);
 lseek( this->fp_rw, 0L, SEEK_SET);
 ret = read( this->fp_rw, ss, 1);
 if ( ret < 0) return( 0);
 return( ( ss[ 0] == '0' ? 0 : 1));  }

int CPin::W( int _val) {
 int ret;
 char x[ 2];
 if ( this->fp_rw < 0) this->fp_rw = this->x_open( this->pin, "value");
 if ( this->fp_rw < 0) return( -1);
 x[ 1] = '\0';
 x[ 0] = ( _val > 0 ? '1' : '0');
 lseek( this->fp_rw, 0L, SEEK_SET);
 ret = write( this->fp_rw, x, 1);
 return( ret);  }

int CPin::dir_get( void) {
 static char ss[ 2];
 int fp = this->x_open( this->pin, "direction");
 if ( fp < 0) return( -1);
 int ret = read( fp, ss, 2);
 close( fp);
 if ( ret < 0) return PIN_DIR_I;
 return( ( ss[ 0] == 'i' ? PIN_DIR_I : PIN_DIR_O));  }

int CPin::dir_set( int _dir) {
 const char *ss = "in";
 int r = 0;
 int fp = this->x_open( this->pin, "direction");
 if ( fp < 0) return( -1);
 if ( _dir == PIN_DIR_O) ss = "out";
 r = write( fp, ss, strlen( ss)*sizeof( char));
 close( fp);
 return( ( r < 2 ? -1 : 0));  }
