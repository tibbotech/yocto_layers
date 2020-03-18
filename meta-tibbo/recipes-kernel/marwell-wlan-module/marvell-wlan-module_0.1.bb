DESCRIPTION = "Marvell kernel module for Wifi"
HOMEPAGE = "http://marvell.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
RDEPENDS_${PN} = "marvell-fw"
LICENSE = "GPL-2.0"
PR = "r1"

# if src in /files/marvell-wlan-module/*
#S = "${WORKDIR}/${PN}"
S = "${WORKDIR}/wlan_src"
# if src in /files/marvell-wlan-module-${PV}/*
#S = "${WORKDIR}/${PN}-${PV}"

inherit module

SRC_URI = "file://*"

MAKE_TARGETS = "KERNEL_PATH=${STAGING_KERNEL_DIR} MAKE='make -e' -C ${STAGING_KERNEL_DIR} SUBDIRS=${S} modules"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra
 install -m 0644 ${S}/mlan${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
 install -m 0644 ${S}/sd8xxx${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
}

# we will get includes in tmp/deploy/glibc/.../ with it!
#do_deploy() {
# install -d ${DEPLOY_DIR_IMAGE}/export/
# install -m 0644 ${S}/ninja_defs_e.h ${DEPLOY_DIR_IMAGE}/export/
#}
#addtask deploy before do_package_stage after do_compile

inherit update-rc.d

INITSCRIPT_NAME = "${PN}"
INITSCRIPT_PARAMS = "defaults 05"

#FILES_${PN} += "/home/root/ninja-module.conf"
#FILES_${PN}-includes = "${includedir}/ninja_defs_e.h"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
