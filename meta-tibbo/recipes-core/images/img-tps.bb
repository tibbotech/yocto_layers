DESCRIPTION = "Tibbo Project System"
LICENSE = "MIT"
PR = "r1"

require inc-tps.inc

# irq test
IMAGE_INSTALL += "kern-mod-dvgit"

## dhcpc
#IMAGE_INSTALL += "\
#    busybox-udhcpc \
#"

# barcodes
#    dmtx-utils

