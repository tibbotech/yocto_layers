
dev="${1}"
sock="${2}"

for s in 115200 57600 38400 19200 9600 4800 2400 115200; do
 stty -F $dev $s
 logger "${sock}: Trying $s speed";
 /usr/sbin/chat -f /etc/chatscripts/115200.chat < $dev > $dev
 if [ $? -eq 0 ]; then
   logger "${sock}: Modem works at $s";
   exit 0;
 fi;
done
exit 1;