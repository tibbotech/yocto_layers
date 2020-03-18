#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#define FILE "/dev/input/event0"

int main( int argc, char *argv[]) {
 int fd, version, ret;
 struct input_event event;
 if ( argc < 2) {
   printf( "%s <tone_value>\n", argv[ 0]);
   return( 1);  }
 if ( ( fd = open( FILE, O_RDWR)) < 0) {  perror( "beep test\n");  return 1;  }
 event.type = EV_SND;
 event.code = SND_BELL;
 event.value = atoi( argv[ 1]);
 ret = write( fd, &event, sizeof( struct input_event));
 close( fd);
 return( 0);  }
