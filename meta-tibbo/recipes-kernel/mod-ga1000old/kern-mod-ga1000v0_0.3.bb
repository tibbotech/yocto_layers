DESCRIPTION = "Tibbo GA1000 old (libertas SPI) Wifi module"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
AUTHOR = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.1"
PR = "r1"
SRCREV="${AUTOREV}"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH_${PN}-includes = "all"

S = "${WORKDIR}/git"

inherit module

SRC_URI = "git://github.com/tibbotech/kern-mod-ga1000v0.git;protocol=https;branch=main"

#MAKE_TARGETS = "KERNEL_PATH=${STAGING_KERNEL_DIR} MAKE='make -e' -C ${STAGING_KERNEL_DIR} SUBDIRS=${S} modules"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 install -m 0644 ${S}/tpsga1000v0${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 # to force load ASAP for non-builtin module: applications start too fast
# install -d ${D}${sysconfdir}/modules-load.d
# install -m 0644 ${S}/tpsleds.conf ${D}${sysconfdir}/modules-load.d/
}

REDEPENDS_${PN} += "linux-firmware-sd8686"
REDEPENDS_${PN} += "linux-firmware-sd8688"
REDEPENDS_${PN} += "ga1000-firmware"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

DEPENDS += "virtual/kernel"