DESCRIPTION = "Broadcom patchram utility"
HOMEPAGE = "https://chromium.googlesource.com/chromiumos/third_party/broadcom/"
SECTION = "console/utils"

SRCREV = "b146b5319f17fe8f21a608869fba86dccf670076"
SRC_URI  = "git://chromium.googlesource.com/chromiumos/third_party/broadcom/;protocol=http;branch=master"
SRC_URI += "file://BCM4345C0_003.001.025.0165.0310.hcd"
SRC_URI += "file://BCM4345C5_003.006.006.0058.0135.hcd"
SRC_URI += "file://brcm-patchram-plus@.service"
SRC_URI += "file://bt-reset.sh"

inherit base autotools

S="${WORKDIR}/git/bluetooth/"
B="${S}"
D="${WORKDIR}/image/"

do_install_append() {
 install -d ${D}/lib/firmware/brcm/
 install -m 644 ${WORKDIR}/BCM4345C0_003.001.025.0165.0310.hcd ${D}/lib/firmware/brcm/
 install -m 644 ${WORKDIR}/BCM4345C5_003.006.006.0058.0135.hcd ${D}/lib/firmware/brcm/
 install -d ${D}/opt/
 install -m 0755 ${WORKDIR}/bt-reset.sh ${D}/opt/bt-reset.sh
 install -d ${D}/lib/systemd/system/
 install -m 0644 ${WORKDIR}/brcm-patchram-plus@.service ${D}/lib/systemd/system/brcm-patchram-plus@.service
}

FILES_${PN}  = "/usr/sbin/*"
FILES_${PN} += "/lib/firmware/brcm/*"
FILES_${PN} += "/lib/systemd/system/*"
FILES_${PN} += "/opt/bt-reset.sh"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

DEPENDS = "bluez5"
RDEPENDS_${PN} += "bluez5"
RDEPENDS_${PN} += "kernel-module-bluetooth"
# it is compiled-in by _BLUETOOTH=y
# RDEPENDS_${PN} += "kernel-module-l2cap"
RDEPENDS_${PN} += "kernel-module-rfcomm"
#RDEPENDS_${PN} += "kernel-module-hci-vhci"
RDEPENDS_${PN} += "kernel-module-bnep"
RDEPENDS_${PN} += "kernel-module-hidp"
RDEPENDS_${PN} += "kernel-module-hci-uart"
# now it compiled-in by _BREDR flag
# RDEPENDS_${PN} += "kernel-module-sco"
RDEPENDS_${PN} += "kernel-module-cmac"

# can't parse @ - fix it
SYSTEMD_SERVICE_brcm-patchram-plus = "brcm-patchram-plus@.service brcm-patchram-plus@S4"
SYSTEMD_AUTO_ENABLE_brcm-patchram-plus = "enable"
