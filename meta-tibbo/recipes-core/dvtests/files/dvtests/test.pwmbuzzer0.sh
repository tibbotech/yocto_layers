#!/bin/sh

# pin 0x194 have to be in mode7, out

echo "101" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio101/direction

while true; do
 echo 0 > /sys/class/gpio/gpio101/value
 echo 1 > /sys/class/gpio/gpio101/value
done;