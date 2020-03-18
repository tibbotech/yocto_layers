#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <unistd.h>

#define BUFSZ 10*1024

int fd = -1;

static void hup_handler( int signum) {
 fprintf( stderr, "Caught HUP\n");
 if ( fd >= 0) close( fd);
 fd = -1;  }

int main( int argc, char *argv[]) {
 int err, i = 0, Ir = 0, Sr = 0, Gret = 1;
 unsigned char Ib[ BUFSZ + 1], Sb[ BUFSZ + 1];
 void ( *old_handler)( int) = signal( SIGHUP, hup_handler); 
 
 if ( argc < 2) {
   fprintf( stderr, "Usage: %s <serialport>\n", argv[ 0]);
   return( 1);  }
 
 // stdin/stdout non-block
 if ( fcntl( STDIN_FILENO,  F_SETFL, O_NONBLOCK | fcntl( STDIN_FILENO,  F_GETFL, 0)) == -1) {
   fprintf( stderr, "Err: Can't make STDin non-block\n");
   return( 1);  }
 if ( fcntl( STDOUT_FILENO, F_SETFL, O_NONBLOCK | fcntl( STDOUT_FILENO, F_GETFL, 0)) == -1) {
   fprintf( stderr, "Err: Can't make STDout non-block\n");
   return( 1);  }

 if ( ( fd = open( argv[ 1], O_NONBLOCK | O_RDWR)) < 0) {
   fprintf( stderr, "Err: Can't open '%s': %d(%s)\n", argv[ 1], errno, strerror( errno));
   return( 1);  }

 memset( Ib, 0, BUFSZ);
 while ( 1) {
   // read from stdin
   err = errno = 0;
   if ( Ir < BUFSZ) err = read( STDIN_FILENO, Ib, BUFSZ - Ir);
   if ( err >= 0) Ir += err;
   if ( err < 0) switch ( errno) {
     case EAGAIN:  err = 0;  break;
     default:
       fprintf( stderr, "Err: Can't read from stdin: %d(%s)\n", errno, strerror( errno));
       return( 1);
       break;
   }
   // write to serial
   err = errno = 0;
   if ( Ir > 0) err = write( fd, Ib, Ir);
   if ( err > 0) {
     printf( "(SER)W %d b\n", err);
     for ( i = 0; i < BUFSZ - err; i++) Ib[ i] = Ib[ err + i];
     Ir -= err;  }
   if ( err < 0) switch ( errno) {
     case EAGAIN:  err = 0;  break;
     default:
       fprintf( stderr, "Err: Can't write to %s: %d(%s)\n", argv[ 1], errno, strerror( errno));
       return( 1);
       break;
   }

   // read from serial
   err = errno = 0;
   memset( Sb, 0, BUFSZ);
   err = read( fd, Sb, BUFSZ);
   if ( err < 0) switch ( errno) {
     case EAGAIN:  err = 0;  break;
     default:
       fprintf( stderr, "Err: Can't read from %s: %d(%s)\n", argv[ 1], errno, strerror( errno));
       return( 1);
       break;
   }
   if ( err > 0) {
     Sr = err;
     printf( "(SER)R %d b:\n", err);
     write( STDOUT_FILENO, Sb, Sr);
     Sr = 0;   }
   usleep( 100);
 }
 if ( fd >= 0) close( fd);
 return( Gret);  }
