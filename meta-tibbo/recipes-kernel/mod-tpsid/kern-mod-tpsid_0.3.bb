DESCRIPTION = "Tibbo TPS id module. Use FUSE emac to get uniq read-only TI cpsw MAC and use it as board ID"
HOMEPAGE = "https://github.com/tibbotech/kern-mod-tpsid"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
AUTHOR = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
SRCREV="${AUTOREV}"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH_${PN}-includes = "all"

S = "${WORKDIR}/git"

inherit module

SRC_URI = "git://github.com/tibbotech/kern-mod-tpsid.git;protocol=https;branch=main"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 install -m 0644 ${S}/tpsid${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 # to force load ASAP for non-builtin module: applications start too fast
 install -d ${D}${sysconfdir}/modules-load.d
 install -m 0644 ${S}/tpsid.conf ${D}${sysconfdir}/modules-load.d/
}

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

DEPENDS += "virtual/kernel"

SRC_URI[sha256sum] = "a51c14ec69519203495639379815ad17dfaecc2c212ab277f5935a8bbc592886"
