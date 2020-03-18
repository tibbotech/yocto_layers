DESCRIPTION = "Realtek SDIO Wi-Fi driver for rtl8723bs"
HOMEPAGE = "https://github.com/hadess/rtl8723bs"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
PR = "r1"

SRCREV="${AUTOREV}"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH_${PN}-includes = "all"

# src in /files/kern-mod-rtl8723bs/*
S = "${WORKDIR}/git"

inherit module

#SRC_URI = "git://github.com/NextThingCo/RTL8723BS.git;branch=master;protocol=git"
SRC_URI = "git://github.com/macromorgan/RTL8723BS.git;branch=master;protocol=git"

#MAKE_TARGETS = "KERNEL_PATH=${STAGING_KERNEL_DIR} MAKE='make -e' -C ${STAGING_KERNEL_DIR} SUBDIRS=${S} modules"
#MAKE_TARGETS = " "

do_install() {
# install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
# install -m 0644 ${S}/rtl8723bs${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
# # to force load ASAP for non-builtin module: applications start too fast
# install -d ${D}${sysconfdir}/modules-load.d
# install -m 0644 ${S}/tpsid.conf ${D}${sysconfdir}/modules-load.d/
}

# add this (but not used)
RDEPENDS_${PN} += "linux-firmware-rtl8723"
RDEPENDS_${PN} += "linux-firmware-rtl8821"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

SRC_URI[md5sum] = "7ebef4599496186c5077c6ad22adcc3e"
SRC_URI[sha256sum] = "a6eee5a2a0b650760930529670336adc2ca08a31fa55742b3ffee96f1b6598f2"
