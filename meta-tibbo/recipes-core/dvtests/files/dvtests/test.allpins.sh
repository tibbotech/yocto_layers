#!/bin/sh

. /opt/tps-shared/bash/pins

ttsleep=${1:-1}

pin_test() {
 x=$(pins_pin ${1})
 echo -ne "Pin ${1}(#${x})\t"
 pin_ex ${x}
 if [ $? -ne 0 ]; then
   echo "ERR: export"
   return 1;
 fi;
 pin_dir ${x} "out"
 pin_set ${x} "0"
 echo -n "1"
 sleep 1
 pin_set ${x} "1"
 echo -n "0"
 sleep 1
 echo
 return 0
}

egrep "(S[0-9]+[ABCD])|(MD)|(WIFI-\w+).*=" /opt/tps-shared/hwini/pins.ini | grep -v GPIO | while read p0 p1 p2; do
  sleep ${ttsleep}
  pin_test $p0
done;