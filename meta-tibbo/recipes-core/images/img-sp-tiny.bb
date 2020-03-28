DESCRIPTION = "Tibbo Project System"
LICENSE = "MIT"
PR = "r1"

require inc-tps-tin.inc
require inc-tps-depends.inc

IMAGE_FEATURES = ""

# add packets for initramfs there
PACKAGE_INSTALL = "${IMAGE_INSTALL}"
