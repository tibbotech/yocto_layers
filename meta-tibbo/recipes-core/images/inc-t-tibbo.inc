
IMAGE_INSTALL += "\
    tps-gprs \
    kern-mod-tpsid \
    kern-mod-tfwm \
    kern-mod-twg \
    twgtest \
    dvtests \
    tps-capi \
    "
#    kern-mod-ga1000v0 
#    linux-firmware-sd8686 
#    linux-firmware-sd8688 
#    ga1000-firmware 

IMAGE_INSTALL += "${@bb.utils.contains("MACHINE_FEATURES", "tpsleds", " tpsled ", "",d)}"
IMAGE_INSTALL += " tps-dts "

#TOOLCHAIN_TARGET_TASK_append += " tps-capi-dev"
#TOOLCHAIN_TARGET_TASK_append += " tps-sysds-dev"

IMAGE_INSTALL += "tps-node-demo"
#DEPENDS += "tps-demo-phpliteadmin"

DEPENDS += "tps-fixboot0"

# required here - not forget to build for repos
DEPENDS += "tps-agent"
DEPENDS += "tps-ag-eap"
DEPENDS += "tps-ag-stable"

IMAGE_INSTALL += "tps-wan"

DEPENDS += "tdevmon"
DEPENDS += "tdevmon-kernel"

# tmp
DEPENDS += "tps-i2c-anum-lcd"

#IMAGE_INSTALL += "tps-tios"

#DEPENDS += "tps-tios-ltps"
#DEPENDS += "tps-tios-klm601"
