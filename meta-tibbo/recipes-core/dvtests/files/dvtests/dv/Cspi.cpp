#include "Cspi.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEV_SPI_PFX "/dev/spidev"
#define SYS_SPI_PFX "/sys/devices/platform/spi"

// by socket name
int Cspi::find_bus( const char *_sock) {
 DIR *d;
 struct dirent *dent;
 char dpath[ PATH_MAX];
 int xbusn = -1;
 memset( dpath, 0, PATH_MAX);
 sprintf( dpath, SYS_SPI_PFX"%s/spi_master/", _sock);
 if ( ( d = opendir( dpath)) == NULL) return( -1);
 while ( ( dent = readdir( d)) != NULL) {
   if ( strncmp( dent->d_name, "spi", 3) != 0) continue;
   xbusn = atoi( dent->d_name + 3);
   break;  }
 closedir( d);
 return( xbusn);  }

int Cspi::x_open( uint16_t _busn, uint8_t _devn) {
 int f;
 char fpath[ PATH_MAX];
 memset( fpath, 0, PATH_MAX);
 sprintf( fpath, DEV_SPI_PFX"%d.%d", _busn, _devn);
 if ( ( f = open( fpath, O_RDWR)) < 0) return( -errno);
 memset( &( this->tr), 0, sizeof( this->tr));
 return( f);  }

int Cspi::set_dev( uint16_t _busn, uint8_t _dev) {
 int f;
 if ( this->busn == _busn && this->addr == _dev && this->f_rw >= 0) return( 1);
 if ( this->f_rw >= 0) close( this->f_rw);
 this->busn = this->addr = this->f_rw = -1;
 if ( ( f = this->x_open( _busn, _dev)) < 0) return( f);
 this->f_rw = f;
 this->busn = _busn;
 this->addr = _dev;
 return( 1);  }

// by name
int Cspi::set_dev( const char *_sock, uint8_t _dev) {
 int xbusn;
 if ( this->f_rw >= 0) close( this->f_rw);
 xbusn = Cspi::find_bus( _sock);
 if ( xbusn < 0) return( -1);
 return( this->set_dev( xbusn, _dev));  }

int Cspi::settings_get( void) {
 if ( this->f_rw < 0) return( this->f_rw);
 if ( ioctl( this->f_rw, SPI_IOC_RD_MODE32, &( this->mode)) < 0) return( -errno);
 if ( ioctl( this->f_rw, SPI_IOC_RD_BITS_PER_WORD, &( this->bits)) < 0) return( -errno);
 if ( ioctl( this->f_rw, SPI_IOC_RD_MAX_SPEED_HZ, &( this->speed)) < 0) return( -errno);
 return( 1);  }

int Cspi::settings_set( void) {
 if ( this->f_rw < 0) return( this->f_rw);
 if ( ioctl( this->f_rw, SPI_IOC_WR_MODE32, &( this->mode)) < 0) return( -errno);
 if ( ioctl( this->f_rw, SPI_IOC_WR_BITS_PER_WORD, &( this->bits)) < 0) return( -errno);
 if ( ioctl( this->f_rw, SPI_IOC_WR_MAX_SPEED_HZ, &( this->speed)) < 0) return( -errno);
 return( 1);  }

int Cspi::WR( uint8_t const *_w, uint8_t const *_r, uint32_t _l) {
 int r;
 this->tr.tx_buf = ( unsigned long)_w;
 this->tr.rx_buf = ( unsigned long)_r;
 this->tr.len = _l;
 this->tr.delay_usecs = this->delay;
 this->tr.speed_hz = this->speed;
 this->tr.bits_per_word = this->bits;

 if ( this->f_rw < 0) return( 0);
 if ( _w == NULL && _r == NULL) return( 0);
 if ( this->mode & SPI_TX_QUAD) this->tr.tx_nbits = 4;
 else if ( this->mode & SPI_TX_DUAL) this->tr.tx_nbits = 2;
 if ( this->mode & SPI_RX_QUAD) this->tr.rx_nbits = 4;
 else if ( this->mode & SPI_RX_DUAL) this->tr.rx_nbits = 2;
 if ( !( this->mode & SPI_LOOP)) {
   if ( this->mode & ( SPI_TX_QUAD | SPI_TX_DUAL)) this->tr.rx_buf = 0;
   else if ( this->mode & ( SPI_RX_QUAD | SPI_RX_DUAL)) this->tr.tx_buf = 0;
 }
 r = ioctl( this->f_rw, SPI_IOC_MESSAGE( 1), &( this->tr));
 if ( r < 0) return( -errno);
 return( r);  }
