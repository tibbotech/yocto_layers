DESCRIPTION = "Tibbo TPS id module. Use FUSE emac to get uniq read-only TI cpsw MAC and use it as board ID"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
AUTHOR = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
PR = "r1"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH_${PN}-includes = "all"

# src in /files/kern-mod-stpctl/*
S = "${WORKDIR}/${PN}"

inherit module

SRC_URI = "file://*"

#MAKE_TARGETS = "KERNEL_PATH=${STAGING_KERNEL_DIR} MAKE='make -e' -C ${STAGING_KERNEL_DIR} SUBDIRS=${S} modules"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 install -m 0644 ${S}/sppctl${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 # to force load ASAP for non-builtin module: applications start too fast
 install -d ${D}${sysconfdir}/modules-load.d
 install -m 0644 ${S}/sppctl.conf ${D}${sysconfdir}/modules-load.d/
 # firmware file
 install -d ${D}/lib/firmware/
 install -m 0644 ${S}/sppctl.bin ${D}/lib/firmware/
}

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

FILES_${PN} += "/lib/firmware/sppctl.bin"

