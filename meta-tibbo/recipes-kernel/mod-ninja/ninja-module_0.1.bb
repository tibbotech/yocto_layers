DESCRIPTION = "Tibbo ninja kernel module for packet interception"
HOMEPAGE = "http://ninja.tibbo.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-3.0"
PR = "r1"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH_${PN}-includes = "all"

# if src in /files/ninja-module/*
S = "${WORKDIR}/${PN}"
# if src in /files/ninja-module-${PV}/*
#S = "${WORKDIR}/${PN}-${PV}"

inherit module

SRC_URI = "file://*"

MAKE_TARGETS = "KERNEL_PATH=${STAGING_KERNEL_DIR} MAKE='make -e' -C ${STAGING_KERNEL_DIR} SUBDIRS=${S} modules"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra
 install -m 0644 ${S}/ninja${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
 install -d ${D}${sysconfdir}
 install -d ${D}${sysconfdir}/init.d
 install -m 0755 ${PN} ${D}${sysconfdir}/init.d
 install -d ${D}/home/root
 install -m 0644 ${S}/ninja-module.conf ${D}/home/root/
 install -d ${D}${includedir}/
 install -m 0644 ${S}/ninja_defs_e.h ${D}${includedir}/
}

# we will get includes in tmp/deploy/glibc/.../ with it!
do_deploy() {
 install -d ${DEPLOY_DIR_IMAGE}/export/
 install -m 0644 ${S}/ninja_defs_e.h ${DEPLOY_DIR_IMAGE}/export/
}
addtask deploy before do_package_stage after do_compile

inherit update-rc.d

INITSCRIPT_NAME = "${PN}"
INITSCRIPT_PARAMS = "defaults 01"

FILES_${PN} += "/home/root/ninja-module.conf"
FILES_${PN} += "/etc/init.d/ninja-module"
FILES_${PN}-includes = "${includedir}/ninja_defs_e.h"

LIC_FILES_CHKSUM = "file://LGPL-3.0;md5=bfccfe952269fff2b407dd11f2f3083b"
