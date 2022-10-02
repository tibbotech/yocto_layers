DESCRIPTION = "TPS ini/dts/dtb converter/writer"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit kernel-arch

S = "${WORKDIR}"

SRC_URI  = "file://part_save"
SRC_URI += "file://template.dtt"
SRC_URI += "file://conv.sh"
SRC_URI += "file://ovl.sh"

FILES:${PN}  = "/opt/${PN}/*"
FILES:${PN} += "/opt/tps-shared/*"

do_install() {
  install -d ${D}/opt/${PN}/
  install -d ${D}/opt/tps-shared/dts-tpl/
  install -d ${D}/opt/tps-shared/dts-orig/include/dt-bindings/
  install -m 0755 ${WORKDIR}/conv.sh ${D}/opt/${PN}/
  install -m 0755 ${WORKDIR}/ovl.sh ${D}/opt/${PN}/
  install -m 0644 ${S}/part_save ${D}/opt/${PN}/
  install -m 0644 ${S}/template.dtt ${D}/opt/tps-shared/dts-tpl/
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/skeleton.dtsi ${D}/opt/tps-shared/dts-orig/
  cp -Lr ${STAGING_KERNEL_DIR}/include/dt-bindings/* ${D}/opt/tps-shared/dts-orig/include/dt-bindings/
}

# convert to
# PROVIDES += "virtual/example"
# COMPATIBLE_MACHINE = "machine1"
# later
do_install:append:tppg1() {
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/tps65910.dtsi ${D}/opt/tps-shared/dts-orig/
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/am33*KLM* ${D}/opt/tps-shared/dts-orig/
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/am33*TPP* ${D}/opt/tps-shared/dts-orig/
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/am33*tpp* ${D}/opt/tps-shared/dts-orig/
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/am33*clocks* ${D}/opt/tps-shared/dts-orig/
}
do_install:append:tppg2() {
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/sp7021*.dtsi ${D}/opt/tps-shared/dts-orig/
  install -m 0644 ${STAGING_KERNEL_DIR}/arch/${ARCH}/boot/dts/sp_remoteproc.dtsi ${D}/opt/tps-shared/dts-orig/
}

RDEPENDS:${PN} += "dtc tps-wifi"
#RRECOMMENDS:${PN} += "tps-ini2dts"
DEPENDS  = "virtual/kernel"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
