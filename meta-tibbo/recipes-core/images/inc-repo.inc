
# dbs
DEPENDS += "mariadb"
DEPENDS += "influxdb"

# multimedia
DEPENDS += "opencv"
DEPENDS += "ncnn"
DEPENDS += "sox"
#DEPENDS += "tensorflow"
DEPENDS += "v4l-utils"
DEPENDS += "gstreamer1.0-plugins-base"
DEPENDS += "gstreamer1.0-plugins-good"
DEPENDS += "gstreamer1.0-plugins-bad"
DEPENDS += "gstreamer1.0-libav"
DEPENDS += "vlc"
#DEPENDS += "mjpg-streamer"
DEPENDS += "ffmpeg"
#DEPENDS += "live555"
#DEPENDS += "minidlna"
#DEPENDS += "gerbera"
DEPENDS += "alsa-utils alsa-plugins alsa-tools ${VIRTUAL-RUNTIME_alsa-state}"

# utility
DEPENDS += "psmisc"
DEPENDS += "mc"
DEPENDS += "iproute2"
DEPENDS += "nano"
DEPENDS += "syslog-ng"
DEPENDS += "logrotate"
DEPENDS += "gptfdisk cifs-utils dosfstools e2fsprogs mtd-utils"

# connectivity
DEPENDS += "wget"
DEPENDS += "lftp"
DEPENDS += "shorewall"
DEPENDS += "mosquitto"
DEPENDS += "freeradius"
DEPENDS += "ethtool"
DEPENDS += "usbutils usb-modeswitch"
DEPENDS += "net-snmp"
#DEPENDS += "ddclient"
# VPNs
DEPENDS += "rp-pppoe"
DEPENDS += "openvpn"
DEPENDS += "wireguard-tools"
DEPENDS += "pptp-linux"
DEPENDS += "xl2tpd"

# development
DEPENDS += "cmake"
DEPENDS += "libgpiod"
DEPENDS += "gdb"
DEPENDS += "tcf-agent"
DEPENDS += "strace"
DEPENDS += "tcpdump"
DEPENDS += "git"
DEPENDS += "minicom"
DEPENDS += "picocom"
DEPENDS += "statserial"
DEPENDS += "valgrind"
DEPENDS += "oprofile"
DEPENDS += "lsof"
DEPENDS += "atop"
DEPENDS += "htop"
DEPENDS += "i2c-tools"
DEPENDS += "can-utils"
DEPENDS += "psmisc"
DEPENDS += "devmem2"
DEPENDS += "lshw"
# requires gtk2
#DEPENDS += "hardinfo"
DEPENDS += "lrzsz"
DEPENDS += "netcat"
DEPENDS += "nmap"

# nodeJS
DEPENDS += "node-red"
#DEPENDS += "node-sqlite3"
DEPENDS += "node-better-sqlite3"
#DEPENDS += "node-serialport"
#DEPENDS += "node-modbus-serial"
##DEPENDS += "node-opencv"
DEPENDS += "node-red-node-sqlite"
DEPENDS += "node-red-contrib-ibm-watson-iot"
DEPENDS += "node-red-node-serialport"
DEPENDS += "node-red-contrib-modbus"
DEPENDS += "node-red-contrib-modbustcp"
#DEPENDS += "node-nvd3"
#DEPENDS += "node-socketcan"
#DEPENDS += "node-i2c-bus"
DEPENDS += "node-libgpiod"
DEPENDS += "node-red-contrib-libgpiod"

# python
DEPENDS += "python3-pip"
DEPENDS += "python3-cython"
#DEPENDS += "python3-opencv"
#DEPENDS += "python3-scikit-learn"

# tests and benchmarks
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
DEPENDS += "serial-test"

DEPENDS += "log4cpp"

DEPENDS += "fuse-exfat exfat-utils"

#TOOLCHAIN_HOST_TASK_append += "nativesdk-dlib-dev"

#DEPENDS += "apt"

DEPENDS += "dstat"
DEPENDS += "nmap"
DEPENDS += "zabbix"
DEPENDS += "grafana"
DEPENDS += "prometheus"
DEPENDS += "prometheus-node-exporter"
DEPENDS += "python3-prometheus-client"

# GUI
DEPENDS += "openbox"
DEPENDS += "twm"
#DEPENDS += "matchbox-wm"
#DEPENDS += "matchbox-desktop"
DEPENDS += "packagegroup-core-x11-base"
DEPENDS += "packagegroup-core-x11-sato"
#DEPENDS += "packagegroup-xfce-base"

# graphics
DEPENDS += "epiphany"
DEPENDS += "surf"
DEPENDS += "fbset"
DEPENDS += "fbida"

# realtime protocols

# OpenAMP
DEPENDS += "${@bb.utils.contains("DISTRO_FEATURES", "openamp", "libmetal open-amp", "",d)}"
DEPENDS += "${@bb.utils.contains("DISTRO_FEATURES", "openamp", "rpmsg-echo-test rpmsg-mat-mul rpmsg-proxy-app", "",d)}"
# OpenAMP /
DEPENDS += "soem"
#DEPENDS += "soes"
# iec-...
DEPENDS += "forte"

# tmp
#DEPENDS += "linux-firmware"
#DEPENDS += "dhcp"

DEPENDS += "dkms"
