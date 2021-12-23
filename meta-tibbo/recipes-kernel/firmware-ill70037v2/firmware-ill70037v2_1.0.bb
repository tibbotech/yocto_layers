SUMMARY = "CTP-ILL70037V2+GT11 touch firmware files"
DESCRIPTION = "Firmware (config) for CTP-ILL70037V2+GT11touch (development)."
LICENSE = "GPLv2+"
SRCREV="${AUTOREV}"

SRC_URI  = "file://GT911_Config_20191113_083123_V67.cfg"

S = "${WORKDIR}"

inherit allarch

do_install() {
    install -d ${D}${nonarch_base_libdir}/firmware/
    install -m 0644 ${S}/GT911_Config_20191113_083123_V67.cfg ${D}${nonarch_base_libdir}/firmware/
}

FILES:${PN} += "${nonarch_base_libdir}/firmware/*"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
