SUMMARY = "Bluetooth Audio ALSA Backend"
DESCRIPTION = "Is a rebirth of a direct integration between Bluez and ALSA."
HOMEPAGE = "https://github.com/Arkq/bluez-alsa"
SECTION = "libs/multimedia"
LICENSE = "MIT"

DEPENDS  = "ortp libbsd bluez5 sbc"
DEPENDS += "fdk-aac"

SRCREV = "${AUTOREV}"

SRC_URI  = "git://github.com/Arkq/bluez-alsa.git;protocol=https;branch=master"
SRC_URI += "file://bluez-alsa.service"

S = "${WORKDIR}/git"

inherit autotools pkgconfig systemd

EXTRA_OECONF += "--enable-aac"
#EXTRA_OECONF += "--enable-debug"

FILES:${PN} += "/usr/share/*"
FILES:${PN} += "/usr/lib/alsa-lib/libasound_*.so"

FILES:${PN}-staticdev += "/usr/lib/alsa-lib/libasound_*.a"
FILES:${PN}-staticdev += "/usr/lib/alsa-lib/libasound_*.la"

#FILES:${PN}-dev += "/usr/lib/*"

do_install:append() {
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${WORKDIR}/${PN}.service ${D}${systemd_unitdir}/system
}

SYSTEMD_SERVICE:${PN} = "${PN}.service"

LIC_FILES_CHKSUM = "file://LICENSE;md5=0835ade698e0bcf8506ecda2f7b4f302"
