SUMMARY = "Broadcom bcmdhd firmware files"
DESCRIPTION = "Firmware for Broadcom BCMDHD (development AP6255)."

SRC_URI  = "file://fw_bcm43456c5_ag.bin"
SRC_URI += "file://fw_bcm43455c0_ag.bin"
SRC_URI += "file://nvram_ap6256.txt"
SRC_URI += "file://nvram_43455.txt"

S = "${WORKDIR}"

inherit allarch

#CLEANBROKEN = "1"

do_install() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/
    install ${S}/fw_bcm43456c5_ag.bin ${D}${nonarch_base_libdir}/firmware/brcm/
    install ${S}/fw_bcm43455c0_ag.bin ${D}${nonarch_base_libdir}/firmware/brcm/
    install ${S}/nvram_ap6256.txt ${D}${nonarch_base_libdir}/firmware/brcm/
    install ${S}/nvram_43455.txt ${D}${nonarch_base_libdir}/firmware/brcm/
}

FILES_${PN} += "${nonarch_base_libdir}/firmware/brcm/*"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
