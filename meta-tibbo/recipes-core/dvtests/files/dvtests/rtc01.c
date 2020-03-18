/*   rtc-test.c
 *
 *   Tests for the Real Time Clock driver.
 *
 *   Copyright (c) Larsen & Toubro Infotech Ltd., 2010
 *   Copyright (c) 2013 Oracle and/or its affiliates. All Rights Reserved.
 *
 *   Author : Silesh C V <Silesh.Vellattu@lntinfotech.com>
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <errno.h>
#include <time.h>
#include <sys/klog.h>

#define KERN_ERR 3

#define tst_brkm(_l,_x...) {  printf( _x);  exit( 1);  }

int rtc_fd = -1;
char *TCID = "rtc01";
int TST_TOTAL = 3;

static char *rtc_dev = "/dev/rtc";
static int dflag;
//static const option_t options[] = {
//	{"d:", &dflag, &rtc_dev},
//	{NULL, NULL, NULL}
//};

static void help(void)
{
	printf("  -d x    rtc device node, default is %s\n",
		rtc_dev);
}

/* Read and Alarm Tests :  Read test reads the Date/time from RTC
 * while Alarm test, sets the alarm to 5 seconds in future and
 * waits for it to ring.The ioctls tested in these tests are
 * RTC_RD_TIME, RTC_ALM_SET, RTC_ALM_READ, RTC_AIE_OFF  */

void read_alarm_test(void)
{
	struct rtc_time rtc_tm;
	int ret;
	unsigned long data;
	fd_set rfds;
	struct timeval tv;

	printf( "RTC READ TEST:\n");

	/*Read RTC Time */
	ret = ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm);
	if (ret == -1) {
		printf( "RTC_RD_TIME ioctl failed\n");
		return;
	}

	printf( "RTC READ TEST Passed\n");

	printf( "Current RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		 rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		 rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	printf( "RTC ALARM TEST :\n");

	/*set Alarm to 5 Seconds */
	rtc_tm.tm_sec += 5;
	if (rtc_tm.tm_sec >= 60) {
		rtc_tm.tm_sec %= 60;
		rtc_tm.tm_min++;
	}

	if (rtc_tm.tm_min == 60) {
		rtc_tm.tm_min = 0;
		rtc_tm.tm_hour++;
	}

	if (rtc_tm.tm_hour == 24)
		rtc_tm.tm_hour = 0;


syslog( KERN_ERR, "RTC_ALM_SET 0");  sync();
	ret = ioctl(rtc_fd, RTC_ALM_SET, &rtc_tm);
	if (ret == -1) {
		printf( "RTC_ALM_SET ioctl failed\n");
		return;
	}

syslog( KERN_ERR, "RTC_ALM_READ 0");  sync();
	/*Read current alarm time */
	ret = ioctl(rtc_fd, RTC_ALM_READ, &rtc_tm);
	if (ret == -1) {
		printf( "RTC_ALM_READ ioctl failed\n");
		return;
	}

syslog( KERN_ERR, "RTC_AIE_ON 0");  sync();
	printf( "Alarm time set to %02d:%02d:%02d.\n",
		 rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	/* Enable alarm interrupts */
	ret = ioctl(rtc_fd, RTC_AIE_ON, 0);
	if (ret == -1) {
		printf( "RTC_AIE_ON ioctl failed\n");
		return;
	}

	printf( "Waiting 5 seconds for the alarm...\n");

	tv.tv_sec = 6;		/*set 6 seconds as the time out */
	tv.tv_usec = 0;

	FD_ZERO(&rfds);
	FD_SET(rtc_fd, &rfds);

syslog( KERN_ERR, "test select");  sync();
	ret = select(rtc_fd + 1, &rfds, NULL, NULL, &tv);	/*wait for alarm */

	if (ret == -1) {
		printf( "select failed\n");
		return;
	} else if (ret) {
		ret = read(rtc_fd, &data, sizeof(unsigned long));
		if (ret == -1) {
			printf( "read failed\n");
			return;
		}
		printf( "Alarm rang.\n");
	} else {
		printf( "Timed out waiting for the alarm\n");
		return;
	}

syslog( KERN_ERR, "RTC_AIE_OFF 0");  sync();
	/* Disable alarm interrupts */
	ret = ioctl(rtc_fd, RTC_AIE_OFF, 0);
	if (ret == -1) {
		printf( "RTC_AIE_OFF ioctl failed\n");
		return;
	}
	printf( "RTC ALARM TEST Passed\n");
}

/* Update_interrupts_test :Once the Update interrupts is enabled,
 * the RTC gives interrupts (1/sec) on the interrupts line(if the rtc
 * has one). This is tested by enabling the update interrupts
 * and then waiting for 5 interrupts.*/

void update_interrupts_test(void)
{
	int ret, i;
	unsigned long data;
	fd_set rfds;
	struct timeval tv;

syslog( KERN_ERR, "RTC_UIE_ON 0");  sync();
	printf( "RTC UPDATE INTERRUPTS TEST :\n");
	/*Turn on update interrupts */
	ret = ioctl(rtc_fd, RTC_UIE_ON, 0);
	if (ret == -1) {
		printf( "RTC_UIE_ON ioctl failed\n");
		return;
	}

syslog( KERN_ERR, "selecting 5 sec...");  sync();
	printf( "Waiting for  5 update interrupts...\n");
	for (i = 1; i < 6; i++) {

		tv.tv_sec = 2;	/*2 sec time out for each interrupt */
		tv.tv_usec = 0;

		FD_ZERO(&rfds);
		FD_SET(rtc_fd, &rfds);

		ret = select(rtc_fd + 1, &rfds, NULL, NULL, &tv);
		if (ret == -1) {
			printf( "select failed\n");
			return;
		} else if (ret) {
			ret = read(rtc_fd, &data, sizeof(unsigned long));
			if (ret == -1) {
				printf( "read failed\n");
				return;
			}
			printf( "Update interrupt %d\n", i);
		} else {
			printf( "Timed out waiting for the update interrupt\n");
			return;
		}
	}

syslog( KERN_ERR, "RTC_UIE_OFF 0");  sync();
	/* Turn off update interrupts */
	ret = ioctl(rtc_fd, RTC_UIE_OFF, 0);
	if (ret == -1) {
		printf( "RTC_UIE_OFF ioctl failed\n");
		return;
	}
	printf( "RTC UPDATE INTERRUPTS TEST Passed");
}

int main(int argc, char *argv[])
{
//	tst_parse_opts(argc, argv, options, help);

//	tst_require_root(NULL);

	if (access(rtc_dev, F_OK) == -1)
		tst_brkm(TCONF, NULL, "couldn't find rtc device '%s'\n", rtc_dev);

	rtc_fd = open(rtc_dev, O_RDONLY);

	if (rtc_fd < 0)
		tst_brkm(TBROK | TERRNO, NULL, "couldn't open %s\n", rtc_dev);

	/*Read and alarm tests */
	read_alarm_test();

	/*Update interrupts test */
	update_interrupts_test();

	close(rtc_fd);

	printf( "RTC Tests Done!");
	exit( 0);
}
