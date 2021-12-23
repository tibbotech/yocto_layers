SUMMARY = "Broadcom bcmdhd firmware files"
DESCRIPTION = "Firmware for Broadcom BCMDHD (development AP6255)."
LICENSE = "Broadcom"
SRCREV="${AUTOREV}"

#SRC_URI  = "git://github.com/tibbotech/bcmdhd-firmware.git;protocol=https;branch=master"
SRC_URI  = ""
SRC_URI += "file://BCM4345C0_003.001.025.0165.0310.hcd"
SRC_URI += "file://BCM4345C5_003.006.006.0058.0135.hcd"

S = "${WORKDIR}/git"

inherit allarch

do_install() {
 install -d ${D}/lib/firmware/brcm/
 install -m 644 ${WORKDIR}/BCM4345C0_003.001.025.0165.0310.hcd ${D}/lib/firmware/brcm/
 install -m 644 ${WORKDIR}/BCM4345C5_003.006.006.0058.0135.hcd ${D}/lib/firmware/brcm/
}

FILES:${PN} += "${nonarch_base_libdir}/firmware/brcm/*"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/custom-licenses/${LICENSE};md5=957f6640d5e2d2acfce73a36a56cb32f"
