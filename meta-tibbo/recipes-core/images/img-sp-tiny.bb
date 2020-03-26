DESCRIPTION = "Tibbo Project System"
LICENSE = "MIT"
PR = "r1"

require inc-tps-tin.inc
require inc-tps-depends.inc
require inc-tps-privdeps.inc

IMAGE_FEATURES = ""

#IMAGE_INSTALL += "tps-tios-sp7021"

# add packets for initramfs there
PACKAGE_INSTALL = "${IMAGE_INSTALL}"
