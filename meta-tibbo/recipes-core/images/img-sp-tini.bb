DESCRIPTION = "Tibbo Project System (RC analyze)"
LICENSE = "MIT"
PR = "r1"

require inc-tps-tin.inc

IMAGE_FEATURES = ""

IMAGE_INSTALL += "tps-tios-sp7021"

IMAGE_INSTALL += "systemd-analyze"
