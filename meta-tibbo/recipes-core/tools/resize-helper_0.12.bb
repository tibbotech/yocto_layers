SUMMARY = "Resize image on first boot"
HOMEPAGE = "https://github.com/96boards/96boards-tools"
SECTION = "devel"
LICENSE = "GPLv2+"

SRC_URI  = "file://resize-helper"
SRC_URI += "file://resize-disk"
SRC_URI += "file://resize-helper.service"

S = "${WORKDIR}"

inherit systemd allarch update-rc.d

do_install () {
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${S}/resize-disk ${D}${sysconfdir}/init.d/

    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/resize-helper.service ${D}${systemd_unitdir}/system

    install -d ${D}${sbindir}
    install -m 0755 ${S}/resize-helper ${D}${sbindir}
}

INITSCRIPT_NAME = "resize-disk"
INITSCRIPT_PARAMS = "start 99 5 2 . stop 20 0 1 6 ."

SYSTEMD_SERVICE:${PN} = "resize-helper.service"
RDEPENDS:${PN} += "e2fsprogs-resize2fs gptfdisk parted util-linux udev"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
