DESCRIPTION = "Tibbo Project System + AggreGate"
LICENSE = "MIT"
PR = "r1"

require inc-tps.inc

# IMAGE_INSTALL += "tps-tios-ltps"

IMAGE_INSTALL += "tps-resetconf"
IMAGE_INSTALL += "tps-wan"

# aggregate-agent
IMAGE_INSTALL += "tps-agent"
#IMAGE_INSTALL += "tps-ag"
#IMAGE_INSTALL += "tps-agsme"
IMAGE_INSTALL += "tps-ag-stable"

IMAGE_INSTALL += " openssh-sftp-server"
IMAGE_INSTALL += " git"
IMAGE_INSTALL += "tps-node-demo"
