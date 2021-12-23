SUMMARY = "Broadcom bcmdhd firmware files"
DESCRIPTION = "Firmware for Broadcom BCMDHD (development AP6255)."
LICENSE = "Broadcom"
SRCREV="${AUTOREV}"

SRC_URI  = "git://github.com/tibbotech/bcmdhd-firmware.git;protocol=https;branch=master"

S = "${WORKDIR}/git"

inherit allarch

do_install() {
 install -d ${D}${nonarch_base_libdir}/firmware/brcm/
 install ${S}/fw_bcm43456c5_ag.bin ${D}${nonarch_base_libdir}/firmware/brcm/
 install ${S}/fw_bcm43455c0_ag.bin ${D}${nonarch_base_libdir}/firmware/brcm/
 install ${S}/nvram_ap6256.txt ${D}${nonarch_base_libdir}/firmware/brcm/
 install ${S}/nvram_43455.txt ${D}${nonarch_base_libdir}/firmware/brcm/
}

FILES:${PN} += "${nonarch_base_libdir}/firmware/brcm/*"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/custom-licenses/${LICENSE};md5=957f6640d5e2d2acfce73a36a56cb32f"
