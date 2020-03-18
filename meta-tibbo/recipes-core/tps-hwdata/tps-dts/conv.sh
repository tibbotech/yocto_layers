#!/bin/sh

. /opt/tps-dts/part_save

if [ $# -lt 1 ]; then
  echo "Usage $0 <file>"
  exit 1;
fi;

DTS_PFX="/opt/tps-shared"

f_type=""
f_ext=`echo "$1" | sed -e 's/.*\.//'`
f_name="$1"

if [ ! -f "${f_name}" ]; then
  echo "No file ${f_name}"
  exit 1
fi;

case "$f_ext" in
  "ini" )  f_type="ini"  ;;
  "dts" )  f_type="dts"  ;;
  "dtb" )  f_type="dtb"  ;;
  * )
    echo "Unknown input type '$t'"
    exit 1
  ;;
esac;

# ???
f_name=`echo "${f_name}" | sed -e "s/\.${f_ext}//"`
#echo "type:$f_type f_n:$f_name f_p:$f_p"

if [ "$f_type" == "ini" ]; then
  f_type="dts"
  /opt/tps-dts/ini2dts ${DTS_PFX}/dts-tpl/template.dtt "${f_name}.ini" "${f_name}.${f_type}"
  if [ $? -ne 0 ]; then
    echo "INI->DTS convertion "
    exit 1
  fi;
fi;
if [ ! -f "${f_name}.${f_type}" ]; then
  echo "No file ${f_name}.${f_type}"
  exit 1
fi;
if [ "$f_type" == "dts" ]; then
  f_type="dtb"
  /usr/bin/mcpp -N -a -Wp -I${DTS_PFX}/dts-orig -I${DTS_PFX}/dts-orig/include -D__DTS__ -o "${f_name}.dts.tmp" "${f_name}.dts"
  if [ $? -ne 0 ]; then
    echo "DTC preprocessor error"
    exit 1
  fi;
  dtc -I dts -O dtb -o "${f_name}.${f_type}" -i ${DTS_PFX}/dts-orig "${f_name}.dts.tmp"
  if [ $? -ne 0 ]; then
    echo "DTC compiler error"
    exit 1
  fi;
fi;
if [ ! -f "${f_name}.dtb" ]; then
  echo "No file ${f_name}.${f_type}"
  exit 1
fi;
if [ "$f_type" == "dtb" ]; then
  dtc -I dtb "${f_name}.${f_type}" > /dev/null
  if [ $? -ne 0 ]; then
    exit 1
  fi;
fi;


part_save "${f_name}";  x=$?;
if [ $x -ne 0 ]; then
  echo "Write error"
  exit 1;
fi;

echo "DONE"

t=`basename "$1"`
if [ "${t}" == "pins.ini" ]; then
  exit 0;
fi;

if [ "${f_ext}" == "ini" ]; then
  cp -f "${f_name}.ini" /opt/tps-shared/hwini/
  sync;
  sync;
fi;
