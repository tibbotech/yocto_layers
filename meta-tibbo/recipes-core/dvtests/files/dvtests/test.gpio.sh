#!/bin/sh

f="/sys/class/gpio"

if [ $# = 0 ]; then
  echo "$0 <legnum>";
  exit 1;
fi;

leg=$1

if [ ! -d "${f}/gpio${leg}" ]; then
  echo $leg > $f/export
  if [ $? -ne 0 ];  then
    echo "Error export leg#${leg}"
    exit 1;
  fi;
fi;


while [ 1 = 1 ]; do
  echo "out,0..."
  echo "out" > $f/gpio${leg}/direction
  echo "0" > $f/gpio${leg}/value
  echo "out,0..."
  sleep 5
  echo "out,1..."
  echo "1" > $f/gpio${leg}/value
  sleep 5
  echo "in..."
  echo "in" > $f/gpio${leg}/direction
  sleep 5
done;
