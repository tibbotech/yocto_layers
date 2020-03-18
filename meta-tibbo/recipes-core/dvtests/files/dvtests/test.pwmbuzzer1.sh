#!/bin/sh

#p="100000"
#d="50000"
p="100000000"
d="50000000"

if [ ! -d "/sys/class/pwm/pwmchip0/pwm1" ]; then
 echo "1" > /sys/class/pwm/pwmchip0/export
 fi;

echo "e0"
echo "0" > /sys/class/pwm/pwmchip0/pwm1/enable

echo "p"
echo $p > /sys/class/pwm/pwmchip0/pwm1/period

echo "d"
echo $d > /sys/class/pwm/pwmchip0/pwm1/duty_cycle

echo "e1"
echo "1" > /sys/class/pwm/pwmchip0/pwm1/enable
