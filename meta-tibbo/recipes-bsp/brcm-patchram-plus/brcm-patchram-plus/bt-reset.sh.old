#!/bin/sh

gpfx="/sys/class/gpio"

if [ ! -f ${gpfx}/P10_02/direction ]; then
  echo 82 > ${gpfx}/export
fi;
echo out > ${gpfx}/P10_02/direction
echo 0 > ${gpfx}/P10_02/value
echo 1 > ${gpfx}/P10_02/value
