DESCRIPTION = "LTPS inteerupt latency test module"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
AUTHOR = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH:${PN}-includes = "all"

# src in /files/kern-mod-intltest/*
S = "${WORKDIR}/${PN}"

inherit module

SRC_URI  = "file://intltest_defs.h"
SRC_URI += "file://intltest.h"
SRC_URI += "file://intltest_main.c"
SRC_URI += "file://intltest_procfs.c"
SRC_URI += "file://intltest_procfs.h"
SRC_URI += "file://intltest_syshdrs.h"
SRC_URI += "file://Makefile"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 install -m 0644 ${S}/intltest${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
}

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
