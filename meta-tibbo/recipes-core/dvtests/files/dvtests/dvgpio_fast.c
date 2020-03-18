#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
//#include <sys/stat.h>
//#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <stdint.h>

#define SYS_GPIO_PFX "/sys/class/gpio/"

#define GPIO0_BASE 0x44E07000
#define GPIO1_BASE 0x4804C000
#define GPIO2_BASE 0x481AC000
#define GPIO3_BASE 0x481AE000

#define GPIO_SIZE  0x00000FFF

// OE: 0 is output, 1 is input
#define GPIO_OE 0x134		// <<bit=1 - in bit=0 - out
#define GPIO_IN 0x138		// (in) - read <<off
#define GPIO_OUT 0x13c		// (out) - write <<off

#define USR0_LED (1<<21)
#define USR1_LED (1<<22)
#define USR2_LED (1<<23)
#define USR3_LED (1<<24)

int mem_fd;
char *gpio_mem, *gpio_map;

uint32_t gpio_bank_off( int _pinN) {
 // find GPIO bank for pin (from INI file?)
 uint32_t ret = GPIO1_BASE;
 return( ret);  }

// call it only once for each leg you need
// _legid is the #of PIN
// ret : 0 - success
int export_pin( int _pinN) {
 FILE *fp;
 DIR *dp;
 char fpath[ PATH_MAX];
 sprintf( fpath, SYS_GPIO_PFX"gpio%d", _pinN);
 // already exported ?
 if ( ( dp = opendir( fpath))) return( 0);
 if ( ( fp = fopen( SYS_GPIO_PFX"export", "w")) == NULL) {
   printf( "Error exporting GPIO #%d: %s\n", _pinN, strerror( errno));
   return( -1);  }
 fprintf( fp, "%d", _pinN);
 fclose( fp);
 return( 0);  }

// ret: NULL - error
volatile unsigned *io_base( int _pinN) {
 int bank_n = gpio_bank_off( _pinN);
 uint32_t bank_off = gpio_bank_off( _pinN);
 volatile unsigned *gpio;
 if ( ( mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
   printf( "can't open /dev/mem \n");
   return( NULL);  }
 // mmap GPIO bank
 gpio_map = ( char *)mmap(
         0,
         GPIO_SIZE,
         PROT_READ | PROT_WRITE,
         MAP_SHARED,
         mem_fd,
         bank_off		// variable
 );
 if ( gpio_map == MAP_FAILED) {
   printf( "mmap error %d\n", ( int)gpio_map);
   return( NULL);  }
 gpio = (volatile unsigned *)gpio_map;
 return( gpio);  }

static void io_setup(void) {
 // Always use the volatile pointer!
}

int main(int argc, char **argv) {
 int pinN = 0;
 volatile unsigned *gpio_base;
 // here have to call pin_Name2Id( 'S1B');
 pinN = 5;
 if ( export_pin( pinN) < 0) return( -1);
 gpio_base = io_base( pinN);
 if ( gpio_base == NULL) return( -1);

 // Get direction control register contents
 unsigned int creg = *(gpio_base + GPIO_OE);
 // Set outputs
 creg = creg & (~USR0_LED);
 creg = creg & (~USR1_LED);
 creg = creg & (~USR2_LED);
 creg = creg & (~USR3_LED);
 // Set new direction control register contents
 *(gpio_base + GPIO_OE) = creg;
 while (1) {
     // Set LEDs
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) | USR0_LED;
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) | USR1_LED;
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) | USR2_LED;
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) | USR3_LED;
     sleep(1);
     // Clear LEDs
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) & (~USR0_LED);
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) & (~USR1_LED);
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) & (~USR2_LED);
     *(gpio_base + GPIO_OUT) = *(gpio_base + GPIO_OUT) & (~USR3_LED);
     sleep(1);
 }
 return( 0);
}
