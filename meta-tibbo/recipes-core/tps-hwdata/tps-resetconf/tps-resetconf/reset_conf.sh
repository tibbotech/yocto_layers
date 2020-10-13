#!/bin/sh

. /opt/tps-shared/bash/pins

btn_isON="${GPIODEFIN}"

logger "Reset configuration daemon started"

mdbt_p=$(pins_pin "CRST")
pin_ex ${mdbt_p}

if [ $? -ne 0 ]; then  echo "OUT:export pin#${mdbt_p}(CRST)";  exit 1;  fi;

# $1 = secs
wait_press() {
 cnt=0
 ret=0
 while [ ${cnt} -lt $1 ]; do
   btn1_v=$(pin_get_i ${mdbt_p})
   if [ "${btn1_v}" == "${btn_isON}" ]; then  ret=1;  fi;
   usleep 1000000;
   cnt=$(( cnt+1));
 done;
 return $ret;
}

# $1 = secs
wait_full_press() {
 cnt=0
 ret=0
 btn1_v=$(pin_get_i ${mdbt_p})
 if [ "${btn1_v}" != "${btn_isON}" ]; then  return 0;  fi;
 ret=1
 while [ ${cnt} -lt $1 ]; do
   btn1_v=$(pin_get_i ${mdbt_p})
   if [ "${btn1_v}" != "${btn_isON}" ]; then  ret=0;  fi;
   usleep 1000000;
   cnt=$(( cnt+1));
 done;
 return $ret;
}

while [ "1" == "1" ]; do
  sleep 1;
  # wait 1 sec, button has to be 1 for the whole sec
  wait_full_press 1;  x=$?;
  if [ ${x} == 0 ]; then continue;  fi;
  logger "ret#0:${x}"
  # beep 2 secs if waiting...
  /opt/tps-beeper/beep_bell 10
  usleep 100000;
  /opt/tps-beeper/beep_bell 0

#  sleep 2;
#  # beep 2 secs while waiting for press#1
#  /home/root/dvtests/beep_bell 10
#  wait_press 2;  x=$?;
#  /home/root/dvtests/beep_bell 0
#  logger "ret#1:$?"
#  if [ ${x} == 0 ]; then continue;  fi;
#
#  sleep 2;
#  # beep 2 secs while waiting for press#2
#  /home/root/dvtests/beep_bell 10
#  wait_press 2;  x=$?;
#  /home/root/dvtests/beep_bell 0
#  logger "ret#2:$?"
#  if [ ${x} == 0 ]; then continue;  fi;
#
#  sleep 2;
#  # beep 2 secs while waiting for press#3
#  /home/root/dvtests/beep_bell 10
#  wait_press 2;  x=$?;
#  /home/root/dvtests/beep_bell 0
#  logger "ret#3:$?"
#  if [ ${x} == 0 ]; then continue;  fi;
  # got seq!
  logger "Doing conf reset..."
  cp -f /etc/systemd/network/eth0.network.orig /etc/systemd/network/eth0.network
  cp -f /var/www/tps-wan/conf/.htpasswd.orig /var/www/tps-wan/conf/.htpasswd
  usermod -p `openssl passwd -1 -salt tibbo 123` root
  sync; sync; reboot
done;
