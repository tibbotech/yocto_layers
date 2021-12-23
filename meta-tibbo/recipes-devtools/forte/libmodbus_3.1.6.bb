SUMMARY = "LibModBus"
SECTION = "network"
HOMEPAGE = "http://libmodbus.org/"
LICENSE = "GPLv2.1"

inherit autotools

SRC_URI  = "http://libmodbus.org/releases/libmodbus-${PV}.tar.gz"

FILES:${PN}  = "/usr/*"
FILES:${PN} += "/lib/*"

SRC_URI[md5sum] = "69cb3ebe134d1f1e2768c2127ed9b5d6"
SRC_URI[sha256sum] = "022f0691d920b8aee3ee49d7af0f69b7ef80fc3c849a8e0281d5bc27db7a24ea"

LIC_FILES_CHKSUM = "file://COPYING.LESSER;md5=c79ff39f19dfec6d293b95dea7b07891"
