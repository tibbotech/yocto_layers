#!/bin/sh

if [ $# -lt 1 ]; then
  echo "Usage $0 <file>"
  exit 1;
fi;

DTS_INC="/opt/tps-shared/dts-orig"

dtc -I dts -O dtb -o $1.dtbo -i ${DTS_INC}/ -@ $1.dts
if [ $? -ne 0 ]; then
  exit 1;
fi;
FN=`basename $1`
cp $1.dtbo /lib/firmware/
mkdir -p /sys/kernel/config/device-tree/overlays/${FN}
echo "${FN}.dtbo" > /sys/kernel/config/device-tree/overlays/${FN}/path
cat /sys/kernel/config/device-tree/overlays/${FN}/status
