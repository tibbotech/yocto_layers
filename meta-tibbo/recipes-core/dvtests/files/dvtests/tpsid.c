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

#include <unistd.h>

#define PFX "/proc/tpsid"

int main( int argc, char *argv[]) {
 int fd, version, ret, i = 0;
 char fname[ 256], buf[ 256], buff[ 265];
 memset( fname, 0, 256);
 sprintf( fname, "%s", PFX);
 if ( ( fd = open( fname, O_RDONLY)) < 0) return( 0);
 memset( buf, 0, 256);
 ret = read( fd, buf, 80);
 memset( buff, 0, 256);
 for ( i = 0; i < ret; i++) sprintf( buff + i*2, "%02X", buf[ i]);
 printf( "%s\n", buff);
 close( fd);
 return( 0);  }
