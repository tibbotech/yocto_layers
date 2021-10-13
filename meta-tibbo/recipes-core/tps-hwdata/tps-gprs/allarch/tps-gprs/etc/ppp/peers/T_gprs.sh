
dev="${1}"
sock="${2}"

for s in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
 logger "${sock}: Waiting for GPRS $s time...";
 echo -n "" > /tmp/${sock}
# /usr/sbin/chat -v -f /etc/chatscripts/gprs_ready.chat < $dev > $dev 2>/tmp/${sock}x
 /usr/sbin/chat -v -f /etc/chatscripts/gprs_ready.chat 2>/tmp/${sock}
 if [ $? -eq 0 ]; then
   logger "${sock}: GPRS enabled";
#   /usr/sbin/chat -VsS "" "AT+GSN" OK 2> /tmp/xx
   grep -A 3 "GSN" /tmp/${sock} | grep -v "AT" | grep -v "^$" | grep -v "OK" > /tmp/${sock}_imei
   grep -A 3 "CSQ" /tmp/${sock} | grep -v "AT" | grep -v "^$" | grep -v "OK" | sed -e 's/.*CSQ: //' > /tmp/${sock}_sigs
   logger "${sock}: IMEI "`cat /tmp/${sock}_imei`;
   logger "${sock}: SIGS "`cat /tmp/${sock}_sigs`;
   exit 0;
 fi;
 logger "${sock}: Retry in 10 sec";
 sleep 10
done
logger "${sock}: No cell networks found?!";
exit 1;