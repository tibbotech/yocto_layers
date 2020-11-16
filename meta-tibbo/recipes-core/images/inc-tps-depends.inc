
# dbs
DEPENDS += "mariadb"

# multimedia
DEPENDS += "opencv"
DEPENDS += "ncnn"
#DEPENDS += "tensorflow"
DEPENDS += "v4l-utils"
DEPENDS += "gstreamer1.0-plugins-base"
DEPENDS += "gstreamer1.0-plugins-good"
DEPENDS += "gstreamer1.0-plugins-bad"
DEPENDS += "gstreamer1.0-libav"

# connectivity
DEPENDS += "shorewall"
DEPENDS += "mosquitto"
DEPENDS += "freeradius"
DEPENDS += "ethtool"
DEPENDS += "usbutils"

# development
DEPENDS += "gdb"
DEPENDS += "strace"
DEPENDS += "tcpdump"
DEPENDS += "minicom"
DEPENDS += "statserial"
DEPENDS += "valgrind"
DEPENDS += "oprofile"
DEPENDS += "lsof"
DEPENDS += "i2c-tools"
DEPENDS += "can-utils"
DEPENDS += "psmisc"
DEPENDS += "devmem2"

# nodeJS
#DEPENDS += "node-red"
#DEPENDS += "node-sqlite3"
#DEPENDS += "node-serialport"
#DEPENDS += "node-modbus-serial"
##DEPENDS += "node-opencv"
#DEPENDS += "node-red-node-sqlite"
#DEPENDS += "node-red-contrib-ibm-watson-iot"
#DEPENDS += "node-red-node-serialport"
#DEPENDS += "node-red-contrib-modbus"
#DEPENDS += "node-red-contrib-modbustcp"
#DEPENDS += "node-nvd3"
#DEPENDS += "node-socketcan"
#DEPENDS += "node-i2c-bus"

# python
DEPENDS += "python3-pip"
DEPENDS += "python3-cython"
#DEPENDS += "python3-opencv"
#DEPENDS += "python3-scikit-learn"

# tests and benchmarks
DEPENDS += "dvtests"
DEPENDS += "iperf3"
DEPENDS += "stress-ng"
DEPENDS += "bonnie++"
DEPENDS += "cpuburn-arm"
DEPENDS += "fio"
#DEPENDS += "glmark2"
DEPENDS += "linpack"
DEPENDS += "lmbench"
DEPENDS += "memtester"
DEPENDS += "nbench-byte"
DEPENDS += "s-suite"
DEPENDS += "sysbench"
DEPENDS += "tinymembench"
DEPENDS += "tiobench"
DEPENDS += "whetstone"

# cli tools
DEPENDS += "mc"
DEPENDS += "nano"
DEPENDS += "syslog-ng"
DEPENDS += "logrotate"
DEPENDS += "log4cpp"

DEPENDS += "fuse-exfat exfat-utils"

#TOOLCHAIN_HOST_TASK_append += " nativesdk-dlib-dev"
#TOOLCHAIN_HOST_TASK_append += " log4cpp-dev"
