SUMMARY = "LibModBus"
SECTION = "network"
HOMEPAGE = "http://libmodbus.org/"
LICENSE = "GPLv2.1"

inherit autotools

SRC_URI  = "http://libmodbus.org/releases/libmodbus-${PV}.tar.gz"

FILES:${PN}  = "/usr/*"
FILES:${PN} += "/lib/*"

SRC_URI[sha256sum] = "d7d9fa94a16edb094e5fdf5d87ae17a0dc3f3e3d687fead81835d9572cf87c16"

LIC_FILES_CHKSUM = "file://COPYING.LESSER;md5=4fbd65380cdd255951079008b364516c"
