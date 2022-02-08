DESCRIPTION = "Realtek SDIO Wi-Fi driver for rtl8723bs"
HOMEPAGE = "https://github.com/hadess/rtl8723bs"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"

SRCREV="${AUTOREV}"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH:${PN}-includes = "all"

# src in /files/kern-mod-rtl8723bs/*
S = "${WORKDIR}/git"

inherit module

#SRC_URI = "git://github.com/NextThingCo/RTL8723BS.git;protocol=https;branch=master"
#SRC_URI = "git://github.com/macromorgan/RTL8723BS.git;protocol=https;branch=master"
SRC_URI = "git://github.com/PopcornComputer/RTL8723BS.git;protocol=https;branch=master"

do_install() {
# install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
# install -m 0644 ${S}/rtl8723bs${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
# # to force load ASAP for non-builtin module: applications start too fast
# install -d ${D}${sysconfdir}/modules-load.d
# install -m 0644 ${S}/tpsid.conf ${D}${sysconfdir}/modules-load.d/
}

# add this (but not used)
RDEPENDS:${PN} += "linux-firmware-rtl8723"
RDEPENDS:${PN} += "linux-firmware-rtl8821"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
