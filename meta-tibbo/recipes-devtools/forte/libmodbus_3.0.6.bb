SUMMARY = "LibModBus"
SECTION = "network"
HOMEPAGE = "http://libmodbus.org/"
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-3.0;md5=c79ff39f19dfec6d293b95dea7b07891"

inherit autotools

SRC_URI  = "http://libmodbus.org/releases/libmodbus-${PV}.tar.gz"

FILES_${PN}  = "/usr/*"
FILES_${PN} += "/lib/*"

SRC_URI[md5sum] = "c80f88b6ca19cabc4ceffc195ca07771"
SRC_URI[sha256sum] = "046d63f10f755e2160dc56ef681e5f5ad3862a57c1955fd82e0ce036b69471b6"
