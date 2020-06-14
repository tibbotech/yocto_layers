#!/bin/sh

echo "["
grep "country " ${1} | sed -e 's/\:.*//' | while read x0 CD; do
  egrep "^${CD} " ${2} | while read y0 c; do echo "{ \"code\":\"${CD}\",\"country\":\"${c}\"},"; done;
done;
echo "]"
