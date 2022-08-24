DESCRIPTION = "Broadcom patchram utility"
HOMEPAGE = "https://chromium.googlesource.com/chromiumos/third_party/broadcom/"
SECTION = "networking"
LICENSE = "Apache-2.0"

SRCREV = "b146b5319f17fe8f21a608869fba86dccf670076"
SRC_URI  = "git://chromium.googlesource.com/chromiumos/third_party/broadcom/;protocol=http;branch=master"
SRC_URI += "file://brcm-patchram-plus@.service"
SRC_URI += "file://bt-reset.sh"

inherit base autotools

S="${WORKDIR}/git/bluetooth"
B="${S}"
D="${WORKDIR}/image/"

do_install:append() {
 install -d ${D}opt/
 install -m 0755 ${WORKDIR}/bt-reset.sh ${D}opt/bt-reset.sh
 install -d ${D}/lib/systemd/system/
 install -m 0644 ${WORKDIR}/brcm-patchram-plus@.service ${D}lib/systemd/system/brcm-patchram-plus@.service
}

FILES:${PN}  = "/usr/sbin/*"
FILES:${PN} += "/lib/systemd/system/*"
FILES:${PN} += "/opt/bt-reset.sh"

DEPENDS = "bluez5"
RDEPENDS:${PN} += "brcm-btfw"
RDEPENDS:${PN} += "bluez5"
RDEPENDS:${PN} += "kernel-module-bluetooth"
# it is compiled-in by _BLUETOOTH=y
# RDEPENDS:${PN} += "kernel-module-l2cap"
RDEPENDS:${PN} += "kernel-module-rfcomm"
#RDEPENDS:${PN} += "kernel-module-hci-vhci"
RDEPENDS:${PN} += "kernel-module-bnep"
RDEPENDS:${PN} += "kernel-module-hidp"
RDEPENDS:${PN} += "kernel-module-hci-uart"
# now it compiled-in by _BREDR flag
# RDEPENDS:${PN} += "kernel-module-sco"
RDEPENDS:${PN} += "kernel-module-cmac"
RDEPENDS:${PN} += "tps-bash"

# can't parse @ - fix it
SYSTEMD_SERVICE:${PN} = "brcm-patchram-plus@S4.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"
