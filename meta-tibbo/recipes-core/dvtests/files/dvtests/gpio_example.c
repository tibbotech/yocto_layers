#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <asm-generic/ioctl.h>

/*--- donot edit */
#define DEVICE_NAME "gpio.0"

#define AT91_PIN_PA16   (0x00 + 16)
#define AT91_PIN_PA17   (0x00 + 17)
#define AT91_PIN_PA18   (0x00 + 18)
#define AT91_PIN_PA19   (0x00 + 19)
#define AT91_PIN_PA20   (0x00 + 20)
#define AT91_PIN_PA21   (0x00 + 21)
#define AT91_PIN_PA22   (0x00 + 22)
#define AT91_PIN_PA23   (0x00 + 23)
#define AT91_PIN_PE8    (0x80 + 8)
#define AT91_PIN_PE9    (0x80 + 9)
#define AT91_PIN_PE10   (0x80 + 10)
#define AT91_PIN_PE11   (0x80 + 11)
#define AT91_PIN_PE12   (0x80 + 12)
#define AT91_PIN_PE13   (0x80 + 13)
#define AT91_PIN_PE14   (0x80 + 14)
#define AT91_PIN_PE15   (0x80 + 15)

#define GPIO_PA16   (AT91_PIN_PA16 << 8 | 1)
#define GPIO_PA17   (AT91_PIN_PA17 << 8 | 2)
#define GPIO_PA18   (AT91_PIN_PA18 << 8 | 3)
#define GPIO_PA19   (AT91_PIN_PA19 << 8 | 4)
#define GPIO_PA20   (AT91_PIN_PA20 << 8 | 5)
#define GPIO_PA21   (AT91_PIN_PA21 << 8 | 6)
#define GPIO_PA22   (AT91_PIN_PA22 << 8 | 7)
#define GPIO_PA23   (AT91_PIN_PA23 << 8 | 8)
#define GPIO_PE8    (AT91_PIN_PE8  << 8 | 9)
#define GPIO_PE9    (AT91_PIN_PE9  << 8 | 10)
#define GPIO_PE10   (AT91_PIN_PE10 << 8 | 11)
#define GPIO_PE11   (AT91_PIN_PE11 << 8 | 12)
#define GPIO_PE12   (AT91_PIN_PE12 << 8 | 13)
#define GPIO_PE13   (AT91_PIN_PE13 << 8 | 14)
#define GPIO_PE14   (AT91_PIN_PE14 << 8 | 15)
#define GPIO_PE15   (AT91_PIN_PE15 << 8 | 16)


#define GPIO_SET_PIN    _IOR('G', 0, int)
#define GPIO_CLR_PIN    _IOR('G', 1, int)
#define GPIO_GET_VALUE  _IOR('G', 2, int)  /* input */

/*----------------------*/
typedef unsigned char bool;

/* This is a simple demo */
static void gpio_demo(const int fd)
{
    unsigned int cmd;
    unsigned int pin;
    int ret = 0;
    while (1) {
        static bool on = 0;

        system ("clear");
        printf ("-----------------------------\n");
        printf ("*     EDM6070 GPIO Demo    *\n");
        printf ("-----------------------------\n");

        printf("GPIO_PA16 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA16));
        printf("GPIO_PA17 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA17));
        printf("GPIO_PA18 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA18));
        printf("GPIO_PA19 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA19));
        printf("GPIO_PA20 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA20));
        printf("GPIO_PA21 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA21));
        printf("GPIO_PA22 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA22));
        printf("GPIO_PA23 INPUT  %d\n", ioctl(fd, GPIO_GET_VALUE, GPIO_PA23));

        printf("GPIO_PE8  OUTPUT %d\n", on);
        printf("GPIO_PE9  OUTPUT %d\n", on);
        printf("GPIO_PE10 OUTPUT %d\n", on);
        printf("GPIO_PE11 OUTPUT %d\n", on);
        printf("GPIO_PE12 OUTPUT %d\n", on);
        printf("GPIO_PE13 OUTPUT %d\n", on);
        printf("GPIO_PE14 OUTPUT %d\n", on);
        printf("GPIO_PE15 OUTPUT %d\n", on);

        if (on) {
            ioctl(fd, GPIO_SET_PIN, GPIO_PE8);
            ioctl(fd, GPIO_SET_PIN, GPIO_PE9);
            ioctl(fd, GPIO_SET_PIN, GPIO_PE10);
            ioctl(fd, GPIO_SET_PIN, GPIO_PE11);
            ioctl(fd, GPIO_SET_PIN, GPIO_PE12);
            ioctl(fd, GPIO_SET_PIN, GPIO_PE13);
            ioctl(fd, GPIO_SET_PIN, GPIO_PE14);
            ioctl(fd, GPIO_SET_PIN, GPIO_PE15);
        }
        else {
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE8);
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE9);
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE10);
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE11);
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE12);
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE13);
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE14);
            ioctl(fd, GPIO_CLR_PIN, GPIO_PE15);
        }
        on = !on;
        usleep(500000);
    }
}

static void print_usage(char *func)
{
    printf ("usage:\n");
    printf ("   %s out pin value: set output pin\n", func);
    printf ("   %s in  pin: get input pin\n", func);
    printf ("      pin: 0,1,2 ...\n");
    printf ("   %s test: run test demo\n", func);
    printf ("   %s: run example demo\n", func);
    printf ("\n");
}

/* produce test */
static int out_pin[] = { GPIO_PA16, GPIO_PA17, GPIO_PA18, GPIO_PA19, GPIO_PA20, GPIO_PA21, GPIO_PA22, GPIO_PA23 };
static int in_pin[] =  { GPIO_PE8, GPIO_PE9, GPIO_PE10, GPIO_PE11, GPIO_PE12, GPIO_PE13, GPIO_PE14, GPIO_PE15 };

#define ARRAY_SIZE(A)       (sizeof(A) / sizeof(A[0]))

static int set_output(const int fd, unsigned int value)
{
    int i;
    value &= (1<<ARRAY_SIZE(out_pin))-1;
    for (i = 0; i < ARRAY_SIZE(out_pin); i++) {
        if (value & (1<<i)) {
            ioctl(fd, GPIO_SET_PIN, out_pin[i]);
        }
        else {
            ioctl(fd, GPIO_CLR_PIN, out_pin[i]);
        }
    }
    usleep (5000);
    return 0;
}
static unsigned int get_input(const int fd)
{
    unsigned int ret = 0;
    int i;
    for (i = 0; i < ARRAY_SIZE(in_pin); i++) {
        int val = ioctl(fd, GPIO_GET_VALUE, in_pin[i]);
        ret |= (val << i);
    }
    return ret;
}

static void print_result(unsigned int left_value, unsigned int right_value)
{
    int i = 0;
    printf ("------------------------------\n");
    printf ("PA16..PA23: ");
    for (i = 0; i < 8; i++) {
        printf ("%d ", (left_value>>i) & 1);
    }
    printf ("\n");
    printf ("PE8..PE15: ");
    for (i = 0; i < 8; i++) {
        printf ("%d ", (right_value>>i) & 1);
    }
    printf ("\n");
}

static int gpio_test(const int fd)
{
    int i;
    unsigned int setvalue, getvalue;
    int result = 0;

    for (i = 0; i < 8; i++) {
        setvalue = 1<<i;
        set_output (fd, setvalue);
        getvalue = get_input (fd);
        
        if (setvalue != getvalue) {
            print_result (setvalue, getvalue);
            result = 1;
            break;
        }
    }

    if (result != 0)
        printf ("GPIO   : FAIL\n");
    else
        printf ("GPIO   : OK\n");
    return result;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    int fd = open("/dev/"DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error: open failed %s\n", strerror( errno));
        return 1;
    }

    if (argc == 1) {
        gpio_demo(fd);
    }
    else if (argc == 3 && !strcmp(argv[1], "in")) {  /* input */
        int idx = atoi(argv[2]);
        int pin;
        switch (idx) {
            case 0:
                pin = GPIO_PE8; break;
            case 1:
                pin = GPIO_PE9; break;
            case 2:
                pin = GPIO_PE10; break;
            case 3:
                pin = GPIO_PE11; break;
            case 4:
                pin = GPIO_PE12; break;
            case 5:
                pin = GPIO_PE13; break;
            case 6:
                pin = GPIO_PE14; break;
            case 7:
                pin = GPIO_PE15; break;

            default:
                return 1;
        }
        ret = ioctl(fd, GPIO_GET_VALUE, pin);
        printf ("IN[%d]: %d\n", idx, ret);
    }
    else if (argc == 4 && !strcmp(argv[1], "out")) { /* output */
        int idx = atoi(argv[2]);
        int value = atoi(argv[3]);
        int pin;
        switch (idx) {
            case 0:
                pin = GPIO_PA16; break;
            case 1:
                pin = GPIO_PA17; break;
            case 2:
                pin = GPIO_PA18; break;
            case 3:
                pin = GPIO_PA19; break;
            case 4:
                pin = GPIO_PA20; break;
            case 5:
                pin = GPIO_PA21; break;
            case 6:
                pin = GPIO_PA22; break;
            case 7:
                pin = GPIO_PA23; break;

            default:
                return 1;
        }
        ret = ioctl(fd, value?GPIO_SET_PIN:GPIO_CLR_PIN, pin);
        printf ("OUT[%d]: %d\n", idx, value);
    }
    else if (argc == 2 && !strcmp(argv[1], "test")) {

        ret = gpio_test(fd);
    }
    else if (argc == 2 && !strcmp(argv[1], "-h")) {
        print_usage(argv[0]);
    }

    if (fd > 0)
        close (fd);
    return ret;
}
