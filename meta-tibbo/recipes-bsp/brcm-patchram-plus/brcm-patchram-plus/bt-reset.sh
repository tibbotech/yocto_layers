#!/bin/sh

. /opt/tps-shared/bash/pins

rpin=$(pins_pin "BTEN")
if [ -z "${rpin}" ]; then
  echo "Pin BTEN not found in pins.ini"
  exit 1;
fi;
pin_ex ${rpin}
if [ $? -ne 0 ]; then
  echo "BTEN:${rpin}:$(pin_name $rpin) export error"
  exit 1;
fi;
pin_dir ${rpin} "out"
pin_set ${rpin} "0"
pin_set ${rpin} "1"
