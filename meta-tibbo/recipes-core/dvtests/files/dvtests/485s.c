#include <linux/serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>

/* Driver-specific ioctls: */
#define TIOCGRS485 0x542E
#define TIOCSRS485 0x542F

/* new defines in linux/serial.h of CL provided kernel */
#ifndef SER_RS485_USE_GPIO
#define SER_RS485_USE_GPIO (1<<5)
#define gpio_pin padding[1]
#endif

#define GPIO3_9 (32*3+9)

#define USE2

/*
 * Test program for 485 control
 */
int main(int argc, char* argv[])
{
	struct serial_rs485 rs485conf;
	char buffer[20], buffer2[21];
	int i, rv, opt;
	struct termios my_termios, new_termios;

	int fd0 = open( argv[ 1], O_RDWR | O_NDELAY | O_NOCTTY);

	rs485conf.flags = (1<<0); //SER_RS485_ENABLED;
//	rs485conf.flags |= SER_RS485_USE_GPIO;
	rs485conf.delay_rts_before_send = 0;
// my
	rs485conf.delay_rts_after_send = 0;
        rs485conf.flags |= SER_RS485_RTS_ON_SEND;
//	rs485conf.gpio_pin = GPIO3_9;

	rv = ioctl (fd0, TIOCSRS485, &rs485conf);
	if (rv) {
		printf("rv = %d\n", rv);
		perror("unable to set IOCTL:");
	}

	tcgetattr(fd0, &my_termios);
	my_termios.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	my_termios.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);
	my_termios.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG | ECHOK | ECHOCTL | ECHOPRT);
	my_termios.c_cflag &= ~(CSIZE | PARENB);
	my_termios.c_cflag &= ~CRTSCTS;
	my_termios.c_cflag |= CS8 | CLOCAL;
	my_termios.c_cflag &= ~CBAUD;
//	my_termios.c_cflag |= B115200; // B19200; // 
	my_termios.c_cflag |= B921600; // B19200; // 
	my_termios.c_cc[VMIN] = 1;
	my_termios.c_cc[VTIME] = 0;
	rv = tcsetattr(fd0, TCSANOW, &my_termios);
	tcgetattr(fd0, &new_termios);

	for (i = 0; i < 20; i++) {
		buffer[i] = 'A'+i;
	}

	while ( 1) {
//	for (i = 0; i < 30; i++) {
//		printf("Writing [%d]\n", i);
		write(fd0, buffer, 20);
//		sleep(1);
	}
	close(fd0);
}
