// call: ./program /dev/input/event1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

void handler( int sig) {
 printf ( "exiting...(%d)n", sig);
 exit( 0); }

void perror_exit( char *error) {
 perror( error);
 handler( 9);  }

int main (int argc, char *argv[]) {
 struct input_event ev[64];
 int fd, rd, value, size = sizeof (struct input_event);
 char name[256] = "Unknown";
 char *device = NULL;

 //Setup check
 if (argv[1] == NULL){
   printf("Please specify (on the command line) the path to the dev event interface devicen");
   exit (0);  }
 if ( ( getuid()) != 0) printf ("You are not root! This may not work...n");
 if ( argc > 1) device = argv[ 1];
 //Open Device
 if ( ( fd = open ( device, O_RDONLY)) == -1) printf ("%s is not a vaild device.\n", device);
  //Print Device Name
 ioctl (fd, EVIOCGNAME (sizeof (name)), name);
 printf ("Reading From : %s (%s)\n", device, name);
 while (1){
   if ((rd = read (fd, ev, size * 64)) < size) perror_exit ("read()");
   printf( "ev[0].type=%d ev[0].code=%d ev[0].value=%d\n", ev[ 0].type, ev[ 0].code, ev[ 0].value);
   printf( "ev[1].type=%d ev[1].code=%d ev[1].value=%d\n", ev[ 1].type, ev[ 1].code, ev[ 1].value);
 }
 return 0;  }
