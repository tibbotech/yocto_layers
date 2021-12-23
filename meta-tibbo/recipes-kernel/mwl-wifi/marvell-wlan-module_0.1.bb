DESCRIPTION = "Marvell kernel module for Wifi"
HOMEPAGE = "http://marvell.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git/wlan_src"

inherit module

# BR="original"
BR="master"

SRC_URI = "git://github.com/tibbotech/mwl-wifi-sd8787.git;protocol=https;branch=${BR}"

MAKE_TARGETS = "KERNEL_PATH=${STAGING_KERNEL_DIR} MAKE='make -e' -C ${STAGING_KERNEL_DIR} SUBDIRS=${S} modules"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra
 install -m 0644 ${S}/mlan${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
 install -m 0644 ${S}/sd8xxx${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
}

inherit update-rc.d

INITSCRIPT_NAME = "${PN}"
INITSCRIPT_PARAMS = "defaults 05"

RDEPENDS:${PN} = "marvell-fw"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
