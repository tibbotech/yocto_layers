#!/bin/sh

. /opt/tps-shared/bash/pins

PIN_R=$(pins_pin "SCRESET")
PIN_P=$(pins_pin "SCPOWER")
PIN_S=$(pins_pin "SCSTATUS")

if [ -z "${PIN_R}" ]; then
  echo "Pin SCRESET not found in pins.ini"
  exit 1;
fi;
if [ -z "${PIN_P}" ]; then
  echo "Pin SCPOWER not found in pins.ini"
  exit 1;
fi;
if [ -z "${PIN_S}" ]; then
  echo "Pin SCSTATUS not found in pins.ini"
  exit 1;
fi;

pin_ex ${PIN_R}
if [ $? -ne 0 ]; then
  echo "PIN_R:${PIN_R}:$(pin_name $PIN_R) export error"
  exit 1;
fi;
pin_ex ${PIN_P}
if [ $? -ne 0 ]; then
  echo "PIN_P:${PIN_P}:$(pin_name $PIN_P) export error"
  exit 1;
fi;
pin_ex ${PIN_S}
if [ $? -ne 0 ]; then
  echo "PIN_S:${PIN_S}:$(pin_name $PIN_S) export error"
  exit 1;
fi;

pin_dir ${PIN_R} "out"
pin_dir ${PIN_P} "out"
pin_dir ${PIN_S} "in"

pin_set ${PIN_R} "1"
sleep 0.1
pin_set ${PIN_R} "0"
sleep 1
pin_set ${PIN_P} "1"
sleep 1.5
