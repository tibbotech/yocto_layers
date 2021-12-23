DESCRIPTION = "Dv GPIO IRQ test module"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
AUTHOR = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
PR = "r1"

PACKAGES =+ "${PN}-includes"
PACKAGE_ARCH:${PN}-includes = "all"

S = "${WORKDIR}/${PN}"

inherit module

SRC_URI = "git://github.com/msperl/linux-rpi.git;protocol=https;branch=upstream-5.2-master-80f23-mcp25xxfd-v8.2;subpath=linux-rpi/drivers/net/can/spi/mcp25xxfd/"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
 install -m 0644 ${S}/dv-git${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
}

LIC_FILES_CHKSUM = "file://${FILESDIR_sunplus}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
