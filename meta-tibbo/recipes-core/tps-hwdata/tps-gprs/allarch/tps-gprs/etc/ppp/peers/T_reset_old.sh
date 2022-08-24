
f_pins="/opt/tps-shared/hwini/pins.ini"

dev="${1}"
sock="${2}"

. /opt/tps-shared/bash/pins

ttmp=`grep -i "${sock}" $f_pins | grep -v GPIO`

A=`echo "$ttmp" | grep "A.*=" | sed -e 's/.*=\s*//'`
B=`echo "$ttmp" | grep "B.*=" | sed -e 's/.*=\s*//'`
C=`echo "$ttmp" | grep "C.*=" | sed -e 's/.*=\s*//'`
D=`echo "$ttmp" | grep "D.*=" | sed -e 's/.*=\s*//'`

logger "Modem at ${sock} A:${A} B:${B} C:${C} D:${D}"

p_SDWIN=$A
p_PWRKEY=$B
p_RESET=$C
p_STATUS=$D

pin_ex ${p_SDWIN}
if [ $? -ne 0 ]; then  exit 1;  fi;
pin_ex ${p_PWRKEY}
if [ $? -ne 0 ]; then  exit 1;  fi;
pin_ex ${p_RESET}
if [ $? -ne 0 ]; then  exit 1;  fi;
pin_ex ${p_STATUS}
if [ $? -ne 0 ]; then  exit 1;  fi;

pin_dir ${p_SDWIN} "out"
pin_dir ${p_PWRKEY} "out"
pin_dir ${p_RESET} "out"
pin_dir ${p_STATUS} "in"

#----------------------
logger "${sock}: Powering up the modem...";
pin_set ${p_RESET} "1"
pin_set ${p_SDWIN} "1"
sleep 1
pin_set ${p_PWRKEY} "1"
sleep 2
pin_set ${p_PWRKEY} "0"
sleep 2
pin_set ${p_RESET} "0"
sleep 5

i=0
s=0
while [[ $i -le 25 ]]; do
  s=$(pin_get_i ${p_STATUS})
#  logger "iter:${i} s:${s}"
  if [ $s -eq 0 ]; then break;  fi;
  ((i+=1));
  sleep 1;
done;
if [ $s -eq 0 ]; then
  logger "${sock}: Modem is ready after powering up";
  exit 0;
fi;
#----------------------
logger "${sock}: Powering down the modem...";
pin_set ${p_RESET} "0"
pin_set ${p_SDWIN} "0"
sleep 2
pin_set ${p_SDWIN} "1"
exit 1
#----------------------
