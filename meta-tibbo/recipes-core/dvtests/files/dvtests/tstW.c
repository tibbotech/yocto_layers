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
 if ( ( fd = open( argv[ 1], O_WRONLY)) < 0) return( 0);
 memset( buf, 1, 256);
 ret = write( fd, buf, 80);
 close( fd);
 return( 0);  }
