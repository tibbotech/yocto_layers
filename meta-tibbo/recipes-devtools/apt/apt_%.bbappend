FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://*"

do_install_append() {
# mkdir -p ${D}${sysconfdir}/apt/sources.list.d/
# install -m 0644 ${WORKDIR}/*.list ${D}${sysconfdir}/apt/sources.list.d/
# echo "deb [arch=${ARCH}] http://tibbo.com/downloads/LTPS/repo ${MACHINE}"
 cp -rpf ${WORKDIR}/sources.list.d ${D}${sysconfdir}/apt/
 cp -rpf ${WORKDIR}/preferences.d ${D}${sysconfdir}/apt/
}

#do_install_append_tppg2() {
#}

FILES_${PN} += "${sysconfdir}/apt/*"

RDEPENDS_${PN} += "coreutils glibc-utils"

